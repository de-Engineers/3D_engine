#include "main.h"
#include "godrays.h"

VEC3 *godraymap;
VEC3 *godraymapB;

void genGodraysMap(){
	for(;;){
		if(lmapC){
			for(u32 i = 0;i < properties->godrayRes;i++){
				for(u32 i2 = 0;i2 < properties->godrayRes;i2++){
					VEC2 px = {player->fov.x*((f32)i2/properties->godrayRes*2.0f-1.0f+1.0f/properties->godrayRes),player->fov.y*((f32)i/properties->godrayRes*2.0f-1.0f+1.0f/properties->godrayRes)};
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
						VEC3 hPos = VEC3addVEC3R((VEC3){ray.ix,ray.iy,ray.iz},getSubCoords(ray));
						switch(map[block].id){
						case 28:
							goto end;
						default:{
							f32 d = VEC3dist(pPos,hPos);
							colData.r += (f32)lpmap[block].p1 * d;
							colData.g += (f32)lpmap[block].p2 * d;
							colData.b += (f32)lpmap[block].p3 * d;
							f32 b = brightness*0.4f;
							itt+=d*b;
							break;
						}
						}
						pPos = hPos;
						rayItterate(&ray); 
					}
				end:
					VEC3div(&colData,itt/properties->godrayAmm*5000.0f);
					godraymapB[i*properties->godrayRes+i2] = colData;
				}
			}
			for(u32 i = 1;i < properties->godrayRes-1;i++){
				for(u32 i2 = 1;i2 < properties->godrayRes-1;i2++){
					godraymap[i*properties->godrayRes+i2] = godraymapB[i*properties->godrayRes+i2];
					VEC3addVEC3(&godraymap[i*properties->godrayRes+i2],VEC3mulR(godraymapB[i*properties->godrayRes+i2+1],0.25f));
					VEC3addVEC3(&godraymap[i*properties->godrayRes+i2],VEC3mulR(godraymapB[i*properties->godrayRes+i2-1],0.25f));
					VEC3addVEC3(&godraymap[i*properties->godrayRes+i2],VEC3mulR(godraymapB[i*properties->godrayRes+i2+properties->godrayRes],0.25f));
					VEC3addVEC3(&godraymap[i*properties->godrayRes+i2],VEC3mulR(godraymapB[i*properties->godrayRes+i2-properties->godrayRes],0.25f));
					VEC3addVEC3(&godraymap[i*properties->godrayRes+i2],VEC3mulR(godraymapB[i*properties->godrayRes+i2+1+properties->godrayRes],0.125f));
					VEC3addVEC3(&godraymap[i*properties->godrayRes+i2],VEC3mulR(godraymapB[i*properties->godrayRes+i2-1+properties->godrayRes],0.125f));
					VEC3addVEC3(&godraymap[i*properties->godrayRes+i2],VEC3mulR(godraymapB[i*properties->godrayRes+i2+1-properties->godrayRes],0.125f));
					VEC3addVEC3(&godraymap[i*properties->godrayRes+i2],VEC3mulR(godraymapB[i*properties->godrayRes+i2-1-properties->godrayRes],0.125f));
				}
			}
		}
		Sleep(15);
	}
}