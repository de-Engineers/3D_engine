#pragma once

typedef struct {
	int x;
	int y;
	int z;
}IVEC3;

inline int IVEC3mulS(IVEC3 p) {
	return p.x * p.y;
}

inline void IVEC3add(IVEC3 *p,int d) {
	p->x += d;
	p->y += d;
	p->z += d;
}

inline void IVEC3sub(IVEC3 *p,int d) {
	p->x -= d;
	p->y -= d;
	p->z -= d;
}

inline void IVEC3mul(IVEC3 *p,int d) {
	p->x *= d;
	p->y *= d;
	p->z *= d;
}

inline void IVEC3div(IVEC3 *p,int d) {
	p->x /= d;
	p->y /= d;
	p->z /= d;
}

inline void IVEC2addIVEC2(IVEC3 *p,IVEC3 p2) {
	p->x += p2.x;
	p->y += p2.y;
	p->z += p2.z;
}

inline void IVEC2subIVEC2(IVEC3 *p,IVEC3 p2) {
	p->x -= p2.x;
	p->y -= p2.y;
	p->z -= p2.z;
}

inline void IVEC2mulIVEC2(IVEC3 *p,IVEC3 p2) {
	p->x *= p2.x;
	p->y *= p2.y;
	p->z *= p2.z;
}

inline void IVEC2divIVEC2(IVEC3 *p,IVEC3 p2) {
	p->x /= p2.x;
	p->y /= p2.y;
	p->z /= p2.z;
}


