#include <windows.h>
#include "main.h"
#include <math.h>
#include <stdio.h> 
#include <intrin.h>

#include "vec3.h"

#define PI 3.141592653f

unsigned int lmapC;
RGB *lmap;
VEC3 *lmapb;

inline unsigned int hash(unsigned int x) {
	x += (x << 10);
	x ^= (x >> 6);
	x += (x << 3);
	x ^= (x >> 11);
	x += (x << 15);
	return x;
}

inline float rnd() {
	union p {
		float f;
		unsigned int u;
	}r;
	r.u = hash(__rdtsc());
	r.u &= 0x007fffff;
	r.u |= 0x3f800000;
	return r.f;
}

inline int irnd() {
	return hash(__rdtsc());
}

inline float fract(float p){
	return p - (int)p;
}

VEC3 getSubCoords(RAY *ray){
	VEC3 p;
	switch(ray->side){
	case 0:
		if(ray->vx < 0.0f){
			p.x = ray->ix+1.0f;
		}
		else{
			p.x = ray->ix;
		}
		p.y = ray->y + (ray->sidex - ray->deltax) * ray->vy;
		p.z = ray->z + (ray->sidex - ray->deltax) * ray->vz;
		break;
	case 1:
		if(ray->vy < 0.0f){
			p.y = ray->iy+1.0f;
		}
		else{
			p.y = ray->iy;
		}
		p.x = ray->x + (ray->sidey - ray->deltay) * ray->vx;
		p.z = ray->z + (ray->sidey - ray->deltay) * ray->vz;
		break;
	case 2:
		if(ray->vz < 0.0f){
			p.z = ray->iz+1.0f;
		}
		else{
			p.z = ray->iz;
		}
		p.x = ray->x + (ray->sidez - ray->deltaz) * ray->vx;
		p.y = ray->y + (ray->sidez - ray->deltaz) * ray->vy;
		break;
	}
	return p;
}

inline void updateLightRay(RAY *ray,float red,float green,float blue){
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
				unsigned int offset = (int)(wall.y*properties->lmapSz)*properties->lmapSz+(int)(wall.x*properties->lmapSz);
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
				unsigned int offset = (int)(wall.y*properties->lmapSz)*properties->lmapSz+(int)(wall.x*properties->lmapSz);
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
				unsigned int offset = (int)(wall.y*properties->lmapSz)*properties->lmapSz+(int)(wall.x*properties->lmapSz)	;
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

void updateLight(int pos,float r,float g,float b){
	RAY ray;
	VEC3 bpos;
	bpos.x = (float)(pos % properties->lvlSz)+0.5f;
	bpos.y = (float)(pos / properties->lvlSz % properties->lvlSz) + 0.5f;
	bpos.z = (float)(pos / (properties->lvlSz*properties->lvlSz)) + 0.5f;
	float mrk = -0.624f;
	for(float i = -PI_025; i < PI_025;i+= 0.003f/(float)properties->lmapSz){
		for(float i2 = -PI_025; i2 < PI_025;i2+= 0.003f/(float)properties->lmapSz){
			float i3 = (rnd() - 1.0f) * PI_2 - PI;
			ray = rayCreate(bpos,(VEC3){sinf(i3)*cosf(i+i2),cosf(i3)*cosf(i+i2),sinf(i+i2)});
			updateLightRay(&ray,r,g,b);
		}
		if(i > mrk){
			printf("%i\n",(int)((mrk+0.78f)*64.11f));
			mrk+=0.156f;
		}
	}
}

void updateLightSingle(unsigned int *block){
	printf("generating...\n");
	if(!lmapC){
		for(int i = 0;i < properties->lvlSz*properties->lvlSz*properties->lvlSz;i++){
			if(map[i].id){
				CVEC3 block = map2crds(i);
				if(block.x > 0 && !map[i-1].id){
					lpmap[i].p1 = lmapC;
					lmapC++;
				}
				if(block.x < properties->lvlSz && !map[i+1].id){
					lpmap[i].p2 = lmapC;
					lmapC++;
				}
				if(block.y > 0 && !map[i-properties->lvlSz].id){
					lpmap[i].p3 = lmapC;
					lmapC++;
				}
				if(block.y < properties->lvlSz && !map[i+properties->lvlSz].id){
					lpmap[i].p4 = lmapC;
					lmapC++;
				}
				if(block.z > 0 && !map[i-properties->lvlSz*properties->lvlSz].id){
					lpmap[i].p5 = lmapC;
					lmapC++;
				}
				if(block.z < properties->lvlSz && !map[i+properties->lvlSz*properties->lvlSz].id){
					lpmap[i].p6 = lmapC;
					lmapC++;
				}
			}
		}
		lmap = HeapAlloc(GetProcessHeap(),8,sizeof(RGB)*properties->lmapSz*properties->lmapSz*lmapC);
	}
	lmapb = HeapAlloc(GetProcessHeap(),8,sizeof(VEC3)*properties->lmapSz*properties->lmapSz*lmapC);
	for(int i = 0;i < properties->lmapSz*properties->lmapSz*lmapC;i++){
		lmapb[i].x = lmap[i].r;
		lmapb[i].y = lmap[i].g;
		lmapb[i].z = lmap[i].b;
	}
	updateLight(*block,(float)(map[*block].r)/255.0f,(float)(map[*block].g)/255.0f,(float)(map[*block].b)/255.0f);
	for(int i = 0;i < properties->lmapSz*properties->lmapSz*lmapC;i++){
		if(lmapb[i].x>255.0f){
			lmapb[i].x = 255.0f;
		}
		if(lmapb[i].y>255.0f){
			lmapb[i].y = 255.0f;
		}
		if(lmapb[i].z>255.0f){
			lmapb[i].z = 255.0f;
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

void updateLight2(){
	printf("generating...\n");
	lmapC = 0;
	for(int i = 0;i < properties->lvlSz*properties->lvlSz*properties->lvlSz;i++){
		if(map[i].id){
			CVEC3 block = map2crds(i);
			if(block.x > 0 && !map[i-1].id){
				lpmap[i].p1 = lmapC;
				lmapC++;
			}
			if(block.x < properties->lvlSz && !map[i+1].id){
				lpmap[i].p2 = lmapC;
				lmapC++;
			}
			if(block.y > 0 && !map[i-properties->lvlSz].id){
				lpmap[i].p3 = lmapC;
				lmapC++;
			}
			if(block.y < properties->lvlSz && !map[i+properties->lvlSz].id){
				lpmap[i].p4 = lmapC;
				lmapC++;
			}
			if(block.z > 0 && !map[i-properties->lvlSz*properties->lvlSz].id){
				lpmap[i].p5 = lmapC;
				lmapC++;
			}
			if(block.z < properties->lvlSz && !map[i+properties->lvlSz*properties->lvlSz].id){
				lpmap[i].p6 = lmapC;
				lmapC++;
			}
		}
	}
	lmap  = HeapAlloc(GetProcessHeap(),8,sizeof(RGB)*properties->lmapSz*properties->lmapSz*lmapC);
	lmapb = HeapAlloc(GetProcessHeap(),8,sizeof(VEC3)*properties->lmapSz*properties->lmapSz*lmapC);
	for(int i = 0;i < properties->lvlSz*properties->lvlSz*properties->lvlSz;i++){
		if(map[i].id == 8){
			updateLight(i,(float)(map[i].r)/255.0f,(float)(map[i].g)/255.0f,(float)(map[i].b)/255.0f);
		}
	}
	for(int i = 0;i < properties->lmapSz*properties->lmapSz*lmapC;i++){
		if(lmapb[i].x>255.0f){
			lmapb[i].x = 255.0f;
		}
		if(lmapb[i].y>255.0f){
			lmapb[i].y = 255.0f;
		}
		if(lmapb[i].z>255.0f){
			lmapb[i].z = 255.0f;
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
