#include "main.h"
#include "reflections.h"

VEC3 *reflectmap;

void genReflectMap(){
	for(;;){
		if(lmapC){
			for(u32 i = 0;i < properties->reflectRes;i++){
				for(u32 i2 = 0;i2 < properties->reflectRes;i2++){
					VEC2 px = {player->fov.x*((f32)i2/properties->reflectRes*2.0f-1.0f+1.0f/properties->reflectRes),player->fov.y*((f32)i/properties->reflectRes*2.0f-1.0f+1.0f/properties->reflectRes)};
					VEC3 ang;
					ang.x = (player->xdir * player->xydir - player->xdir * player->zdir * px.y) - player->ydir * px.x;
					ang.y = (player->ydir * player->xydir - player->ydir * player->zdir * px.y) + player->xdir * px.x; 
					ang.z = player->zdir + player->xydir * px.y;
					ang = VEC3normalize(ang);
					RAY ray = rayCreate(player->pos,ang);
					VEC3 colData = {0.0f,0.0f,0.0f};
					VEC3 pPos = player->pos;
					f32 itt = 1.0f;
					rayItterate(&ray);
					while(ray.ix>=0&&ray.ix<properties->lvlSz&&ray.iy>=0&&ray.iy<properties->lvlSz&&ray.iz>=0&&ray.iz<properties->lvlSz){
						u32 block = crds2map(ray.ix,ray.iy,ray.iz);
						switch(map[block].id){
						case BLOCK_AIR:
							break;
						case BLOCK_REFLECTIVE2:
							switch(ray.sid){
							case 0:
								ray.dir.x = -ray.dir.x;
								ray = rayCreate(VEC3addVEC3R((VEC3){ray.ix+1,ray.iy,ray.iz},getSubCoords(ray)),ray.dir);
								break;
							case 1:
								ray.dir.y = -ray.dir.y;
								ray = rayCreate(VEC3addVEC3R((VEC3){ray.ix,ray.iy+1,ray.iz},getSubCoords(ray)),ray.dir);
								break;
							case 2:
								ray.dir.z = -ray.dir.z;
								ray = rayCreate(VEC3addVEC3R((VEC3){ray.ix,ray.iy,ray.iz+1},getSubCoords(ray)),ray.dir);
								break;
							}
							rayItterate(&ray);
							i32 l = getLmapLocation(&ray);
							if(l!=-1){
								reflectmap[i*properties->reflectRes+i2] = VEC3mulR((VEC3){lmap[l].r,lmap[l].g,lmap[l].b},0.002f);
							}
							else{
								reflectmap[i*properties->reflectRes+i2] = (VEC3){0.0f,0.0f,0.0f};
							}
							goto end;
						default:
							goto end;
						}
						rayItterate(&ray); 
					}
				end:
					continue;
				}
			}
		}
		Sleep(15);
	}
}