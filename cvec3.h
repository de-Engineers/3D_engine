#pragma once
#include <math.h>

typedef struct{
	unsigned char x;
	unsigned char y;	
	unsigned char z;
}CVEC3;

inline void CVEC3add(CVEC3 *p,unsigned char d){
	p->x += d;
	p->y += d;
	p->z += d;
}

inline void CVEC3sub(CVEC3 *p,unsigned char d){
	p->x -= d;
	p->y -= d;
	p->z -= d;
}

inline void CVEC3mul(CVEC3 *p,unsigned char d){
	p->x *= d;
	p->y *= d;
	p->z *= d;
}

inline void CVEC3div(CVEC3 *p,unsigned char d){
	p->x /= d;
	p->y /= d;
	p->z /= d;
}

inline CVEC3 CVEC3addR(CVEC3 p,unsigned char d){
	p.x += d;
	p.y += d;
	p.z += d;
	return p;
}

inline CVEC3 CVEC3subR(CVEC3 p,unsigned char d){
	p.x -= d;
	p.y -= d;
	p.z += d;
	return p;
}

inline CVEC3 CVEC3mulR(CVEC3 p,unsigned char d){
	p.x *= d;
	p.y *= d;
	p.z *= d;
	return p;
}

inline CVEC3 CVEC3divR(CVEC3 p,unsigned char d){
	p.x /= d;
	p.y /= d;
	p.z /= d;
	return p;
}

inline void CVEC3addCVEC3(CVEC3 *p,CVEC3 p2){
	p->x += p2.x;
	p->y += p2.y;
	p->z += p2.z;
}

inline void CVEC3subCVEC3(CVEC3 *p,CVEC3 p2){
	p->x -= p2.x;
	p->y -= p2.y;
	p->z -= p2.z;
}

inline void CVEC3mulCVEC3(CVEC3 *p,CVEC3 p2){
	p->x *= p2.x;
	p->y *= p2.y;
	p->z *= p2.z;
}

inline void CVEC3divCVEC3(CVEC3 *p,CVEC3 p2){
	p->x /= p2.x;
	p->y /= p2.y;
	p->z /= p2.z;
}

inline CVEC3 CVEC3addCVEC3R(CVEC3 p,CVEC3 p2){
	CVEC3 r;
	r.x = p.x + p2.x;
	r.y = p.y + p2.y;
	r.z = p.z + p2.z;
	return r;
}

inline CVEC3 CVEC3subCVEC3R(CVEC3 p,CVEC3 p2){
	CVEC3 r;
	r.x = p.x - p2.x;
	r.y = p.y - p2.y;
	r.z = p.z - p2.z;
	return r;
}

inline CVEC3 CVEC3mulCVEC3R(CVEC3 p,CVEC3 p2){
	CVEC3 r;
	r.x = p.x * p2.x;
	r.y = p.y * p2.y;
	r.z = p.z * p2.z;
	return r;
}

inline CVEC3 CVEC3divCVEC3R(CVEC3 p,CVEC3 p2){
	CVEC3 r;
	r.x = p.x / p2.x;
	r.y = p.y / p2.y;
	r.z = p.z / p2.z;
	return r;
}

inline unsigned char CVEC3dot(CVEC3 p,CVEC3 p2){
	return p.x * p2.x + p.y * p2.y + p.z * p2.z;
}

inline CVEC3 CVEC3reflect(CVEC3 p,CVEC3 p2){
	CVEC3 r;
	unsigned char d = CVEC3dot(p,p2);
	r.x = p.x - 2 * d * p2.x;
	r.y = p.y - 2 * d * p2.y;
	r.z = p.z - 2 * d * p2.z;
	return r;
}



#pragma once
