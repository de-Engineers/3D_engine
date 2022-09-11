#pragma once

#include "smallTypes.h"
#include "vec3.h"

typedef struct{
	VEC3 pos;
}NETWORKPLAYER;

extern NETWORKPLAYER networkplayer[8];
extern u8 networkplayerC;