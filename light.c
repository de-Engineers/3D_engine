#include <windows.h>
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

unsigned int lmapC;

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

inline i32 hash(i32 x) {
	x += (x << 10);
	x ^= (x >> 6);
	x += (x << 3);
	x ^= (x >> 11);
	x += (x << 15);
	return x;
}

inline f32 rnd(){
	union p {
		float f;
		i32 u;
	}r;
	r.u = hash(__rdtsc());
	r.u &= 0x007fffff;
	r.u |= 0x3f800000;
	return r.f;
}

inline i32 irnd(){
	return hash(__rdtsc());
}

float fract(float p){
	return p - (i32)p;
}

void initOpenCL(){
	openclKernel = loadFile("OpenCL/light.cl");
	clGetPlatformIDs(20,clPlatformIds,&clPlatformC);
	clGetDeviceIDs(clPlatformIds[0],CL_DEVICE_TYPE_DEFAULT,1,&clDeviceId,0);
	clContext      = clCreateContext(0,1,&clDeviceId,0,0,0);
	clCommandQueue = clCreateCommandQueueWithProperties(clContext,clDeviceId,0,0);
	clProgram      = clCreateProgramWithSource(clContext,1,(const char**)&openclKernel,0,0);
	clBuildProgram(clProgram,0,0,0,0,0);
	clKernel = clCreateKernel(clProgram,"lighting",0);
	clAmbientX = clCreateKernel(clProgram,"ambientX",0);
	clAmbientY = clCreateKernel(clProgram,"ambientY",0);
	clAmbientZ = clCreateKernel(clProgram,"ambientZ",0);
}

void GPUgenLight(u64 totalRays,u32 block){
	RAY ray;
	VEC3 bpos;
	VEC3 lm = {1.0f-(f32)metadt[block].r/255.0f,1.0f-(f32)metadt[block].g/255.0f,1.0f-(f32){metadt[block].id/255.0f}};
	VEC3 lmo = {1.0f-(f32)metadt2[block].r/127.5f,1.0f-(f32)metadt2[block].g/127.5f,1.0f-(f32){metadt2[block].id/127.5f}};
	bpos.x = (f32)(block % properties->lvlSz)+0.5f;
	bpos.y = (f32)(block / properties->lvlSz % properties->lvlSz) + 0.5f;
	bpos.z = (f32)(block / (properties->lvlSz*properties->lvlSz)) + 0.5f;
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

	u32 lmapCB = lmapC;

	lmapC = lmapCB;
	if(clKernel){
		for(u32 i = 0;i < metadt3[block].id+1;i++){
			lmapCB = lmapC;
			clEnqueueWriteBuffer(clCommandQueue,clLightmap,1,0,sizeof(VEC3)*properties->lmapSz*properties->lmapSz*lmapC,lmapb,0,0,0);
			printf("%i\n",clEnqueueNDRangeKernel(clCommandQueue,clKernel,1,0,&totalRays,&one,0,0,0));
			clEnqueueReadBuffer(clCommandQueue,clLightmap,1,0,sizeof(VEC3)*properties->lmapSz*properties->lmapSz*lmapC,lmapb,0,0,0);
			lmapC = lmapCB;
		}
	}
	else{

		for (u32 i = 0; i < metadt3[block].id + 1; i++) {
			cpuGenLight(bpos,color,totalRays);
			printf("0\n");
		}
	}
}

inline IVEC2 lmapToCrds(u32 p){
	return (IVEC2){p/properties->lmapSz,p%properties->lmapSz};
}

inline void simulateLightRay(RAY ray,VEC3 col){
	rayItterate(&ray);
	while(ray.ix >= 0 && ray.iy >= 0 && ray.iz >= 0 && ray.ix < properties->lvlSz && ray.iy < properties->lvlSz && ray.iz < properties->lvlSz){
		u32 block = crds2map(ray.ix,ray.iy,ray.iz);
		switch(map[block].id){
		case 28:{
			VEC2 wall;
			switch(ray.sid){
			case 0:{
				wall.x = fract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y);
				wall.y = fract(ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z);
				u32 offset = (int)(wall.y*properties->lmapSz)*properties->lmapSz+(int)(wall.x*properties->lmapSz);
				if(ray.dir.x>0.0f){
					lmapb[lpmap[block].p1*properties->lmapSz*properties->lmapSz+offset].x += col.x;
					lmapb[lpmap[block].p1*properties->lmapSz*properties->lmapSz+offset].y += col.y;
					lmapb[lpmap[block].p1*properties->lmapSz*properties->lmapSz+offset].z += col.z;
					col.x *= (float)map[block].r/265.0f;
					col.y *= (float)map[block].g/265.0f;
					col.z *= (float)map[block].b/265.0f;
					VEC3 dir;
					dir.x = rnd()-2.0f;
					dir.x = -sqrtf(-dir.x);
					dir.y = rnd()*2.0f-3.0f;
					dir.z = rnd()*2.0f-3.0f;
					ray = rayCreate((VEC3){ray.ix,(float)ray.iy+wall.y,(float)ray.iz+wall.x},dir);
				}
				else{
					lmapb[lpmap[block].p2*properties->lmapSz*properties->lmapSz+offset].x += col.x;
					lmapb[lpmap[block].p2*properties->lmapSz*properties->lmapSz+offset].y += col.y;
					lmapb[lpmap[block].p2*properties->lmapSz*properties->lmapSz+offset].z += col.z;
					col.x *= (float)map[block].r/265.0f;
					col.y *= (float)map[block].g/265.0f;
					col.z *= (float)map[block].b/265.0f;
					VEC3 dir;
					dir.x = rnd()-1.0f;
					dir.x = sqrtf(dir.x);
					dir.y = rnd()*2.0f-3.0f;
					dir.z = rnd()*2.0f-3.0f;
					ray = rayCreate((VEC3){ray.ix+1,(float)ray.iy+wall.y,(float)ray.iz+wall.x},dir);
				}
				break;
				}
			case 1:{
				wall.x = fract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x);
				wall.y = fract(ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z);
				u32 offset = (int)(wall.y*properties->lmapSz)*properties->lmapSz+(int)(wall.x*properties->lmapSz);
				if(ray.dir.y>0.0f){
					lmapb[lpmap[block].p3*properties->lmapSz*properties->lmapSz+offset].x += col.x;
					lmapb[lpmap[block].p3*properties->lmapSz*properties->lmapSz+offset].y += col.y;
					lmapb[lpmap[block].p3*properties->lmapSz*properties->lmapSz+offset].z += col.z;
					col.x *= (float)map[block].r/265.0f;
					col.y *= (float)map[block].g/265.0f;
					col.z *= (float)map[block].b/265.0f;
					VEC3 dir;
					dir.x = rnd()*2.0f-3.0f;
					dir.y = rnd()-2.0f;
					dir.y = -sqrtf(-dir.y);
					dir.z = rnd()*2.0f-3.0f;
					ray = rayCreate((VEC3){(float)ray.ix+wall.x,ray.iy,(float)ray.iz+wall.y},dir);
				}
				else{
					lmapb[lpmap[block].p4*properties->lmapSz*properties->lmapSz+offset].x += col.x;
					lmapb[lpmap[block].p4*properties->lmapSz*properties->lmapSz+offset].y += col.y;
					lmapb[lpmap[block].p4*properties->lmapSz*properties->lmapSz+offset].z += col.z;
					col.x *= (float)map[block].r/265.0f;
					col.y *= (float)map[block].g/265.0f;
					col.z *= (float)map[block].r/265.0f;
					VEC3 dir;
					dir.x = rnd()*2.0f-3.0f;
					dir.y = rnd()-1.0f;
					dir.y = sqrtf(dir.y);
					dir.z = rnd()*2.0f-3.0f;
					ray = rayCreate((VEC3){(float)ray.ix+wall.x,ray.iy+1,(float)ray.iz+wall.y},dir);
				}
				break;
			}
			case 2:{	 
				wall.x = fract(ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x);
				wall.y = fract(ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y);
				u32 offset = (int)(wall.y*properties->lmapSz)*properties->lmapSz+(int)(wall.x*properties->lmapSz);
				if(ray.dir.z>0.0f){
					lmapb[lpmap[block].p5*properties->lmapSz*properties->lmapSz+offset].x += col.x;
					lmapb[lpmap[block].p5*properties->lmapSz*properties->lmapSz+offset].y += col.y;
					lmapb[lpmap[block].p5*properties->lmapSz*properties->lmapSz+offset].z += col.z;
					col.x *= (float)map[block].r/265.0f;
					col.y *= (float)map[block].g/265.0f;
					col.z *= (float)map[block].b/265.0f;
					VEC3 dir;
					dir.x = rnd()*2.0f-3.0f;
					dir.y = rnd()*2.0f-3.0f;
					dir.z = rnd()-2.0f;
					dir.z = -sqrtf(-dir.z);
					ray = rayCreate((VEC3){(float)ray.ix+wall.x,(float)ray.iy+wall.y,ray.iz},dir);
				}
				else{
					lmapb[lpmap[block].p6*properties->lmapSz*properties->lmapSz+offset].x += col.x;
					lmapb[lpmap[block].p6*properties->lmapSz*properties->lmapSz+offset].y += col.y;
					lmapb[lpmap[block].p6*properties->lmapSz*properties->lmapSz+offset].z += col.z;
					col.x *= (float)map[block].r/265.0f;
					col.y *= (float)map[block].g/265.0f;
					col.z *= (float)map[block].b/265.0f;
					VEC3 dir;
					dir.x = rnd()*2.0f-3.0f;
					dir.y = rnd()*2.0f-3.0f;
					dir.z = rnd()-1.0f;
					dir.z = sqrtf(dir.z);
					ray = rayCreate((VEC3){(float)ray.ix+wall.x,(float)ray.iy+wall.y,ray.iz+1},dir);
				}
				break;
			}
			}
			if(col.x < 0.01f || col.y < 0.01f || col.z < 0.01f){
				return;
			}
			break;
			}
		}
		rayItterate(&ray);
	}
}

void updateLight2(){
	HeapFree(GetProcessHeap(),0,lmap);
	lmapC = 0;
	for(u32 i = 0;i < BLOCKCOUNT;i++){
		switch(map[i].id){
		case 9:
			lpmap[i].p1 = lmapC;
			lpmap[i].p2 = lmapC+1;
			lpmap[i].p3 = lmapC+2;
			lpmap[i].p4 = lmapC+3;
			lmapC+=4;
			break;
		case 12:
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
		case 30:{
			CVEC3 block = map2crds(i);
			if(block.x > 0){
				switch(map[i-1].id){
				case 30:
					break;
				default:
					lpmap[i].p7 = lmapC;
					lmapC++;
					break;
				}
			}
			if(block.x < properties->lvlSz){
				switch(map[i+1].id){
				case 30:
					break;
				default:
					lpmap[i].p8 = lmapC;
					lmapC++;
					break;
				}
			}
			if(block.y > 0){
				switch(map[i-properties->lvlSz].id){
				case 30:
					break;
				default:
					lpmap[i].p9 = lmapC;
					lmapC++;
					break;
				}
			}
			if(block.y < properties->lvlSz){
				switch(map[i+properties->lvlSz].id){
				case 30:
					break;
				default:
					lpmap[i].p10 = lmapC;
					lmapC++;
					break;
				}
			}
			if(block.z > 0){
				switch(map[i-properties->lvlSz*properties->lvlSz].id){
				case 30:
					break;
				default:
					lpmap[i].p11 = lmapC;
					lmapC++;
					break;
				}
			}
			if(block.z < properties->lvlSz){
				switch(map[i+properties->lvlSz*properties->lvlSz].id){
				case 30:
					break;
				default:
					lpmap[i].p12 = lmapC;
					lmapC++;
					break;
				}
			}
		}
		case 32:
		case 27:
		case 28:{
			CVEC3 block = map2crds(i);
			if(block.x > 0){
				switch(map[i-1].id){
				case 27:
				case 28:
				case 30:
				case 32:
					break;
				default:
					lpmap[i].p1 = lmapC;
					lmapC++;
					break;
				}
			}
			if(block.x < properties->lvlSz){
				switch(map[i+1].id){
				case 27:
				case 28:
				case 30:
				case 32:
					break;
				default:
					lpmap[i].p2 = lmapC;
					lmapC++;
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
					lpmap[i].p3 = lmapC;
					lmapC++;
					break;
				}
			}
			if(block.y < properties->lvlSz){
				switch(map[i+properties->lvlSz].id){
				case 27:
				case 28:
				case 30:
				case 32:
					break;
				default:
					lpmap[i].p4 = lmapC;
					lmapC++;
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
					lpmap[i].p5 = lmapC;
					lmapC++;
					break;
				}
			}
			if(block.z < properties->lvlSz){
				switch(map[i+properties->lvlSz*properties->lvlSz].id){
				case 27:
				case 28:
				case 30:
				case 32:
					break;
				default:
					lpmap[i].p6 = lmapC;
					lmapC++;
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

	clSetKernelArg(clKernel,0,sizeof(cl_mem),(void*)&clMap);
	clSetKernelArg(clKernel,1,sizeof(cl_mem),(void*)&clMetadt);
	clSetKernelArg(clKernel,2,sizeof(cl_mem),(void*)&clMetadt2);
	clSetKernelArg(clKernel,3,sizeof(cl_mem),(void*)&clMetadt3);
	clSetKernelArg(clKernel,4,sizeof(cl_mem),(void*)&clMetadt4);
	clSetKernelArg(clKernel,5,sizeof(cl_mem),(void*)&clMetadt5);
	clSetKernelArg(clKernel,6,sizeof(cl_mem),(void*)&clMetadt6);
	clSetKernelArg(clKernel,7,sizeof(cl_mem),(void*)&clLightmap);
	clSetKernelArg(clKernel,8,sizeof(cl_mem),(void*)&clLpmap);

	clSetKernelArg(clAmbientX,0,sizeof(cl_mem),(void*)&clMap);
	clSetKernelArg(clAmbientX,1,sizeof(cl_mem),(void*)&clMetadt);
	clSetKernelArg(clAmbientX,2,sizeof(cl_mem),(void*)&clMetadt2);
	clSetKernelArg(clAmbientX,3,sizeof(cl_mem),(void*)&clMetadt3);
	clSetKernelArg(clAmbientX,4,sizeof(cl_mem),(void*)&clMetadt4);
	clSetKernelArg(clAmbientX,5,sizeof(cl_mem),(void*)&clMetadt5);
	clSetKernelArg(clAmbientX,6,sizeof(cl_mem),(void*)&clMetadt6);
	clSetKernelArg(clAmbientX,7,sizeof(cl_mem),(void*)&clLightmap);
	clSetKernelArg(clAmbientX,8,sizeof(cl_mem),(void*)&clLpmap);

	clSetKernelArg(clAmbientY,0,sizeof(cl_mem),(void*)&clMap);
	clSetKernelArg(clAmbientY,1,sizeof(cl_mem),(void*)&clMetadt);
	clSetKernelArg(clAmbientY,2,sizeof(cl_mem),(void*)&clMetadt2);
	clSetKernelArg(clAmbientY,3,sizeof(cl_mem),(void*)&clMetadt3);
	clSetKernelArg(clAmbientY,4,sizeof(cl_mem),(void*)&clMetadt4);
	clSetKernelArg(clAmbientY,5,sizeof(cl_mem),(void*)&clMetadt5);
	clSetKernelArg(clAmbientY,6,sizeof(cl_mem),(void*)&clMetadt6);
	clSetKernelArg(clAmbientY,7,sizeof(cl_mem),(void*)&clLightmap);
	clSetKernelArg(clAmbientY,8,sizeof(cl_mem),(void*)&clLpmap);

	clSetKernelArg(clAmbientZ,0,sizeof(cl_mem),(void*)&clMap);
	clSetKernelArg(clAmbientZ,1,sizeof(cl_mem),(void*)&clMetadt);
	clSetKernelArg(clAmbientZ,2,sizeof(cl_mem),(void*)&clMetadt2);
	clSetKernelArg(clAmbientZ,3,sizeof(cl_mem),(void*)&clMetadt3);
	clSetKernelArg(clAmbientZ,4,sizeof(cl_mem),(void*)&clMetadt4);
	clSetKernelArg(clAmbientZ,5,sizeof(cl_mem),(void*)&clMetadt5);
	clSetKernelArg(clAmbientZ,6,sizeof(cl_mem),(void*)&clMetadt6);
	clSetKernelArg(clAmbientZ,7,sizeof(cl_mem),(void*)&clLightmap);
	clSetKernelArg(clAmbientZ,8,sizeof(cl_mem),(void*)&clLpmap);

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
			star[starC].skyCol  = (RGB){metadt2[i].r,metadt2[i].g,metadt2[i].id};
			starC++;

			clSetKernelArg(clAmbientX,9,sizeof(f32),&properties->lvlSz);
			clSetKernelArg(clAmbientX,10,sizeof(f32),&properties->lmapSz);
			clSetKernelArg(clAmbientX,11,sizeof(f32),&color2.x);
			clSetKernelArg(clAmbientX,12,sizeof(f32),&color2.y);
			clSetKernelArg(clAmbientX,13,sizeof(f32),&color2.z);
			clSetKernelArg(clAmbientX,14,sizeof(f32),&ang.x);
			clSetKernelArg(clAmbientX,15,sizeof(f32),&ang.y);
			clSetKernelArg(clAmbientX,16,sizeof(f32),&ang.z);

			if(ang.x > 0.0f){
				float temp = 0.0f;
				clSetKernelArg(clAmbientX,17,sizeof(f32),&temp);
			}
			else{
				float temp = properties->lvlSz;
				clSetKernelArg(clAmbientX,17,sizeof(f32),&temp);
			}

			clSetKernelArg(clAmbientY,9,sizeof(f32),&properties->lvlSz);
			clSetKernelArg(clAmbientY,10,sizeof(f32),&properties->lmapSz);
			clSetKernelArg(clAmbientY,11,sizeof(f32),&color2.x);
			clSetKernelArg(clAmbientY,12,sizeof(f32),&color2.y);
			clSetKernelArg(clAmbientY,13,sizeof(f32),&color2.z);
			clSetKernelArg(clAmbientY,14,sizeof(f32),&ang.x);
			clSetKernelArg(clAmbientY,15,sizeof(f32),&ang.y);
			clSetKernelArg(clAmbientY,16,sizeof(f32),&ang.z);

			if(ang.y > 0.0f){
				float temp = 0.0f;
				clSetKernelArg(clAmbientY,17,sizeof(f32),&temp);
			}
			else{
				float temp = properties->lvlSz;
				clSetKernelArg(clAmbientY,17,sizeof(f32),&temp);
			}

			clSetKernelArg(clAmbientZ,9,sizeof(f32),&properties->lvlSz);
			clSetKernelArg(clAmbientZ,10,sizeof(f32),&properties->lmapSz);
			clSetKernelArg(clAmbientZ,11,sizeof(f32),&color2.x);
			clSetKernelArg(clAmbientZ,12,sizeof(f32),&color2.y);
			clSetKernelArg(clAmbientZ,13,sizeof(f32),&color2.z);
			clSetKernelArg(clAmbientZ,14,sizeof(f32),&ang.x);
			clSetKernelArg(clAmbientZ,15,sizeof(f32),&ang.y);
			clSetKernelArg(clAmbientZ,16,sizeof(f32),&ang.z);

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
				clEnqueueWriteBuffer(clCommandQueue,clLightmap,1,0,sizeof(VEC3)*properties->lmapSz*properties->lmapSz*lmapC,lmapb,0,0,0);
				printf("%i\n",clEnqueueNDRangeKernel(clCommandQueue,clAmbientX,1,0,&sampleC,&one,0,0,0));
				clEnqueueReadBuffer(clCommandQueue,clLightmap,1,0,sizeof(VEC3)*properties->lmapSz*properties->lmapSz*lmapC,lmapb,0,0,0);
			}
			else{
				cpuGenLightAmbientX(ang,color2,sampleC);
			}

			sampleC = 4096.0f*4096.0f*properties->lmapSz*properties->lmapSz*sqrtf(1.0f-fabsf(ang.x))*sqrtf(1.0f-fabsf(ang.z));
			sampleC -= sampleC%one;

			if(clAmbientY){
				clEnqueueWriteBuffer(clCommandQueue,clLightmap,1,0,sizeof(VEC3)*properties->lmapSz*properties->lmapSz*lmapC,lmapb,0,0,0);
				printf("%i\n",clEnqueueNDRangeKernel(clCommandQueue,clAmbientZ,1,0,&sampleC,&one,0,0,0));
				clEnqueueReadBuffer(clCommandQueue,clLightmap,1,0,sizeof(VEC3)*properties->lmapSz*properties->lmapSz*lmapC,lmapb,0,0,0);
			}
			else{
				cpuGenLightAmbientY(ang,color2,sampleC);
			}
	
			sampleC = 4096.0f*4096.0f*properties->lmapSz*properties->lmapSz*sqrtf(1.0f-fabsf(ang.x))*sqrtf(1.0f-fabsf(ang.y));
			sampleC -= sampleC%one;

			if(clAmbientZ){
				clEnqueueWriteBuffer(clCommandQueue,clLightmap,1,0,sizeof(VEC3)*properties->lmapSz*properties->lmapSz*lmapC,lmapb,0,0,0);
				printf("%i\n",clEnqueueNDRangeKernel(clCommandQueue,clAmbientZ,1,0,&sampleC,&one,0,0,0));
				clEnqueueReadBuffer(clCommandQueue,clLightmap,1,0,sizeof(VEC3)*properties->lmapSz*properties->lmapSz*lmapC,lmapb,0,0,0);
			}
			else{
				cpuGenLightAmbientZ(ang,color2,sampleC);
			}

			break;
		}
		case 3:
			GPUgenLight(properties->lmapSz*properties->lmapSz*0x000800,i);
			break;
		case 4:
			GPUgenLight(properties->lmapSz*properties->lmapSz*0x002000,i);
			break;
		case 5:
			GPUgenLight(properties->lmapSz*properties->lmapSz*0x008000,i);
			break;
		case 6:
			GPUgenLight(properties->lmapSz*properties->lmapSz*0x020000,i);
			break;
		case 7:
			GPUgenLight(properties->lmapSz*properties->lmapSz*0x080000,i);
			break;
		case 8:{
			GPUgenLight(properties->lmapSz*properties->lmapSz*0x200000,i);
			break;	
			}
		}
	}

	playerspawnC = 0;
	for(u32 i = 0;i < BLOCKCOUNT;i++){
		switch(map[i].id){
		case BLOCK_SPAWN:{
			CVEC3 spwncrd = map2crds(i);
			playerspawn[playerspawnC] = (VEC3){spwncrd.x+0.5f,spwncrd.y+0.5f,spwncrd.z+2.0f};
			playerspawnC++;
			break;
		}
		case BLOCK_CUBE:
			if(metadt4[i].r){
				CVEC3 crd = map2crds(i);
				turret[turretC].pos = (VEC3){(f32)crd.x+0.5f,(f32)crd.y+0.5f,(f32)crd.z+0.5f};
				turret[turretC].id  = metadt4[i].r-1;
				turretC++;
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
		lmap[i].r = lmapb[i].x;
		lmap[i].g = lmapb[i].y;
		lmap[i].b = lmapb[i].z;
		bmap[i].r = lmapb[i].x;
		bmap[i].g = lmapb[i].y;
		bmap[i].b = lmapb[i].z;
	}
	HeapFree(GetProcessHeap(),0,lmapb);
	glMes[glMesC].id = 6;
	glMesC++;
	levelSave("level");
	printf("done\n");
}
