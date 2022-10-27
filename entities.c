#include <stdio.h>
#include <math.h>

#include "main.h"
#include "network.h"
#include "ivec3.h"
#include "raytracing.h"

#define LUMINANCESAMPLECOUNT 16

char sprite;
u32 entityC;
ENTITY entity;

RGB *entityTexture;

i32 getLmapLocation(RAY *ray){
	while(ray->ix >= 0 && ray->iy >= 0 && ray->iz >= 0 && ray->ix < properties->lvlSz && ray->iy < properties->lvlSz && ray->iz < properties->lvlSz){
		u32 block = crds2map(ray->ix,ray->iy,ray->iz);
		switch(map[block].id){
		case BLOCK_REFLECTIVE:
		case BLOCK_SOLID:{
			VEC2 wall;
			switch(ray->sid){
			case 0:{
				wall.x = fract(ray->pos.y + (ray->side.x - ray->delta.x) * ray->dir.y);
				wall.y = fract(ray->pos.z + (ray->side.x - ray->delta.x) * ray->dir.z);
				u32 offset = (int)(wall.y*properties->lmapSz)*properties->lmapSz+(int)(wall.x*properties->lmapSz);
				if(ray->dir.x>0.0f){
					return lpmap[block].p1*properties->lmapSz*properties->lmapSz+offset;
				}
				else{
					return lpmap[block].p2*properties->lmapSz*properties->lmapSz+offset;
				}
				break;
				}
			case 1:{
				wall.x = fract(ray->pos.x + (ray->side.y - ray->delta.y) * ray->dir.x);
				wall.y = fract(ray->pos.z + (ray->side.y - ray->delta.y) * ray->dir.z);
				u32 offset = (int)(wall.y*properties->lmapSz)*properties->lmapSz+(int)(wall.x*properties->lmapSz);
				if(ray->dir.y>0.0f){
					return lpmap[block].p3*properties->lmapSz*properties->lmapSz+offset;
				}
				else{
					return lpmap[block].p4*properties->lmapSz*properties->lmapSz+offset;
				}
				break;
			}
			case 2:{	 
				wall.x = fract(ray->pos.x + (ray->side.z - ray->delta.z) * ray->dir.x);
				wall.y = fract(ray->pos.y + (ray->side.z - ray->delta.z) * ray->dir.y);
				u32 offset = (int)(wall.y*properties->lmapSz)*properties->lmapSz+(int)(wall.x*properties->lmapSz);
				if(ray->dir.z>0.0f){
					return lpmap[block].p5*properties->lmapSz*properties->lmapSz+offset;
				}
				else{
					return lpmap[block].p6*properties->lmapSz*properties->lmapSz+offset;
				}
				break;
			}
			}
		}
		}
		rayItterate(ray);
	}
	return -1;
}

void spawnPlayer(u8 id){
	for(u32 i = 0;i < ENTITYTEXTSZ;i++){
		for(u32 i2 = 0;i2 < ENTITYTEXTSZ;i2++){
			u8 b = (irnd() & 0xff) / 2 + 127;
			entityTexture[entityC*ENTITYTEXTSZ*ENTITYTEXTSZ+i*ENTITYTEXTSZ+i2] = (RGB){b,b,b};
		}
	}
		
	entityTexture[entityC*ENTITYTEXTSZ*ENTITYTEXTSZ+26*ENTITYTEXTSZ+23] = (RGB){255,0,0};
	entityTexture[entityC*ENTITYTEXTSZ*ENTITYTEXTSZ+26*ENTITYTEXTSZ+25] = (RGB){255,0,0};

	entityTexture[entityC*ENTITYTEXTSZ*ENTITYTEXTSZ+24*ENTITYTEXTSZ+22] = (RGB){255,0,0};
	entityTexture[entityC*ENTITYTEXTSZ*ENTITYTEXTSZ+23*ENTITYTEXTSZ+23] = (RGB){255,0,0};
	entityTexture[entityC*ENTITYTEXTSZ*ENTITYTEXTSZ+23*ENTITYTEXTSZ+24] = (RGB){255,0,0};
	entityTexture[entityC*ENTITYTEXTSZ*ENTITYTEXTSZ+23*ENTITYTEXTSZ+25] = (RGB){255,0,0};
	entityTexture[entityC*ENTITYTEXTSZ*ENTITYTEXTSZ+24*ENTITYTEXTSZ+26] = (RGB){255,0,0};

	entity.gpu[entityC].rad = 0.2f;
	entity.gpu[entityC].pos2 = (VEC3){0.0f,0.0f,-1.8f};
	entity.gpu[entityC].id = 3;
	entity.cpu[entityC].id = 9;
	entity.gpu[entityC].color = (VEC3){0.5f,0.5f,0.5f};
	entity.cpu[entityC].baseColor = (VEC3){0.5f,0.5f,0.5f};
	entity.cpu[entityC].playerid = id;
	entity.gpu[entityC].tId = entityC;
	entityC++;
	glMes[glMesC].id = 12;
	glMesC++;
}

void spawnEntityEx(VEC3 pos,VEC3 pos2,VEC3 vel,u8 id,VEC3 color){
	entity.cpu[entityC].id = id;
	entity.gpu[entityC].pos = pos;
	entity.cpu[entityC].vel = vel;
	entity.gpu[entityC].color = color;
	entity.cpu[entityC].baseColor = color;
	entity.gpu[entityC].tId = entityC;
	switch(id){
	case 1:
		entity.gpu[entityC].rad = 0.05f;
		entity.gpu[entityC].pos2 = pos2;
		entity.cpu[entityC].health = 60;
		entity.gpu[entityC].id = 1;
		break;
	case 2:
		entity.gpu[entityC].rad = 0.05f;
		entity.gpu[entityC].pos2 = pos2;
		entity.cpu[entityC].health = 120;
		entity.gpu[entityC].id = 1;
		break;
	case 3:
		entity.gpu[entityC].rad = 0.1f;
		entity.gpu[entityC].pos2 = pos2;
		entity.cpu[entityC].pos = VEC3addVEC3R(pos,(VEC3){0.25f,0.0f,0.0f});
		entity.gpu[entityC].id = 2;
		break;
	case 4:
		entity.gpu[entityC].rad = 0.02f;
		entity.gpu[entityC].pos2 = pos2;
		entity.cpu[entityC].pos = VEC3addVEC3R(pos,(VEC3){0.25f,0.0f,0.0f});
		entity.gpu[entityC].id = 2;
		entity.cpu[entityC].aniType = 1;
		entity.cpu[entityC].aniTime = 15;
		break;
	case 7:
		entity.gpu[entityC].rad = 0.1f;
		entity.gpu[entityC].pos2 = pos2;
		entity.cpu[entityC].pos = VEC3addVEC3R(pos,(VEC3){0.25f,0.0f,0.0f});
		entity.gpu[entityC].id = 2;
		break;
	case 10:
		entity.gpu[entityC].rad = 0.05f;
		entity.gpu[entityC].pos2 = pos2;
		entity.cpu[entityC].health = 5;
		entity.gpu[entityC].id = 1;
		break;
	}
	entityC++;
}

void spawnEntity(VEC3 pos,VEC3 vel,u8 id){
	entity.cpu[entityC].id = id;
	entity.gpu[entityC].pos = pos;
	entity.cpu[entityC].vel = vel;
	entity.gpu[entityC].tId = entityC;
	switch(id){
	case 0:
		entity.gpu[entityC].id = 0;
		entity.gpu[entityC].rad = 0.05f;
		entity.gpu[entityC].color = (VEC3){0.1f,0.7f,0.1f};
		break;
	case 5:
		entity.cpu[entityC].baseColor = (VEC3){0.5f,0.1f,0.1f};
		entity.gpu[entityC].id = 0;
		entity.gpu[entityC].rad = 0.2f;
		entity.gpu[entityC].color = (VEC3){0.5f,0.1f,0.1f};
		break;
	case 6:
		entity.cpu[entityC].baseColor = (VEC3){0.5f,0.5f,0.5f};
		entity.gpu[entityC].id = 0;
		entity.gpu[entityC].rad = 0.05f;
		entity.gpu[entityC].color = (VEC3){0.5f,0.5f,0.5f};
		entity.cpu[entityC].health = (u32)irnd()%10;
		break;
	case 8:
		entity.cpu[entityC].baseColor = (VEC3){0.5f,0.5f,0.5f};
		entity.gpu[entityC].id = 0;
		entity.gpu[entityC].rad = 0.5f;
		entity.gpu[entityC].color = (VEC3){0.5f,0.5f,0.5f};
		for(u32 i = 0;i < ENTITYTEXTSZ;i++){
			for(u32 i2 = 0;i2 < ENTITYTEXTSZ;i2++){
				u8 b = irnd();
				entityTexture[entityC*ENTITYTEXTSZ*ENTITYTEXTSZ+i*ENTITYTEXTSZ+i2].r = b;
				entityTexture[entityC*ENTITYTEXTSZ*ENTITYTEXTSZ+i*ENTITYTEXTSZ+i2].g = b;
				entityTexture[entityC*ENTITYTEXTSZ*ENTITYTEXTSZ+i*ENTITYTEXTSZ+i2].b = b;
			}
		}
		break;
	}
	entityC++;
	glMes[glMesC].id = 12;
	glMesC++;
}

VEC2 rotVEC2(VEC2 p,f32 rot){
    VEC2 r;
    r.x = cosf(rot) * p.x - sinf(rot) * p.y;
    r.y = sinf(rot) * p.x + cosf(rot) * p.y;
    return r;
}

void entityDeath(int id){
	for(int i = id;i < entityC;i++){
		entity.gpu[i] = entity.gpu[i+1];
		entity.cpu[i] = entity.cpu[i+1];
	}
	entityC--;
}

unsigned int crds2map(int x,int y,int z){
	return ((int)x+(int)y*properties->lvlSz+(int)z*properties->lvlSz*properties->lvlSz);
}

f32 PointBoxDistance(VEC3 spherePos,VEC3 boxPos,VEC3 boxSZ){ 
	VEC3 p = VEC3subVEC3R(spherePos,boxPos);
	VEC3 q = VEC3subVEC3R(VEC3absR(p),boxSZ);
	return VEC3length(VEC3maxR(q,0.0f)) + fminf(fmaxf(q.x,fmaxf(q.y,q.z)),0.0f);
}

VEC3 PointBoxNormal(VEC3 spherePos,VEC3 boxPos,VEC3 boxSz){
	VEC3 p = VEC3subVEC3R(spherePos,boxPos);
	if(p.x > 0.0f){
		p.x -= boxSz.x;
	}
	else{
		p.x = fabsf(p.x);
	}
	if(p.y > 0.0f){
		p.y -= boxSz.y;
	}
	else{
		p.y = fabsf(p.y);
	}
	if(p.z > 0.0f){
		p.z -= boxSz.z;
	}
	else{
		p.z = fabsf(p.z);
	}
	return VEC3maxR(p,0.0f);
}

typedef struct{
	union{
		u32 r;
		u32 x;
	};
	union{
		u32 g;
		u32 y;
	};
	union{
		u32 b;
		u32 z;
	};
}UVEC3;

void SphereMapCollision(IVEC3 ePos,u32 i){
	if(ePos.x < 0 || ePos.y < 0 || ePos.z < 0 || ePos.x >= properties->lvlSz || ePos.y >= properties->lvlSz || ePos.z >= properties->lvlSz){
		entityDeath(i);
		return;
	}
	if(ePos.x < 1){
		ePos.x = 1;
	}
	if(ePos.y < 1){
		ePos.y = 1;
	}
	if(ePos.z < 1){
		ePos.z = 1;
	}
	if(ePos.x > properties->lvlSz - 2){
		ePos.x = properties->lvlSz - 2;
	}
	if(ePos.y > properties->lvlSz - 2){
		ePos.y = properties->lvlSz - 2;
	}
	if(ePos.z > properties->lvlSz - 2){
		ePos.z = properties->lvlSz - 2;
	}
	for(i32 i2 = -1;i2 < 2;i2++){
		for(i32 i3 = -1;i3 < 2;i3++){
			for(i32 i4 = -1;i4 < 2;i4++){
				switch(map[crds2map(ePos.x+i2,ePos.y+i3,ePos.z+i4)].id){
				case BLOCK_REFLECTIVE:
				case BLOCK_SOLID:{
					f32 p = PointBoxDistance(entity.gpu[i].pos,(VEC3){ePos.x+i2,ePos.y+i3,ePos.z+i4},(VEC3){1.0f,1.0f,1.0f});
					if(p < entity.gpu[i].rad){
						entityDeath(i);
					}
					break;
				}
				}
			}
		}
	}
}

void calculateLuminance(u8 id){
	UVEC3 entityLuminance = {0,0,0};
	u32 hits = 0;
	for(i32 i2 = 0;i2 < LUMINANCESAMPLECOUNT;i2++){
		for(i32 i3 = 0;i3 < LUMINANCESAMPLECOUNT;i3++){
			RAY ray = rayCreate(entity.gpu[id].pos,(VEC3){sinf((f32)i3/LUMINANCESAMPLECOUNT*PI_2)*-sinf((f32)i2/LUMINANCESAMPLECOUNT*PI),cosf((f32)i3/LUMINANCESAMPLECOUNT*PI_2)*-sinf((f32)i2/LUMINANCESAMPLECOUNT*PI),-cosf((f32)i2/LUMINANCESAMPLECOUNT*PI)});
			rayItterate(&ray);
			i32 l = getLmapLocation(&ray);
			if(l != -1){
				entityLuminance.r += lmap[l].r;
				entityLuminance.g += lmap[l].g;
				entityLuminance.b += lmap[l].b;
				hits++;
			}
		}
	}
	entity.gpu[id].color.r = (f32)entityLuminance.r/hits*entity.cpu[id].baseColor.r / (brightness+1.0f)*4.0f;
	entity.gpu[id].color.g = (f32)entityLuminance.g/hits*entity.cpu[id].baseColor.g / (brightness+1.0f)*4.0f;
	entity.gpu[id].color.b = (f32)entityLuminance.b/hits*entity.cpu[id].baseColor.b / (brightness+1.0f)*4.0f;
}

void entities(){
	for(;;){
		for(u32 i = 0;i < entityC;i++){
			entity.cpu[i].playerDist = VEC3dist(player->pos,entity.gpu[i].pos);
			VEC3addVEC3(&entity.gpu[i].pos,entity.cpu[i].vel);
			IVEC3 ss = {entity.gpu[i].pos.x,entity.gpu[i].pos.y,entity.gpu[i].pos.z};
			switch((u32)entity.cpu[i].id){
			case 0:
				SphereMapCollision(ss,i);
				break;
			case 1:
				if(entity.cpu[i].health){
					entity.cpu[i].health--;
				}
				else{
					spawnEntityEx(entity.gpu[i].pos,entity.gpu[i].pos2,(VEC3){0.0,0.0,0.0},2,(VEC3){0.4f,0.04f,0.04f});
					entityDeath(i);
				}
				break;
			case 2:
				if(entity.cpu[i].health){
					entity.cpu[i].health--;
				}
				else{
					entityDeath(i);
				}
				break;
			case 3:{
				VEC2 r = rotVEC2((VEC2){entity.gpu[i].pos.x-entity.cpu[i].pos.x,entity.gpu[i].pos.y-entity.cpu[i].pos.y},0.017f);
				entity.gpu[i].pos.x = r.x+entity.cpu[i].pos.x;
				entity.gpu[i].pos.y = r.y+entity.cpu[i].pos.y;
				VEC2 r2 = rotVEC2((VEC2){entity.gpu[i].pos2.x,entity.gpu[i].pos2.y},0.017f);
				entity.gpu[i].pos2.x = r2.x;
				entity.gpu[i].pos2.y = r2.y;
				if(player->pos.x > entity.cpu[i].pos.x - 0.3f  && player->pos.x < entity.cpu[i].pos.x + 0.3f &&
					player->pos.y > entity.cpu[i].pos.y - 0.3f && player->pos.y < entity.cpu[i].pos.y + 0.3f && 
					player->pos.z > entity.cpu[i].pos.z - 0.2f && player->pos.z < entity.cpu[i].pos.z + 2.0f){
					entityDeath(i);
					player->weaponEquiped = 1;
					spawnEntityEx((VEC3){player->pos.x+player->xdir,player->pos.y+player->ydir,player->pos.z},(VEC3){0.1f,0.1f,-0.3f},(VEC3){0.0f,0.0f,0.0f},4,(VEC3){0.0f,0.04f,0.0f});
				}
				break;
			}
			case 4:
				break;/*
			case 5:{
				if(ss.x<0||ss.y<0||ss.z<0||ss.x>properties->lvlSz-1||ss.y>properties->lvlSz-1||ss.z>properties->lvlSz-1){
					entityDeath(i);
					continue;
				}
				if(ss.x < 1){
					ss.x = 1;
				}
				if(ss.y < 1){
					ss.y = 1;
				}
				if(ss.z < 1){
					ss.z = 1;
				}
				if(ss.x > properties->lvlSz - 2){
					ss.x = properties->lvlSz - 2;
				}
				if(ss.y > properties->lvlSz - 2){
					ss.y = properties->lvlSz - 2;
				}
				if(ss.z > properties->lvlSz - 2){
					ss.z = properties->lvlSz - 2;
				}
				for(i32 i2 = -1;i2 < 2;i2++){
					for(i32 i3 = -1;i3 < 2;i3++){
						for(i32 i4 = -1;i4 < 2;i4++){
							switch(map[crds2map(ss.x+i2,ss.y+i3,ss.z+i4)].id){
							case BLOCK_SOLID:{
								VEC4 p = PointBoxDistance(entity.gpu[i].pos,(VEC3){ss.x+i2+0.5f,ss.y+i3+0.5f,ss.z+i4+0.5f},(VEC3){0.5f,0.5f,0.5f});
								if(p.x < entity.gpu[i].rad){
									VEC2 rp = VEC2normalize(VEC2subVEC2R((VEC2){entity.gpu[i].pos.x,entity.gpu[i].pos.y},(VEC2){player->pos.x,player->pos.y}));
									if(p.y < entity.gpu[i].rad){
										entity.gpu[i].pos.x-=entity.cpu[i].vel.x;
										entity.cpu[i].vel.x = 0.0;
									}
									if(p.z < entity.gpu[i].rad){
										entity.gpu[i].pos.y-=entity.cpu[i].vel.y;
										entity.cpu[i].vel.y = 0.0f;
									}
									if(p.w < entity.gpu[i].rad){
										entity.gpu[i].pos.z-=entity.cpu[i].vel.z;
										entity.cpu[i].vel.x /= 1.1f;
										entity.cpu[i].vel.y /= 1.1f;
										entity.cpu[i].vel.z = 0.0f;
									}
									if(irnd()%40==1){
										RAY ray = rayCreate(entity.gpu[i].pos,VEC3subVEC3R(player->pos,entity.gpu[i].pos));
										rayItterate(&ray);
										while(ray.ix>=0&&ray.ix<properties->lvlSz&&ray.iy>=0&&ray.iy<properties->lvlSz&&ray.iz>=0&&ray.iz<properties->lvlSz){
											u32 block = crds2map(ray.ix,ray.iy,ray.iz);
											switch(map[block].id){
											case BLOCK_AIR:
												break;
											default:
												goto end;
											}
											if((u32)player->pos.x == ray.ix && (u32)player->pos.y == ray.iy && (u32)player->pos.z == ray.iz){
												entity.cpu[i].vel.x = -rp.x/5.0f;
												entity.cpu[i].vel.y = -rp.y/5.0f;
												entity.cpu[i].vel.z = 0.21f;
												break;
											}
											rayItterate(&ray);
											continue;
										end:
											break;
										}
									}
									goto end2;
								}
								break;
							}
							}
						}
					}
				}
			end2:
				entity.cpu[i].vel.z -= 0.015f;
				if(player->pos.x > entity.gpu[i].pos.x - 0.3f  && player->pos.x < entity.gpu[i].pos.x + 0.3f &&
					player->pos.y > entity.gpu[i].pos.y - 0.3f && player->pos.y < entity.gpu[i].pos.y + 0.3f && 
					player->pos.z > entity.gpu[i].pos.z - 0.2f && player->pos.z < entity.gpu[i].pos.z + 2.0f){
					if(!player->wounded){
						player->wounded = 1;
						player->aniType = 1;
						player->aniTime = 30;
					}
					else if(!player->aniTime||player->aniType!=1){
						playerDeath();
					}
				}
				calculateLuminance(i);
				break;
			}*/
			case 6:
				if(!entity.cpu[i].health){
					entityDeath(i);
				}
				else{
					entity.gpu[i].rad/=1.5f;
					entity.cpu[i].health--;
				}
				break;
			case 7:{
				VEC2 r = rotVEC2((VEC2){entity.gpu[i].pos.x-entity.cpu[i].pos.x,entity.gpu[i].pos.y-entity.cpu[i].pos.y},0.017f);
				entity.gpu[i].pos.x = r.x+entity.cpu[i].pos.x;
				entity.gpu[i].pos.y = r.y+entity.cpu[i].pos.y;
				VEC2 r2 = rotVEC2((VEC2){entity.gpu[i].pos2.x,entity.gpu[i].pos2.y},0.017f);
				entity.gpu[i].pos2.x = r2.x;
				entity.gpu[i].pos2.y = r2.y;
				if(player->pos.x > entity.cpu[i].pos.x - 0.3f  && player->pos.x < entity.cpu[i].pos.x + 0.3f &&
					player->pos.y > entity.cpu[i].pos.y - 0.3f && player->pos.y < entity.cpu[i].pos.y + 0.3f && 
					player->pos.z > entity.cpu[i].pos.z - 0.2f && player->pos.z < entity.cpu[i].pos.z + 2.0f && player->health != 100){
					entityDeath(i);
					player->health = 100;
				}
				break;
			}
			case 8:{
				entity.gpu[i].rot.x += entity.cpu[i].vel.x * 2.0f;
				entity.gpu[i].rot.y += entity.cpu[i].vel.y * 2.0f;
				if(ss.x<0||ss.y<0||ss.z<0||ss.x>properties->lvlSz-1||ss.y>properties->lvlSz-1||ss.z>properties->lvlSz-1){
					entityDeath(i);
					continue;
				}
				if(ss.x < 1){
					ss.x = 1;
				}
				if(ss.y < 1){
					ss.y = 1;
				}
				if(ss.z < 1){
					ss.z = 1;
				}
				if(ss.x > properties->lvlSz - 2){
					ss.x = properties->lvlSz - 2;
				}
				if(ss.y > properties->lvlSz - 2){
					ss.y = properties->lvlSz - 2;
				}
				if(ss.z > properties->lvlSz - 2){
					ss.z = properties->lvlSz - 2;
				}
				u8 axisE = 0;
				VEC3 depth[3] = {0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f};
				for(i32 i2 = -1;i2 < 2;i2++){
					for(i32 i3 = -1;i3 < 2;i3++){
						for(i32 i4 = -1;i4 < 2;i4++){
							switch(map[crds2map(ss.x+i2,ss.y+i3,ss.z+i4)].id){
							case BLOCK_SOLID:{
								f32 p = PointBoxDistance(entity.gpu[i].pos,(VEC3){ss.x+i2+0.5f,ss.y+i3+0.5f,ss.z+i4+0.5f},(VEC3){0.5f,0.5f,0.5f});
								if(p < entity.gpu[i].rad){
									VEC3 pnorm = PointBoxNormal(entity.gpu[i].pos,(VEC3){ ss.x+i2,ss.y+i3,ss.z+i4 },(VEC3){ 1.0f,1.0f,1.0f });
									pnorm = VEC3normalize(pnorm);
									if(pnorm.x > pnorm.y){
										if(pnorm.x > pnorm.z){
											if(depth[0].x < pnorm.x){
												axisE |= 0x01;
												depth[0] = pnorm;
											}
										}
										else{
											if(depth[2].z < pnorm.z){
												axisE |= 0x04;
												depth[2] = pnorm;
											}
										}
									}
									else if(pnorm.y > pnorm.z){
										if(depth[1].y < pnorm.y){
											axisE |= 0x02;
											depth[1] = pnorm;
										}
									}
									else{
										if(depth[2].z < pnorm.z){
											axisE |= 0x04;
											depth[2] = pnorm;
										}
									}					
								}
								break;
							}
							}
						}
					}
				}
				if(axisE&0x01){
					VEC3subVEC3(&entity.gpu[i].pos,VEC3mulVEC3R(entity.cpu[i].vel,depth[0]));
					entity.cpu[i].vel = VEC3reflect(entity.cpu[i].vel,depth[0]);
					entity.cpu[i].vel.x *= 0.8f;
				}
				if(axisE&0x02){
					VEC3subVEC3(&entity.gpu[i].pos,VEC3mulVEC3R(entity.cpu[i].vel,depth[1]));
					entity.cpu[i].vel = VEC3reflect(entity.cpu[i].vel,depth[1]);
					entity.cpu[i].vel.y *= 0.8f;
				}
				if(axisE&0x04){
					VEC3subVEC3(&entity.gpu[i].pos,VEC3mulVEC3R(entity.cpu[i].vel,depth[2]));
					entity.cpu[i].vel = VEC3reflect(entity.cpu[i].vel,depth[2]);
					entity.cpu[i].vel.z *= 0.8f;
				}
				entity.cpu[i].vel.z -= 0.015f;
				for(u32 i2 = i+1;i2 < entityC;i2++){
					switch(entity.cpu[i2].id){
					case 8:
						if(VEC3dist(entity.gpu[i2].pos,entity.gpu[i].pos) < entity.gpu[i].rad + entity.gpu[i2].rad){
							VEC3subVEC3(&entity.gpu[i].pos,entity.cpu[i].vel);
							f32 m1 = entity.gpu[i].rad * entity.gpu[i].rad * PI;
							f32 m2 = entity.gpu[i2].rad * entity.gpu[i2].rad * PI;
							VEC3 normal = VEC3normalize(VEC3subVEC3R(entity.gpu[i2].pos,entity.gpu[i].pos));
							VEC3 ang = {normal.z,normal.y,-normal.x};
							f32 normalVel1 = VEC3dot(entity.cpu[i].vel, normal);
							f32 tangVel1 = VEC3dot(entity.cpu[i].vel, ang);
							f32 normalVel2 = VEC3dot(entity.cpu[i2].pos, normal);
							f32 tangVel2 = VEC3dot(entity.cpu[i2].pos, ang);
							VEC3 normal1 = VEC3mulR(normal,(normalVel1 * (m1 - m2) + 2 * m2 * normalVel2) / (m1 + m2));
							VEC3 normal2 = VEC3mulR(normal,(normalVel2 * (m2 - m1) + 2 * m1 * normalVel1) / (m1 + m2));
							VEC3 tang1 = VEC3mulR(ang, tangVel1);
							VEC3 tang2 = VEC3mulR(ang, tangVel2);
							VEC3 temp = VEC3addVEC3R(normal1,tang1);
							entity.cpu[i].vel.x = temp.x;
							entity.cpu[i].vel.y = temp.y;
							entity.cpu[i].vel.z = temp.z;
							temp = VEC3addVEC3R(normal2,tang2);
							entity.cpu[i2].vel.x = temp.x;
							entity.cpu[i2].vel.y = temp.y;
							entity.cpu[i2].vel.z = temp.z;
							break;
						}
						break;
					}
				}
				if(VEC2dist((VEC2){player->pos.x,player->pos.y},(VEC2){entity.gpu[i].pos.x,entity.gpu[i].pos.y}) < 0.3f + entity.gpu[i].rad){
					f32 m1 = entity.gpu[i].rad * entity.gpu[i].rad * PI;
					f32 m2 = 25.0f;
					VEC2 normal = VEC2normalize(VEC2subVEC2R((VEC2){player->pos.x,player->pos.y},(VEC2){entity.gpu[i].pos.x,entity.gpu[i].pos.y}));
					VEC2 ang = (VEC2){ normal.y,-normal.x };
					f32 normalVel1 = VEC2dot((VEC2){entity.cpu[i].vel.x,entity.cpu[i].vel.y}, normal);
					f32 tangVel1 = VEC2dot((VEC2){entity.cpu[i].vel.x,entity.cpu[i].vel.y}, ang);
					f32 normalVel2 = VEC2dot((VEC2){player->vel.x,player->vel.y}, normal);
					f32 tangVel2 = VEC2dot((VEC2){player->vel.x,player->vel.y}, ang);
					VEC2 normal1 = VEC2mulR(normal,(normalVel1 * (m1 - m2) + 2 * m2 * normalVel2) / (m1 + m2));
					VEC2 normal2 = VEC2mulR(normal,(normalVel2 * (m2 - m1) + 2 * m1 * normalVel1) / (m1 + m2));
					VEC2 tang1 = VEC2mulR(ang, tangVel1);
					VEC2 tang2 = VEC2mulR(ang, tangVel2);
					VEC2 temp = VEC2addVEC2R(normal1,tang1);
					entity.cpu[i].vel.x = temp.x*2.0f;
					entity.cpu[i].vel.y = temp.y*2.0f;
					temp = VEC2addVEC2R(normal2,tang2);
					player->vel.x = temp.x;
					player->vel.y = temp.y;
				}
				calculateLuminance(i);
				break;
			}
			case 9:
				entity.gpu[i].pos = networkplayer.player[entity.cpu[i].playerid].pos;
				entity.gpu[i].srot.x = -networkplayer.player[entity.cpu[i].playerid].rot;
				calculateLuminance(i);
				break;
			case 10:{
				if(entity.cpu[i].health == 5){
					ExitProcess(0);
					VEC3 laserDirNorm = VEC3normalize(entity.gpu[i].pos2);
					f32 d = iCylinder(player->pos,laserDirNorm,(VEC3){0.0f,0.0f,-1.7f},0.2f);
					if(d > 0.0f && d < VEC3dist(player->pos,entity.gpu[i].pos)){
						playerDeath();
					}
				}
				if(entity.cpu[i].health){
					entity.cpu[i].health--;
				}
				else{
					entityDeath(i);
				}
				break;
			}
			default:{
				calculateLuminance(i);
				break;
			}
			}
		}
		u32 s = 1;
		u32 j = entityC;
		while(s){
			s = 0;
			for(u32 i = 1; i < j; i++) {
				if(entity.cpu[i].playerDist < entity.cpu[i - 1].playerDist){
					CPUDATA ctemp = entity.cpu[i];
					GPUDATA gtemp = entity.gpu[i];
					entity.cpu[i] = entity.cpu[i - 1];
					entity.gpu[i] = entity.gpu[i - 1];
					entity.cpu[i - 1] = ctemp;
					entity.gpu[i - 1] = gtemp;
					s = 1;
				}
			}
			j--;
		}
		Sleep(15);
	}
}