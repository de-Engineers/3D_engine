#pragma once

#include "ivec3.h"
#include "vec3.h"
#include "smallTypes.h"

typedef struct{
	VEC3 pos;
	VEC3 rot;
	VEC3 size;
}BLOCKPROP;

typedef struct{
	BLOCKPROP buf;
	BLOCKPROP vel;
	BLOCKPROP temp;
	u32 block;
	u32 itt;
}BLOCKMOVE;

extern BLOCKMOVE *blockmove;

void createMoveBlock(u32 block);
void moveBlocks();