#include <windows.h>
#include <main.h>
#include <math.h>
#include <stdio.h> 
#include <intrin.h>

float *lightmap;

typedef union{
	float f;
	unsigned int ui;
}FLOATINT;

typedef struct{
	FLOATINT x;
	FLOATINT y;
}FLOATINTVEC2;

unsigned int hash(unsigned int x){
    x += x << 10;
    x ^= x >>  6;
    x += x <<  3;
    x ^= x >> 11;
    x += x << 15;
    return x;
}

float random(FLOATINTVEC2 x){
	FLOATINT r;
	r.ui = hash(x.x.ui) ^ hash(x.y.ui);
	r.ui &= 0x007fffff;
	r.ui |= 0x3F800000;
	return r.f-1.0f;
}

float randoms(FLOATINT x){
	x.ui = hash(x.ui);
	x.ui &= 0x007fffff;
	x.ui |= 0x3F800000;
	return x.f-1.0f;
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

inline VEC3 vec3div(VEC3 v,float x){
	v.x /= x;
	v.y /= x;
	v.z /= x;
	return v;
}
	
inline VEC3 vec3fract(VEC3 v){
	v.x -= (int)v.x;
	v.y -= (int)v.y;
	v.z -= (int)v.z;
	return v;
}

inline float vec3distance(VEC3 v,VEC3 v2){
	float x = v.x-v2.x;
	float y = v.y-v2.y;
	float z = v.z-v2.z;
	return sqrtf(x*x+y*y+z*z);
}

inline float vec3dot(VEC3 v,VEC3 v2){
	return v.x * v2.x + v.y * v2.y + v.z * v2.z;
}

inline VEC3 vec3reflect(VEC3 v,VEC3 v2){
	float d = vec3dot(v2,v);
	v.x = v.x - 2.0f * d * v2.x;
	v.y = v.y - 2.0f * d * v2.y;
	v.z = v.z - 2.0f * d * v2.z;
	return v;
}

inline VEC3 vec3normalize(VEC3 v){
	float m = fmaxf(fmaxf(v.x,v.y),v.z);
	v.x /= m;
	v.y /= m;
	v.z /= m;
	return v;
}

inline VEC3 vec3sub(VEC3 v,VEC3 v2){
	v.x -= v2.x;
	v.y -= v2.y;
	v.z -= v2.z;
	return v;
}

void updateLightRay(RAY *ray,float red,float green,float blue){
	while(green > 0.003f || red > 0.003f || blue > 0.003f){
		rayItterate(ray);
		if(ray->ix < 0 || ray->iy < 0 || ray->iz < 0 || ray->ix >= properties->lvlSz*properties->lmapSz2 || ray->iy >= properties->lvlSz*properties->lmapSz2 || ray->iz >= properties->lvlSz*properties->lmapSz2){
			break;
		}
		int block  = crds2map(ray->ix/properties->lmapSz2,ray->iy/properties->lmapSz2,ray->iz/properties->lmapSz2);
		int block2 = crds2lmap(ray->ix,ray->iy,ray->iz);
		if(map[block]){
			switch(map[block]){
			case 4:{
				VEC3 p = vec3fract(vec3div(getSubCoords(ray),properties->lmapSz2));
				if(vec3distance(p,(VEC3){0.5f,0.5f,0.5f})<0.4f){
					VEC3 d = vec3reflect((VEC3){ray->vx,ray->vy,ray->vz},vec3normalize(vec3sub(p,(VEC3){0.5f,0.5f,0.5f})));
					red /= 2.0f;
					green /= 2.0f;
					blue /= 2.0f;
					*ray = rayCreate(p.x,p.y,p.z,d.x,d.y,d.z);
				}
				break;
			}
			case 9:
				break;
			case 12:{
				VEC3 p = vec3fract(vec3div(getSubCoords(ray),properties->lmapSz2));
				if(p.x < 0.625f && p.x > 0.425f && p.y < 0.625f && p.y > 0.425f){
					VEC3 d;
					float r1 = (float)(rand()^_rdtsc())/RAND_MAX * 3.14f - 3.14f;
					float r2 = (float)(rand()^_rdtsc())/RAND_MAX * 3.14f - 3.14f;
					float r3 = r1 + r2;
					float r4 = (float)(rand()^_rdtsc())/RAND_MAX * 6.28f - 3.14f;
					d.x = cosf(r4) * cosf(r3);
					d.y = sinf(r4) * cosf(r3);
					d.z = sinf(r3);
					switch(ray->side){
					case 0:
						if(ray->vx < 0.0f && d.x < 0.0f){
							red = 0.0f;
							green = 0.0f;
							blue = 0.0f;
						}
						if(ray->vx >= 0.0f && d.x >= 0.0f){
							red = 0.0f;
							green = 0.0f;
							blue = 0.0f;
						}
						break;
					case 1:
						if(ray->vy < 0.0f && d.y < 0.0f){
							red = 0.0f;
							green = 0.0f;
							blue = 0.0f;
						}
						if(ray->vy >= 0.0f && d.y >= 0.0f){
							red = 0.0f;
							green = 0.0f;
							blue = 0.0f;
						}
						break;
					case 2:
						if(ray->vz < 0.0f && d.z < 0.0f){
							red = 0.0f;
							green = 0.0f;
							blue = 0.0f;
						}
						if(ray->vz >= 0.0f && d.z >= 0.0f){
							red = 0.0f;
							green = 0.0f;
							blue = 0.0f;
						}
						break;
					}
					*ray = rayCreate(p.x,p.y,p.z,d.x,d.y,d.z);
				}
				break;
			}
			case 20:
				if((float)rand() / RAND_MAX > 1.0f){
					goto dflt;
				}
				break;
			case 21:
				break;
			case 60:
				break;	
			dflt:
			default:{
				VEC3 p = getSubCoords(ray);
				VEC3 d;
				float r1 = (float)(rand()^_rdtsc());
				float r2 = (float)(rand()^_rdtsc());
				float r3 = r1 + r2;
				float r4 = (float)(rand()^_rdtsc());
				d.x = cosf(r4) * cosf(r3);
				d.y = sinf(r4) * cosf(r3);
				d.z = sinf(r3);
				red   *= (float)map[block+1]*0.002f;
				green *= (float)map[block+2]*0.002f;
				blue  *= (float)map[block+3]*0.002f;
				switch(ray->side){
				case 0:
					if(ray->vx < 0.0f){
						if(d.x < 0.0f){
							d.x = -d.x;
						}
					}
					if(ray->vx >= 0.0f){
						if(d.x >= 0.0f){
							d.x = -d.x;
						}
					}
					break;
				case 1:
					if(ray->vy < 0.0f){
						if(d.y < 0.0f){
							d.y = -d.y;
						}
					}
					if(ray->vy >= 0.0f){
						if(d.y >= 0.0f){
							d.y = -d.y;
						}
					}
					break;
				case 2:
					if(ray->vz < 0.0f){
						if(d.z < 0.0f){
							d.z = -d.z;
						}
					}
					if(ray->vz >= 0.0f){
						if(d.z >= 0.0f){
							d.z = -d.z;
						}
					}	
					break;
				}
				*ray = rayCreate(p.x,p.y,p.z,d.x,d.y,d.z);
				continue;
				}
			}
			switch(map[block]){
			case 6:{
				VEC2 wall;
				switch(ray->side){
				case 0:
					wall.x = ray->y + (ray->sidex - ray->deltax) * ray->vy;
					wall.y = ray->z + (ray->sidex - ray->deltax) * ray->vz;
					break;
				case 1:
					wall.x = ray->x + (ray->sidey - ray->deltay) * ray->vx;
					wall.y = ray->z + (ray->sidey - ray->deltay) * ray->vz;
					break;
				case 2:
					wall.x = ray->x + (ray->sidez - ray->deltaz) * ray->vx;
					wall.y = ray->y + (ray->sidez - ray->deltaz) * ray->vy;
					break;
				}
				FLOATINTVEC2 fiv2;
				fiv2.x.f = floorf(wall.x);
				fiv2.y.f = floorf(wall.y);
				FLOATINT r;
				r.f = floorf(random(fiv2)*3.0f) + (float)mapdata[block]/255.0;
				red   *= randoms(r);
				r.f+=0.01f;
				green *= randoms(r);
				r.f+=0.01f;
				blue  *= randoms(r);
				continue;
			}
			case 9:
				if(lightmap[block+1] > 255 || lightmap[block+2] > 255 || lightmap[block+3] > 255){
					continue;
				}
				lightmap[block+1] += red;
				lightmap[block+2] += green;
				lightmap[block+3] += blue;
				red   *= 0.95f;
				green *= 0.95f;
				blue  *= 0.95f;
				continue;
			case 12:
			case 4:
				break;
			case 20:
				if(lightmap[block+1] > 255 || lightmap[block+2] > 255 || lightmap[block+3] > 255){
					continue;
				}
				lightmap[block2+1] += red * 0.45f;
				lightmap[block2+2] += green * 0.45f;
				lightmap[block2+3] += blue * 0.45f;
				continue;
			case 21:{
				VEC3 p = vec3fract(vec3div(getSubCoords(ray),properties->lmapSz2));
				if(p.x > 0.45f && p.x < 0.55f){
					float r = map[block+1];
					float g = map[block+2];
					float b = map[block+3];
					float m = fmaxf(r,fmaxf(g,b));
					r /= m;
					g /= m;
					b /= m;
					red   *= r;
					green *= g;
					blue  *= b;
				}
				break;
			}
			case 29:
				red   *= 0.5f;
				green *= 0.05f;
				blue  *= 0.05f;
				break;
			default:
				red   *= 1.0f;
				green *= 1.0f;
				blue  *= 1.0f;
				continue;
			}
		}
		if(lightmap[block2+0] > 255 || lightmap[block2+1] > 255 || lightmap[block2+2] > 255){
			continue;
		}
		lightmap[block2+0] += red;
		lightmap[block2+1] += green;
		lightmap[block2+2] += blue;
	}
}

void updateLight(int pos,float r,float g,float b){
	RAY ray;
	VEC3 bpos;
	bpos.x = (float)(pos%(properties->lvlSz*4)/4)+0.5f;
	bpos.y = (float)(pos / (properties->lvlSz*4) * (properties->lvlSz*4) % (properties->lvlSz*properties->lvlSz*4) / (properties->lvlSz*4*2)) + 0.5f;
	bpos.z = (float)(pos / (properties->lvlSz*properties->lvlSz*4)) + 0.5f;
	bpos.x *= properties->lmapSz2;
	bpos.y *= properties->lmapSz2*2;
	bpos.z *= properties->lmapSz2;
	for(float i = -0.78f; i < 0.78f;i+= 0.003f/properties->lmapSz2){
		for(float i2 = -0.78f; i2 < 0.78f;i2+= 0.003f/properties->lmapSz2){
			float i3 = (float)rand() /RAND_MAX * 6.28f - 3.14f;
			ray = rayCreate(bpos.x,bpos.y,bpos.z,sinf(i3) * cosf(i+i2),cosf(i3) * cosf(i+i2), sinf(i+i2));
			updateLightRay(&ray,r,g,b);
		}
	}
}

void updateLight2(){
	lightmap = HeapAlloc(GetProcessHeap(),8,sizeof(float) * properties->lvlSz*properties->lvlSz*properties->lvlSz*4*properties->lmapSz3);	
	for(int i = 0;i < properties->lvlSz*properties->lvlSz*properties->lvlSz*4;i+=4){
		if(map[i] == 8){
			map[i] = 0;
			updateLight(i+1,(float)(map[i+1])/255.0,(float)(map[i+2])/255.0,(float)(map[i+3])/255.0);
			map[i] = 8;	
		}
	}
	for(unsigned int i = 0;i < properties->lvlSz*properties->lvlSz*properties->lvlSz*4*properties->lmapSz3;i+=4){
		mapdata[i]   = lightmap[i+0];
		mapdata[i+1] = lightmap[i+1];
		mapdata[i+2] = lightmap[i+2];
	}
	HeapFree(GetProcessHeap(),0,lightmap);
	glMes[glMesC].id = 6;
	glMesC++;
}
