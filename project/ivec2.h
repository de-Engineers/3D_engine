typedef struct {
	int x;
	int y;
}IVEC2;

inline int isqrt(long long p){
    long long q = 1;
    int r = 0;
    while(q <= p){
        q <<= 2;
    }
    while(q > 1){
        q >>= 2;
        long t = p - r - q;
        r >>= 1;
        if(t >= 0){
            p = t;
            r += q;
        }
    }
    return r;
}

inline int IVEC2mulS(IVEC2 p) {
	return p.x * p.y;
}

inline void IVEC2add(IVEC2 *p,int d) {
	p->x += d;
	p->y += d;
}

inline void IVEC2sub(IVEC2 *p,int d) {
	p->x -= d;
	p->y -= d;
}

inline void IVEC2mul(IVEC2 *p,int d) {
	p->x *= d;
	p->y *= d;
}

inline void IVEC2div(IVEC2 *p,int d) {
	p->x /= d;
	p->y /= d;
}

inline void IVEC2addIVEC2(IVEC2 *p,IVEC2 p2) {
	p->x += p2.x;
	p->y += p2.y;
}

inline void IVEC2subIVEC2(IVEC2 *p,IVEC2 p2) {
	p->x -= p2.x;
	p->y -= p2.y;
}

inline void IVEC2mulIVEC2(IVEC2 *p,IVEC2 p2) {
	p->x *= p2.x;
	p->y *= p2.y;
}

inline void IVEC2divIVEC2(IVEC2 *p,IVEC2 p2) {
	p->x /= p2.x;
	p->y /= p2.y;
}

inline int IVEC2dist(IVEC2 p,IVEC2 p2){
	IVEC2subIVEC2(&p,p2);
	return isqrt(p.x*p.x+p.y*p.y);
}



