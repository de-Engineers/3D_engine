#pragma once

#define MAXPLAYERS 8

#include "smallTypes.h"
#include "vec3.h"

typedef struct{
	VEC3 pos;
}NETWORKPLAYER;

typedef struct{
	VEC3 posBuf;
	VEC3 vel;
}LAGCOMP;

typedef struct{
	LAGCOMP lagcomp[MAXPLAYERS];
	NETWORKPLAYER player[MAXPLAYERS];
}NETWORKDATA;

extern NETWORKDATA networkplayer;
extern u8 networkplayerC;
extern u8 connectStatus;