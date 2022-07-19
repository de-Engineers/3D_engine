#include <windows.h>
#include <math.h>
#include <stdio.h> 
#include <intrin.h>
#include "cl.h"

#include "main.h"
#include "vec3.h"

#pragma comment(lib,"OpenCL.lib")

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

int clPlatformC;

inline i32 hash(i32 x) {
	x += (x << 10);
	x ^= (x >> 6);
	x += (x << 3);
	x ^= (x >> 11);
	x += (x << 15);
	return x;
}

inline f32 rnd() {
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

inline float fract(float p){
	return p - (int)p;
}

void initOpenCL(){
	openclKernel = loadFile("OpenCL/light.cl");
	clGetPlatformIDs(20,clPlatformIds,&clPlatformC);
	clGetDeviceIDs(clPlatformIds[0],CL_DEVICE_TYPE_DEFAULT,1,&clDeviceId,0);
	clContext = clCreateContext(0,1,&clDeviceId,0,0,0);
	clCommandQueue = clCreateCommandQueueWithProperties(clContext,clDeviceId,0,0);
	clProgram      = clCreateProgramWithSource(clContext,1,(const char**)&openclKernel,0,0);
	clBuildProgram(clProgram,0,0,0,0,0);
	clKernel = clCreateKernel(clProgram,"lighting",0);
}

cl_mem clMap;
cl_mem clLightmap;
cl_mem clLpmap;
cl_mem clMetadt;
cl_mem clMetadt2;
cl_mem clMetadt3;

void GPUgenLight(u64 totalRays,u32 block){
	RAY ray;
	VEC3 bpos;
	bpos.x = (f32)(block % properties->lvlSz)+0.5f;
	bpos.y = (f32)(block / properties->lvlSz % properties->lvlSz) + 0.5f;
	bpos.z = (f32)(block / (properties->lvlSz*properties->lvlSz)) + 0.5f;
	VEC3 color = {(f32)(map[block].r)/25.0f,(f32)(map[block].g)/25.0f,(f32)(map[block].b)/25.0f};
	u64 one = properties->lmapSz*properties->lmapSz;
	f32 lightrd = (f32)metadt[block].b/255.0f;
	VEC2 rot = {(f32)metadt[block].r/255.0f,(f32)metadt[block].g/255.0f};

	totalRays = (f64)totalRays * (f64)lightrd;
	totalRays /= properties->lmapSz*properties->lmapSz;
	totalRays *= properties->lmapSz*properties->lmapSz;

	printf("%f\n",lightrd);

	if(one > 512){
		one = 512;
	}
	
	clSetKernelArg(clKernel,6,sizeof(f32),&bpos.x);
	clSetKernelArg(clKernel,7,sizeof(f32),&bpos.y);
	clSetKernelArg(clKernel,8,sizeof(f32),&bpos.z);
	clSetKernelArg(clKernel,9,sizeof(f32),&color.x);
	clSetKernelArg(clKernel,10,sizeof(f32),&color.y);
	clSetKernelArg(clKernel,11,sizeof(f32),&color.z);
	clSetKernelArg(clKernel,12,sizeof(u32),&properties->lvlSz);
	clSetKernelArg(clKernel,13,sizeof(u32),&properties->lmapSz);
	clSetKernelArg(clKernel,14,sizeof(f32),&lightrd);
	clSetKernelArg(clKernel,15,sizeof(f32),&rot.x);
	clSetKernelArg(clKernel,16,sizeof(f32),&rot.y);

	clEnqueueWriteBuffer(clCommandQueue,clMap,1,0,BLOCKCOUNT*sizeof(MAP),map,0,0,0);
	clEnqueueWriteBuffer(clCommandQueue,clMetadt,1,0,sizeof(MAP)*BLOCKCOUNT,metadt,0,0,0);
	clEnqueueWriteBuffer(clCommandQueue,clMetadt2,1,0,sizeof(MAP)*BLOCKCOUNT,metadt2,0,0,0);
	clEnqueueWriteBuffer(clCommandQueue,clMetadt3,1,0,sizeof(MAP)*BLOCKCOUNT,metadt3,0,0,0);
	clEnqueueWriteBuffer(clCommandQueue,clLightmap,1,0,sizeof(VEC3)*properties->lmapSz*properties->lmapSz*lmapC,lmapb,0,0,0);
	clEnqueueWriteBuffer(clCommandQueue,clLpmap,1,0,BLOCKCOUNT*sizeof(LPMAP),lpmap,0,0,0); 

	printf("%i\n",clEnqueueNDRangeKernel(clCommandQueue,clKernel,1,0,&totalRays,&one,0,0,0));

	clEnqueueReadBuffer(clCommandQueue,clLightmap,1,0,sizeof(VEC3)*properties->lmapSz*properties->lmapSz*lmapC,lmapb,0,0,0);
}

void updateLight2(){
	printf("generating...\n");
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
		case 28:{
			CVEC3 block = map2crds(i);
			if(block.x > 0){
				switch(map[i-1].id){
				case 0:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
					lpmap[i].p1 = lmapC;
					lmapC++;
					break;
				}
			}
			if(block.x < properties->lvlSz){
				switch(map[i+1].id){
				case 0:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
					lpmap[i].p2 = lmapC;
					lmapC++;
					break;
				}
			}
			if(block.y > 0){
				switch(map[i-properties->lvlSz].id){
				case 0:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
					lpmap[i].p3 = lmapC;
					lmapC++;
					break;
				}
			}
			if(block.y < properties->lvlSz){
				switch(map[i+properties->lvlSz].id){
				case 0:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
					lpmap[i].p4 = lmapC;
					lmapC++;
					break;
				}
			}
			if(block.z > 0){
				switch(map[i-properties->lvlSz*properties->lvlSz].id){
				case 0:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11:
				case 12:
					lpmap[i].p5 = lmapC;
					lmapC++;
					break;
				}
			}
			if(block.z < properties->lvlSz){
				switch(map[i+properties->lvlSz*properties->lvlSz].id){
				case 0:
				case 4:
				case 5:
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:
				case 11: 
				case 12:
					lpmap[i].p6 = lmapC;
					lmapC++;
					break;
				}
			}
		}
		}
	}

	lmap  = HeapAlloc(GetProcessHeap(),8,sizeof(EXRGB)*properties->lmapSzb*properties->lmapSzb*lmapC);
	lmapb = HeapAlloc(GetProcessHeap(),8,sizeof(VEC3)*properties->lmapSzb*properties->lmapSzb*lmapC);

	properties->lmapSz = properties->lmapSzb;

	clMap      = clCreateBuffer(clContext,CL_MEM_READ_WRITE,MAPRAM,0,0);
	clMetadt   = clCreateBuffer(clContext,CL_MEM_READ_WRITE,BLOCKCOUNT*sizeof(MAP),0,0);
	clMetadt2  = clCreateBuffer(clContext,CL_MEM_READ_WRITE,BLOCKCOUNT*sizeof(MAP),0,0);
	clMetadt3  = clCreateBuffer(clContext,CL_MEM_READ_WRITE,BLOCKCOUNT*sizeof(MAP),0,0);
	clLightmap = clCreateBuffer(clContext,CL_MEM_READ_WRITE,sizeof(VEC3)*properties->lmapSz*properties->lmapSz*lmapC,0,0);
	clLpmap    = clCreateBuffer(clContext,CL_MEM_READ_WRITE,BLOCKCOUNT*sizeof(LPMAP),0,0);

	clSetKernelArg(clKernel,0,sizeof(cl_mem),(void*)&clMap);
	clSetKernelArg(clKernel,1,sizeof(cl_mem),(void*)&clMetadt);
	clSetKernelArg(clKernel,2,sizeof(cl_mem),(void*)&clMetadt2);
	clSetKernelArg(clKernel,3,sizeof(cl_mem),(void*)&clMetadt3);
	clSetKernelArg(clKernel,4,sizeof(cl_mem),(void*)&clLightmap);
	clSetKernelArg(clKernel,5,sizeof(cl_mem),(void*)&clLpmap);

	for(u32 i = 0;i < properties->lvlSz*properties->lvlSz*properties->lvlSz;i++){
		switch(map[i].id){
		case 3:
			GPUgenLight(properties->lmapSz*properties->lmapSz*properties->lmapSz*0x000800,i);
			break;
		case 4:
			GPUgenLight(properties->lmapSz*properties->lmapSz*properties->lmapSz*0x002000,i);
			break;
		case 5:
			GPUgenLight(properties->lmapSz*properties->lmapSz*properties->lmapSz*0x008000,i);
			break;
		case 6:
			GPUgenLight(properties->lmapSz*properties->lmapSz*properties->lmapSz*0x020000,i);
			break;
		case 7:
			GPUgenLight(properties->lmapSz*properties->lmapSz*properties->lmapSz*0x080000,i);
			break;
		case 8:{
			GPUgenLight(properties->lmapSz*properties->lmapSz*properties->lmapSz*0x200000,i);
			break;	
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
	}
	HeapFree(GetProcessHeap(),0,lmapb);
	printf("generated light\n");
	glMes[glMesC].id = 6;
	glMesC++;
}

/*
inline void lightRayShoot(RAY *ray,float red,float green,float blue){
	while(green > 0.003f || red > 0.003f || blue > 0.003f){
		rayItterate(ray);
		if(ray->ix < 0 || ray->iy < 0 || ray->iz < 0 || ray->ix >= properties->lvlSz || ray->iy >= properties->lvlSz || ray->iz >= properties->lvlSz){
			break;
		}
		unsigned int block = crds2map(ray->ix,ray->iy,ray->iz);
		switch(map[block].id){
		case 0:
		case 8:
			break;
		case 28:{
			VEC2 wall;
			switch(ray->side){
			case 0:{
				wall.x = fract(ray->y + (ray->sidex - ray->deltax) * ray->vy);
				wall.y = fract(ray->z + (ray->sidex - ray->deltax) * ray->vz);
				u32 offset = (int)(wall.y*properties->lmapSz)*properties->lmapSz+(int)(wall.x*properties->lmapSz);
				if(ray->vx>0.0f){
					lmapb[lpmap[block].p1*properties->lmapSz*properties->lmapSz+offset].x += red;
					lmapb[lpmap[block].p1*properties->lmapSz*properties->lmapSz+offset].y += green;
					lmapb[lpmap[block].p1*properties->lmapSz*properties->lmapSz+offset].z += blue;
					red   *= (float)map[block].r/265.0f;
					green *= (float)map[block].g/265.0f;
					blue  *= (float)map[block].b/265.0f;
					VEC3 dir;
					dir.x = rnd()-2.0f;
					dir.x = -sqrtf(-dir.x);
					dir.y = rnd()*2.0f-3.0f;
					dir.z = rnd()*2.0f-3.0f;
					*ray = rayCreate((VEC3){ray->ix,(float)ray->iy+wall.y,(float)ray->iz+wall.x},dir);
				}
				else{
					lmapb[lpmap[block].p2*properties->lmapSz*properties->lmapSz+offset].x += red;
					lmapb[lpmap[block].p2*properties->lmapSz*properties->lmapSz+offset].y += green;
					lmapb[lpmap[block].p2*properties->lmapSz*properties->lmapSz+offset].z += blue;
					red   *= (float)map[block].r/265.0f;
					green *= (float)map[block].g/265.0f;
					blue  *= (float)map[block].b/265.0f;
					VEC3 dir;
					dir.x = rnd()-1.0f;
					dir.x = sqrtf(dir.x);
					dir.y = rnd()*2.0f-3.0f;
					dir.z = rnd()*2.0f-3.0f;
					*ray = rayCreate((VEC3){ray->ix+1,(float)ray->iy+wall.y,(float)ray->iz+wall.x},dir);
				}
				break;
				}
			case 1:{
				wall.x = fract(ray->x + (ray->sidey - ray->deltay) * ray->vx);
				wall.y = fract(ray->z + (ray->sidey - ray->deltay) * ray->vz);
				u32 offset = (int)(wall.y*properties->lmapSz)*properties->lmapSz+(int)(wall.x*properties->lmapSz);
				if(ray->vy>0.0f){
					lmapb[lpmap[block].p3*properties->lmapSz*properties->lmapSz+offset].x += red;
					lmapb[lpmap[block].p3*properties->lmapSz*properties->lmapSz+offset].y += green;
					lmapb[lpmap[block].p3*properties->lmapSz*properties->lmapSz+offset].z += blue;
					red   *= (float)map[block].r/265.0f;
					green *= (float)map[block].g/265.0f;
					blue  *= (float)map[block].b/265.0f;
					VEC3 dir;
					dir.x = rnd()*2.0f-3.0f;
					dir.y = rnd()-2.0f;
					dir.y = -sqrtf(-dir.y);
					dir.z = rnd()*2.0f-3.0f;
					*ray = rayCreate((VEC3){(float)ray->ix+wall.x,ray->iy,(float)ray->iz+wall.y},dir);
				}
				else{
					lmapb[lpmap[block].p4*properties->lmapSz*properties->lmapSz+offset].x += red;
					lmapb[lpmap[block].p4*properties->lmapSz*properties->lmapSz+offset].y += green;
					lmapb[lpmap[block].p4*properties->lmapSz*properties->lmapSz+offset].z += blue;
					red   *= (float)map[block].r/265.0f;
					green *= (float)map[block].g/265.0f;
					blue  *= (float)map[block].r/265.0f;
					VEC3 dir;
					dir.x = rnd()*2.0f-3.0f;
					dir.y = rnd()-1.0f;
					dir.y = sqrtf(dir.y);
					dir.z = rnd()*2.0f-3.0f;
					*ray = rayCreate((VEC3){(float)ray->ix+wall.x,ray->iy+1,(float)ray->iz+wall.y},dir);
				}
				break;
			}
			case 2:{	 
				wall.x = fract(ray->x + (ray->sidez - ray->deltaz) * ray->vx);
				wall.y = fract(ray->y + (ray->sidez - ray->deltaz) * ray->vy);
				u32 offset = (int)(wall.y*properties->lmapSz)*properties->lmapSz+(int)(wall.x*properties->lmapSz);
				if(ray->vz>0.0f){
					lmapb[lpmap[block].p5*properties->lmapSz*properties->lmapSz+offset].x += red;
					lmapb[lpmap[block].p5*properties->lmapSz*properties->lmapSz+offset].y += green;
					lmapb[lpmap[block].p5*properties->lmapSz*properties->lmapSz+offset].z += blue;
					red   *= (float)map[block].r/265.0f;
					green *= (float)map[block].g/265.0f;
					blue  *= (float)map[block].b/265.0f;
					VEC3 dir;
					dir.x = rnd()*2.0f-3.0f;
					dir.y = rnd()*2.0f-3.0f;
					dir.z = rnd()-2.0f;
					dir.z = -sqrtf(-dir.z);
					*ray = rayCreate((VEC3){(float)ray->ix+wall.x,(float)ray->iy+wall.y,ray->iz},dir);
				}
				else{
					lmapb[lpmap[block].p6*properties->lmapSz*properties->lmapSz+offset].x += red;
					lmapb[lpmap[block].p6*properties->lmapSz*properties->lmapSz+offset].y += green;
					lmapb[lpmap[block].p6*properties->lmapSz*properties->lmapSz+offset].z += blue;
					red   *= (float)map[block].r/265.0f;
					green *= (float)map[block].g/265.0f;
					blue  *= (float)map[block].b/265.0f;
					VEC3 dir;
					dir.x = rnd()*2.0f-3.0f;
					dir.y = rnd()*2.0f-3.0f;
					dir.z = rnd()-1.0f;
					dir.z = sqrtf(dir.z);
					*ray = rayCreate((VEC3){(float)ray->ix+wall.x,(float)ray->iy+wall.y,ray->iz+1},dir);
				}
				break;
			}
			}
			break;
		}
		default:
			break;
		}
	continue;
	end:
		break;
	}
}
*/
