#include <stdlib.h>

#include "main.h"

i8 chatLineSel = -1;

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
			glMes[glMesC].id = 10;
			glMesC++;
		}
		break;
	case 4:
		if(!memcmp(cmd,"quit",4)){
			closeEngine();
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
			glMes[glMesC].id = 3;
			glMesC++;
		}
		break;
	case 8:
		if(!memcmp(cmd,"lightgen",8)){
			CreateThread(0,0,updateLight2,0,0,0);
		}
		break;
	case 10:
		if(!memcmp(cmd,"textrndcol",10)){
			for(u32 i = 0;i < lmapC;i++){
				lmap[i] = (EXRGB){irnd(),irnd(),irnd()};
			}
			glMes[glMesC].id = 6;
			glMesC++;
		}
		break;
	case 11:
		if(!memcmp(cmd,"sensitivity",11)){
			properties->sensitivity = strtof(cmd+12,0);
		}
		break;
	}
}