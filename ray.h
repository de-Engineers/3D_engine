#pragma once

#include "ivec2.h"

typedef struct{
	VEC2 pos;
	VEC2 dir;
	VEC2 delta;
	VEC2 side;

	IVEC2 step;
	IVEC2 roundPos;

	int hitSide;
}RAY2D;

typedef struct{
	VEC3 pos;
	VEC3 dir;
	VEC3 delta;
	VEC3 side;

	IVEC3 step;
	IVEC3 roundPos;

	int hitSide;
}RAY3D;

RAY2D ray2dCreate(VEC2 pos,VEC2 dir);
RAY3D ray3dCreate(VEC3 pos,VEC3 dir);
void ray2dItterate(RAY2D *ray);
void ray3dItterate(RAY3D *ray);
i32 getLmapLocation(RAY3D *ray);
VEC3 getSubCoords(RAY3D ray);
VEC3 getCoords(RAY3D ray);
VEC2 getXside(RAY3D ray);
VEC2 getYside(RAY3D ray);
VEC2 getZside(RAY3D ray);
