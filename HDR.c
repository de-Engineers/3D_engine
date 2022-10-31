#include "main.h"
#include "vec2.h"
#include "vec3.h"
#include "ivec2.h"
#include "raytracing.h"

f32 brightness;

#define HDR_RES 60

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
	for(;;){
		if(GetKeyState(VK_LBUTTON)&0x80){
			for(u32 i = 0;i < sliderC;i++){
				if(mousePos.x > slider[i].pos.x - 0.266666667f && mousePos.x < slider[i].pos.x + 0.26666667f
					&& mousePos.y > slider[i].pos.y - 0.015f && mousePos.y < slider[i].pos.y + 0.015f){
					sliderId = slider[i].id;
					sliderPos = (mousePos.x-slider[i].pos.x+0.266666667f)*480.0f;
				}
			}
			if(sliderId!=-1){
				sliders[sliderId](sliderPos);
				sliderId = -1;
				glMes[glMesC].id = 13;
				glMesC++;
			}
		}
		u32 hits = 0;
		f32 polBrightness = 0;
		for(u32 i = 0;i < HDR_RES;i++){
			for(u32 i2 = 0;i2 < HDR_RES;i2++){
				VEC2 px = {player->fov.x*(((f32)i2/HDR_RES*2.0f)-1.0f),player->fov.y*(((f32)i/HDR_RES*2.0f)-1.0f)};
				VEC3 ang;
				ang.x = (player->xdir * player->xydir - player->xdir * player->zdir * px.y) - player->ydir * px.x;
				ang.y = (player->ydir * player->xydir - player->ydir * player->zdir * px.y) + player->xdir * px.x; 
				ang.z = player->zdir + player->xydir * px.y;
				ang = VEC3normalize(ang);
				RAY ray = rayCreate(player->pos,ang);
				rayItterate(&ray);
				while(ray.ix>=0&&ray.ix<properties->lvlSz&&ray.iy>=0&&ray.iy<properties->lvlSz&&ray.iz>=0&&ray.iz<properties->lvlSz){
					u32 block = crds2map(ray.ix,ray.iy,ray.iz);
					switch(map[block].id){ 
					case 9:{
						VEC3 spos = getSubCoords(ray);
						f32 d = iSphere(VEC3subVEC3R(spos,VEC3subR((VEC3){metadt[block].r,metadt[block].g,metadt[block].b},255.0f)),ray.dir,(f32)metadt[block].id/255.0f);
						if(d > 0.0f){
							f32 rcpos = (atan2f(spos.x-0.5f,spos.y-0.5f)+PI)*properties->lmapSz/PI;

							u32 offset = fract(rcpos)*properties->lmapSz;
							f32 minz,maxz;
							maxz = (f32)metadt[block].b/255.0f+(f32)metadt[block].id/255.0f;
							minz = (f32)metadt[block].b/255.0f-(f32)metadt[block].id/255.0f;
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
							polBrightness += max(lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+offset].r,max(lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+offset].g,lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+offset].b));
						}
						}
						goto end;
					case BLOCK_CUBE:{
						VEC3 spos  = getSubCoords(ray);
						VEC3 mtdt  = {(f32)metadt[block].r/255.0f ,(f32)metadt[block].g/255.0f ,(f32)metadt[block].id/255.0f };
						VEC3 mtdt2 = {(f32)metadt2[block].r/255.0f,(f32)metadt2[block].g/255.0f,(f32)metadt2[block].id/255.0f};
						VEC3 mtdt3 = {(f32)metadt3[block].r/255.0f,(f32)metadt3[block].g/255.0f,(f32)metadt3[block].id/255.0f};
						VEC3 angr = ray.dir;
						(VEC2){angr.x,angr.y} = rotVEC2((VEC2){angr.x,angr.y},mtdt3.x*PI);
						(VEC2){angr.x,angr.z} = rotVEC2((VEC2){angr.x,angr.z},mtdt3.y*PI);
						(VEC2){angr.y,angr.z} = rotVEC2((VEC2){angr.y,angr.z},mtdt3.z*PI);
						(VEC2){spos.x,spos.y} = rotVEC2((VEC2){spos.x,spos.y},mtdt3.x*PI);
						(VEC2){spos.x,spos.z} = rotVEC2((VEC2){spos.x,spos.z},mtdt3.y*PI);
						(VEC2){spos.y,spos.z} = rotVEC2((VEC2){spos.y,spos.z},mtdt3.z*PI);
						(VEC2){mtdt.x,mtdt.y} = rotVEC2((VEC2){mtdt.x,mtdt.y},mtdt3.x*PI);
						(VEC2){mtdt.x,mtdt.z} = rotVEC2((VEC2){mtdt.x,mtdt.z},mtdt3.y*PI);
						(VEC2){mtdt.y,mtdt.z} = rotVEC2((VEC2){mtdt.y,mtdt.z},mtdt3.z*PI);
						f32 d = iBox(VEC3subVEC3R(spos,mtdt),angr,mtdt2);
						if(d > 0.0f){
							VEC3addVEC3(&spos,VEC3mulR(angr,d));
							VEC3 nspos = spos;
							(VEC2){nspos.x,nspos.y} = rotVEC2((VEC2){nspos.y,spos.z},-mtdt3.z*PI);
							(VEC2){nspos.x,nspos.z} = rotVEC2((VEC2){nspos.x,spos.z},-mtdt3.y*PI);
							(VEC2){nspos.y,nspos.z} = rotVEC2((VEC2){nspos.x,spos.y},-mtdt3.x*PI);
							if(nspos.x>=0.0f&&nspos.y>=0.0f&&nspos.z>=0.0f&&nspos.x<=1.0f&&nspos.y<=1.0f&&nspos.z<=1.0f){
								if(spos.x>mtdt.x-mtdt2.x-0.0001f&&spos.x<mtdt.x-mtdt2.x+0.0001f){
									f32 mt = fmaxf(mtdt2.y,mtdt2.z)*2.0f;
									IVEC2 offsetb = {(spos.y-mtdt.y+mtdt2.y)*properties->lmapSz/mt,(spos.z-mtdt.z+mtdt2.z)*properties->lmapSz/mt};
									u16 offsetf = offsetb.x + offsetb.y * properties->lmapSz;
									polBrightness += max(lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+offsetf].r,max(lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+offsetf].g,lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+offsetf].b));
								}
								else if(spos.y>mtdt.y-mtdt2.y-0.0001f&&spos.y<mtdt.y-mtdt2.y+0.0001f){
									f32 mt = fmaxf(mtdt2.x,mtdt2.z)*2.0f;
									IVEC2 offsetb = {(spos.x-mtdt.x+mtdt2.x)*properties->lmapSz/mt,(spos.z-mtdt.z+mtdt2.z)*properties->lmapSz/mt};
									u16 offsetf = offsetb.x + offsetb.y * properties->lmapSz;
									polBrightness += max(lmap[lpmap[block].p2*properties->lmapSz*properties->lmapSz+offsetf].r,max(lmap[lpmap[block].p2*properties->lmapSz*properties->lmapSz+offsetf].g,lmap[lpmap[block].p2*properties->lmapSz*properties->lmapSz+offsetf].b));
								}
								else if(spos.z>mtdt.z-mtdt2.z-0.0001f&&spos.z<mtdt.z-mtdt2.z+0.0001f){
									f32 mt = fmaxf(mtdt2.x,mtdt2.y)*2.0f;
									IVEC2 offsetb = {(spos.x-mtdt.x+mtdt2.x)*properties->lmapSz/mt,(spos.y-mtdt.y+mtdt2.y)*properties->lmapSz/mt};
									u16 offsetf = offsetb.x + offsetb.y * properties->lmapSz;
									polBrightness += max(lmap[lpmap[block].p3*properties->lmapSz*properties->lmapSz+offsetf].r,max(lmap[lpmap[block].p3*properties->lmapSz*properties->lmapSz+offsetf].g,lmap[lpmap[block].p3*properties->lmapSz*properties->lmapSz+offsetf].b));
								}
								else if(spos.x>mtdt.x+mtdt2.x-0.0001f&&spos.x<mtdt.x+mtdt2.x+0.0001f){
									f32 mt = fmaxf(mtdt2.y,mtdt2.z)*2.0f;
									IVEC2 offsetb = {(spos.y-mtdt.y+mtdt2.y)*properties->lmapSz/mt,(spos.z-mtdt.z+mtdt2.z)*properties->lmapSz/mt};
									u16 offsetf = offsetb.x + offsetb.y * properties->lmapSz;
									polBrightness += max(lmap[lpmap[block].p4*properties->lmapSz*properties->lmapSz+offsetf].r,max(lmap[lpmap[block].p4*properties->lmapSz*properties->lmapSz+offsetf].g,lmap[lpmap[block].p4*properties->lmapSz*properties->lmapSz+offsetf].b));
								}
								else if(spos.y>mtdt.y+mtdt2.y-0.0001f&&spos.y<mtdt.y+mtdt2.y+0.0001f){
									f32 mt = fmaxf(mtdt2.x,mtdt2.z)*2.0f;
									IVEC2 offsetb = {(spos.x-mtdt.x+mtdt2.x)*properties->lmapSz/mt,(spos.z-mtdt.z+mtdt2.z)*properties->lmapSz/mt};
									u16 offsetf = offsetb.x + offsetb.y * properties->lmapSz;
									polBrightness += max(lmap[lpmap[block].p5*properties->lmapSz*properties->lmapSz+offsetf].r,max(lmap[lpmap[block].p5*properties->lmapSz*properties->lmapSz+offsetf].g,lmap[lpmap[block].p5*properties->lmapSz*properties->lmapSz+offsetf].b));
								}
								else if(spos.z>mtdt.z+mtdt2.z-0.0001f&&spos.z<mtdt.z+mtdt2.z+0.0001f){
									f32 mt = fmaxf(mtdt2.x,mtdt2.y)*2.0f;
									IVEC2 offsetb = {(spos.x-mtdt.x+mtdt2.x)*properties->lmapSz/mt,(spos.y-mtdt.y+mtdt2.y)*properties->lmapSz/mt};
									u16 offsetf = offsetb.x + offsetb.y * properties->lmapSz;
									polBrightness += max(lmap[lpmap[block].p6*properties->lmapSz*properties->lmapSz+offsetf].r,max(lmap[lpmap[block].p6*properties->lmapSz*properties->lmapSz+offsetf].g,lmap[lpmap[block].p6*properties->lmapSz*properties->lmapSz+offsetf].b));
								}
							}
						}
						}
						goto end;
					case BLOCK_SOLID:{
						VEC2 wall;
						u32 offset;
						switch(ray.sid){
						case 0:
							wall.x = fract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y);
							wall.y = fract(ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z);
							offset = (int)(wall.y*properties->lmapSz)*properties->lmapSz+(int)(wall.x*properties->lmapSz);
							if(ray.dir.x>0.0f){
								polBrightness += max(lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+offset].r,max(lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+offset].g,lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+offset].b));
							}
							else{
								polBrightness += max(lmap[lpmap[block].p2*properties->lmapSz*properties->lmapSz+offset].r,max(lmap[lpmap[block].p2*properties->lmapSz*properties->lmapSz+offset].g,lmap[lpmap[block].p2*properties->lmapSz*properties->lmapSz+offset].b));
							}
							break;
						case 1:
							wall.x = fract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x);
							wall.y = fract(ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z);
							offset = (int)(wall.y*properties->lmapSz)*properties->lmapSz+(int)(wall.x*properties->lmapSz);
							if(ray.dir.y>0.0f){
								polBrightness += max(lmap[lpmap[block].p3*properties->lmapSz*properties->lmapSz+offset].r,max(lmap[lpmap[block].p3*properties->lmapSz*properties->lmapSz+offset].g,lmap[lpmap[block].p3*properties->lmapSz*properties->lmapSz+offset].b));
							}
							else{
								polBrightness += max(lmap[lpmap[block].p4*properties->lmapSz*properties->lmapSz+offset].r,max(lmap[lpmap[block].p4*properties->lmapSz*properties->lmapSz+offset].g,lmap[lpmap[block].p4*properties->lmapSz*properties->lmapSz+offset].b));
							}
							break;
						case 2:
							wall.x = fract(ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x);
							wall.y = fract(ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y);
							offset = (int)(wall.y*properties->lmapSz)*properties->lmapSz+(int)(wall.x*properties->lmapSz);
							if(ray.dir.z>0.0f){
								polBrightness += max(lmap[lpmap[block].p5*properties->lmapSz*properties->lmapSz+offset].r,max(lmap[lpmap[block].p5*properties->lmapSz*properties->lmapSz+offset].g,lmap[lpmap[block].p5*properties->lmapSz*properties->lmapSz+offset].b));
							}
							else{
								polBrightness += max(lmap[lpmap[block].p6*properties->lmapSz*properties->lmapSz+offset].r,max(lmap[lpmap[block].p6*properties->lmapSz*properties->lmapSz+offset].g,lmap[lpmap[block].p6*properties->lmapSz*properties->lmapSz+offset].b));
							}
							break;
						}
						goto end;
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
			polBrightness /= hits * 1024.0f / (HDR_RES * HDR_RES);
			brightness = (brightness * 199.0f + polBrightness) / 200.0f;
			brightness = (brightness * 199.0f + 512.0f) / 200.0f;
		}
		Sleep(15);
	}
}