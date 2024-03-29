#include "main.h"
#include "sound.h"

u8 touchStatus;

VEC3 angleSteep;

u8 effect;
u8 effectdata;

f32 stepSoundCooldown;

u8 touchedSpace;

void blockDetection(VEC3 bpos,int axis){
	int block = crds2map(bpos.x,bpos.y,bpos.z);
	VEC3 spos = VEC3fractR(bpos);
	switch(map[block].id){
	case BLOCK_CUBE:
	case BLOCK_GLASS:
		VEC2 r;
		r = rotVEC2((VEC2){spos.x-(f32)metadt[block].id/255.0f,spos.y-(f32)metadt[block].r/255.0f},(f32)metadt3[block].id/255.0f*PI);
		spos.x = r.x+(f32)metadt[block].id/255.0f;
		spos.y = r.y+(f32)metadt[block].r/255.0f;
		r = rotVEC2((VEC2){spos.x-(f32)metadt[block].id/255.0f,spos.z-(f32)metadt[block].g/255.0f},(f32)metadt3[block].r/255.0f*PI);
		spos.x = r.x+(f32)metadt[block].id/255.0f;
		spos.z = r.y+(f32)metadt[block].g/255.0f;
		r = rotVEC2((VEC2){spos.y-(f32)metadt[block].r/255.0f,spos.z-(f32)metadt[block].g/255.0f},(f32)metadt3[block].g/255.0f*PI);
		spos.y = r.x+(f32)metadt[block].r/255.0f;
		spos.z = r.y+(f32)metadt[block].g/255.0f;
		if(spos.x > (f32)metadt[block].id/255.0f - (f32)metadt2[block].id/255.0f && spos.x < (f32)metadt[block].id/255.0f + (f32)metadt2[block].id/255.0f &&
			spos.y > (f32)metadt[block].r/255.0f - (f32)metadt2[block].r/255.0f && spos.y < (f32)metadt[block].r/255.0f + (f32)metadt2[block].r/255.0f && 
			spos.z > (f32)metadt[block].g/255.0f - (f32)metadt2[block].g/255.0f - 0.02f && spos.z < (f32)metadt[block].g/255.0f + (f32)metadt2[block].g/255.0f + 0.02f){
			if(metadt3[block].r == 0 && metadt3[block].id == 0 && metadt3[block].g == 0 && (f32)metadt[block].g/255.0f + (f32)metadt2[block].g/255.0f + (u32)bpos.z > player->pos.z - player->hitboxHeight && 
				(f32)metadt[block].g/255.0f + (f32)metadt2[block].g/255.0f + (u32)bpos.z < player->pos.z - player->hitboxHeight + 0.5f){
				player->pos.z += (f32)metadt[block].g/255.0f + (f32)metadt2[block].g/255.0f + (u32)bpos.z - player->pos.z + player->hitboxHeight + 0.1f;
				player->vel.z = 0.0f;
			}
			else{
				angleSteep.x = fmaxf(sinf((f32)metadt3[block].id/127.0f*PI) * 0.60f,angleSteep.x);
				angleSteep.y = fmaxf(sinf((f32)metadt3[block].r/127.0f*PI) * 0.60f,angleSteep.y);
				angleSteep.z = fmaxf(sinf((f32)metadt3[block].g/127.0f*PI) * 0.60f,angleSteep.z);
				if(angleSteep.y){
					axis = axis & ~0x30;
				}
				if(angleSteep.z){
					axis = axis & ~0x0c;
				}
				touchStatus |= axis;
			}
		}
		break;
	case BLOCK_CLIP:
	case BLOCK_REFLECTIVE2:
	case BLOCK_REFLECTIVE:
	case BLOCK_SOLID:
		if(spos.z > 0.5f && bpos.z > player->pos.z-player->hitboxHeight && bpos.z < player->pos.z-player->hitboxHeight+0.5f){
			switch(map[block+properties->lvlSz*properties->lvlSz].id){
			case BLOCK_CLIP:
			case BLOCK_SOLID:
			case BLOCK_REFLECTIVE:
				touchStatus |= axis;
				break;
			default:{
				u32 standingBlock = crds2map(player->pos.x,player->pos.y,player->pos.z-player->hitboxHeight-0.05f);
				switch(map[standingBlock].id){
				case BLOCK_AIR:
					touchStatus |= axis;
					break;
				default:
					player->pos.z += player->pos.z - bpos.z - player->hitboxHeight + 0.52f;
					player->vel.z = 0.0f;
					break;
				}
				break;
			}
			}
		}
		else{
			touchStatus |= axis;
		}
		break;
	}
}

void hitboxZdown(VEC3 bpos){
	bpos.x -= player->vel.x;
	bpos.y -= player->vel.y;
	bpos.z += player->vel.z;
	blockDetection(bpos,1);
}

void hitboxZup(VEC3 bpos){
	bpos.x -= player->vel.x;
	bpos.y -= player->vel.y;
	bpos.z += player->vel.z;
	blockDetection(bpos,2);
}

void hitboxXdown(VEC3 bpos){
	bpos.x += player->vel.x;
	bpos.y -= player->vel.y;
	bpos.z -= player->vel.z;
	blockDetection(bpos,4);
}

void hitboxXup(VEC3 bpos){
	bpos.x += player->vel.x;
	bpos.y -= player->vel.y;
	bpos.z -= player->vel.z;
	blockDetection(bpos,8);
}

void hitboxYdown(VEC3 bpos){
	bpos.y += player->vel.y;
	bpos.x -= player->vel.x;
	bpos.z -= player->vel.z;
	blockDetection(bpos,16);
}

void hitboxYup(VEC3 bpos){
	bpos.y += player->vel.y;
	bpos.x -= player->vel.x;
	bpos.z -= player->vel.z;
	blockDetection(bpos,32);
}

void playerWorldCollision(){
	if(player->pos.z < player->hitboxHeight+0.01f){
		playerDeath();
	}
	if(player->stamina < 1.0f){
		player->stamina+= 0.045f;
	}
	else{
		player->stamina = 1.0f;
	}
	if(player->vel.z < 0.0f){
		for(f32 i = -0.2f;i <= 0.2f;i+=0.1f){
			for(f32 i2 = -0.2f;i2 <= 0.2f;i2+=0.05f){
				hitboxZdown((VEC3){player->pos.x + i, player->pos.y + i2, player->pos.z - player->hitboxHeight});
			}
		}
	}
	else{
		for(f32 i = -0.2f;i <= 0.2f;i+=0.1f){
			for(f32 i2 = -0.2f;i2 <= 0.2f;i2+=0.05f){
				hitboxZup((VEC3){player->pos.x + i, player->pos.y + i2, player->pos.z + 0.1f});
			}
		}
	}
	if(player->vel.x < 0.0f){
		for(f32 i = -player->hitboxHeight;i <= 0.1f;i+=0.1f){
			for(f32 i2 = -0.2;i2 <= 0.2;i2+=0.05){
				hitboxXdown((VEC3){player->pos.x - 0.2f, player->pos.y + i2, player->pos.z + i});
			}
		}

	}
	else{
		for(f32 i = -player->hitboxHeight;i <= 0.1f;i+=0.1f){
			for(f32 i2 = -0.2f;i2 <= 0.2f;i2+=0.05f){
				hitboxXup((VEC3){player->pos.x + 0.2f, player->pos.y + i2, player->pos.z + i});
			}
		}
	}
	if(player->vel.y < 0.0f){
		for(f32 i = -player->hitboxHeight;i <= 0.1f;i+=0.1f){
			for(f32 i2 = -0.2f;i2 <= 0.2f;i2+=0.05f){
				hitboxYdown((VEC3){player->pos.x + i2, player->pos.y - 0.2f, player->pos.z + i});
			}
		}

	}
	else{
		for(f32 i = -player->hitboxHeight;i <= 0.1f;i+=0.1f){
			for(f32 i2 = -0.2f;i2 <= 0.2f;i2+=0.05f){
				hitboxYup((VEC3){player->pos.x + i2, player->pos.y + 0.2f, player->pos.z + i});
			}
		}
	}
	if(touchStatus & 0x01){
		if(player->hitboxWantedHeightQueued>player->hitboxWantedHeight){
			player->hitboxWantedHeight = player->hitboxWantedHeightQueued;
			player->hitboxWantedHeightQueued = 0.0f;
		}
		if(player->vel.z < -0.03f){
			switch(metadt6[crds2map(player->pos.x,player->pos.y,player->pos.z-player->hitboxHeight-0.1f)].r){
			case 0:
				playSound(landSound,0,-2000);
				break;
			case 1:
				playSound(landEchoSound,0,-2000);
				break;
			}
		}
		if(settings & SETTINGS_GAMEPLAY){
			if(player->vel.z < -0.3f){
				u16 fallDamage = (-player->vel.z-0.3f)*150.0f;
				if(fallDamage > player->health){
					playerDeath();
				}	
				else{
					player->health -= fallDamage;
				}
				switch(metadt6[crds2map(player->pos.x,player->pos.y,player->pos.z-player->hitboxHeight-0.1f)].r){
				case 0:
					playSound(boneBreakSound,0,0);
					break;
				case 1:
					playSound(boneBreakSound,0,0);
					break;
				}
			}
		}
		if(stepSoundCooldown < 0.0f){
			switch(metadt6[crds2map(player->pos.x,player->pos.y,player->pos.z-player->hitboxHeight-0.1f)].r){
			case 0:
				playSound(stepSound,1,0);
				break;
			case 1:
				playSound(stepEchoSound,1,0);
				break;
			}
			stepSoundCooldown = 2.0f;
		}
		else{
			stepSoundCooldown -= fabsf(player->vel.x) + fabsf(player->vel.y);
		}
		player->pos.z -= (1.0f-fmaxf(angleSteep.y,angleSteep.z)) * player->vel.z;
		player->pos.x -= angleSteep.y * player->vel.z;
		player->vel.x -= angleSteep.y * player->vel.z;
		player->pos.y -= angleSteep.z * player->vel.z;
		player->vel.y -= angleSteep.z * player->vel.z;
		player->vel.z *= fmaxf(angleSteep.y,angleSteep.z);
		if(touchStatus == 1){
			switch(effect){
			case 1:
				player->vel.z = (f32)effectdata/255.0f;
				effect = 0;
				break;
			}
		}
		if((GetKeyState(VK_SPACE) & 0x80) && touchedSpace == 0 && menuSel == 0){
			player->vel.z += player->jumpHeight * player->stamina;
			player->vel.x *= 1.7f * player->stamina;
			player->vel.y *= 1.7f * player->stamina;
			player->stamina = 0.0f;
		}
	}
	if(touchStatus & 0x02){
		player->pos.z -= (1.0f-fmaxf(angleSteep.y,angleSteep.z)) * player->vel.z;
		player->pos.x += angleSteep.y * player->vel.z;
		player->vel.x += angleSteep.y * player->vel.z;
		player->pos.y += angleSteep.z * player->vel.z;
		player->vel.y += angleSteep.z * player->vel.z;
		player->vel.z *= fmaxf(angleSteep.y,angleSteep.z);
	}
	if(touchStatus & 0x04){
		if(angleSteep.x){
			player->pos.x -= (1.0f-angleSteep.x) * player->vel.x;
			player->pos.y -= angleSteep.x * player->vel.x;
			player->vel.y -= angleSteep.x * player->vel.x;
			player->vel.x *= angleSteep.x;
		}
		if(angleSteep.y){
			player->pos.x -= player->vel.x * angleSteep.y;
			player->pos.z += angleSteep.y * (player->vel.x + 0.05f * (1.0f - angleSteep.y));
			player->vel.z += angleSteep.y * (player->vel.x + 0.05f * (1.0f - angleSteep.y));
			player->vel.x *= 1.0f - angleSteep.y;
			player->vel.y *= 1.1f;
		}
		if(!angleSteep.x && !angleSteep.y){
			player->pos.x -= player->vel.x;
			player->vel.x = 0;
		}
		if(player->vel.z < -0.05f){
			player->vel.z = -0.05f;
		}
		switch(effect){
		case 1:
			player->vel.x = (f32)effectdata/255.0f;
			effect = 0;
			break;
		}
		if(touchStatus == 0x04 && (GetKeyState(VK_SPACE) & 0x80) && touchedSpace == 0 && menuSel == 0){
			player->vel.z += player->jumpHeight * player->stamina;
			player->vel.x += player->jumpHeight * player->stamina;
			player->stamina = 0.0f;
		}
	}
	if(touchStatus & 0x08){
		if(angleSteep.x){
			player->pos.x -= (1.0f-angleSteep.x) * player->vel.x;
			player->pos.y -= angleSteep.x * player->vel.x;
			player->vel.y -= angleSteep.x * player->vel.x;
			player->vel.x *= angleSteep.x;
		}
		if(angleSteep.y){
			player->pos.x -= player->vel.x * angleSteep.y;
			player->pos.z += angleSteep.y * (player->vel.x + 0.05f * (1.0f - angleSteep.y));
			player->vel.z += angleSteep.y * (player->vel.x + 0.05f * (1.0f - angleSteep.y));
			player->vel.x *= 1.0f - angleSteep.y;
			player->vel.y *= 1.1f;
		}
		if(!angleSteep.x && !angleSteep.y){
			player->pos.x -= player->vel.x;
			player->vel.x = 0;
		}
		if(player->vel.z < -0.05f){
			player->vel.z = -0.05f;
		}
		switch(effect){
		case 1:
			player->vel.x = -(f32)effectdata/255.0f;
			effect = 0;
			break;
		}
		if(touchStatus == 0x08 && (GetKeyState(VK_SPACE) & 0x80) && touchedSpace == 0 && menuSel == 0){
			player->vel.z += player->jumpHeight * player->stamina;
			player->vel.x += -player->jumpHeight * player->stamina;
			player->stamina = 0.0f;
		}
	}
	if(touchStatus & 0x10){
		if(angleSteep.x){
			player->pos.y -= (1.0f-angleSteep.x) * player->vel.y;
			player->pos.x -= angleSteep.x * player->vel.y;
			player->vel.x -= angleSteep.x * player->vel.y;
			player->vel.y *= angleSteep.x;
		}
		if(angleSteep.z){
			player->pos.y -= player->vel.y * angleSteep.z;
			player->pos.z += angleSteep.z * (player->vel.y + 0.05f * (1.0f - angleSteep.z));
			player->vel.z += angleSteep.z * (player->vel.y + 0.05f * (1.0f - angleSteep.z));
			player->vel.y *= 1.0f - angleSteep.z;
			player->vel.x *= 1.1f;
		}
		if(!angleSteep.x && !angleSteep.z){
			player->pos.y -= player->vel.y;
			player->vel.y = 0.0f;
		}
		if(player->vel.z < -0.05f){
			player->vel.z = -0.05f;
		}
		switch(effect){
		case 1:
			player->vel.y = (f32)effectdata/255.0f;
			effect = 0;
			break;
		}
		if(touchStatus == 0x10 && (GetKeyState(VK_SPACE) & 0x80) && touchedSpace == 0 && menuSel == 0){
			player->vel.z += player->jumpHeight * player->stamina;
			player->vel.y += player->jumpHeight * player->stamina;
			player->stamina = 0.0f;
		}
	}
	if(touchStatus & 0x20){
		if(angleSteep.x){
			player->pos.y -= (1.0f-angleSteep.x) * player->vel.y;
			player->pos.x -= angleSteep.x * player->vel.y;
			player->vel.x -= angleSteep.x * player->vel.y;
			player->vel.y *= angleSteep.x;
		}
		if(angleSteep.z){
			player->pos.y -= player->vel.y * angleSteep.z;
			player->pos.z += angleSteep.z * (player->vel.y + 0.05f * (1.0f - angleSteep.z));
			player->vel.z += angleSteep.z * (player->vel.y + 0.05f * (1.0f - angleSteep.z));
			player->vel.y *= 1.0f - angleSteep.z;
			player->vel.x *= 1.2f;
		}
		if(!angleSteep.x && !angleSteep.z){
			player->pos.y -= player->vel.y;
			player->vel.y = 0;
		}
		switch(effect){
		case 1:
			player->vel.y = -(f32)effectdata/255.0f;
			effect = 0;
			break;
		}
		if(touchStatus == 0x20 && (GetKeyState(VK_SPACE) & 0x80) && touchedSpace == 0 && menuSel == 0){
			player->vel.z += player->jumpHeight * player->stamina;
			player->vel.y += -player->jumpHeight * player->stamina;
			player->stamina = 0.0f;
		}
	}
	if(player->hitboxHeight<player->hitboxWantedHeight && player->vel.z > -0.05f){
		player->hitboxHeight += player->vel.z;
		if(player->hitboxHeight>player->hitboxWantedHeight){
			player->hitboxHeight = player->hitboxWantedHeight;
			player->vel.z = 0.0f;
		}
		else{
			player->vel.z = 0.1f;
		}
	}
	else if(player->hitboxHeight>player->hitboxWantedHeight){
		player->hitboxHeight -= 0.1f;
		if(player->hitboxHeight<player->hitboxWantedHeight){
			player->hitboxHeight = player->hitboxWantedHeight;
		}
		else{
			player->vel.z = -0.1f + 0.015f;
		}
	}
	switch(map[crds2map(player->pos.x,player->pos.y,player->pos.z-player->hitboxHeight-0.1f)].id){
	case BLOCK_AIR:
		player->vel.x *= properties->airFrictionHor;
		player->vel.y *= properties->airFrictionHor;
		break;
	default:
		player->vel.x *= properties->groundFriction;
		player->vel.y *= properties->groundFriction;
		break;
	}
	if((GetKeyState(VK_SPACE)&0x80) && menuSel == 0){
		touchedSpace = 1;
	}
	touchStatus = 0;
	angleSteep = (VEC3){0.0f,0.0f,0.0f};
}
