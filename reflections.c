#include "main.h"
#include "reflections.h"
#include "ray.h"

VEC3 *reflectmap;

float tnoise2(VEC2 pos){
	VEC2 fpos = VEC2floorR(pos);
	VEC2 fpos2 = VEC2fractR(pos);
	f32 r1 = VEC2rnd(fpos);
	f32 r2 = VEC2rnd(VEC2addVEC2R(fpos,(VEC2){1.0f,0.0f}));
	f32 r3 = VEC2rnd(VEC2addVEC2R(fpos,(VEC2){0.0f,1.0f}));
	f32 r4 = VEC2rnd(VEC2addVEC2R(fpos,(VEC2){1.0f,1.0f}));
	f32 m1 = mix(r1,r2,fpos2.x);
	f32 m2 = mix(r3,r4,fpos2.x);
	return mix(m1,m2,fpos2.y);
}

void genReflectMap(){
	for(;;){
		if(lmapC){
			for(u32 i = 0;i < properties->reflectRes;i++){
				for(u32 i2 = 0;i2 < properties->reflectRes;i2++){
					VEC2 px = {player->fov.x*((f32)i2/properties->reflectRes*2.0f-1.0f+1.0f/properties->reflectRes),player->fov.y*((f32)i/properties->reflectRes*2.0f-1.0f+1.0f/properties->reflectRes)};
					VEC3 ang = screenUVto3D(px);
					RAY3D ray = ray3dCreate(player->pos,ang);
					ray3dItterate(&ray);
					while(ray.roundPos.x>=0&&ray.roundPos.x<properties->lvlSz&&ray.roundPos.y>=0&&ray.roundPos.y<properties->lvlSz&&ray.roundPos.z>=0&&ray.roundPos.z<properties->lvlSz){
						u32 block = crds2map(ray.roundPos.x,ray.roundPos.y,ray.roundPos.z);
						switch(map[block].id){
						case BLOCK_AIR:
							break;
						case BLOCK_REFLECTIVE2:{
							VEC3 raycrd = getCoords(ray);
							switch(ray.hitSide){
							case 0:{
								if(ray.dir.x > 0.0f){
									ray.roundPos.x-=2;
								}
								ray.dir.x = -ray.dir.x;
								ray = ray3dCreate(raycrd,ray.dir);
								break;
							}
							case 1:{
								if(ray.dir.y > 0.0f){
									ray.roundPos.y-=2;
								}
								ray.dir.y = -ray.dir.y;
								ray = ray3dCreate(raycrd,ray.dir);
								break;
							}
							case 2:{
								if(ray.dir.z > 0.0f){
									ray.roundPos.z-=2;
								}
								ray.dir.z = -ray.dir.z;
								ray = ray3dCreate(raycrd,ray.dir);
								break;
							}
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
						}
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