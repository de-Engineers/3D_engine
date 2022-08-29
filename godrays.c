#include "main.h"

RGBA *godraymap;

typedef struct{
	u64 r;
	u64 g;
	u64 b;
	u64 a;
}IVEC4;

void genGodraysMap(){
	/*
	for(;;){
		if(lmapC){
			for(u32 i = 0;i < godraySz;i++){
				for(u32 i2 = 0;i2 < godraySz;i2++){
					VEC2 px = {player->xfov*(((f32)i2/godraySz*2.0f)-1.0f),player->yfov*(((f32)i/godraySz*2.0f)-1.0f)};
					VEC3 ang;
					ang.x = (player->xdir * player->xydir - player->xdir * player->zdir * px.y) - player->ydir * px.x;
					ang.y = (player->ydir * player->xydir - player->ydir * player->zdir * px.y) + player->xdir * px.x; 
					ang.z = player->zdir + player->xydir * px.y;
					ang = VEC3normalize(ang);
					RAY ray = rayCreate((VEC3){player->xpos,player->ypos,player->zpos},ang);
					IVEC4 colData = {0,0,0,1};
					f32 itt = 1.0f;
					rayItterate(&ray);
					while(ray.ix>=0&&ray.ix<properties->lvlSz&&ray.iy>=0&&ray.iy<properties->lvlSz&&ray.iz>=0&&ray.iz<properties->lvlSz){
						u32 block = crds2map(ray.ix,ray.iy,ray.iz);
						switch(map[block].id){
						case 28:
							goto end;
						default:
							colData.r += lpmap[block].p1;
							colData.g += lpmap[block].p2;
							colData.b += lpmap[block].p3;
							itt+=25000.0f/brightness/brightness;
							break;
						}
						rayItterate(&ray);
					}
				end:
					godraymap[i*godraySz+i2].r = min(colData.r/itt/brightness,255);
					godraymap[i*godraySz+i2].g = min(colData.g/itt/brightness,255);
					godraymap[i*godraySz+i2].b = min(colData.b/itt/brightness,255);
					godraymap[i*godraySz+i2].a = min(itt,255);
				}
			}
		}
		Sleep(15);
	}
	*/
}