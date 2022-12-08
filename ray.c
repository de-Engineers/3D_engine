#include "main.h"
#include "ray.h"
#include "raytracing.h"

RAY2D ray2dCreate(VEC2 pos,VEC2 dir){
	RAY2D ray;

	ray.pos = pos;
	ray.dir = dir;
	ray.delta = VEC2absR(VEC2divFR(ray.dir,1.0f));

	if(ray.dir.x < 0.0f){
		ray.step.x = -1;
		ray.side.x = (ray.pos.x-(int)ray.pos.x) * ray.delta.x;
	}
	else{
		ray.step.x = 1;
		ray.side.x = ((int)ray.pos.x + 1.0f - ray.pos.x) * ray.delta.x;
	}
	if(ray.dir.y < 0.0f){
		ray.step.y = -1;
		ray.side.y = (ray.pos.y-(int)ray.pos.y) * ray.delta.y;
	}
	else{
		ray.step.y = 1;
		ray.side.y = ((int)ray.pos.y + 1.0f - ray.pos.y) * ray.delta.y;
	}
	ray.roundPos.x = ray.pos.x;
	ray.roundPos.y = ray.pos.y;
	return ray;
}

RAY3D ray3dCreate(VEC3 pos,VEC3 dir){
	RAY3D ray;

	ray.pos = pos;
	ray.dir = dir;

	ray.delta = VEC3absR(VEC3divFR(ray.dir,1.0f));

	if(ray.dir.x < 0.0f){
		ray.step.x = -1;
		ray.side.x = (ray.pos.x-(int)ray.pos.x) * ray.delta.x;
	}
	else{
		ray.step.x = 1;
		ray.side.x = ((int)ray.pos.x + 1.0f - ray.pos.x) * ray.delta.x;
	}
	if(ray.dir.y < 0.0f){
		ray.step.y = -1;
		ray.side.y = (ray.pos.y-(int)ray.pos.y) * ray.delta.y;
	}
	else{
		ray.step.y = 1;
		ray.side.y = ((int)ray.pos.y + 1.0f - ray.pos.y) * ray.delta.y;
	}
	if(ray.dir.z < 0.0f){
		ray.step.z = -1;
		ray.side.z = (ray.pos.z-(int)ray.pos.z) * ray.delta.z;
	}
	else{
		ray.step.z = 1;
		ray.side.z = ((int)ray.pos.z + 1.0f - ray.pos.z) * ray.delta.z;
	}
	ray.roundPos.x = ray.pos.x;
	ray.roundPos.y = ray.pos.y;
	ray.roundPos.z = ray.pos.z;
	return ray;
}

void ray2dItterate(RAY2D *ray){
	if(ray->side.x < ray->side.y){
		ray->roundPos.x += ray->step.x;
		ray->side.x += ray->delta.x;
		ray->hitSide = 0;
	}
	else{
		ray->roundPos.y += ray->step.y;
		ray->side.y += ray->delta.y;
		ray->hitSide = 1;
	}
}

void ray3dItterate(RAY3D *ray){
	if(ray->side.x < ray->side.y){
		if(ray->side.x < ray->side.z){
			ray->roundPos.x += ray->step.x;
			ray->side.x += ray->delta.x;
			ray->hitSide = 0;
		}
		else{
			ray->roundPos.z += ray->step.z;
			ray->side.z += ray->delta.z;
			ray->hitSide = 2;
		}
	}
	else if(ray->side.y < ray->side.z){
		ray->roundPos.y += ray->step.y;
		ray->side.y += ray->delta.y;
		ray->hitSide = 1;
	}
	else{
		ray->roundPos.z += ray->step.z;
		ray->side.z += ray->delta.z;
		ray->hitSide = 2;
	}
}

i32 getLmapLocation(RAY3D *ray){
	while(ray->roundPos.x >= 0 && ray->roundPos.y >= 0 && ray->roundPos.z >= 0 && ray->roundPos.x < properties->lvlSz && ray->roundPos.y < properties->lvlSz && ray->roundPos.z < properties->lvlSz){
		u32 block = crds2map(ray->roundPos.x,ray->roundPos.y,ray->roundPos.z);
		switch(map[block].id){
		case BLOCK_CUBE:{
			VEC3 spos  = getSubCoords(*ray);
			VEC3 mtdt  = {(f32)metadt[block].r/255.0f ,(f32)metadt[block].g/255.0f ,(f32)metadt[block].id/255.0f };
			VEC3 mtdt2 = {(f32)metadt2[block].r/255.0f,(f32)metadt2[block].g/255.0f,(f32)metadt2[block].id/255.0f};
			VEC3 mtdt3 = {(f32)metadt3[block].r/255.0f,(f32)metadt3[block].g/255.0f,(f32)metadt3[block].id/255.0f};
			VEC3 angr = ray->dir;
			(VEC2){angr.x,angr.y} = rotVEC2((VEC2){angr.x,angr.y},mtdt3.x*PI);
			(VEC2){angr.x,angr.z} = rotVEC2((VEC2){angr.x,angr.z},mtdt3.y*PI);
			(VEC2){angr.y,angr.z} = rotVEC2((VEC2){angr.y,angr.z},mtdt3.z*PI);
			(VEC2){spos.x,spos.y} = rotVEC2((VEC2){spos.x,spos.y},mtdt3.x*PI);
			(VEC2){spos.x,spos.z} = rotVEC2((VEC2){spos.x,spos.z},mtdt3.y*PI);
			(VEC2){spos.y,spos.z} = rotVEC2((VEC2){spos.y,spos.z},mtdt3.z*PI);
			(VEC2){mtdt.x,mtdt.y} = rotVEC2((VEC2){mtdt.x,mtdt.y},mtdt3.x*PI);
			(VEC2){mtdt.x,mtdt.z} = rotVEC2((VEC2){mtdt.x,mtdt.z},mtdt3.y*PI);
			(VEC2){mtdt.y,mtdt.z} = rotVEC2((VEC2){mtdt.y,mtdt.z},mtdt3.z*PI);
			f32 d = iBox(VEC3subVEC3R(spos,mtdt),angr,mtdt2);
			if(d > 0.0f){
				VEC3addVEC3(&spos,VEC3mulR(angr,d));
				VEC3 nspos = spos;
				(VEC2){nspos.x,nspos.y} = rotVEC2((VEC2){nspos.y,spos.z},-mtdt3.z*PI);
				(VEC2){nspos.x,nspos.z} = rotVEC2((VEC2){nspos.x,spos.z},-mtdt3.y*PI);
				(VEC2){nspos.y,nspos.z} = rotVEC2((VEC2){nspos.x,spos.y},-mtdt3.x*PI);
				if(nspos.x>=0.0f&&nspos.y>=0.0f&&nspos.z>=0.0f&&nspos.x<=1.0f&&nspos.y<=1.0f&&nspos.z<=1.0f){
					if(spos.x>mtdt.x-mtdt2.x-0.0001f&&spos.x<mtdt.x-mtdt2.x+0.0001f){
						f32 mt = fmaxf(mtdt2.y,mtdt2.z)*2.0f;
						IVEC2 offsetb = {(spos.y-mtdt.y+mtdt2.y)*properties->lmapSz/mt,(spos.z-mtdt.z+mtdt2.z)*properties->lmapSz/mt};
						u16 offsetf = offsetb.x + offsetb.y * properties->lmapSz;
						return lpmap[block].p1*properties->lmapSz*properties->lmapSz+offsetf;
					}
					else if(spos.y>mtdt.y-mtdt2.y-0.0001f&&spos.y<mtdt.y-mtdt2.y+0.0001f){
						f32 mt = fmaxf(mtdt2.x,mtdt2.z)*2.0f;
						IVEC2 offsetb = {(spos.x-mtdt.x+mtdt2.x)*properties->lmapSz/mt,(spos.z-mtdt.z+mtdt2.z)*properties->lmapSz/mt};
						u16 offsetf = offsetb.x + offsetb.y * properties->lmapSz;
						return lpmap[block].p2*properties->lmapSz*properties->lmapSz+offsetf;
					}
					else if(spos.z>mtdt.z-mtdt2.z-0.0001f&&spos.z<mtdt.z-mtdt2.z+0.0001f){
						f32 mt = fmaxf(mtdt2.x,mtdt2.y)*2.0f;
						IVEC2 offsetb = {(spos.x-mtdt.x+mtdt2.x)*properties->lmapSz/mt,(spos.y-mtdt.y+mtdt2.y)*properties->lmapSz/mt};
						u16 offsetf = offsetb.x + offsetb.y * properties->lmapSz;
						return lpmap[block].p3*properties->lmapSz*properties->lmapSz+offsetf;
					}
					else if(spos.x>mtdt.x+mtdt2.x-0.0001f&&spos.x<mtdt.x+mtdt2.x+0.0001f){
						f32 mt = fmaxf(mtdt2.y,mtdt2.z)*2.0f;
						IVEC2 offsetb = {(spos.y-mtdt.y+mtdt2.y)*properties->lmapSz/mt,(spos.z-mtdt.z+mtdt2.z)*properties->lmapSz/mt};
						u16 offsetf = offsetb.x + offsetb.y * properties->lmapSz;
						return lpmap[block].p4*properties->lmapSz*properties->lmapSz+offsetf;
					}
					else if(spos.y>mtdt.y+mtdt2.y-0.0001f&&spos.y<mtdt.y+mtdt2.y+0.0001f){
						f32 mt = fmaxf(mtdt2.x,mtdt2.z)*2.0f;
						IVEC2 offsetb = {(spos.x-mtdt.x+mtdt2.x)*properties->lmapSz/mt,(spos.z-mtdt.z+mtdt2.z)*properties->lmapSz/mt};
						u16 offsetf = offsetb.x + offsetb.y * properties->lmapSz;
						return lpmap[block].p5*properties->lmapSz*properties->lmapSz+offsetf;
					}
					else if(spos.z>mtdt.z+mtdt2.z-0.0001f&&spos.z<mtdt.z+mtdt2.z+0.0001f){
						f32 mt = fmaxf(mtdt2.x,mtdt2.y)*2.0f;
						IVEC2 offsetb = {(spos.x-mtdt.x+mtdt2.x)*properties->lmapSz/mt,(spos.y-mtdt.y+mtdt2.y)*properties->lmapSz/mt};
						u16 offsetf = offsetb.x + offsetb.y * properties->lmapSz;
						return lpmap[block].p6*properties->lmapSz*properties->lmapSz+offsetf;
					}
				}
			}
			}
			break;
		case BLOCK_REFLECTIVE2:
		case BLOCK_REFLECTIVE:
		case BLOCK_SOLID:{
			VEC2 wall;
			switch(ray->hitSide){
			case 0:{
				wall.x = fract(ray->pos.y + (ray->side.x - ray->delta.x) * ray->dir.y);
				wall.y = fract(ray->pos.z + (ray->side.x - ray->delta.x) * ray->dir.z);
				u32 offset = (int)(wall.y*properties->lmapSz)*properties->lmapSz+(int)(wall.x*properties->lmapSz);
				if(ray->dir.x>0.0f){
					return lpmap[block].p1*properties->lmapSz*properties->lmapSz+offset;
				}
				else{
					return lpmap[block].p2*properties->lmapSz*properties->lmapSz+offset;
				}
				break;
			}
			case 1:{
				wall.x = fract(ray->pos.x + (ray->side.y - ray->delta.y) * ray->dir.x);
				wall.y = fract(ray->pos.z + (ray->side.y - ray->delta.y) * ray->dir.z);
				u32 offset = (int)(wall.y*properties->lmapSz)*properties->lmapSz+(int)(wall.x*properties->lmapSz);
				if(ray->dir.y>0.0f){
					return lpmap[block].p3*properties->lmapSz*properties->lmapSz+offset;
				}
				else{
					return lpmap[block].p4*properties->lmapSz*properties->lmapSz+offset;
				}
				break;
			}
			case 2:{
				wall.x = fract(ray->pos.x + (ray->side.z - ray->delta.z) * ray->dir.x);
				wall.y = fract(ray->pos.y + (ray->side.z - ray->delta.z) * ray->dir.y);
				u32 offset = (int)(wall.y*properties->lmapSz)*properties->lmapSz+(int)(wall.x*properties->lmapSz);
				if(ray->dir.z>0.0f){
					return lpmap[block].p5*properties->lmapSz*properties->lmapSz+offset;
				}
				else{
					return lpmap[block].p6*properties->lmapSz*properties->lmapSz+offset;
				}
				break;
			}
			}
		}
		}
		ray3dItterate(ray);
	}
	return -1;
}

VEC3 getSubCoords(RAY3D ray){
	VEC2 wall;
	switch(ray.hitSide){
	case 0:
		wall.x = fract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y);
		wall.y = fract(ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z);
		if(ray.dir.x > 0.0f){
			return (VEC3){ 0.0f,wall.x,wall.y };
		}
		else{
			return (VEC3){ 1.0f,wall.x,wall.y };
		}
	case 1:
		wall.x = fract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x);
		wall.y = fract(ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z);
		if(ray.dir.y > 0.0f){
			return (VEC3){ wall.x,0.0f,wall.y };
		}
		else{
			return (VEC3){ wall.x,1.0f,wall.y };
		}
	case 2:
		wall.x = fract(ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x);
		wall.y = fract(ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y);
		if(ray.dir.z > 0.0f){
			return (VEC3){ wall.x,wall.y,0.0f };
		}
		else{
			return (VEC3){ wall.x,wall.y,1.0f };
		}
	}
}

VEC3 getCoords(RAY3D ray){
	VEC2 wall;
	switch(ray.hitSide){
	case 0:
		wall.x = ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y;
		wall.y = ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z;
		if(ray.dir.x > 0.0f){
			return (VEC3){ ray.roundPos.x,wall.x,wall.y };
		}
		else{
			return (VEC3){ ray.roundPos.x+1.0f,wall.x,wall.y };
		}
	case 1:
		wall.x = ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x;
		wall.y = ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z;
		if(ray.dir.y > 0.0f){
			return (VEC3){ wall.x,ray.roundPos.y,wall.y };
		}
		else{
			return (VEC3){ wall.x,ray.roundPos.y+1.0f,wall.y };
		}
	case 2:
		wall.x = ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x;
		wall.y = ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y;
		if(ray.dir.z > 0.0f){
			return (VEC3){ wall.x,wall.y,ray.roundPos.z };
		}
		else{
			return (VEC3){ wall.x,wall.y,ray.roundPos.z+1.0f };
		}
	}
}

VEC2 getXside(RAY3D ray){
	VEC2 r;
	r.x = ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y;
	r.y = ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z;
	return r;
}

VEC2 getYside(RAY3D ray){
	VEC2 r;
	r.x = ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x;
	r.y = ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z;
	return r;
}

VEC2 getZside(RAY3D ray){
	VEC2 r;
	r.x = ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x;
	r.y = ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y;
	return r;
}