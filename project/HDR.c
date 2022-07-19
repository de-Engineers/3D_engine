#include "main.h"
#include "vec2.h"
#include "vec3.h"

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
	
inline float fract(float p){
	return p - (int)p;
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

f32 iSphere(VEC3 ro,VEC3 rd,f32 ra){
    f32 b = VEC3dot(ro,rd);
    f32 c = VEC3dot(ro,ro) - ra*ra;
    f32 h = b*b - c;
    if(h<0.0){
        return -1.0;
    }
    return -b-sqrtf(h);
}

void HDR(){
	u32 polBrightness = 0;
	u16 hits = 0;
	for(f32 i = player->xangle - 0.5f;i < player->xangle + 0.5f;i+=0.01f){
		for(f32 i2 = player->yangle - 0.5f;i2 < player->yangle + 0.5f;i2+=0.01f){
			RAY ray = rayCreate((VEC3){player->xpos,player->ypos,player->zpos},(VEC3){cosf(i)*cosf(i2),sinf(i)*cosf(i2),sinf(i2)});
			while(ray.ix>=0&&ray.ix<properties->lvlSz&&ray.iy>=0&&ray.iy<properties->lvlSz&&ray.iz>=0&&ray.iz<properties->lvlSz){
				u32 block = crds2map(ray.ix,ray.iy,ray.iz);
				switch(map[block].id){ 
				case 0:
					break;
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
				default:{
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
				}
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
		if(brightness < 70.0f){
			brightness = 70.0f;
		}
	}

}