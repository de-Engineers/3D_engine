#include "main.h"
#include "vec2.h"
#include "vec3.h"
#include "ivec2.h"
#include "raytracing.h"

f32 brightness;

inline u32 max3(u32 val1,u32 val2,u32 val3){
	if(val1 > val2){
		if(val1 > val3){
			return val1;
		}
		else{
			return val3;
		}
	}
	else if(val2 > val3){
		return val2;
	}
	else{
		return val3;
	}
}

VEC3 getSubCoords(RAY ray){
	VEC2 wall;
    switch(ray.sid){
    case 0:
        wall.x = fract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y);
        wall.y = fract(ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z);
		if(ray.dir.x > 0.0f){
            return (VEC3){0.0f,wall.x,wall.y};
		}
		else{
            return (VEC3){1.0f,wall.x,wall.y};
		}
	case 1:
        wall.x = fract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x);
        wall.y = fract(ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z);
		if(ray.dir.y > 0.0f){
			return (VEC3){wall.x,0.0f,wall.y};
		}
		else{
			return (VEC3){wall.x,1.0f,wall.y};
		}
	case 2:
        wall.x = fract(ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x);
        wall.y = fract(ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y);
		if(ray.dir.z > 0.0f){
            return (VEC3){wall.x,wall.y,0.0f};
		}
		else{
			return (VEC3){wall.x,wall.y,1.0f};
		}
	}
}

void HDR(){
	u32 polBrightness = 0;
	u16 hits = 0;
	for(f32 i = player->xangle - 0.5f;i < player->xangle + 0.5f;i+=0.05f){
		for(f32 i2 = player->yangle - 0.5f;i2 < player->yangle + 0.5f;i2+=0.05f){
			RAY ray = rayCreate(player->pos,(VEC3){cosf(i)*cosf(i2),sinf(i)*cosf(i2),sinf(i2)});
			rayItterate(&ray);
			u8 itt = 1;
			while(ray.ix>=0&&ray.ix<properties->lvlSz&&ray.iy>=0&&ray.iy<properties->lvlSz&&ray.iz>=0&&ray.iz<properties->lvlSz){
				u32 block = crds2map(ray.ix,ray.iy,ray.iz);
				switch(map[block].id){ 
				case 9:{
					VEC3 spos = getSubCoords(ray);
					float d = iSphere(VEC3subVEC3R(spos,VEC3subR((VEC3){metadt[block].r,metadt[block].g,metadt[block].b},255.0f)),ray.dir,(float)metadt[block].id/255.0f);
					if(d > 0.0f){
						f32 rcpos = (atan2f(spos.x-0.5f,spos.y-0.5f)+PI)*properties->lmapSz/PI;

						u32 offset = fract(rcpos)*properties->lmapSz;
						f32 minz,maxz;
						maxz = (float)metadt[block].b/255.0f+(float)metadt[block].id/255.0f;
						minz = (float)metadt[block].b/255.0f-(float)metadt[block].id/255.0f;
						if(maxz > 1.0f){
							maxz = 1.0f;
						}
						if(minz < 0.0f){
							minz = 0.0f;
						}
						f32 rspos = spos.z - minz;
						rspos /= maxz;
						rspos *= 2.0f;
						offset += (u32)(fract(rspos)*properties->lmapSz)*properties->lmapSz;
						offset += (u32)rspos*properties->lmapSz*properties->lmapSz*2;
						polBrightness += max3(lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+offset].r,lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+offset].g,lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+offset].b);
						goto end;
					}
					}
					break;
				case BLOCK_CUBE:{
					VEC3 spos  = getSubCoords(ray);
					VEC3 mtdt  = {(f32)metadt[block].r/255.0f ,(f32)metadt[block].g/255.0f ,(f32)metadt[block].id/255.0f };
					VEC3 mtdt2 = {(f32)metadt2[block].r/255.0f,(f32)metadt2[block].g/255.0f,(f32)metadt2[block].id/255.0f};
					VEC3 mtdt3 = {(f32)metadt3[block].r/255.0f,(f32)metadt3[block].g/255.0f,(f32)metadt3[block].id/255.0f};
					VEC3 angr = ray.dir;
					VEC2 temp;
					temp = rotVEC2((VEC2){angr.x,angr.y},mtdt3.x*PI);
					angr.x = temp.x;
					angr.y = temp.y;
					temp = rotVEC2((VEC2){angr.x,angr.z},mtdt3.y*PI);
					angr.x = temp.x;
					angr.z = temp.y;
					temp = rotVEC2((VEC2){angr.y,angr.z},mtdt3.z*PI);
					angr.y = temp.x;
					angr.z = temp.y;
					temp = rotVEC2((VEC2){spos.x,spos.y},mtdt3.x*PI);
					spos.x = temp.x;
					spos.y = temp.y;
					temp = rotVEC2((VEC2){spos.x,spos.z},mtdt3.y*PI);
					spos.x = temp.x;
					spos.z = temp.y;
					temp = rotVEC2((VEC2){spos.y,spos.z},mtdt3.z*PI);
					spos.y = temp.x;
					spos.z = temp.y;
					temp = rotVEC2((VEC2){mtdt.x,mtdt.y},mtdt3.x*PI);
					mtdt.x = temp.x;
					mtdt.y = temp.y;
					temp = rotVEC2((VEC2){mtdt.x,mtdt.z},mtdt3.y*PI);
					mtdt.x = temp.x;
					mtdt.z = temp.y;
					temp = rotVEC2((VEC2){mtdt.y,mtdt.z},mtdt3.z*PI);
					mtdt.y = temp.x;
					mtdt.z = temp.y;
					f32 d = iBox(VEC3subVEC3R(spos,mtdt),angr,mtdt2);
					if(d > 0.0f){
						VEC3addVEC3(&spos,VEC3mulR(angr,d));
						VEC3 nspos = spos;
						temp = rotVEC2((VEC2){nspos.y,spos.z},-mtdt3.z*PI);
						nspos.x = temp.x;
						nspos.y = temp.y;
						temp = rotVEC2((VEC2){nspos.x,spos.z},-mtdt3.y*PI);
						nspos.x = temp.x;
						nspos.z = temp.y;
						temp = rotVEC2((VEC2){nspos.x,spos.y},-mtdt3.x*PI);
						nspos.y = temp.x;
						nspos.z = temp.y;
						if(nspos.x>=0.0f&&nspos.y>=0.0f&&nspos.z>=0.0f&&nspos.x<=1.0f&&nspos.y<=1.0f&&nspos.z<=1.0f){
							if(spos.x>mtdt.x-mtdt2.x-0.0001f&&spos.x<mtdt.x-mtdt2.x+0.0001f){
								f32 mt = fmaxf(mtdt2.y,mtdt2.z)*2.0f;
								IVEC2 offsetb = {(spos.y-mtdt.y+mtdt2.y)*properties->lmapSz/mt,(spos.z-mtdt.z+mtdt2.z)*properties->lmapSz/mt};
								u16 offsetf = offsetb.x + offsetb.y * properties->lmapSz;
								polBrightness += max3(lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+offsetf].r,lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+offsetf].g,lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+offsetf].b);
							}
							if(spos.y>mtdt.y-mtdt2.y-0.0001f&&spos.y<mtdt.y-mtdt2.y+0.0001f){
								f32 mt = fmaxf(mtdt2.x,mtdt2.z)*2.0f;
								IVEC2 offsetb = {(spos.x-mtdt.x+mtdt2.x)*properties->lmapSz/mt,(spos.z-mtdt.z+mtdt2.z)*properties->lmapSz/mt};
								u16 offsetf = offsetb.x + offsetb.y * properties->lmapSz;
								polBrightness += max3(lmap[lpmap[block].p2*properties->lmapSz*properties->lmapSz+offsetf].r,lmap[lpmap[block].p2*properties->lmapSz*properties->lmapSz+offsetf].g,lmap[lpmap[block].p2*properties->lmapSz*properties->lmapSz+offsetf].b);
							}
							if(spos.z>mtdt.z-mtdt2.z-0.0001f&&spos.z<mtdt.z-mtdt2.z+0.0001f){
								f32 mt = fmaxf(mtdt2.x,mtdt2.y)*2.0f;
								IVEC2 offsetb = {(spos.x-mtdt.x+mtdt2.x)*properties->lmapSz/mt,(spos.y-mtdt.y+mtdt2.y)*properties->lmapSz/mt};
								u16 offsetf = offsetb.x + offsetb.y * properties->lmapSz;
								polBrightness += max3(lmap[lpmap[block].p3*properties->lmapSz*properties->lmapSz+offsetf].r,lmap[lpmap[block].p3*properties->lmapSz*properties->lmapSz+offsetf].g,lmap[lpmap[block].p3*properties->lmapSz*properties->lmapSz+offsetf].b);
							}
							if(spos.x>mtdt.x+mtdt2.x-0.0001f&&spos.x<mtdt.x+mtdt2.x+0.0001f){
								f32 mt = fmaxf(mtdt2.y,mtdt2.z)*2.0f;
								IVEC2 offsetb = {(spos.y-mtdt.y+mtdt2.y)*properties->lmapSz/mt,(spos.z-mtdt.z+mtdt2.z)*properties->lmapSz/mt};
								u16 offsetf = offsetb.x + offsetb.y * properties->lmapSz;
								polBrightness += max3(lmap[lpmap[block].p4*properties->lmapSz*properties->lmapSz+offsetf].r,lmap[lpmap[block].p4*properties->lmapSz*properties->lmapSz+offsetf].g,lmap[lpmap[block].p4*properties->lmapSz*properties->lmapSz+offsetf].b);
							}
							if(spos.y>mtdt.y+mtdt2.y-0.0001f&&spos.y<mtdt.y+mtdt2.y+0.0001f){
								f32 mt = fmaxf(mtdt2.x,mtdt2.z)*2.0f;
								IVEC2 offsetb = {(spos.x-mtdt.x+mtdt2.x)*properties->lmapSz/mt,(spos.z-mtdt.z+mtdt2.z)*properties->lmapSz/mt};
								u16 offsetf = offsetb.x + offsetb.y * properties->lmapSz;
								polBrightness += max3(lmap[lpmap[block].p5*properties->lmapSz*properties->lmapSz+offsetf].r,lmap[lpmap[block].p5*properties->lmapSz*properties->lmapSz+offsetf].g,lmap[lpmap[block].p5*properties->lmapSz*properties->lmapSz+offsetf].b);
							}
							if(spos.z>mtdt.z+mtdt2.z-0.0001f&&spos.z<mtdt.z+mtdt2.z+0.0001f){
								f32 mt = fmaxf(mtdt2.x,mtdt2.y)*2.0f;
								IVEC2 offsetb = {(spos.x-mtdt.x+mtdt2.x)*properties->lmapSz/mt,(spos.y-mtdt.y+mtdt2.y)*properties->lmapSz/mt};
								u16 offsetf = offsetb.x + offsetb.y * properties->lmapSz;
								polBrightness += max3(lmap[lpmap[block].p6*properties->lmapSz*properties->lmapSz+offsetf].r,lmap[lpmap[block].p6*properties->lmapSz*properties->lmapSz+offsetf].g,lmap[lpmap[block].p6*properties->lmapSz*properties->lmapSz+offsetf].b);
							}
							goto end;
						}
					}
					break;
					}
				case BLOCK_REFLECTIVE:
				case BLOCK_SOLID:{
					VEC2 wall;
					switch(ray.sid){
					case 0:{
						wall.x = fract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y);
						wall.y = fract(ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z);
						u32 offset = (int)(wall.y*properties->lmapSz)*properties->lmapSz+(int)(wall.x*properties->lmapSz);
						if(ray.dir.x>0.0f){
							polBrightness += max3(lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+offset].r,lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+offset].g,lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+offset].b);
						}
						else{
							polBrightness += max3(lmap[lpmap[block].p2*properties->lmapSz*properties->lmapSz+offset].r,lmap[lpmap[block].p2*properties->lmapSz*properties->lmapSz+offset].g,lmap[lpmap[block].p2*properties->lmapSz*properties->lmapSz+offset].b);
						}
						goto end;
					}
					case 1:{
						wall.x = fract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x);
						wall.y = fract(ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z);
						u32 offset = (int)(wall.y*properties->lmapSz)*properties->lmapSz+(int)(wall.x*properties->lmapSz);
						if(ray.dir.y>0.0f){
							polBrightness += max3(lmap[lpmap[block].p3*properties->lmapSz*properties->lmapSz+offset].r,lmap[lpmap[block].p3*properties->lmapSz*properties->lmapSz+offset].g,lmap[lpmap[block].p3*properties->lmapSz*properties->lmapSz+offset].b);
						}
						else{
							polBrightness += max3(lmap[lpmap[block].p4*properties->lmapSz*properties->lmapSz+offset].r,lmap[lpmap[block].p4*properties->lmapSz*properties->lmapSz+offset].g,lmap[lpmap[block].p4*properties->lmapSz*properties->lmapSz+offset].b);
						}
						goto end;
					}
					case 2:{
						wall.x = fract(ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x);
						wall.y = fract(ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y);
						u32 offset = (int)(wall.y*properties->lmapSz)*properties->lmapSz+(int)(wall.x*properties->lmapSz);
						if(ray.dir.z>0.0f){
							polBrightness += max3(lmap[lpmap[block].p5*properties->lmapSz*properties->lmapSz+offset].r,lmap[lpmap[block].p5*properties->lmapSz*properties->lmapSz+offset].g,lmap[lpmap[block].p5*properties->lmapSz*properties->lmapSz+offset].b);
						}
						else{
							polBrightness += max3(lmap[lpmap[block].p6*properties->lmapSz*properties->lmapSz+offset].r,lmap[lpmap[block].p6*properties->lmapSz*properties->lmapSz+offset].g,lmap[lpmap[block].p6*properties->lmapSz*properties->lmapSz+offset].b);
						}
						goto end;
						}
					}
					}
				default:
					break;
				}
				itt++;
				rayItterate(&ray);
			}
			continue;
		end:
			hits++;
			continue;
		}
	}
	if(hits){
		polBrightness /= hits;
		brightness = (brightness * 99 + polBrightness) / 100;
		brightness = (brightness * 49 + 512) / 50;
	}

}