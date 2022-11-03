#include "main.h"
#include "godrays.h"
#include "ray.h"

VEC3 *godraymap;
VEC3 *godraymapB;

void genGodraysMap(){
	for(;;){
		if(lmapC){
			for(u32 i = 0;i < properties->godrayRes;i++){
				for(u32 i2 = 0;i2 < properties->godrayRes;i2++){
					VEC2 px = {player->fov.x*((f32)i2/properties->godrayRes*2.0f-1.0f+1.0f/properties->godrayRes),player->fov.y*((f32)i/properties->godrayRes*2.0f-1.0f+1.0f/properties->godrayRes)};
					VEC3 ang = screenUVto3D(px);
					RAY3D ray = ray3dCreate(player->pos,ang);
					VEC3 colData = {0.0f,0.0f,0.0f};
					VEC3 pPos = player->pos;
					f32 itt = 1.0f;
					ray3dItterate(&ray);
					while(ray.roundPos.x>=0&&ray.roundPos.x<properties->lvlSz&&ray.roundPos.y>=0&&ray.roundPos.y<properties->lvlSz&&ray.roundPos.z>=0&&ray.roundPos.z<properties->lvlSz){
						u32 block = crds2map(ray.roundPos.x,ray.roundPos.y,ray.roundPos.z);
						VEC3 hPos = getCoords(ray);
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
						ray3dItterate(&ray); 
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