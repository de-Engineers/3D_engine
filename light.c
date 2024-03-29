﻿#include <windows.h>
#include <math.h>
#include <stdio.h> 
#include <intrin.h>

#include "cl.h"
#include "main.h"
#include "vec3.h"
#include "ivec2.h"

#pragma comment(lib,"OpenCL.lib")

u64 one = 0;

unsigned char *openclKernel;

u32 lmapC;

EXRGB *lmap;

VEC3 *lmapb;

cl_platform_id   clPlatformIds[20];
cl_context       clContext;
cl_device_id     clDeviceId;
cl_command_queue clCommandQueue;
cl_program       clProgram;
cl_kernel        clKernel;
cl_kernel        clAmbientX;
cl_kernel        clAmbientY;
cl_kernel        clAmbientZ;

cl_mem clMap;
cl_mem clLightmap;
cl_mem clLpmap;
cl_mem clMetadt;
cl_mem clMetadt2;
cl_mem clMetadt3;
cl_mem clMetadt4;
cl_mem clMetadt5;
cl_mem clMetadt6;

int clPlatformC;

f32 fract(f32 p){
	return p - floorf(p);
}

void initOpenCL(){
	openclKernel = loadFile("OpenCL/light.cl");
	clGetPlatformIDs(20,clPlatformIds,&clPlatformC);
	clGetDeviceIDs(clPlatformIds[0],CL_DEVICE_TYPE_DEFAULT,1,&clDeviceId,0);
	clContext      = clCreateContext(0,1,&clDeviceId,0,0,0);
	clCommandQueue = clCreateCommandQueueWithProperties(clContext,clDeviceId,0,0);
	clProgram      = clCreateProgramWithSource(clContext,1,(const char**)&openclKernel,0,0);
	clBuildProgram(clProgram,0,0,"-Werror",0,0);
	clKernel = clCreateKernel(clProgram,"lighting",0);
	clAmbientX = clCreateKernel(clProgram,"ambientX",0);
	clAmbientY = clCreateKernel(clProgram,"ambientY",0);
	clAmbientZ = clCreateKernel(clProgram,"ambientZ",0);
	HeapFree(GetProcessHeap(),0,openclKernel);
}

void openclSetKernelArgs(cl_kernel kernel){
	clSetKernelArg(kernel,0,sizeof(cl_mem),(void*)&clMap);
	clSetKernelArg(kernel,1,sizeof(cl_mem),(void*)&clMetadt);
	clSetKernelArg(kernel,2,sizeof(cl_mem),(void*)&clMetadt2);
	clSetKernelArg(kernel,3,sizeof(cl_mem),(void*)&clMetadt3);
	clSetKernelArg(kernel,4,sizeof(cl_mem),(void*)&clMetadt4);
	clSetKernelArg(kernel,5,sizeof(cl_mem),(void*)&clMetadt5);
	clSetKernelArg(kernel,6,sizeof(cl_mem),(void*)&clMetadt6);
	clSetKernelArg(kernel,7,sizeof(cl_mem),(void*)&clLightmap);
	clSetKernelArg(kernel,8,sizeof(cl_mem),(void*)&clLpmap);
}

void openclSetKernelArgs2(cl_kernel kernel,VEC3 color,VEC3 ang){
	clSetKernelArg(kernel,9,sizeof(f32),&properties->lvlSz);
	clSetKernelArg(kernel,10,sizeof(f32),&properties->lmapSz);
	clSetKernelArg(kernel,11,sizeof(f32),&color.x);
	clSetKernelArg(kernel,12,sizeof(f32),&color.y);
	clSetKernelArg(kernel,13,sizeof(f32),&color.z);
	clSetKernelArg(kernel,14,sizeof(f32),&ang.x);
	clSetKernelArg(kernel,15,sizeof(f32),&ang.y);
	clSetKernelArg(kernel,16,sizeof(f32),&ang.z);
}

void genLight(u64 totalRays,u32 block){
	VEC3 bpos;
	bpos.x = (f32)(block % properties->lvlSz)+0.5f;
	bpos.y = (f32)(block / properties->lvlSz % properties->lvlSz) + 0.5f;
	bpos.z = (f32)(block / (properties->lvlSz*properties->lvlSz)) + 0.5f;

	VEC3 lm = {1.0f-(f32)metadt[block].r/255.0f,1.0f-(f32)metadt[block].g/255.0f,1.0f-(f32){metadt[block].id/255.0f}};
	VEC3 lmo = {1.0f-(f32)metadt2[block].r/127.5f,1.0f-(f32)metadt2[block].g/127.5f,1.0f-(f32){metadt2[block].id/127.5f}};
	VEC3 color = {(f32)(map[block].r)/255.0f,(f32)(map[block].g)/255.0f,(f32)(map[block].b)/255.0f};
	f32 roff = (f32)metadt3[block].g/50.0f;

	totalRays /= properties->lmapSz*properties->lmapSz;
	totalRays *= properties->lmapSz*properties->lmapSz;

	clSetKernelArg(clKernel,9,sizeof(f32),&bpos.x);
	clSetKernelArg(clKernel,10,sizeof(f32),&bpos.y);
	clSetKernelArg(clKernel,11,sizeof(f32),&bpos.z);
	clSetKernelArg(clKernel,12,sizeof(f32),&color.x);
	clSetKernelArg(clKernel,13,sizeof(f32),&color.y);
	clSetKernelArg(clKernel,14,sizeof(f32),&color.z);
	clSetKernelArg(clKernel,15,sizeof(u32),&properties->lvlSz);
	clSetKernelArg(clKernel,16,sizeof(u32),&properties->lmapSz);
	clSetKernelArg(clKernel,17,sizeof(f32),&roff);
	clSetKernelArg(clKernel,18,sizeof(f32),&lm.x);
	clSetKernelArg(clKernel,19,sizeof(f32),&lm.y);
	clSetKernelArg(clKernel,20,sizeof(f32),&lm.z);
	clSetKernelArg(clKernel,21,sizeof(f32),&lmo.x);
	clSetKernelArg(clKernel,22,sizeof(f32),&lmo.y);
	clSetKernelArg(clKernel,23,sizeof(f32),&lmo.z);

	u64 t = __rdtsc();
	if(clKernel && properties->rayAcceleration){
		for(u32 i = 0;i < metadt3[block].id+1;i++){
			clEnqueueWriteBuffer(clCommandQueue,clLpmap,1,0,sizeof(LPMAP)*BLOCKCOUNT,lpmap,0,0,0);
			clEnqueueWriteBuffer(clCommandQueue,clLightmap,1,0,sizeof(VEC3)*properties->lmapSz*properties->lmapSz*lmapC,lmapb,0,0,0);
			printf("%i\n",clEnqueueNDRangeKernel(clCommandQueue,clKernel,1,0,&totalRays,&one,0,0,0));
			clEnqueueReadBuffer(clCommandQueue,clLightmap,1,0,sizeof(VEC3)*properties->lmapSz*properties->lmapSz*lmapC,lmapb,0,0,0);
			clEnqueueReadBuffer(clCommandQueue,clLpmap,1,0,sizeof(LPMAP)*BLOCKCOUNT,lpmap,0,0,0);
		}
	}
	else{
		for(u32 i = 0;i < metadt3[block].id+1;i++){
			cpuGenLight(bpos,color,totalRays,roff);
			printf("0\n");
		}
	}
	printf("time = %i\n",__rdtsc()-t>>16);
}

IVEC2 lmapToCrds(u32 p){
	return (IVEC2){p/properties->lmapSz,p%properties->lmapSz};
}

void updateLight2(){
	SuspendThread(HDRthread);
	HeapFree(GetProcessHeap(),0,lmap);
	lmapC = 0;
	for(u32 i = 0;i < BLOCKCOUNT;i++){
		switch(map[i].id){
		case BLOCK_AIR:
			lpmap[i].p1 = 0;
			lpmap[i].p2 = 0;
			lpmap[i].p3 = 0;
			break;
		case BLOCK_SPHERE:
			lpmap[i].p1 = lmapC;
			lpmap[i].p2 = lmapC+1;
			lpmap[i].p3 = lmapC+2;
			lpmap[i].p4 = lmapC+3;
			lmapC+=4;
			break;
		case BLOCK_CUBE:
			lpmap[i].p1 = lmapC;
			lpmap[i].p2 = lmapC+1;
			lpmap[i].p3 = lmapC+2;
			lpmap[i].p4 = lmapC+3;
			lpmap[i].p5 = lmapC+4;
			lpmap[i].p6 = lmapC+5;
			lmapC+=6;
			break;
		case 13:
			lpmap[i].p1 = lmapC;
			lpmap[i].p2 = lmapC+1;
			lmapC+=2;
			break;
		case 14:
			lpmap[i].p1 = lmapC;
			lpmap[i].p2 = lmapC+1;
			lmapC+=2;
			break;
		case 16:
			lpmap[i].p1 = lmapC;
			lpmap[i].p2 = lmapC+1;
			lpmap[i].p3 = lmapC+2;
			lpmap[i].p4 = lmapC+3;
			lpmap[i].p5 = lmapC+4;
			lpmap[i].p6 = lmapC+5;
			lpmap[i].p7 = lmapC+6;
			lpmap[i].p8 = lmapC+7;
			lpmap[i].p9 = lmapC+8;
			lpmap[i].p10 = lmapC+9;
			lpmap[i].p11 = lmapC+10;
			lpmap[i].p12 = lmapC+11;
			lmapC+=12;
			break;
		case BLOCK_REFLECTIVE2:
		case BLOCK_REFLECTIVE:
		case BLOCK_SOLID:{
			CVEC3 block = map2crds(i);
			if(block.x > 0){
				switch(map[i-1].id){
				case 27:
				case 28:
				case 30:
				case 32:
					break;
				default:
					lpmap[i].p1 = lmapC++;
					break;
				}
			}
			if(block.x < properties->lvlSz-1){
				switch(map[i+1].id){
				case 27:
				case 28:
				case 30:
				case 32:
					break;
				default:
					lpmap[i].p2 = lmapC++;
					break;
				}
			}
			if(block.y > 0){
				switch(map[i-properties->lvlSz].id){
				case 27:
				case 28:
				case 30:
				case 32:
					break;
				default:
					lpmap[i].p3 = lmapC++;
					break;
				}
			}
			if(block.y < properties->lvlSz-1){
				switch(map[i+properties->lvlSz].id){
				case 27:
				case 28:
				case 30:
				case 32:
					break;
				default:
					lpmap[i].p4 = lmapC++;
					break;
				}
			}
			if(block.z > 0){
				switch(map[i-properties->lvlSz*properties->lvlSz].id){
				case 27:
				case 28:
				case 30:
				case 32:
					break;
				default:
					lpmap[i].p5 = lmapC++;
					break;
				}
			}
			if(block.z < properties->lvlSz-1){
				switch(map[i+properties->lvlSz*properties->lvlSz].id){
				case 27:
				case 28:
				case 30:
				case 32:
					break;
				default:
					lpmap[i].p6 = lmapC++;
					break;
				}
			}
		}
		}
	}

	printf("totalTextures:");
	printf("%i\n",lmapC);

	lmap  = HeapAlloc(GetProcessHeap(),8,sizeof(EXRGB)*properties->lmapSzb*properties->lmapSzb*lmapC);
	lmapb = HeapAlloc(GetProcessHeap(),8,sizeof(VEC3)*properties->lmapSzb*properties->lmapSzb*lmapC);
	bmap  = HeapAlloc(GetProcessHeap(),8,sizeof(EXRGB)*properties->lmapSzb*properties->lmapSzb*lmapC);

	properties->lmapSz = properties->lmapSzb;

	clMap      = clCreateBuffer(clContext,CL_MEM_READ_WRITE,MAPRAM,0,0);
	clMetadt   = clCreateBuffer(clContext,CL_MEM_READ_WRITE,BLOCKCOUNT*sizeof(MAP),0,0);
	clMetadt2  = clCreateBuffer(clContext,CL_MEM_READ_WRITE,BLOCKCOUNT*sizeof(MAP),0,0);
	clMetadt3  = clCreateBuffer(clContext,CL_MEM_READ_WRITE,BLOCKCOUNT*sizeof(MAP),0,0);
	clMetadt4  = clCreateBuffer(clContext,CL_MEM_READ_WRITE,BLOCKCOUNT*sizeof(MAP),0,0);
	clMetadt5  = clCreateBuffer(clContext,CL_MEM_READ_WRITE,BLOCKCOUNT*sizeof(MAP),0,0);
	clMetadt6  = clCreateBuffer(clContext,CL_MEM_READ_WRITE,BLOCKCOUNT*sizeof(MAP),0,0);
	clLightmap = clCreateBuffer(clContext,CL_MEM_READ_WRITE,sizeof(VEC3)*properties->lmapSz*properties->lmapSz*lmapC,0,0);
	clLpmap    = clCreateBuffer(clContext,CL_MEM_READ_WRITE,BLOCKCOUNT*sizeof(LPMAP),0,0);

	clEnqueueWriteBuffer(clCommandQueue,clMap,1,0,BLOCKCOUNT*sizeof(MAP),map,0,0,0);
	clEnqueueWriteBuffer(clCommandQueue,clMetadt,1,0,sizeof(MAP)*BLOCKCOUNT,metadt,0,0,0);
	clEnqueueWriteBuffer(clCommandQueue,clMetadt2,1,0,sizeof(MAP)*BLOCKCOUNT,metadt2,0,0,0);
	clEnqueueWriteBuffer(clCommandQueue,clMetadt3,1,0,sizeof(MAP)*BLOCKCOUNT,metadt3,0,0,0);
	clEnqueueWriteBuffer(clCommandQueue,clMetadt4,1,0,sizeof(MAP)*BLOCKCOUNT,metadt4,0,0,0);
	clEnqueueWriteBuffer(clCommandQueue,clMetadt5,1,0,sizeof(MAP)*BLOCKCOUNT,metadt5,0,0,0);
	clEnqueueWriteBuffer(clCommandQueue,clMetadt6,1,0,sizeof(MAP)*BLOCKCOUNT,metadt6,0,0,0);
	clEnqueueWriteBuffer(clCommandQueue,clLpmap,1,0,sizeof(LPMAP)*BLOCKCOUNT,lpmap,0,0,0);

	openclSetKernelArgs(clKernel);
	openclSetKernelArgs(clAmbientX);
	openclSetKernelArgs(clAmbientY);
	openclSetKernelArgs(clAmbientZ);

	one = properties->lmapSz*properties->lmapSz;

	if(one > 512){
		one = 512;
	}

	starC = 0;

	for(u32 i = 0;i < properties->lvlSz*properties->lvlSz*properties->lvlSz;i++){
		switch(map[i].id){
		case 2:{
			VEC3 color2 = {(f32)map[i].r/255.0f,(f32)map[i].g/255.0f,(f32)map[i].b/255.0f};
			VEC3 ang = {127.0f-metadt[i].r,127.0f-metadt[i].g,127.0f-metadt[i].id};
			ang = VEC3normalize(ang);

			star[starC].pos = ang;
			star[starC].col = (RGB){map[i].r,map[i].g,map[i].b};
			star[starC++].skyCol  = (RGB){metadt2[i].r,metadt2[i].g,metadt2[i].id};

			openclSetKernelArgs2(clAmbientX,color2,ang);

			if(ang.x > 0.0f){
				float temp = 0.0f;
				clSetKernelArg(clAmbientX,17,sizeof(f32),&temp);
			}
			else{
				float temp = properties->lvlSz;
				clSetKernelArg(clAmbientX,17,sizeof(f32),&temp);
			}

			openclSetKernelArgs2(clAmbientY,color2,ang);

			if(ang.y > 0.0f){
				float temp = 0.0f;
				clSetKernelArg(clAmbientY,17,sizeof(f32),&temp);
			}
			else{
				float temp = properties->lvlSz;
				clSetKernelArg(clAmbientY,17,sizeof(f32),&temp);
			}

			openclSetKernelArgs2(clAmbientZ,color2,ang);

			if(ang.z > 0.0f){
				float temp = 0.0f;
				clSetKernelArg(clAmbientZ,17,sizeof(f32),&temp);
			}
			else{
				float temp = properties->lvlSz;
				clSetKernelArg(clAmbientZ,17,sizeof(f32),&temp);
			}

			u64 sampleC;
			sampleC = 4096.0f*4096.0f*properties->lmapSz*properties->lmapSz*sqrtf(1.0f-fabsf(ang.y))*sqrtf(1.0f-fabsf(ang.z));
			sampleC -= sampleC%one;

			if(clAmbientX){
				clEnqueueWriteBuffer(clCommandQueue,clLpmap,1,0,sizeof(LPMAP)*BLOCKCOUNT,lpmap,0,0,0);
				clEnqueueWriteBuffer(clCommandQueue,clLightmap,1,0,sizeof(VEC3)*properties->lmapSz*properties->lmapSz*lmapC,lmapb,0,0,0);
				printf("%i\n",clEnqueueNDRangeKernel(clCommandQueue,clAmbientX,1,0,&sampleC,&one,0,0,0));
				clEnqueueReadBuffer(clCommandQueue,clLightmap,1,0,sizeof(VEC3)*properties->lmapSz*properties->lmapSz*lmapC,lmapb,0,0,0);
				clEnqueueReadBuffer(clCommandQueue,clLpmap,1,0,sizeof(LPMAP)*BLOCKCOUNT,lpmap,0,0,0);
			}
			else{
				cpuGenLightAmbientX(ang,color2,sampleC);
			}

			sampleC = 4096.0f*4096.0f*properties->lmapSz*properties->lmapSz*sqrtf(1.0f-fabsf(ang.x))*sqrtf(1.0f-fabsf(ang.z));
			sampleC -= sampleC%one;

			if(clAmbientY){
				clEnqueueWriteBuffer(clCommandQueue,clLpmap,1,0,sizeof(LPMAP)*BLOCKCOUNT,lpmap,0,0,0);
				clEnqueueWriteBuffer(clCommandQueue,clLightmap,1,0,sizeof(VEC3)*properties->lmapSz*properties->lmapSz*lmapC,lmapb,0,0,0);
				printf("%i\n",clEnqueueNDRangeKernel(clCommandQueue,clAmbientZ,1,0,&sampleC,&one,0,0,0));
				clEnqueueReadBuffer(clCommandQueue,clLightmap,1,0,sizeof(VEC3)*properties->lmapSz*properties->lmapSz*lmapC,lmapb,0,0,0);
				clEnqueueReadBuffer(clCommandQueue,clLpmap,1,0,sizeof(LPMAP)*BLOCKCOUNT,lpmap,0,0,0);
			}
			else{
				cpuGenLightAmbientY(ang,color2,sampleC);
			}
	
			sampleC = 4096.0f*4096.0f*properties->lmapSz*properties->lmapSz*sqrtf(1.0f-fabsf(ang.x))*sqrtf(1.0f-fabsf(ang.y));
			sampleC -= sampleC%one;

			if(clAmbientZ){
				clEnqueueWriteBuffer(clCommandQueue,clLpmap,1,0,sizeof(LPMAP)*BLOCKCOUNT,lpmap,0,0,0);
				clEnqueueWriteBuffer(clCommandQueue,clLightmap,1,0,sizeof(VEC3)*properties->lmapSz*properties->lmapSz*lmapC,lmapb,0,0,0);
				printf("%i\n",clEnqueueNDRangeKernel(clCommandQueue,clAmbientZ,1,0,&sampleC,&one,0,0,0));
				clEnqueueReadBuffer(clCommandQueue,clLightmap,1,0,sizeof(VEC3)*properties->lmapSz*properties->lmapSz*lmapC,lmapb,0,0,0);
				clEnqueueReadBuffer(clCommandQueue,clLpmap,1,0,sizeof(LPMAP)*BLOCKCOUNT,lpmap,0,0,0);
			}
			else{
				cpuGenLightAmbientZ(ang,color2,sampleC);
			}

			break;
		}
		case 3:
			genLight(properties->lmapSz*properties->lmapSz*0x000800,i);
			break;
		case 4:
			genLight(properties->lmapSz*properties->lmapSz*0x002000,i);
			break;
		case 5:
			genLight(properties->lmapSz*properties->lmapSz*0x008000,i);
			break;
		case 6:
			genLight(properties->lmapSz*properties->lmapSz*0x020000,i);
			break;
		case 7:
			genLight(properties->lmapSz*properties->lmapSz*0x080000,i);
			break;
		case 8:{
			genLight(properties->lmapSz*properties->lmapSz*0x200000,i);
			break;	
			}
		}
	}

	playerspawnC = 0;
	for(u32 i = 0;i < BLOCKCOUNT;i++){
		switch(map[i].id){
		case BLOCK_SPAWN:{
			CVEC3 spwncrd = map2crds(i);
			playerspawn[playerspawnC++] = (VEC3){spwncrd.x+0.5f,spwncrd.y+0.5f,spwncrd.z+2.0f};
			break;
		}
		case BLOCK_CUBE:
			if(metadt4[i].b&&metadt4[i].b!=1){
				CVEC3 crd = map2crds(i);
				turret[turretC].pos = (VEC3){(f32)crd.x+0.5f,(f32)crd.y+0.5f,(f32)crd.z+0.5f};
				turret[turretC].id  = metadt4[i].b-2;
				turret[turretC].power = metadt4[i].g;
				turret[turretC++].totalCooldown = metadt4[i].id+1;
			}
		}
	}
	for(u32 i = 0;i < properties->lmapSz*properties->lmapSz*lmapC;i++){
		if(lmapb[i].x>65535.0f){
			lmapb[i].x = 65535.0f;
		}
		if(lmapb[i].y>65535.0f){
			lmapb[i].y = 65535.0f;
		}
		if(lmapb[i].z>65535.0f){
			lmapb[i].z = 65535.0f;
		}
		lmap[i] = (EXRGB){lmapb[i].x,lmapb[i].y,lmapb[i].z};
		bmap[i] = (EXRGB){lmapb[i].x,lmapb[i].y,lmapb[i].z};
	}
	HeapFree(GetProcessHeap(),0,lmapb);
	glMes[glMesC++].id = 6;
	levelSave("level");
	printf("done\n");
	ResumeThread(HDRthread);
}
