#pragma once
#include <math.h>

typedef struct{
	union{
		float x;
		float r;
	};
	union{
		float y;
		float g;
	};
	union{
		float z;
		float b;
	};
}VEC3;

inline void VEC3add(VEC3 *p,float d){
	p->x += d;
	p->y += d;
	p->z += d;
}

inline void VEC3sub(VEC3 *p,float d){
	p->x -= d;
	p->y -= d;
	p->z -= d;
}

inline void VEC3mul(VEC3 *p,float d){
	p->x *= d;
	p->y *= d;
	p->z *= d;
}

inline void VEC3div(VEC3 *p,float d){
	p->x /= d;
	p->y /= d;
	p->z /= d;
}

inline VEC3 VEC3addR(VEC3 p,float d){
	p.x += d;
	p.y += d;
	p.z += d;
	return p;
}

inline VEC3 VEC3subR(VEC3 p,float d){
	p.x -= d;
	p.y -= d;
	p.z += d;
	return p;
}

inline VEC3 VEC3mulR(VEC3 p,float d){
	p.x *= d;
	p.y *= d;
	p.z *= d;
	return p;
}

inline VEC3 VEC3divR(VEC3 p,float d){
	p.x /= d;
	p.y /= d;
	p.z /= d;
	return p;
}

inline VEC3 VEC3addFR(VEC3 p,float d){
	p.x = d + p.x;
	p.y = d + p.y;
	p.z = d + p.z;
	return p;
}

inline VEC3 VEC3subFR(VEC3 p,float d){
	p.x = d - p.x;
	p.y = d - p.y;
	p.z = d - p.z;
	return p;
}

inline VEC3 VEC3mulFR(VEC3 p,float d){
	p.x = d * p.x;
	p.y = d * p.y;
	p.z = d * p.z;
	return p;
}

inline VEC3 VEC3divFR(VEC3 p,float d){
	p.x = d / p.x;
	p.y = d / p.y;
	p.z = d / p.z;
	return p;
}

inline void VEC3addVEC3(VEC3 *p,VEC3 p2){
	p->x += p2.x;
	p->y += p2.y;
	p->z += p2.z;
}

inline void VEC3subVEC3(VEC3 *p,VEC3 p2){
	p->x -= p2.x;
	p->y -= p2.y;
	p->z -= p2.z;
}

inline void VEC3mulVEC3(VEC3 *p,VEC3 p2){
	p->x *= p2.x;
	p->y *= p2.y;
	p->z *= p2.z;
}

inline void VEC3divVEC3(VEC3 *p,VEC3 p2){
	p->x /= p2.x;
	p->y /= p2.y;
	p->z /= p2.z;
}

inline VEC3 VEC3addVEC3R(VEC3 p,VEC3 p2){
	VEC3 r;
	r.x = p.x + p2.x;
	r.y = p.y + p2.y;
	r.z = p.z + p2.z;
	return r;
}

inline VEC3 VEC3subVEC3R(VEC3 p,VEC3 p2){
	VEC3 r;
	r.x = p.x - p2.x;
	r.y = p.y - p2.y;
	r.z = p.z - p2.z;
	return r;
}

inline VEC3 VEC3mulVEC3R(VEC3 p,VEC3 p2){
	VEC3 r;
	r.x = p.x * p2.x;
	r.y = p.y * p2.y;
	r.z = p.z * p2.z;
	return r;
}

inline VEC3 VEC3divVEC3R(VEC3 p,VEC3 p2){
	VEC3 r;
	r.x = p.x / p2.x;
	r.y = p.y / p2.y;
	r.z = p.z / p2.z;
	return r;
}

inline float VEC3dist(VEC3 p,VEC3 p2){
	VEC3subVEC3(&p,p2);
	return sqrtf(p.x*p.x+p.y*p.y+p.z*p.z);
}

inline float VEC3dot(VEC3 p,VEC3 p2){
	return p.x * p2.x + p.y * p2.y + p.z * p2.z;
}

inline VEC3 VEC3reflect(VEC3 p,VEC3 p2){
	VEC3 r;
	float d = VEC3dot(p,p2);
	r.x = p.x - 2.0f * d * p2.x;
	r.y = p.y - 2.0f * d * p2.y;
	r.z = p.z - 2.0f * d * p2.z;
	return r;
}

inline float VEC3length(VEC3 p){
	return sqrtf(p.x*p.x+p.y*p.y+p.z*p.z);
}

inline VEC3 VEC3normalize(VEC3 p){
	float w = VEC3length(p);
	p.x /= w;
	p.y /= w;
	p.z /= w;
	return p;
}

inline VEC3 VEC3absR(VEC3 p){
	if(p.x < 0.0f){
		p.x = -p.x;
	}
	if(p.y < 0.0f){
		p.y = -p.y;
	}
	if(p.z < 0.0f){
		p.z = -p.z;
	}
	return p;
}

inline VEC3 VEC3maxR(VEC3 p,float p2){
	if(p2 > p.x){
		p.x = p2;
	}
	if(p2 > p.y){
		p.y = p2;
	}
	if(p2 > p.z){
		p.z = p2;
	}
	return p;
}

inline VEC3 VEC3fractR(VEC3 p){
	return (VEC3){p.x-(int)p.x,p.y-(int)p.y,p.z-(int)p.z};
}



