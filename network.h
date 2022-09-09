#pragma once

#include "smallTypes.h"
#include "vec3.h"

typedef struct{
	u8 p1;
	u8 p2;
	u8 p3;
	u8 p4;
}IPADDRESS;

typedef struct{
	VEC3 pos;
}NETWORKPLAYER;

extern NETWORKPLAYER networkplayer[8];
extern u8 networkplayerC;
extern IPADDRESS serverIP;