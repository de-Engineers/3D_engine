#include <math.h>
#include <stdio.h>

#include "main.h"

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
						RAY ray = rayCreate(player->pos,VEC3subVEC3R(turret[i].pos,player->pos));
						rayItterate(&ray);
						while(ray.ix>=0&&ray.ix<properties->lvlSz&&ray.iy>=0&&ray.iy<properties->lvlSz&&ray.iz>=0&&ray.iz<properties->lvlSz){
							u32 block = crds2map(ray.ix,ray.iy,ray.iz);
							if((u32)turret[i].pos.x == ray.ix && (u32)turret[i].pos.y == ray.iy && (u32)turret[i].pos.z == ray.iz){
								spawnEntity(turret[i].pos,VEC3divR(VEC3normalize(VEC3subVEC3R(player->pos,turret[i].pos)),turret[i].power+1),0);
								turret[i].cooldown = turret[i].totalCooldown * 3 + 30;
								break;
							}
							if(map[block].id!=BLOCK_AIR){
								break;
							}
							rayItterate(&ray);
						}
					}
					break;
				case 1:
					if(turret[i].cooldown){
						turret[i].cooldown--;
					}
					else{
						RAY ray = rayCreate(player->pos,VEC3subVEC3R(turret[i].pos,(VEC3){player->pos.x,player->pos.y,player->pos.z-0.35f}));
						rayItterate(&ray);
						while(ray.ix>=0&&ray.ix<properties->lvlSz&&ray.iy>=0&&ray.iy<properties->lvlSz&&ray.iz>=0&&ray.iz<properties->lvlSz){
							u32 block = crds2map(ray.ix,ray.iy,ray.iz);
							if((u32)turret[i].pos.x == ray.ix && (u32)turret[i].pos.y == ray.iy && (u32)turret[i].pos.z == ray.iz){
								ray = rayCreate(turret[i].pos,VEC3subVEC3R((VEC3){player->pos.x,player->pos.y,player->pos.z-0.35f},turret[i].pos));
								rayItterate(&ray);
								while(ray.ix>=0&&ray.ix<properties->lvlSz&&ray.iy>=0&&ray.iy<properties->lvlSz&&ray.iz>=0&&ray.iz<properties->lvlSz){
									block = crds2map(ray.ix,ray.iy,ray.iz);
									if(map[block].id!=BLOCK_AIR){
										spawnEntityEx(turret[i].pos,VEC3subVEC3R(getCoords(ray),turret[i].pos),(VEC3){0.0f,0.0f,0.0f},1,(VEC3){0.4f,0.4f,0.04f});
										turret[i].cooldown = turret[i].totalCooldown * 3 + 180;
										goto end;
									}
									rayItterate(&ray);
								}
								spawnEntityEx(turret[i].pos,VEC3subVEC3R(getCoords(ray),turret[i].pos),(VEC3){0.0f,0.0f,0.0f},1,(VEC3){0.4f,0.4f,0.04f});
								turret[i].cooldown = turret[i].totalCooldown * 3 + 180;
							end:
								break;
							}
							if(map[block].id!=BLOCK_AIR){
								break;
							}
							rayItterate(&ray);
						}
					}
					break;
				case 2:
					if(!turret[i].cooldown){
						RAY ray = rayCreate(player->pos,VEC3subVEC3R(turret[i].pos,(VEC3){player->pos.x,player->pos.y,player->pos.z-0.35f}));
						rayItterate(&ray);
						while(ray.ix>=0&&ray.ix<properties->lvlSz&&ray.iy>=0&&ray.iy<properties->lvlSz&&ray.iz>=0&&ray.iz<properties->lvlSz){
							u32 block = crds2map(ray.ix,ray.iy,ray.iz);
							if((u32)turret[i].pos.x == ray.ix && (u32)turret[i].pos.y == ray.iy && (u32)turret[i].pos.z == ray.iz){
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
							rayItterate(&ray);
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
