#include "main.h"
#include "reflections.h"
#include "ray.h"

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
					RAY3D ray = ray3dCreate(player->pos,ang);
					ray3dItterate(&ray);
					while(ray.roundPos.x>=0&&ray.roundPos.x<properties->lvlSz&&ray.roundPos.y>=0&&ray.roundPos.y<properties->lvlSz&&ray.roundPos.z>=0&&ray.roundPos.z<properties->lvlSz){
						u32 block = crds2map(ray.roundPos.x,ray.roundPos.y,ray.roundPos.z);
						switch(map[block].id){
						case BLOCK_AIR:
							break;
						case BLOCK_REFLECTIVE2:
							switch(ray.hitSide){
							case 0:
								ray.dir.x = -ray.dir.x;
								ray = ray3dCreate(VEC3addVEC3R((VEC3){ray.roundPos.x+1,ray.roundPos.y,ray.roundPos.z},getSubCoords(ray)),ray.dir);
								break;
							case 1:
								ray.dir.y = -ray.dir.y;
								ray = ray3dCreate(VEC3addVEC3R((VEC3){ray.roundPos.x,ray.roundPos.y+1,ray.roundPos.z},getSubCoords(ray)),ray.dir);
								break;
							case 2:
								ray.dir.z = -ray.dir.z;
								ray = ray3dCreate(VEC3addVEC3R((VEC3){ray.roundPos.x,ray.roundPos.y,ray.roundPos.z+1},getSubCoords(ray)),ray.dir);
								break;
							}
							ray3dItterate(&ray);
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
						ray3dItterate(&ray); 
					}
				end:
					continue;
				}
			}
		}
		Sleep(15);
	}
}