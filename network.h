#pragma once

#define MAXPLAYERS 8

#include "smallTypes.h"
#include "vec3.h"

typedef struct{
	VEC3 pos;
	f32 rot;
	VEC3 vel;
}NETWORKPLAYER;

typedef struct{
	VEC3 posBuf;
	VEC3 vel;
}LAGCOMP;

typedef struct{
	LAGCOMP lagcomp[MAXPLAYERS];
	NETWORKPLAYER player[MAXPLAYERS];
}NETWORKDATA;

typedef struct{
	VEC3 pos1;
	VEC3 pos2;
}PACKETDATA;

extern NETWORKDATA networkplayer;

extern PACKETDATA packetdata;

extern u8 networkplayerC;
extern u8 connectStatus;

extern u8 packetID;

extern u8 *playerName;