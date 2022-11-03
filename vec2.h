#pragma once

#include <math.h>

typedef struct{
	float x;
	float y;	
}VEC2;

inline void VEC2add(VEC2 *p,float d){
	p->x += d;
	p->y += d;
}

inline void VEC2sub(VEC2 *p,float d){
	p->x -= d;
	p->y -= d;
}

inline void VEC2mul(VEC2 *p,float d){
	p->x *= d;
	p->y *= d;
}

inline void VEC2div(VEC2 *p,float d){
	p->x /= d;
	p->y /= d;
}

inline VEC2 VEC2addR(VEC2 p,float d){
	p.x += d;
	p.y += d;
	return p;
}

inline VEC2 VEC2subR(VEC2 p,float d){
	p.x -= d;
	p.y -= d;
	return p;
}

inline VEC2 VEC2mulR(VEC2 p,float d){
	p.x *= d;
	p.y *= d;
	return p;
}

inline VEC2 VEC2divR(VEC2 p,float d){
	p.x /= d;
	p.y /= d;
	return p;
}

inline VEC2 VEC2addFR(VEC2 p,float d){
	p.x = d + p.x;
	p.y = d + p.y;
	return p;
}

inline VEC2 VEC2subFR(VEC2 p,float d){
	p.x = d - p.x;
	p.y = d - p.y;
	return p;
}

inline VEC2 VEC2mulFR(VEC2 p,float d){
	p.x = d * p.x;
	p.y = d * p.y;
	return p;
}

inline VEC2 VEC2divFR(VEC2 p,float d){
	p.x = d / p.x;
	p.y = d / p.y;
	return p;
}

inline void VEC2addVEC2(VEC2 *p,VEC2 p2){
	p->x += p2.x;
	p->y += p2.y;
}

inline void VEC2subVEC2(VEC2 *p,VEC2 p2){
	p->x -= p2.x;
	p->y -= p2.y;
}

inline void VEC2mulVEC2(VEC2 *p,VEC2 p2){
	p->x *= p2.x;
	p->y *= p2.y;
}

inline void VEC2divVEC2(VEC2 *p,VEC2 p2){
	p->x /= p2.x;
	p->y /= p2.y;
}

inline VEC2 VEC2addVEC2R(VEC2 p,VEC2 p2){
	return (VEC2){p.x+p2.x,p.y+p2.y};
}

inline VEC2 VEC2subVEC2R(VEC2 p,VEC2 p2){
	return (VEC2){p.x-p2.x,p.y-p2.y};
}

inline VEC2 VEC2mulVEC2R(VEC2 p,VEC2 p2){
	return (VEC2){p.x*p2.x,p.y*p2.y};
}

inline VEC2 VEC2divVEC2R(VEC2 p,VEC2 p2){
	return (VEC2){p.x/p2.x,p.y/p2.y};
}

inline float VEC2dist(VEC2 p,VEC2 p2){
	VEC2subVEC2(&p,p2);
	return sqrtf(p.x*p.x+p.y*p.y);
}

inline float VEC2dot(VEC2 p,VEC2 p2){
	return p.x * p2.x + p.y * p2.y;
}

inline VEC2 VEC2reflect(VEC2 p,VEC2 p2){
	VEC2 r;
	float d = VEC2dot(p,p2);
	r.x = p.x - 2.0f * d * p2.x;
	r.y = p.y - 2.0f * d * p2.y;
	return r;
}

inline float VEC2length(VEC2 p){
	return sqrtf(p.x*p.x+p.y*p.y);
}

inline VEC2 VEC2normalize(VEC2 p){
	return VEC2divR(p,VEC2length(p));
}

inline void VEC2normalizeR(VEC2 *p){
	VEC2div(p,VEC2length(*p));
}

inline float VEC2max(VEC2 p){
	if(p.x > p.y){
		return p.x;
	}
	return p.y;
}

inline VEC2 VEC2absR(VEC2 p){
	if(p.x < 0.0f){
		p.x = -p.x;
	}
	if(p.y < 0.0f){
		p.y = -p.y;
	}
	return p;
}

inline VEC2 VEC2floorR(VEC2 p){
	p.x = (int)p.x;
	p.y = (int)p.y;
	return p;
}

inline VEC2 VEC2fractR(VEC2 p){
	return VEC2subVEC2R(p,VEC2floorR(p));
}

inline int VEC2hash(int x){
	x += (x << 10);
	x ^= (x >> 6);
	x += (x << 3);
	x ^= (x >> 11);
	x += (x << 15);
	return x;
}

inline float VEC2rnd(VEC2 p){
	union u{
		VEC2 f;
		struct{
			int x;
			int y;
		}i;
	}r;
	r.f = p;
	r.i.x = VEC2hash(VEC2hash(r.i.x) ^ r.i.y); 
	r.i.x &= 0x007fffff;
	r.i.x |= 0x3f800000;
	return r.f.x;
}
/*
inline VEC2 VEC2reflect(VEC2 p,VEC2 p2){
	VEC2 r;
	float d = VEC2dot(p,p2);
	r.x = p.x - 2.0f * d * p2.x;
	r.y = p.y - 2.0f * d * p2.y;
	return r;
}*/



