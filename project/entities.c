#include "main.h"
#include <stdio.h>
#include <math.h>

#define cos45 (0.5 * sqrt(2))

char sprite;

max3f(float val1,float val2,float val3){
	if(val1 > val2){
		if(val1 > val3){
			return val1;
		}
		else{
			return val3;
		}
	}
	else if(val2 > val3){
		return val2;
	}
	else{
		return val3;
	}
}

int entityC;
ENTITY *entity;

void entityDeath(int id){
	for(int i = id;i < entityC;i++){
		entity[i] = entity[i+1];
	}
	entityC--;
}

unsigned int crds2map(int x,int y,int z){
	return ((int)x+(int)y*properties->lvlSz+(int)z*properties->lvlSz*properties->lvlSz);
}

VEC2 spriteLocation(int i){
  // XY
  	VEC2 result;

    float length;
    //  Variablen kleiner maken, is makkelijker (ff toevoegen btw)
float posX = player->ydir;
    float posXZ = player->zdir;
    float posY = player->xdir;
    float posZ = player->xydir;

	length = sqrt(posX * posX + posY * posY);

	float normDirX = posX / length;
	float normDirY = posY / length;

	float entityDirX = player->xpos - entity[i].x;
	float entityDirY = player->ypos - entity[i].y;

	length = sqrt(entityDirX * entityDirX + entityDirY * entityDirY);

	float normEntityX = entityDirX / length;
	float normEntityY = entityDirY / length;

	float angle = (normDirX * normEntityX + normDirY * normEntityY) * 2.0 * 0.3;
	
	result.x = angle / (cos45);

	length = sqrt(posXZ * posXZ + posZ * posZ);

	normDirX = posXZ / length;
	float normDirZ = posZ / length;

	entityDirX = player->xpos - entity[i].x;
	float entityDirZ = player->zpos - entity[i].z;

	length = sqrt(entityDirX * entityDirX + entityDirZ * entityDirZ);

	normEntityX = entityDirX / length;
	float normEntityZ = entityDirZ / length;

	angle = (normDirX * normEntityX + normDirZ * normEntityZ) * 2.0 * 0.3;
	
	result.y = angle / (cos45);

  return result;
}

float spriteSize(int i){
	float repx = entity[i].x-player->xpos;
	float repy = entity[i].y-player->ypos;
	float repz = entity[i].z-player->zpos;
	float s = sqrtf(repx*repx+repy*repy+repz*repz);
	return 2.0/s;
}


void entities(){

}







