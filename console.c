#include <stdlib.h>

#include "main.h"
#include "godrays.h"
#include "reflections.h"

i8 chatLineSel = -1;

f32 tnoise3(VEC3 pos){
	VEC3 fpos = VEC3floorR(pos);
	VEC3 fpos2 = VEC3fractR(pos);
	f32 r1 = VEC3rnd(fpos);
	f32 r2 = VEC3rnd(VEC3addVEC3R(fpos,(VEC3){1.0f,0.0f,0.0f}));
	f32 r3 = VEC3rnd(VEC3addVEC3R(fpos,(VEC3){0.0f,1.0f,0.0f}));
	f32 r4 = VEC3rnd(VEC3addVEC3R(fpos,(VEC3){1.0f,1.0f,0.0f}));
	f32 r5 = VEC3rnd(VEC3addVEC3R(fpos,(VEC3){0.0f,0.0f,1.0f}));
	f32 r6 = VEC3rnd(VEC3addVEC3R(fpos,(VEC3){1.0f,0.0f,1.0f}));
	f32 r7 = VEC3rnd(VEC3addVEC3R(fpos,(VEC3){0.0f,1.0f,1.0f}));
	f32 r8 = VEC3rnd(VEC3addVEC3R(fpos,(VEC3){1.0f,1.0f,1.0f}));
	f32 m1 = mix(r1,r2,fpos2.x);
	f32 m2 = mix(r3,r4,fpos2.x);
	f32 m3 = mix(r5,r6,fpos2.x);
	f32 m4 = mix(r7,r8,fpos2.x);
	f32 mm1= mix(m1,m2,fpos2.y);
	f32 mm2= mix(m3,m4,fpos2.y);
	return mix(mm1,mm2,fpos2.z);
}

void executeCommand(u8 *cmd){
	u8 cmdlen = 20;
	for(u32 i = 0;i < 20;i++){
		if(cmd[i] == ' ' || cmd[i] == '\0'){
			cmdlen = i;
			break;
		}
	}
	switch(cmdlen){
	case 2: 
		if(!memcmp(cmd,"ui",2)){
			settings ^= SETTINGS_UI;
		}
		break;
	case 3:
		if(!memcmp(cmd,"fly",3)){
			settings ^= SETTINGS_MOVEMENT;
		}
		if(!memcmp(cmd,"fov",3)){
			player->fov.y = strtof(cmd+4,0);
			player->fov.x = player->fov.y*16.0f/9.0f;
			glMes[glMesC++].id = 10;
		}
		break;
	case 4:
		if(!memcmp(cmd,"quit",4)){
			closeEngine();
		}
		if(!memcmp(cmd,"rand",4)){
			for(u32 i = 0;i < properties->lvlSz;i++){
				for(u32 i2 = 0;i2 < properties->lvlSz;i2++){
					for(u32 i3 = 0;i3 < properties->lvlSz;i3++){
						if(tnoise3(VEC3divR((VEC3){i,i2,i3},4.0f))<1.06f){
							setBlock(crds2map(i,i2,i3));
						}
						else{
							map[crds2map(i,i2,i3)].id = BLOCK_AIR;
						}
					}
				}
			}
			glMes[glMesC++].id = 3;
		}
		break;
	case 5:
		if(!memcmp(cmd,"speed",5)){
			player->movementSpeed = strtof(cmd+6,0);
		}
		break;
	case 6:
		if(!memcmp(cmd,"editor",6)){
			settings ^= SETTINGS_LIGHTING;
		}
		if(!memcmp(cmd,"sphere",6)){
			u32 size = atoi(cmd+7);
			VEC3 middle = {player->pos.x+size/2.0f,player->pos.y+size/2.0f,player->pos.z+size/2.0f};

			u32 x = min(player->pos.x + size,properties->lvlSz);
			u32 y = min(player->pos.y + size,properties->lvlSz);
			u32 z = min(player->pos.z + size,properties->lvlSz);

			for(u32 i = player->pos.x;i < x;i++){
				for(u32 i2 = player->pos.y;i2 < y;i2++){
					for(u32 i3 = player->pos.z;i3 < z;i3++){
						if(VEC3dist((VEC3){i,i2,i3},middle) < size/2.0f){
							setBlock(crds2map(i,i2,i3));
						}
					}
				}
			}
			glMes[glMesC++].id = 3;
		}
		break;
	case 7:
		if(!memcmp(cmd,"gravity",7)){
			properties->gravity = strtof(cmd+8,0);
		}
		break;
	case 8:
		if(!memcmp(cmd,"lightgen",8)){
			CreateThread(0,0,updateLight2,0,0,0);
		}
		break;
	case 9:
		if(!memcmp(cmd,"godrayres",9)){
			u16 res = atoi(cmd+10);
			godraymap  = HeapReAlloc(GetProcessHeap(),8,godraymap,sizeof(VEC3)*res*res);
			godraymapB = HeapReAlloc(GetProcessHeap(),8,godraymapB,sizeof(VEC3)*res*res);
			properties->godrayRes = res;
		}
		if(!memcmp(cmd,"godrayamm",9)){
			properties->godrayAmm = strtof(cmd+10,0);
		}
		break;
	case 10:
		if(!memcmp(cmd,"reflectres",10)){
			u16 res = atoi(cmd+11);
			reflectmap  = HeapReAlloc(GetProcessHeap(),8,reflectmap,sizeof(VEC3)*res*res);
			reflectmapB = HeapReAlloc(GetProcessHeap(),8,reflectmapB,sizeof(VEC3)*res*res);
			properties->reflectRes = res;
		}
		break;
	case 11:
		if(!memcmp(cmd,"flightspeed",11)){
			player->flightSpeed = strtof(cmd+12,0);
		}
		if(!memcmp(cmd,"sensitivity",11)){
			properties->sensitivity = strtof(cmd+12,0);
		}
		break;
	case 13:
		if(!memcmp(cmd,"texturecolrnd",13)){
			for(u32 i = 0;i < properties->lmapSz*properties->lmapSz*lmapC;i++){
				lmap[i] = (EXRGB){(f32)bmap[i].r/(rnd()-1.0f),(f32)bmap[i].g/(rnd()-1.0f),(f32)bmap[i].b/(rnd()-1.0f)};
			}
			glMes[glMesC++].id = 3;
			glMes[glMesC++].id = 6;
		}
		if(!memcmp(cmd,"texturemozaic",13)){
			for(u32 i = 0;i < properties->lmapSz*properties->lmapSz*lmapC;i++){
				switch(irnd() & 3){
				case 0:
					lmap[i].r = bmap[i].r;
					lmap[i].g = 0;
					lmap[i].b = 0;
					break;
				case 1:
					lmap[i].r = 0;
					lmap[i].g = bmap[i].g;
					lmap[i].b = 0;
					break;
				case 2:
					lmap[i].r = 0;
					lmap[i].g = 0;
					lmap[i].b = bmap[i].b;
					break;
				case 3:
					i--;
					continue;
				}
			}
			glMes[glMesC++].id = 3;
			glMes[glMesC++].id = 6;
		}
		break;
	case 15:
		if(!memcmp(cmd,"rayacceleration",15)){
			properties->rayAcceleration = atoi(cmd+16);
		}
		break;
	}

}