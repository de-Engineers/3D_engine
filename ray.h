#pragma once

#include "vec2.h"
#include "vec3.h"

typedef struct{
  VEC2 pos;
  VEC2 dir;
  VEC2 delta;
  VEC2 side;
	int stepx;
	int stepy;
	int sid;
	int ix;
	int iy;
}RAY2D;

typedef struct{
	VEC3 pos;
	VEC3 dir;
	VEC3 delta;
	VEC3 side;
	int stepx;
	int stepy;
	int stepz;
	int sid;
	int ix;
	int iy;
	int iz;
}RAY3D;

RAY2D ray2dCreate(VEC2 pos,VEC2 dir){
	RAY2D ray;

	ray.pos = pos;
	ray.dir = dir;
	ray.delta = VEC2absR(VEC2divFR(ray.dir,1.0f));

	if(ray.dir.x < 0.0f){
		ray.stepx = -1;
		ray.side.x = (ray.pos.x-(int)ray.pos.x) * ray.delta.x;
	}
	else{
		ray.stepx = 1;
		ray.side.x = ((int)ray.pos.x + 1.0f - ray.pos.x) * ray.delta.x;
	}
	if(ray.dir.y < 0.0f){
		ray.stepy = -1;
		ray.side.y = (ray.pos.y-(int)ray.pos.y) * ray.delta.y;
	}
	else{
		ray.stepy = 1;
		ray.side.y = ((int)ray.pos.y + 1.0f - ray.pos.y) * ray.delta.y;
	}
	ray.ix = ray.pos.x;
	ray.iy = ray.pos.y;
	return ray;
}

RAY3D ray3dCreate(VEC3 pos,VEC3 dir){
	RAY3D ray;

	ray.pos = pos;
	ray.dir = dir;
	
	ray.delta = VEC3absR(VEC3divFR(ray.dir,1.0f));

	if(ray.dir.x < 0.0f){
		ray.stepx = -1;
		ray.side.x = (ray.pos.x-(int)ray.pos.x) * ray.delta.x;
	}
	else{
		ray.stepx = 1;
		ray.side.x = ((int)ray.pos.x + 1.0f - ray.pos.x) * ray.delta.x;
	}
	if(ray.dir.y < 0.0f){
		ray.stepy = -1;
		ray.side.y = (ray.pos.y-(int)ray.pos.y) * ray.delta.y;
	}
	else{
		ray.stepy = 1;
		ray.side.y = ((int)ray.pos.y + 1.0f - ray.pos.y) * ray.delta.y;
	}
	if(ray.dir.z < 0.0f){
		ray.stepz = -1;
		ray.side.z = (ray.pos.z-(int)ray.pos.z) * ray.delta.z;
	}
	else{
		ray.stepz = 1;
		ray.side.z = ((int)ray.pos.z + 1.0f - ray.pos.z) * ray.delta.z;
	}
	ray.ix = ray.pos.x;
	ray.iy = ray.pos.y;
	ray.iz = ray.pos.z;
	return ray;
}

void ray2dItterate(RAY2D *ray){
  if(ray->side.x < ray->side.y){
    ray->ix += ray->stepx;
    ray->side.x += ray->delta.x;
    ray->sid = 0;
  }
  else{
    ray->iy += ray->stepy;
    ray->side.y += ray->delta.y;
    ray->sid = 1;
  }
}

void ray3dItterate(RAY3D *ray){
  if(ray->side.x < ray->side.y){
      if(ray->side.x < ray->side.z){
        ray->ix += ray->stepx;
        ray->side.x += ray->delta.x;
        ray->sid = 0;
      }
      else{
        ray->iz += ray->stepz;
        ray->side.z += ray->delta.z;
        ray->sid = 2;
      }
  }
  else if(ray->side.y < ray->side.z){
    ray->iy += ray->stepy;
    ray->side.y += ray->delta.y;
    ray->sid = 1;
  }
  else{
    ray->iz += ray->stepz;
    ray->side.z += ray->delta.z;
    ray->sid = 2;
  }
}
