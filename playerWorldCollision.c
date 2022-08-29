#include "main.h"
#include "sound.h"

u8 touchStatus;

volatile VEC3 angleSteep;

u8 effect;
u8 effectdata;

f32 stepSoundCooldown;

u8 touchedSpace;

void blockDetection(float x,float y,float z,int axis){
	int block = crds2map(x,y,z);
	VEC3 spos = VEC3fractR((VEC3){x,y,z});
	switch(map[block].id){
	case 3:
		break;
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
			if(metadt3[block].r == 0 && metadt3[block].id == 0 && metadt3[block].g == 0 && (f32)metadt[block].g/255.0f + (f32)metadt2[block].g/255.0f + (u32)z > player->zpos - 1.7f && 
				(f32)metadt[block].g/255.0f + (f32)metadt2[block].g/255.0f + (u32)z < player->zpos - 1.2f){
				player->zpos += (f32)metadt[block].g/255.0f + (f32)metadt2[block].g/255.0f + (u32)z - player->zpos + 1.8f;
				player->zvel = 0.0f;
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
	case BLOCK_REFLECTIVE:
	case BLOCK_SOLID:
		if((u32)z < player->zpos-1.7f && (u32)z > player->zpos-2.5f){
			switch(map[block+properties->lvlSz*properties->lvlSz].id){
			case BLOCK_CLIP:
			case BLOCK_SOLID:
			case BLOCK_REFLECTIVE:
				touchStatus |= axis;
				break;
			default:{
				u32 standingBlock = crds2map(player->xpos,player->ypos,player->zpos) - (u32)player->hitboxHeight * properties->lvlSz * properties->lvlSz;
				switch(map[standingBlock].id){
				case 1:
					touchStatus |= axis;
					break;
				default:
					player->zpos += player->zpos - z - 1.2f;
					player->zvel = 0.0f;
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

void hitboxZdown(float x,float y,float z){
	x -= player->xvel;
	y -= player->yvel;
	z += player->zvel;
	blockDetection(x,y,z,1);
}

void hitboxZup(float x,float y,float z){
	x -= player->xvel;
	y -= player->yvel;
	z += player->zvel;
	blockDetection(x,y,z,2);
}

void hitboxXdown(float x,float y,float z){
	x += player->xvel;
	y -= player->yvel;
	z -= player->zvel;
	blockDetection(x,y,z,4);
}

void hitboxXup(float x,float y,float z){
	x += player->xvel;
	y -= player->yvel;
	z -= player->zvel;
	blockDetection(x,y,z,8);
}

void hitboxYdown(float x,float y,float z){
	y += player->yvel;
	x -= player->xvel;
	z -= player->zvel;
	blockDetection(x,y,z,16);
}

void hitboxYup(float x,float y,float z){
	y += player->yvel;
	x -= player->xvel;
	z -= player->zvel;
	blockDetection(x,y,z,32);
}

void playerWorldCollision(){
	if(player->stamina < 1.0f){
		player->stamina+= 0.045f;
	}
	else{
		player->stamina = 1.0f;
	}
	if(player->zvel < 0.0f){
		for(float i = -0.2f;i <= 0.2f;i+=0.1f){
			for(float i2 = -0.2f;i2 <= 0.2f;i2+=0.05f){
				hitboxZdown(player->xpos + i, player->ypos + i2, player->zpos - player->hitboxHeight);
			}
		}
	}
	else{
		for(float i = -0.2f;i <= 0.2f;i+=0.1f){
			for(float i2 = -0.2f;i2 <= 0.2f;i2+=0.05f){
				hitboxZup(player->xpos + i, player->ypos + i2, player->zpos + 0.2f);
			}
		}
	}
	if(player->xvel < 0.0f){
		for(float i = -player->hitboxHeight;i <= 0.2;i+=0.1){
			for(float i2 = -0.2;i2 <= 0.2;i2+=0.05){
				hitboxXdown(player->xpos - 0.2f, player->ypos + i2, player->zpos + i);
			}
		}

	}
	else{
		for(float i = -player->hitboxHeight;i <= 0.2;i+=0.1){
			for(float i2 = -0.2;i2 <= 0.2;i2+=0.05){
				hitboxXup(player->xpos + 0.2f, player->ypos + i2, player->zpos + i);
			}
		}
	}
	if(player->yvel < 0.0f){
		for(float i = -player->hitboxHeight;i <= 0.2f;i+=0.1f){
			for(float i2 = -0.2f;i2 <= 0.2f;i2+=0.05f){
				hitboxYdown(player->xpos + i2, player->ypos - 0.2f, player->zpos + i);
			}
		}

	}
	else{
		for(float i = -player->hitboxHeight;i <= 0.2f;i+=0.1f){
			for(float i2 = -0.2f;i2 <= 0.2f;i2+=0.05f){
				hitboxYup(player->xpos + i2, player->ypos + 0.2f, player->zpos + i);
			}
		}
	}
	if(touchStatus & 0x01){
		if(player->hitboxWantedHeightQueued>player->hitboxWantedHeight){
			player->hitboxWantedHeight = player->hitboxWantedHeightQueued;
			player->hitboxWantedHeightQueued = 0.0f;
		}
		if(player->zvel < -0.1f){
			switch(metadt6[crds2map(player->xpos,player->ypos,player->zpos-player->hitboxHeight-0.1f)].r){
			case 0:
				playSound(landSound,0,-2000);
				break;
			case 1:
				playSound(landEchoSound,0,-2000);
				break;
			}
		}
		if(player->zvel < -0.3f){
			if(!player->wounded){
				player->wounded = 1;
			}
			else{
				playerDeath();
			}
			switch(metadt6[crds2map(player->xpos,player->ypos,player->zpos-player->hitboxHeight-0.1f)].r){
			case 0:
				playSound(boneBreakSound,0,0);
				break;
			case 1:
				playSound(boneBreakSound,0,0);
				break;
			}
		}
		if(stepSoundCooldown < 0.0f){
			switch(metadt6[crds2map(player->xpos,player->ypos,player->zpos-player->hitboxHeight-0.1f)].r){
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
			stepSoundCooldown -= fabsf(player->xvel) + fabsf(player->yvel);
		}
		player->zpos -= (1.0f-fmaxf(angleSteep.y,angleSteep.z)) * player->zvel;
		player->xpos -= angleSteep.y * player->zvel;
		player->xvel -= angleSteep.y * player->zvel;
		player->ypos -= angleSteep.z * player->zvel;
		player->yvel -= angleSteep.z * player->zvel;
		player->zvel *= fmaxf(angleSteep.y,angleSteep.z);
		if(touchStatus == 1){
			switch(effect){
			case 1:
				player->zvel = (f32)effectdata/255.0f;
				effect = 0;
				break;
			}
		}
		if(GetKeyState(VK_SPACE) & 0x80 && touchedSpace == 0){
			player->zvel += 0.2f * player->stamina;
			player->xvel *= 1.7f * player->stamina;
			player->yvel *= 1.7f * player->stamina;
			player->stamina = 0.0;
		}
	}
	if(touchStatus & 0x02){
		player->zpos -= (1.0f-fmaxf(angleSteep.y,angleSteep.z)) * player->zvel;
		player->xpos += angleSteep.y * player->zvel;
		player->xvel += angleSteep.y * player->zvel;
		player->ypos += angleSteep.z * player->zvel;
		player->yvel += angleSteep.z * player->zvel;
		player->zvel *= fmaxf(angleSteep.y,angleSteep.z);
	}
	if(touchStatus & 0x04){
		if(angleSteep.x){
			player->xpos -= (1.0f-angleSteep.x) * player->xvel;
			player->ypos -= angleSteep.x * player->xvel;
			player->yvel -= angleSteep.x * player->xvel;
			player->xvel *= angleSteep.x;
		}
		if(angleSteep.y){
			player->xpos -= player->xvel * angleSteep.y;
			player->zpos += angleSteep.y * (player->xvel + 0.05f * (1.0f - angleSteep.y));
			player->zvel += angleSteep.y * (player->xvel + 0.05f * (1.0f - angleSteep.y));
			player->xvel *= 1.0f - angleSteep.y;
			player->yvel *= 1.1f;
		}
		if(!angleSteep.x && !angleSteep.y){
			player->xpos -= player->xvel;
			player->xvel = 0;
		}
		if(player->zvel < -0.05){
			player->zvel = -0.05;
		}
		switch(effect){
		case 1:
			player->xvel = (f32)effectdata/255.0f;
			effect = 0;
			break;
		}
		if(touchStatus == 0x04 && GetKeyState(VK_SPACE) & 0x80 && touchedSpace == 0){
			player->zvel += 0.25 * player->stamina;
			player->xvel += 0.25 * player->stamina;
			player->stamina = 0.0;
		}
	}
	if(touchStatus & 0x08){
		if(angleSteep.x){
			player->xpos -= (1.0f-angleSteep.x) * player->xvel;
			player->ypos -= angleSteep.x * player->xvel;
			player->yvel -= angleSteep.x * player->xvel;
			player->xvel *= angleSteep.x;
		}
		if(angleSteep.y){
			player->xpos -= player->xvel * angleSteep.y;
			player->zpos += angleSteep.y * (player->xvel + 0.05f * (1.0f - angleSteep.y));
			player->zvel += angleSteep.y * (player->xvel + 0.05f * (1.0f - angleSteep.y));
			player->xvel *= 1.0f - angleSteep.y;
			player->yvel *= 1.1f;
		}
		if(!angleSteep.x && !angleSteep.y){
			player->xpos -= player->xvel;
			player->xvel = 0;
		}
		if(player->zvel < -0.05f){
			player->zvel = -0.05f;
		}
		switch(effect){
		case 1:
			player->xvel = -(f32)effectdata/255.0f;
			effect = 0;
			break;
		}
		if(touchStatus == 0x08 && GetKeyState(VK_SPACE) & 0x80 && touchedSpace == 0){
			player->zvel += 0.25f * player->stamina;
			player->xvel += -0.25f * player->stamina;
			player->stamina = 0.0f;
		}
	}
	if(touchStatus & 0x10){
		if(angleSteep.x){
			player->ypos -= (1.0f-angleSteep.x) * player->yvel;
			player->xpos -= angleSteep.x * player->yvel;
			player->xvel -= angleSteep.x * player->yvel;
			player->yvel *= angleSteep.x;
		}
		if(angleSteep.z){
			player->ypos -= player->yvel * angleSteep.z;
			player->zpos += angleSteep.z * (player->yvel + 0.05f * (1.0f - angleSteep.z));
			player->zvel += angleSteep.z * (player->yvel + 0.05f * (1.0f - angleSteep.z));
			player->yvel *= 1.0f - angleSteep.z;
			player->xvel *= 1.1f;
		}
		if(!angleSteep.x && !angleSteep.z){
			player->ypos -= player->yvel;
			player->yvel = 0;
		}
		if(player->zvel < -0.05){
			player->zvel = -0.05;
		}
		switch(effect){
		case 1:
			player->yvel = (f32)effectdata/255.0f;
			effect = 0;
			break;
		}
		if(touchStatus == 0x10 && GetKeyState(VK_SPACE) & 0x80 && touchedSpace == 0){
			player->zvel += 0.25f * player->stamina;
			player->yvel += 0.25f * player->stamina;
			player->stamina = 0.0f;
		}
	}
	if(touchStatus & 0x20){
		if(angleSteep.x){
			player->ypos -= (1.0f-angleSteep.x) * player->yvel;
			player->xpos -= angleSteep.x * player->yvel;
			player->xvel -= angleSteep.x * player->yvel;
			player->yvel *= angleSteep.x;
		}
		if(angleSteep.z){
			player->ypos -= player->yvel * angleSteep.z;
			player->zpos += angleSteep.z * (player->yvel + 0.05f * (1.0f - angleSteep.z));
			player->zvel += angleSteep.z * (player->yvel + 0.05f * (1.0f - angleSteep.z));
			player->yvel *= 1.0f - angleSteep.z;
			player->xvel *= 1.2f;
		}
		if(!angleSteep.x && !angleSteep.z){
			player->ypos -= player->yvel;
			player->yvel = 0;
		}
		switch(effect){
		case 1:
			player->yvel = -(f32)effectdata/255.0f;
			effect = 0;
			break;
		}
		if(touchStatus == 0x20 && GetKeyState(VK_SPACE) & 0x80 && touchedSpace == 0){
			player->zvel += 0.25 * player->stamina;
			player->yvel += -0.25 * player->stamina;
			player->stamina = 0.0;
		}
	}
	if(player->zpos < 1.81f){
		playerDeath();
	}
	if(player->hitboxHeight<player->hitboxWantedHeight){
		player->hitboxHeight += player->zvel;
		if(player->hitboxHeight>player->hitboxWantedHeight){
			player->hitboxHeight = player->hitboxWantedHeight;
			player->zvel = 0.0f;
		}
		else{
			player->zvel = 0.1f;
		}
	}
	else if(player->hitboxHeight>player->hitboxWantedHeight){
		player->hitboxHeight -= 0.1f;
		if(player->hitboxHeight<player->hitboxWantedHeight){
			player->hitboxHeight = player->hitboxWantedHeight;
		}
		else{
			player->zvel = -0.1f + 0.015f;
		}
	}
	switch(map[crds2map(player->xpos,player->ypos,player->zpos-player->hitboxHeight-0.1f)].id){
	case BLOCK_CLIP:
	case BLOCK_CUBE:
	case BLOCK_SOLID:
	case BLOCK_GLASS:
		player->xvel /= 1.12;
		player->yvel /= 1.12;
		break;
	default:
		player->xvel /= 1.06;
		player->yvel /= 1.06;
		break;
	}
	if(GetKeyState(VK_SPACE)&0x80){
		touchedSpace = 1;
	}
	touchStatus = 0;
	angleSteep = (VEC3){0.0f,0.0f,0.0f};
}
