#include <math.h>
#include <stdio.h>

#include "main.h"
#include "ray.h"

u32 turretC;
TURRET *turret;

CVEC3 map2crds(u32 map){
	CVEC3 m;
	m.x = map % properties->lvlSz;
	m.y = map / properties->lvlSz % properties->lvlSz;
	m.z = map / properties->lvlSz / properties->lvlSz;
	return m;
}

void ittmap(){
	for(;;){
		moveBlocks();
		if(settings & 0x100){
			for(u32 i = 0;i < turretC;i++){
				switch(turret[i].id){
				case 0:
					if(turret[i].cooldown){
						turret[i].cooldown--;
					}
					else{
						RAY3D ray = ray3dCreate(player->pos,VEC3subVEC3R(turret[i].pos,player->pos));
						ray3dItterate(&ray);
						while(ray.roundPos.x>=0&&ray.roundPos.x<properties->lvlSz&&ray.roundPos.y>=0&&ray.roundPos.y<properties->lvlSz&&ray.roundPos.z>=0&&ray.roundPos.z<properties->lvlSz){
							u32 block = crds2map(ray.roundPos.x,ray.roundPos.y,ray.roundPos.z);
							if((u32)turret[i].pos.x == ray.roundPos.x && (u32)turret[i].pos.y == ray.roundPos.y && (u32)turret[i].pos.z == ray.roundPos.z){
								spawnEntity(turret[i].pos,VEC3divR(VEC3normalize(VEC3subVEC3R(player->pos,turret[i].pos)),turret[i].power+1),0);
								turret[i].cooldown = turret[i].totalCooldown * 3 + 30;
								break;
							}
							if(map[block].id!=BLOCK_AIR){
								break;
							}
							ray3dItterate(&ray);
						}
					}
					break;
				case 1:
					if(turret[i].cooldown){
						turret[i].cooldown--;
					}
					else{
						RAY3D ray = ray3dCreate(player->pos,VEC3subVEC3R(turret[i].pos,(VEC3){player->pos.x,player->pos.y,player->pos.z-0.35f}));
						ray3dItterate(&ray);
						while(ray.roundPos.x>=0&&ray.roundPos.x<properties->lvlSz&&ray.roundPos.y>=0&&ray.roundPos.y<properties->lvlSz&&ray.roundPos.z>=0&&ray.roundPos.z<properties->lvlSz){
							u32 block = crds2map(ray.roundPos.x,ray.roundPos.y,ray.roundPos.z);
							if((u32)turret[i].pos.x == ray.roundPos.x && (u32)turret[i].pos.y == ray.roundPos.y && (u32)turret[i].pos.z == ray.roundPos.z){
								ray = ray3dCreate(turret[i].pos,VEC3subVEC3R((VEC3){player->pos.x,player->pos.y,player->pos.z-0.35f},turret[i].pos));
								ray3dItterate(&ray);
								while(ray.roundPos.x>=0&&ray.roundPos.x<properties->lvlSz&&ray.roundPos.y>=0&&ray.roundPos.y<properties->lvlSz&&ray.roundPos.z>=0&&ray.roundPos.z<properties->lvlSz){
									block = crds2map(ray.roundPos.x,ray.roundPos.y,ray.roundPos.z);
									if(map[block].id!=BLOCK_AIR){
										spawnEntityEx(turret[i].pos,VEC3subVEC3R(getCoords(ray),turret[i].pos),(VEC3){0.0f,0.0f,0.0f},1,(VEC3){0.4f,0.4f,0.04f});
										turret[i].cooldown = turret[i].totalCooldown * 3 + 180;
										goto end;
									}
									ray3dItterate(&ray);
								}
								spawnEntityEx(turret[i].pos,VEC3subVEC3R(getCoords(ray),turret[i].pos),(VEC3){0.0f,0.0f,0.0f},1,(VEC3){0.4f,0.4f,0.04f});
								turret[i].cooldown = turret[i].totalCooldown * 3 + 180;
							end:
								break;
							}
							if(map[block].id!=BLOCK_AIR){
								break;
							}
							ray3dItterate(&ray);
						}
					}
					break;
				case 2:
					if(!turret[i].cooldown){
						RAY3D ray = ray3dCreate(player->pos,VEC3subVEC3R(turret[i].pos,(VEC3){player->pos.x,player->pos.y,player->pos.z-0.35f}));
						ray3dItterate(&ray);
						while(ray.roundPos.x>=0&&ray.roundPos.x<properties->lvlSz&&ray.roundPos.y>=0&&ray.roundPos.y<properties->lvlSz&&ray.roundPos.z>=0&&ray.roundPos.z<properties->lvlSz){
							u32 block = crds2map(ray.roundPos.x,ray.roundPos.y,ray.roundPos.z);
							if((u32)turret[i].pos.x == ray.roundPos.x && (u32)turret[i].pos.y == ray.roundPos.y && (u32)turret[i].pos.z == ray.roundPos.z){
								switch(turret[i].pickupId){
								case 1:
									spawnEntityEx(VEC3subVEC3R(turret[i].pos,(VEC3){0.25f,0.0f,0.0f}),(VEC3){0.5f,0.0f,0.0f},(VEC3){0.0f,0.0f,0.0f},3,(VEC3){0.01f,0.1f,0.01f});
									break;
								case 2:
									spawnEntity(turret[i].pos,(VEC3){0.0f,0.0f,0.0f},5);
									break;
								case 3:
									spawnEntityEx(VEC3subVEC3R(turret[i].pos,(VEC3){0.25f,0.0f,0.0f}),(VEC3){0.5f,0.0f,0.0f},(VEC3){0.0f,0.0f,0.0f},7,(VEC3){0.1f,0.01f,0.01f});
									break;
								case 4:
									spawnEntity(turret[i].pos,(VEC3){0.0f,0.0f,0.0f},8);
									break;
								}
								turret[i].cooldown = 1;
								break;
							}
							if(map[block].id!=BLOCK_AIR){
								break;
							}
							ray3dItterate(&ray);
						}
						break;
					}
					break;
				}
			}
		}
		Sleep(15);
	}
}
