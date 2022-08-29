#define PI_05  1.57079632679f
#define PI_2   6.28318530718f
#define PI     3.14159265358f
#define PI_025 0.78539816339f

typedef struct{
    float3 pos;
    float3 dir;
    float3 delta;
    float3 side;
    int3 step;
	int sid;
    int3 ipos;
}RAY;

uint seed;
float2 wall;

inline uint hash(unsigned int x) {
	x += (x << 10);
	x ^= (x >> 6);
	x += (x << 3);
	x ^= (x >> 11);
	x += (x << 15);
	return x;
}

inline float rnd() {
	union p {
		float f;
		uint u;
	}r;
	r.u = hash(seed);
	r.u &= 0x007fffff;
	r.u |= 0x3f800000;
    seed+=get_global_id(0);
	return r.f;
}

uint isqrt(ulong x) {
    ulong q = 1, r = 0;
    while (q <= x) {
        q <<= 2;
    }
    while (q > 1) {
    ulong t;
        q >>= 2;
        t = x - r - q;
        r >>= 1;
        if (t >= 0) {
            x = t;
            r += q;
        }
    }
    return r;
}

float tfract(float x){
    return x - floor(x);
}

RAY rayCreate(float3 pos,float3 dir){
    RAY r;
    r.pos = pos;
    r.dir = dir;
    r.delta = fabs((float3){1.0f,1.0f,1.0f}/r.dir);
    if(r.dir.x < 0.0f){
        r.step.x = -1;
        r.side.x = (r.pos.x-(int)r.pos.x) * r.delta.x;
    }
    else{
        r.step.x = 1;
        r.side.x = ((int)r.pos.x + 1.0f - r.pos.x) * r.delta.x;
    }
    if(r.dir.y < 0.0f){
        r.step.y = -1;
        r.side.y = (r.pos.y-(int)r.pos.y) * r.delta.y;
    }
    else{
        r.step.y = 1;
        r.side.y = ((int)r.pos.y + 1.0f - r.pos.y) * r.delta.y;
    }
    if(r.dir.z < 0.0f){
        r.step.z = -1;
        r.side.z = (r.pos.z-(int)r.pos.z) * r.delta.z;
    }
    else{
        r.step.z = 1;
        r.side.z = ((int)r.pos.z + 1.0f - r.pos.z) * r.delta.z;
    }
    r.ipos.x = r.pos.x;
    r.ipos.y = r.pos.y;
    r.ipos.z = r.pos.z;
    return r;
}

void rayItterate(RAY *ray){
    if(ray->side.x < ray->side.y){
        if(ray->side.x < ray->side.z){
			ray->ipos.x += ray->step.x;
			ray->side.x += ray->delta.x;
			ray->sid = 0;
        }
        else{
			ray->ipos.z += ray->step.z;
			ray->side.z += ray->delta.z;
			ray->sid = 2;
        }
    }
    else if(ray->side.y < ray->side.z){
		ray->ipos.y += ray->step.y;
		ray->side.y += ray->delta.y;
		ray->sid = 1;
    }
    else{
		ray->ipos.z += ray->step.z;
		ray->side.z += ray->delta.z;
		ray->sid = 2;
    }
}

float3 getSubCoords(RAY ray){
    switch(ray.sid){
    case 0:
        wall.x = tfract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y);
        wall.y = tfract(ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z);
		if(ray.dir.x > 0.0f){
            return (float3){0.0f,wall.x,wall.y};
		}
		else{
            return (float3){1.0f,wall.x,wall.y};
		}
	case 1:
            wall.x = tfract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x);
        wall.y = tfract(ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z);
		if(ray.dir.y > 0.0f){
			return (float3){wall.x,0.0f,wall.y};
		}
		else{
			return (float3){wall.x,1.0f,wall.y};
		}
	case 2:
        wall.x = tfract(ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x);
        wall.y = tfract(ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y);
		if(ray.dir.z > 0.0f){
            return (float3){wall.x,wall.y,0.0f};
		}
		else{
			return (float3){wall.x,wall.y,1.0f};
		}
	}
}

float iSphere(float3 ro,float3 rd,float ra){
    float b = dot(ro,rd);
    float c = dot(ro,ro) - ra*ra;
    float h = b*b - c;
    if(h<0.0){
        return -1.0;
    }
    return -b-sqrt(h);
}

float iSphere2(float3 ro,float3 rd,float ra){
    float b = dot(ro,rd);
    float c = dot(ro,ro) - ra*ra;
    float h = b*b - c;
    if(h<0.0){
        return -1.0;
    }
    return -b+sqrt(h);
}

float tmax(float p1,float p2){
    if(p1 > p2){
        return p1;
    }
    return p2;
}
float tmin(float p1,float p2){
    if(p1 < p2){
        return p1;
    }
    return p2;
}

float sphDensity(float3 ro,float3 rd,float rad){
    float ndbuffer = 1000.0/rad;
    float3 rc = ro/rad;
	
    float b = dot(rd,rc);
    float c = dot(rc,rc) - 1.0;
    float h = b*b - c;
    if( h<0.0 ){
         return 0.0;
    }
    h = sqrt( h );
    float t1 = -b - h;
    float t2 = -b + h;

    if( t2<0.0 || t1>ndbuffer ){ 
        return 0.0;
    }
    t1 = tmax( t1, 0.0 );
    t2 = tmin( t2, ndbuffer );

    float i1 = -(c*t1 + b*t1*t1 + t1*t1*t1/3.0);
    float i2 = -(c*t2 + b*t2*t2 + t2*t2*t2/3.0);
    return (i2-i1)*(3.0/4.0);
}

float3 reflect(float3 I,float3 N){
    return I - (float3){2.0,2.0,2.0} * dot(N,I) * N;
}

float3 refract(float3 I,float3 N,float eta){
    float k = 1.0 - eta * eta * (1.0 - dot(N, I) * dot(N, I));
    if (k < 0.0){
        return (float3){0.0,0.0,0.0};
    }
    else{
        return eta * I - (eta * dot(N, I) + sqrt(k)) * N;
    }
}

float3 tabs(float3 x){
    if(x.x<0.0){
        x.x = -x.x;
    }
    if(x.y<0.0){
        x.y = -x.y;
    }
    if(x.z<0.0){
        x.z = -x.z;
    }
    return x;
}

float tabsf(float x){
    if(x<0.0){
        return -x;
    }
    return x;
}

float tlength(float2 x){
    return sqrt(x.x*x.x+x.y*x.y);
}

float iBox(float3 ro,float3 rd,float3 boxSize){
    float3 m = (float3)1.0f/rd;
    float3 n = m*ro;
    float3 k = tabs(m)*boxSize;
    float3 t1 = -n - k;
    float3 t2 = -n + k;
    float tN = tmax(tmax(t1.x,t1.y),t1.z);
    float tF = tmin(tmin(t2.x,t2.y),t2.z);
    if(tN>tF || tF<0.0f){ 
        return -1.0f;
    }
    return tN;
}

float iBox2(float3 ro,float3 rd,float3 boxSize){
    float3 m = (float3)1.0f/rd;
    float3 n = m*ro;
    float3 k = tabs(m)*boxSize;
    float3 t1 = -n - k;
    float3 t2 = -n + k;
    float tN = tmax(tmax(t1.x,t1.y),t1.z);
    float tF = tmin(tmin(t2.x,t2.y),t2.z);
    if(tN>tF || tF<0.0f){ 
        return -1.0f;
    }
    return tF;
}

float2 rotVEC2(float2 p,float rot){
    float2 r;
    r.x = cos(rot) * p.x - sin(rot) * p.y;
    r.y = sin(rot) * p.x + cos(rot) * p.y;
    return r;
}

float iTorus(float3 ro,float3 rd,float2 tor){
    float po = 1.0f;
    float Ra2 = tor.x*tor.x;
    float ra2 = tor.y*tor.y;
    float m = dot(ro,ro);
    float n = dot(ro,rd);
    float k = (m + Ra2 - ra2)/2.0f;
    float k3 = n;
    float k2 = n*n - Ra2*dot(rd.xy,rd.xy) + k;
    float k1 = n*k - Ra2*dot(rd.xy,ro.xy);
    float k0 = k*k - Ra2*dot(ro.xy,ro.xy);

    if(tabsf(k3*(k3*k3-k2)+k1) < 0.01f){

        po = -1.0f;
        float tmp=k1; 
        k1=k3; 
        k3=tmp;
        k0 = 1.0/k0;
        k1 = k1*k0;
        k2 = k2*k0;
        k3 = k3*k0;
    }
    float c2 = k2*2.0f - 3.0f*k3*k3;
    float c1 = k3*(k3*k3-k2)+k1;
    float c0 = k3*(k3*(c2+2.0f*k2)-8.0f*k1)+4.0f*k0;
    c2 /= 3.0f;
    c1 *= 2.0f;
    c0 /= 3.0f;
    float Q = c2*c2 + c0;
    float R = c2*c2*c2 - 3.0f*c2*c0 + c1*c1;
    float h = R*R - Q*Q*Q;
    
    if( h>=0.0 ){
        h = sqrt(h);
        float v = sign(R+h)*pow(tabsf(R+h),1.0f/3.0f); // cube root
        float u = sign(R-h)*pow(tabsf(R-h),1.0f/3.0f); // cube root
        float2 s = (float2){(v+u)+4.0f*c2,(v-u)*sqrt(3.0f)};
        float y = sqrt(0.5f*(tlength(s)+s.x));
        float x = 0.5f*s.y/y;
        float r = 2.0f*c1/(x*x+y*y);
        float t1 =  x - r - k3; 
        t1 = (po<0.0f)?2.0f/t1:t1;
        float t2 = -x - r - k3; 
        t2 = (po<0.0f)?2.0f/t2:t2;
        float t = 1e20;
        if(t1>0.0f){
            t=t1;
        }
        if(t2>0.0f){
            t=tmin(t,t2);
        }
        return t;
    }
    
    float sQ = sqrt(Q);
    float w = sQ*cos(acos(-R/(sQ*Q)) / 3.0f);
    float d2 = -(w+c2); if( d2<0.0f ) return -1.0f;
    float d1 = sqrt(d2);
    float h1 = sqrt(w - 2.0f*c2 + c1/d1);
    float h2 = sqrt(w - 2.0f*c2 - c1/d1);
    float t1 = -d1 - h1 - k3; t1 = (po<0.0f)?2.0f/t1:t1;
    float t2 = -d1 + h1 - k3; t2 = (po<0.0f)?2.0f/t2:t2;
    float t3 =  d1 - h2 - k3; t3 = (po<0.0f)?2.0f/t3:t3;
    float t4 =  d1 + h2 - k3; t4 = (po<0.0f)?2.0f/t4:t4;
    float t = 1e20;
    if( t1>0.0f) t=t1;
    if( t2>0.0f) t=tmin(t,t2);
    if( t3>0.0f) t=tmin(t,t3);
    if( t4>0.0f) t=tmin(t,t4);
    return t;
}

float4 iCylinder(float3 ro,float3 rd,float3 pb,float ra){
    float3 pa = -pb;
    float3 ca = pb-pa;
    float3 oc = ro-pa;
    float caca = dot(ca,ca);
    float card = dot(ca,rd);
    float caoc = dot(ca,oc);
    float a = caca - card*card;
    float b = caca*dot(oc,rd) - caoc*card;
    float c = caca*dot(oc,oc) - caoc*caoc - ra*ra*caca;
    float h = b*b - a*c;
    if(h<0.0f){
        return (float4){-1.0f,-1.0f,-1.0f,-1.0f};
    }
    h = sqrt(h);
    float t = (-b-h)/a;
    float y = caoc + t*card;
    if( y>0.0 && y<caca ){
        return (float4){t,(oc+t*rd-ca*y/caca)/ra};
    }
    t = (((y<0.0f)?0.0f:caca) - caoc)/card;
    if(tabsf(b+a*t)<h){
        return (float4){t, ca*sign(y)/caca};
    }
    return (float4){-1.0f,-1.0f,-1.0f,-1.0f};
}

float refractDynamicCube(float3 spos,float3 mtdt,float3 mtdt2,float3 *ang){
	float3 tang;
	if(spos.x>mtdt.x-mtdt2.x-0.0001f&&spos.x<mtdt.x-mtdt2.x+0.0001f){
		tang = refract(*ang,(float3){1.0f,0.0f,0.0f},1.52f);
		if(tang.x == 0.0f && tang.y == 0.0f && tang.z == 0.0f){
			*ang = reflect(*ang,(float3){1.0,0.0,0.0});
			return 0.0f;
		}
	}
	if(spos.y>mtdt.y-mtdt2.y-0.0001&&spos.y<mtdt.y-mtdt2.y+0.0001f){
		tang = refract(*ang,(float3){0.0f,1.0f,0.0f},1.52);
		if(tang.x == 0.0f && tang.y == 0.0f && tang.z == 0.0f){
			*ang = reflect(*ang,(float3){0.0f,1.0f,0.0f});
			return 0.0f;
		}
	}
	if(spos.z>mtdt.z-mtdt2.z-0.0001f&&spos.z<mtdt.z-mtdt2.z+0.0001f){
		tang = refract(*ang,(float3){0.0f,0.0f,1.0f},1.52f);
		if(tang.x == 0.0f && tang.y == 0.0f && tang.z == 0.0f){
			*ang = reflect(*ang,(float3){0.0f,0.0f,1.0f});
			return 0.0f;
		}
	}
	if(spos.x>mtdt.x+mtdt2.x-0.0001f&&spos.x<mtdt.x+mtdt2.x+0.0001f){
		tang = refract(*ang,(float3){-1.0f,0.0f,0.0f},1.52f);
		if(tang.x == 0.0f && tang.y == 0.0f && tang.z == 0.0f){
			*ang = reflect(*ang,(float3){-1.0f,0.0f,0.0f});
			return 0.0f;
		}
	}
	if(spos.y>mtdt.y+mtdt2.y-0.0001f&&spos.y<mtdt.y+mtdt2.y+0.0001f){
		tang = refract(*ang,(float3){0.0f,-1.0f,0.0f},1.52f);
		if(tang.x == 0.0f && tang.y == 0.0f && tang.z == 0.0f){
			*ang = reflect(*ang,(float3){0.0f,-1.0f,0.0f});
			return 0.0f;
		}
	}
	if(spos.z>mtdt.z+mtdt2.z-0.0001f&&spos.z<mtdt.z+mtdt2.z+0.0001f){
		tang = refract(*ang,(float3){0.0f,0.0f,-1.0f},1.52f);
		if(tang.x == 0.0f && tang.y == 0.0f && tang.z == 0.0f){
			*ang = reflect(*ang,(float3){0.0f,0.0f,-1.0f});
			return 0.0f;
		}
	}
	*ang = tang;
	return 1.0f;
}

kernel void lighting(global uchar *map,global uchar *metadt,global uchar *metadt2,global uchar *metadt3,
global uchar *metadt4,global uchar *metadt5,global uchar *metadt6,
global float *lightmap,global uint *lpmap,
float posx,float posy,float posz,
float red,float green,float blue,
uint mapSz,uint lmapSz,float roff,
float lx,float ly,float lz,
float lxo,float lyo,float lzo){
    float3 dir;
    for(int i = 0;i < 3;i++){
        dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
    }

    float3 rofff = {(rnd()-1.5f)*2.0f*roff,(rnd()-1.5f)*2.0f*roff,(rnd()-1.5f)*2.0f*roff};

    dir = normalize(dir);

    if(dir.x < -lx+lxo|| dir.x > lx +lxo|| dir.y < -ly+lyo || dir.y > ly+lyo || dir.z < -lz+lzo || dir.z > lz+lzo){
        return;
    }

    RAY ray = rayCreate((float3){posx,posy,posz}+rofff,dir);
    rayItterate(&ray);
    while(ray.ipos.x >= 0 && ray.ipos.y >= 0 && ray.ipos.z >= 0 && ray.ipos.x < 64 && ray.ipos.y < 64 && ray.ipos.z < 64){
        unsigned int block = ray.ipos.x+ray.ipos.y*mapSz+ray.ipos.z*mapSz*mapSz;
        switch(map[block*4]){
        case 9:{
            float3 spos = getSubCoords(ray);
            float d = iSphere(spos-(float3){metadt[block*4],metadt[block*4+1],metadt[block*4+2]}/255.0f,ray.dir,(float)metadt[block*4+3]/255.0f);
            if(d > 0){
                spos += ray.dir * d;
                if(spos.x > 0.0 && spos.y > 0.0 && spos.z > 0.0 && spos.x < 1.0 && spos.y < 1.0 && spos.z < 1.0){
                    float rcpos = (atan2(spos.x-(float)metadt[block*4]/255.0f,spos.y-(float)metadt[block*4+1]/255.0f)+PI)/PI;
                    uint offset = tfract(rcpos)*lmapSz;
                    offset += (uint)rcpos*lmapSz*lmapSz;
                    float maxz,minz;
                    maxz = (float)metadt[block*4+2]/255.0f+(float)metadt[block*4+3]/255.0f;
                    minz = (float)metadt[block*4+2]/255.0f-(float)metadt[block*4+3]/255.0f;
                    if(maxz > 1.0f){
                        maxz = 1.0f;
                    }
                    if(minz < 0.0f){
                        minz = 0.0f;
                    }
                    float rspos = spos.z - minz;
                    rspos /= maxz;
                    rspos *= 2.0f;
                    offset += (uint)(tfract(rspos)*lmapSz)*lmapSz;
                    offset += (uint)rspos*lmapSz*lmapSz*2;
                    lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+0] += red   / ((float)metadt[block*4+3]/255.0f);
                    lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+1] += green / ((float)metadt[block*4+3]/255.0f);
                    lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+2] += blue  / ((float)metadt[block*4+3]/255.0f);
                    dir = reflect(ray.dir,normalize(spos-(float3){metadt[block*4],metadt[block*4+1],metadt[block*4+2]}/255.0f));
                    ray = rayCreate((float3){ray.ipos.x,ray.ipos.y,ray.ipos.z}+spos,dir);
                    red   *= sqrt((float)map[block*4+1]/265.0f);
                    green *= sqrt((float)map[block*4+2]/265.0f);
                    blue  *= sqrt((float)map[block*4+3]/265.0f);
                    if(red < 0.01f && green < 0.01f && blue < 0.01f){
                        return;
                    }
                }
            }
            break;
            }
        case 10:{
            float3 spos = getSubCoords(ray);
            float d = iSphere(spos-(float3){metadt[block*4],metadt[block*4+1],metadt[block*4+2]}/255.0f,ray.dir,(float)metadt[block*4+3]/255.0f);
            if(d > 0){
                spos += ray.dir * d;
                dir = refract(ray.dir,normalize(spos-(float3){metadt[block*4],metadt[block*4+1],metadt[block*4+2]}/255.0f),0.6591349);
                ray = rayCreate((float3){ray.ipos.x,ray.ipos.y,ray.ipos.z}+spos,dir);
                spos += ray.dir * iSphere2(spos-(float3){metadt[block*4],metadt[block*4+1],metadt[block*4+2]}/255.0f,ray.dir,(float)metadt[block*4+3]/255.0f);
                dir = refract(ray.dir,-normalize(spos-(float3){metadt[block*4],metadt[block*4+1],metadt[block*4+2]}/255.0f),1.52);
                ray = rayCreate((float3){ray.ipos.x,ray.ipos.y,ray.ipos.z}+spos,dir);
                red   *= sqrt((float)map[block*4+1]/255.0f);
                green *= sqrt((float)map[block*4+2]/255.0f);
                blue  *= sqrt((float)map[block*4+3]/255.0f);
                if(red < 0.01f && green < 0.01f && blue < 0.01f){
                    return;
                }
            }
            break;
        }
        case 11:{
            float3 spos = getSubCoords(ray);
            float d = iSphere(spos-(float3){metadt[block*4],metadt[block*4+1],metadt[block*4+2]}/255.0f,ray.dir,(float)metadt[block*4+3]/255.0f);
            if(d > 0){
                spos += ray.dir * d;
                dir = reflect(ray.dir,normalize(spos-(float3){metadt[block*4],metadt[block*4+1],metadt[block*4+2]}/255.0f));
                ray = rayCreate((float3){ray.ipos.x,ray.ipos.y,ray.ipos.z}+spos,dir);
                red   *= sqrt((float)map[block*4+1]/255.0f);
                green *= sqrt((float)map[block*4+2]/255.0f);
                blue  *= sqrt((float)map[block*4+3]/255.0f);
                if(red < 0.01f && green < 0.01f && blue < 0.01f){
                    return;
                }
            }
            break;
        }
        case 12:{
            float3 spos = getSubCoords(ray);
            float3 mtdt = {(float)metadt[block*4]/255.0f,(float)metadt[block*4+1]/255.0f,(float)metadt[block*4+2]/255.0f};
            float3 mtdt2 = {(float)metadt2[block*4]/255.0f,(float)metadt2[block*4+1]/255.0f,(float)metadt2[block*4+2]/255.0f};
            float3 rotdir = ray.dir;
            mtdt.xy = rotVEC2(mtdt.xy,(float)metadt3[block*4]/255.0*PI);
            mtdt.xz = rotVEC2(mtdt.xz,(float)metadt3[block*4+1]/255.0*PI);
            mtdt.yz = rotVEC2(mtdt.yz,(float)metadt3[block*4+2]/255.0*PI);
            spos.xy = rotVEC2(spos.xy,(float)metadt3[block*4]/255.0*PI);
            spos.xz = rotVEC2(spos.xz,(float)metadt3[block*4+1]/255.0*PI);
            spos.yz = rotVEC2(spos.yz,(float)metadt3[block*4+2]/255.0*PI);
            rotdir.xy = rotVEC2(rotdir.xy,(float)metadt3[block*4]/255.0*PI);
            rotdir.xz = rotVEC2(rotdir.xz,(float)metadt3[block*4+1]/255.0*PI);
            rotdir.yz = rotVEC2(rotdir.yz,(float)metadt3[block*4+2]/255.0*PI);
            float d = iBox(spos-mtdt,rotdir,(float3){metadt2[block*4],metadt2[block*4+1],metadt2[block*4+2]}/255.0f);
            if(d > 0){
                spos += rotdir * d;
                float3 nspos = spos;
                nspos.yz = rotVEC2(nspos.yz,-(float)metadt3[block*4+2]/255.0f*PI);
                nspos.xz = rotVEC2(nspos.xz,-(float)metadt3[block*4+1]/255.0f*PI);
                nspos.xy = rotVEC2(nspos.xy,-(float)metadt3[block*4]/255.0f*PI);
				if(nspos.x>=0.0&&nspos.y>=0.0&&nspos.z>=0.0&&nspos.x<=1.0&&nspos.y<=1.0&&nspos.z<=1.0){
                    if(spos.x>mtdt.x-mtdt2.x-0.0001&&spos.x<mtdt.x-mtdt2.x+0.0001){
                        float mt = tmax(mtdt2.y,mtdt2.z)*2.0;
                        uint xt = (spos.y-mtdt.y+mtdt2.y)*lmapSz/mt;
                        uint yt = (spos.z-mtdt.z+mtdt2.z)*lmapSz/mt;
                        uint offset = xt + yt * lmapSz;
                        lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+0] += red   / mt / mt;
                        lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                        lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+2] += blue  / mt / mt;
                        dir = (float3){(rnd()-2.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                        for(int i = 0;i < 2;i++){
                            dir += (float3){(rnd()-2.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                        }
                    }
                    if(spos.y>mtdt.y-mtdt2.y-0.0001&&spos.y<mtdt.y-mtdt2.y+0.0001){
                        float mt = tmax(mtdt2.x,mtdt2.z)*2.0;
                        uint xt = (spos.x-mtdt.x+mtdt2.x)*lmapSz/mt;
                        uint yt = (spos.z-mtdt.z+mtdt2.z)*lmapSz/mt;
                        uint offset = xt + yt * lmapSz;
                        lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+0] += red   / mt / mt;
                        lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                        lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+2] += blue  / mt / mt;
                        dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-2.0f),(rnd()-1.5f)*2.0f};
                        for(int i = 0;i < 2;i++){
                            dir +=(float3){(rnd()-1.5f)*2.0f,(rnd()-2.0f),(rnd()-1.5f)*2.0f};
                        }
                    }
                    if(spos.z>mtdt.z-mtdt2.z-0.0001&&spos.z<mtdt.z-mtdt2.z+0.0001){
                        float mt = tmax(mtdt2.x,mtdt2.y)*2.0;
                        uint xt = (spos.x-mtdt.x+mtdt2.x)*lmapSz/mt;
                        uint yt = (spos.y-mtdt.y+mtdt2.y)*lmapSz/mt;
                        uint offset = xt + yt * lmapSz;
                        lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+0] += red   / mt / mt;
                        lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                        lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+2] += blue  / mt / mt;
                        dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-2.0f)};
                        for(int i = 0;i < 2;i++){
                            dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-2.0f)};
                        }
                    }
                    if(spos.x>mtdt.x+mtdt2.x-0.0001&&spos.x<mtdt.x+mtdt2.x+0.0001){
                        float mt = tmax(mtdt2.y,mtdt2.z)*2.0;
                        uint xt = (spos.y-mtdt.y+mtdt2.y)*lmapSz/mt;
                        uint yt = (spos.z-mtdt.z+mtdt2.z)*lmapSz/mt;
                        uint offset = xt + yt * lmapSz;
                        lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+0] += red   / mt / mt;
                        lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                        lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+2] += blue  / mt / mt;
                        dir = (float3){(rnd()-1.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                        for(int i = 0;i < 2;i++){
                            dir += (float3){(rnd()-1.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                        }
                    }
                    if(spos.y>mtdt.y+mtdt2.y-0.0001&&spos.y<mtdt.y+mtdt2.y+0.0001){
                        float mt = tmax(mtdt2.x,mtdt2.z)*2.0;
                        uint xt = (spos.x-mtdt.x+mtdt2.x)*lmapSz/mt;
                        uint yt = (spos.z-mtdt.z+mtdt2.z)*lmapSz/mt;
                        uint offset = xt + yt * lmapSz;
                        lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+0] += red   / mt / mt;
                        lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                        lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+2] += blue  / mt / mt;
                        dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.0f),(rnd()-1.5f)*2.0f};
                        for(int i = 0;i < 2;i++){
                            dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.0f),(rnd()-1.5f)*2.0f};
                        }
                    }
                    if(spos.z>mtdt.z+mtdt2.z-0.0001&&spos.z<mtdt.z+mtdt2.z+0.0001){
                        float mt = tmax(mtdt2.x,mtdt2.y)*2.0;
                        uint xt = (spos.x-mtdt.x+mtdt2.x)*lmapSz/mt;
                        uint yt = (spos.y-mtdt.y+mtdt2.y)*lmapSz/mt;
                        uint offset = xt + yt * lmapSz;
                        lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+0] += red   / mt / mt;
                        lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                        lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+2] += blue  / mt / mt;
                        dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-1.0f)};
                        for(int i = 0;i < 2;i++){
                            dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-1.0f)};
                        }
                    }
                    dir.xy = rotVEC2(dir.xy,(float){metadt3[block*4]}/255.0*PI);
                    dir.xz = rotVEC2(dir.xz,(float){metadt3[block*4+1]}/255.0*PI);
                    dir.yz = rotVEC2(dir.yz,(float){metadt3[block*4+2]}/255.0*PI);
                    ray = rayCreate((float3){ray.ipos.x,(float)ray.ipos.y,(float)ray.ipos.z}+nspos,normalize(dir));
                    red   *= sqrt((float)map[block*4+1]/265.0f);
                    green *= sqrt((float)map[block*4+2]/265.0f);
                    blue  *= sqrt((float)map[block*4+3]/265.0f);
                    if(red < 0.01f && green < 0.01f && blue < 0.01f){
                        return;
                    }
                }
            }
            break;
        }
        case 13:{                                          
            float3 spos = getSubCoords(ray);
            float3 mtdt = {(float)metadt[block*4]/255.0f,(float)metadt[block*4+1]/255.0f,(float)metadt[block*4+2]/255.0f};
            float2 mtdt2= {(float)metadt2[block*4]/255.0f,(float)metadt2[block*4+1]/255.0f};
            float d = iTorus(spos-mtdt,ray.dir,mtdt2);      
            if(d > 0.0){
                spos += ray.dir * d;
                if(spos.x > 0.0 && spos.y > 0.0 && spos.z > 0.0 && spos.x < 1.0 && spos.y < 1.0 && spos.z < 1.0){
                    uint tx = (atan2(spos.x-mtdt.x,spos.y-mtdt.y)+PI)/PI_2*lmapSz*2;
                    uint ty = (uint)((atan2((spos.z-mtdt.z),tlength(spos.xy-mtdt.xy)-mtdt2.x)+PI)/PI_2*lmapSz)*lmapSz;
                    uint txt = tx / lmapSz;
                    tx %= lmapSz;
                    uint offset = tx+ty;
                    lightmap[(lpmap[block*12+txt]*lmapSz*lmapSz+offset)*3+0] += red   * mtdt2.x * mtdt2.y * 4.0f;
                    lightmap[(lpmap[block*12+txt]*lmapSz*lmapSz+offset)*3+1] += green * mtdt2.x * mtdt2.y * 4.0f;
                    lightmap[(lpmap[block*12+txt]*lmapSz*lmapSz+offset)*3+2] += blue  * mtdt2.x * mtdt2.y * 4.0f;
                    red   *= sqrt((float)map[block*4+1]/265.0f);
                    green *= sqrt((float)map[block*4+2]/265.0f);
                    blue  *= sqrt((float)map[block*4+3]/265.0f);
                    if(red < 0.01f && green < 0.01f && blue < 0.01f){
                        return;
                    }
                    float2 n = mtdt.xy+normalize(spos.xy-mtdt.xy)*mtdt2.x;
                    dir = reflect(dir,normalize((float3){(spos.x-n.x),(spos.y-n.y),(spos.z-mtdt.z)}));
                    ray = rayCreate((float3){ray.ipos.x+spos.x,(float)ray.ipos.y+spos.y,(float)ray.ipos.z+spos.z},dir);
                }
            }
            break;
        }
        case 14:{
            float3 spos = getSubCoords(ray);
            float4 mtdt = {(float)metadt[block*4]/255.0f,(float)metadt[block*4+1]/255.0f,(float)metadt[block*4+2]/255.0f,(float)metadt[block*4+3]/255.0f};
            float3 mtdt2= {(float)metadt2[block*4]/255.0f,(float)metadt2[block*4+1]/255.0f,(float)metadt2[block*4+2]/255.0f};
            float3 mtdt3 ={(float)metadt3[block*4]/255.0f,(float)metadt3[block*4+1]/255.0f,(float)metadt3[block*4+2]/255.0f};
            float4 d = iCylinder(spos-mtdt.xyz,ray.dir,mtdt3.xyz,mtdt.a);
            if(d.x > 0.0f){
				spos += ray.dir * d.x;
                if(spos.x > 0.0f && spos.y > 0.0f && spos.z > 0.0f && spos.x < 1.0f && spos.y < 1.0f && spos.z < 1.0f){
                    float3 w = normalize(mtdt3.xyz);
					float3 u = normalize(cross(w,(float3){1.0f,1.0f,1.0f}));
					float3 v = normalize(cross(u,w));
                    float3 tq = spos+mtdt3.xyz-mtdt.xyz;
                    float3 q;
                    q.x = dot(tq,u);
                    q.y = dot(tq,v);
                    q.z = dot(tq,w);
                    uint offset = (atan2(q.y,q.x)+PI)/PI_2*lmapSz*2;
                    uint txt = offset / lmapSz;
                    offset %= lmapSz;
                    offset += (uint)(q.z*lmapSz)*lmapSz;
                    lightmap[(lpmap[block*12+txt]*lmapSz*lmapSz+offset)*3+0] += red   / mtdt2.x / 2.0f;
                    lightmap[(lpmap[block*12+txt]*lmapSz*lmapSz+offset)*3+1] += green / mtdt2.x / 2.0f;
                    lightmap[(lpmap[block*12+txt]*lmapSz*lmapSz+offset)*3+2] += blue  / mtdt2.x / 2.0f;
                    red   *= sqrt((float)map[block*4+1]/265.0f);
                    green *= sqrt((float)map[block*4+2]/265.0f);
                    blue  *= sqrt((float)map[block*4+3]/265.0f);
                    dir = reflect(dir,normalize(d.yzw));
                    ray = rayCreate((float3){ray.ipos.x+spos.x,(float)ray.ipos.y+spos.y,(float)ray.ipos.z+spos.z},dir);
                    if(red < 0.01f && green < 0.01f && blue < 0.01f){
                        return;
                    }
                }
            }
            break;
        }
        case 15:{
            float3 spos = getSubCoords(ray);
            float3 mtdt = {(float)metadt[block*4]/255.0f,(float)metadt[block*4+1]/255.0f,(float)metadt[block*4+2]/255.0f};
            float3 mtdt2= {(float)metadt2[block*4]/255.0f,(float)metadt2[block*4+1]/255.0f,(float)metadt2[block*4+2]/255.0f};
            float d = iBox(spos-mtdt,ray.dir,mtdt2);
            if(d > 0.0f){
                spos += ray.dir * d;
                if(spos.x>=0.0f&&spos.y>=0.0f&&spos.z>=0.0f&&spos.x<=1.0&&spos.y<=1.0f&&spos.z<=1.0f){
                    if(spos.x>mtdt.x-mtdt2.x-0.0001f&&spos.x<mtdt.x-mtdt2.x+0.0001f){
                        ray.dir = refract(ray.dir,(float3){-1.0f,0.0f,0.0f},1.0f/1.52f);
                    }
                    else if(spos.y>mtdt.y-mtdt2.y-0.0001&&spos.y<mtdt.y-mtdt2.y+0.0001f){
                        ray.dir = refract(ray.dir,(float3){0.0f,-1.0f,0.0f},1.0/1.52);
                    }
                    else if(spos.z>mtdt.z-mtdt2.z-0.0001&&spos.z<mtdt.z-mtdt2.z+0.0001){
                        ray.dir = refract(ray.dir,(float3){0.0f,0.0f,-1.0f},1.0/1.52);
                    }
                    else if(spos.x>mtdt.x+mtdt2.x-0.0001&&spos.x<mtdt.x+mtdt2.x+0.0001){
                        ray.dir = refract(ray.dir,(float3){1.0f,0.0f,0.0f},1.0/1.52);
                    }
                    else if(spos.y>mtdt.y+mtdt2.y-0.0001f&&spos.y<mtdt.y+mtdt2.y+0.0001f){
                        ray.dir = refract(ray.dir,(float3){0.0f,1.0f,0.0f},1.0f/1.52f);
                    }
                    else if(spos.z>mtdt.z+mtdt2.z-0.0001f&&spos.z<mtdt.z+mtdt2.z+0.0001f){
                        ray.dir = refract(ray.dir,(float3){0.0f,0.0f,1.0f},1.0f/1.52f);
                    }
                    spos += ray.dir * iBox2(spos-mtdt.xyz,ray.dir,mtdt2.xyz);
                    if(refractDynamicCube(spos,mtdt.xyz,mtdt2.xyz,&ray.dir)==0.0f){
                        spos += ray.dir * iBox2(spos-mtdt.xyz,ray.dir,mtdt2.xyz);
                        refractDynamicCube(spos,mtdt.xyz,mtdt2.xyz,&ray.dir);
                    }
                    ray = rayCreate((float3){ray.ipos.x+spos.x,(float)ray.ipos.y+spos.y,(float)ray.ipos.z+spos.z},ray.dir);
                    red   *= sqrt((float)map[block*4+1]/265.0f);
                    green *= sqrt((float)map[block*4+2]/265.0f);
                    blue  *= sqrt((float)map[block*4+3]/265.0f);
                    if(red < 0.01f && green < 0.01f && blue < 0.01f){
                        return;
                    }
                    break;
                }
            }
            break;
        }
        case 16:{
            float3 spos = getSubCoords(ray);
            float3 mtdt  = {(float)metadt[block*4]/255.0f,(float)metadt[block*4+1]/255.0f,(float)metadt[block*4+2]/255.0f};
            float3 mtdt2 = {(float)metadt2[block*4]/255.0f,(float)metadt2[block*4+1]/255.0f,(float)metadt2[block*4+2]/255.0f};
            float3 mtdt4 = {(float)metadt4[block*4]/255.0f,(float)metadt4[block*4+1]/255.0f,(float)metadt4[block*4+2]/255.0f};
            float3 mtdt5 = {(float)metadt5[block*4]/255.0f,(float)metadt5[block*4+1]/255.0f,(float)metadt5[block*4+2]/255.0f};

            float d  = iBox(spos-mtdt ,ray.dir,mtdt2);
            float d2 = iBox(spos-mtdt4,ray.dir,mtdt5);

            if(d < 0.0){
                d = 999999.0;
            }
            if(d2 < 0.0){
                d2 = 999999.0;
            }

            if(d < d2 && d != 999999.0){   
                spos += ray.dir * d;        
                if(spos.x>mtdt.x-mtdt2.x-0.0001f&&spos.x<mtdt.x-mtdt2.x+0.0001f){
                    float mt = tmax(mtdt2.x,mtdt2.z)*2.0f;
                    uint xt = (spos.y-mtdt.y+mtdt2.y)*lmapSz/mt;
                    uint yt = (uint)((spos.z-mtdt.z+mtdt2.z)*lmapSz/mt)*lmapSz;
                    uint offset = xt + yt;
                    lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+0] += red / mt / mt;
                    lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                    lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+2] += blue / mt / mt;
                    dir = (float3){(rnd()-2.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-2.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                    }
                }
                if(spos.y>mtdt.y-mtdt2.y-0.0001&&spos.y<mtdt.y-mtdt2.y+0.0001){
                    float mt = tmax(mtdt2.x,mtdt2.z)*2.0;
                    uint xt = (spos.x-mtdt.x+mtdt2.x)*lmapSz/mt;
                    uint yt = (uint)((spos.z-mtdt.z+mtdt2.z)*lmapSz/mt)*lmapSz;
                    uint offset = xt + yt;
                    lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+0] += red / mt / mt;
                    lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                    lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+2] += blue / mt / mt;
                    dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-2.0f),(rnd()-1.5f)*2.0f};
                    for(int i = 0;i < 2;i++){
                        dir +=(float3){(rnd()-1.5f)*2.0f,(rnd()-2.0f),(rnd()-1.5f)*2.0f};
                    }
                }
                if(spos.z>mtdt.z-mtdt.z-0.0001&&spos.z<mtdt.z-mtdt.z+0.0001){
                    float mt = tmax(mtdt2.x,mtdt2.x)*2.0;
                    uint xt = (spos.x-mtdt.x+mtdt2.x)*lmapSz/mt;
                    uint yt = (uint)((spos.y-mtdt.y+mtdt2.y)*lmapSz/mt)*lmapSz;
                    uint offset = xt + yt;
                    lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+0] += red / mt / mt;
                    lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                    lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+2] += blue / mt / mt;
                    dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-2.0f)};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-2.0f)};
                    }
                }
                if(spos.x>mtdt.x+mtdt2.x-0.0001&&spos.x<mtdt.x+mtdt2.x+0.0001){
                    float mt = tmax(mtdt2.x,mtdt2.z)*2.0;
                    uint xt = (spos.y-mtdt.y+mtdt2.y)*lmapSz/mt;
                    uint yt = (uint)((spos.z-mtdt.z+mtdt2.z)*lmapSz/mt)*lmapSz;
                    uint offset = xt + yt;
                    lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+0] += red / mt / mt;
                    lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                    lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+2] += blue / mt / mt;
                    dir = (float3){(rnd()-1.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-1.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                    }
                }
                if(spos.y>mtdt.y+mtdt2.y-0.0001&&spos.y<mtdt.y+mtdt2.y+0.0001){
                    float mt = tmax(mtdt2.x,mtdt2.z)*2.0;
                    uint xt = (spos.x-mtdt.x+mtdt2.x)*lmapSz/mt;
                    uint yt = (uint)((spos.z-mtdt.z+mtdt2.z)*lmapSz/mt)*lmapSz;
                    uint offset = xt + yt;
                    lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+0] += red / mt / mt;
                    lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                    lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+2] += blue / mt / mt;
                    dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.0f),(rnd()-1.5f)*2.0f};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.0f),(rnd()-1.5f)*2.0f};
                    }
                }
                if(spos.z>mtdt.z+mtdt2.z-0.0001&&spos.z<mtdt.z+mtdt2.z+0.0001){
                    float mt = tmax(mtdt2.x,mtdt2.x)*2.0;
                    uint xt = (spos.x-mtdt.x+mtdt2.x)*lmapSz/mt;
                    uint yt = (uint)((spos.y-mtdt.y+mtdt2.y)*lmapSz/mt)*lmapSz;
                    uint offset = xt + yt;
                    lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+0] += red / mt / mt;
                    lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                    lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+2] += blue / mt / mt;
                    dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-1.0f)};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-1.0f)};
                    }
                }
                ray = rayCreate((float3){ray.ipos.x,(float)ray.ipos.y,(float)ray.ipos.z}+spos,normalize(dir));
                red   *= sqrt((float)map[block*4+1]/265.0f);
                green *= sqrt((float)map[block*4+2]/265.0f);
                blue  *= sqrt((float)map[block*4+3]/265.0f);
                if(red < 0.01f && green < 0.01f && blue < 0.01f){
                    return;
                }
            }
            else if(d2 != 999999.0){
                spos += ray.dir * d2;
                if(spos.x>mtdt4.x-mtdt5.x-0.0001&&spos.x<mtdt4.x-mtdt5.x+0.0001){
                    float mt = tmax(mtdt5.x,mtdt5.z)*2.0;
                    uint xt = (spos.y-mtdt4.y+mtdt5.y)*lmapSz/mt;
                    uint yt = (uint)((spos.z-mtdt4.z+mtdt5.z)*lmapSz/mt)*lmapSz;
                    uint offset = xt + yt;
                    lightmap[(lpmap[block*12+6]*lmapSz*lmapSz+offset)*3+0] += red / mt / mt;
                    lightmap[(lpmap[block*12+6]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                    lightmap[(lpmap[block*12+6]*lmapSz*lmapSz+offset)*3+2] += blue / mt / mt;
                    dir = (float3){(rnd()-2.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-2.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                    }
                }
                if(spos.y>mtdt4.y-mtdt5.y-0.0001&&spos.y<mtdt4.y-mtdt5.y+0.0001){
                    float mt = tmax(mtdt5.x,mtdt5.z)*2.0;
                    uint xt = (spos.x-mtdt4.x+mtdt5.x)*lmapSz/mt;
                    uint yt = (uint)((spos.z-mtdt4.z+mtdt5.z)*lmapSz/mt)*lmapSz;
                    uint offset = xt + yt;
                    lightmap[(lpmap[block*12+7]*lmapSz*lmapSz+offset)*3+0] += red / mt / mt;
                    lightmap[(lpmap[block*12+7]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                    lightmap[(lpmap[block*12+7]*lmapSz*lmapSz+offset)*3+2] += blue / mt / mt;
                    dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-2.0f),(rnd()-1.5f)*2.0f};
                    for(int i = 0;i < 2;i++){
                        dir +=(float3){(rnd()-1.5f)*2.0f,(rnd()-2.0f),(rnd()-1.5f)*2.0f};
                    }
                }
                if(spos.z>mtdt4.z-mtdt5.z-0.0001&&spos.z<mtdt4.z-mtdt5.z+0.0001){
                    float mt = tmax(mtdt5.x,mtdt5.x)*2.0;
                    uint xt = (spos.x-mtdt4.x+mtdt5.x)*lmapSz/mt;
                    uint yt = (uint)((spos.y-mtdt4.y+mtdt5.y)*lmapSz/mt)*lmapSz;
                    uint offset = xt + yt;
                    lightmap[(lpmap[block*12+8]*lmapSz*lmapSz+offset)*3+0] += red / mt / mt;
                    lightmap[(lpmap[block*12+8]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                    lightmap[(lpmap[block*12+8]*lmapSz*lmapSz+offset)*3+2] += blue / mt / mt;
                    dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-2.0f)};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-2.0f)};
                    }
                }
                if(spos.x>mtdt4.x+mtdt5.x-0.0001&&spos.x<mtdt4.x+mtdt5.x+0.0001){
                    float mt = tmax(mtdt5.x,mtdt5.z)*2.0;
                    uint xt = (spos.y-mtdt4.y+mtdt5.y)*lmapSz/mt;
                    uint yt = (uint)((spos.z-mtdt4.z+mtdt5.z)*lmapSz/mt)*lmapSz;
                    uint offset = xt + yt;
                    lightmap[(lpmap[block*12+9]*lmapSz*lmapSz+offset)*3+0] += red / mt / mt;
                    lightmap[(lpmap[block*12+9]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                    lightmap[(lpmap[block*12+9]*lmapSz*lmapSz+offset)*3+2] += blue / mt / mt;
                    dir = (float3){(rnd()-1.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-1.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                    }
                }
                if(spos.y>mtdt4.y+mtdt5.y-0.0001&&spos.y<mtdt4.y+mtdt5.y+0.0001){
                    float mt = tmax(mtdt5.x,mtdt5.z)*2.0;
                    uint xt = (spos.x-mtdt4.x+mtdt5.x)*lmapSz/mt;
                    uint yt = (uint)((spos.z-mtdt4.z+mtdt5.z)*lmapSz/mt)*lmapSz;
                    uint offset = xt + yt;
                    lightmap[(lpmap[block*12+10]*lmapSz*lmapSz+offset)*3+0] += red / mt / mt;
                    lightmap[(lpmap[block*12+10]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                    lightmap[(lpmap[block*12+10]*lmapSz*lmapSz+offset)*3+2] += blue / mt / mt;
                    dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.0f),(rnd()-1.5f)*2.0f};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.0f),(rnd()-1.5f)*2.0f};
                    }
                }
                if(spos.z>mtdt4.z+mtdt5.z-0.0001&&spos.z<mtdt4.z+mtdt5.z+0.0001){
                    float mt = tmax(mtdt5.x,mtdt5.x)*2.0;
                    uint xt = (spos.x-mtdt4.x+mtdt5.x)*lmapSz/mt;
                    uint yt = (uint)((spos.y-mtdt4.y+mtdt5.y)*lmapSz/mt)*lmapSz;
                    uint offset = xt + yt;
                    lightmap[(lpmap[block*12+11]*lmapSz*lmapSz+offset)*3+0] += red / mt / mt;
                    lightmap[(lpmap[block*12+11]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                    lightmap[(lpmap[block*12+11]*lmapSz*lmapSz+offset)*3+2] += blue / mt / mt;
                    dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-1.0f)};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-1.0f)};
                    }
                }
                ray = rayCreate((float3){ray.ipos.x,(float)ray.ipos.y,(float)ray.ipos.z}+spos,normalize(dir));
                red   *= sqrt((float)map[block*4+1]/265.0f);
                green *= sqrt((float)map[block*4+2]/265.0f);
                blue  *= sqrt((float)map[block*4+3]/265.0f);
                if(red < 0.01f && green < 0.01f && blue < 0.01f){
                    return;
                }
            }
            break;
        }
        case 27:
        case 32:
        case 30:
        case 28:{
            switch(ray.sid){
            case 0:{
                wall.x = tfract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y);
                wall.y = tfract(ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z);
                uint offset = (int)(tfract(wall.y)*lmapSz)*lmapSz+(int)(tfract(wall.x)*lmapSz);
                if(ray.dir.x > 0.0f){
                    lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+0] += red;
                    lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+1] += green;
                    lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+2] += blue;
                    dir = (float3){(rnd()-2.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-2.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                    }
                    dir = normalize(dir);
                    ray = rayCreate((float3){ray.ipos.x,ray.ipos.y+tfract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y),ray.ipos.z+tfract(ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z)},dir);
                }
                else{
                    lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+0] += red;
                    lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+1] += green;
                    lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+2] += blue;
                    dir = (float3){(rnd()-1.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                    for(int i = 0;i < 2;i++){
                        dir +=(float3){(rnd()-1.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                    }
                    dir = normalize(dir);
                    ray = rayCreate((float3){ray.ipos.x+1,ray.ipos.y+tfract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y),ray.ipos.z+tfract(ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z)},dir);
                }
                break;
            }
            case 1:{
                wall.x = tfract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x);
                wall.y = tfract(ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z);
                unsigned int offset = (int)(tfract(wall.y)*lmapSz)*lmapSz+(int)(tfract(wall.x)*lmapSz);
                if(ray.dir.y > 0.0f){
                    lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+0] += red;
                    lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+1] += green;
                    lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+2] += blue;
                    dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-2.0f),(rnd()-1.5f)*2.0f};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-2.0f),(rnd()-1.5f)*2.0f};
                    }
                    dir = normalize(dir);
                    ray = rayCreate((float3){ray.ipos.x+tfract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x),ray.ipos.y,ray.ipos.z+tfract(ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z)},dir);
                }
                else{
                    lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+0] += red;
                    lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+1] += green;
                    lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+2] += blue;
                    dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.0f),(rnd()-1.5f)*2.0f};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.0f),(rnd()-1.5f)*2.0f};
                    }
                    dir = normalize(dir);
                    ray = rayCreate((float3){ray.ipos.x+tfract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x),ray.ipos.y+1,ray.ipos.z+tfract(ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z)},dir);
                }
                break;
            }
            case 2:{
                wall.x = tfract(ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x);
                wall.y = tfract(ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y);
                unsigned int offset = (int)(tfract(wall.y)*lmapSz)*lmapSz+(int)(tfract(wall.x)*lmapSz);
                if(ray.dir.z > 0.0f){
                    lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+0] += red;
                    lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+1] += green;
                    lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+2] += blue;
                    dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-2.0f)};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-2.0f)};
                    }
                    dir = normalize(dir);
                    ray = rayCreate((float3){ray.ipos.x+tfract(ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x),ray.ipos.y+tfract(ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y),ray.ipos.z},dir);
                }
                else{
                    lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+0] += red;
                    lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+1] += green;
                    lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+2] += blue;
                    dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-1.0f)};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-1.0f)};
                    }
                    dir = normalize(dir);
                    ray = rayCreate((float3){ray.ipos.x+tfract(ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x),ray.ipos.y+tfract(ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y),ray.ipos.z+1},dir);
                }
                break;
            }
            }
            red   *= sqrt((float)map[block*4+1]/265.0f);
            green *= sqrt((float)map[block*4+2]/265.0f);
            blue  *= sqrt((float)map[block*4+3]/265.0f);
            if(red < 0.01f && green < 0.01f && blue < 0.01f){
                return;
            }
            break;
        }
        case 29:{
            float3 spos = getSubCoords(ray);
            spos.x += (float)ray.pos.x;
            spos.y += (float)ray.pos.y;
            spos.z += (float)ray.pos.z;
            switch(ray.sid){
            case 0:
				ray = rayCreate(spos,(float3){-ray.dir.x,ray.dir.y,ray.dir.z});
				break;
			case 1:
                ray = rayCreate(spos,(float3){ray.dir.x,-ray.dir.y,ray.dir.z});
                break;
            case 2:
                ray = rayCreate(spos,(float3){ray.dir.x,ray.dir.y,-ray.dir.z});
                break;
            }
            if(red < 0.01f && green < 0.01f && blue < 0.01f){
                return;
            }
            break;
        }
        }
        rayItterate(&ray);
    }
}

kernel void ambientX(global uchar *map,global uchar *metadt,global uchar *metadt2,global uchar *metadt3,
global uchar *metadt4,global uchar *metadt5,global uchar *metadt6,
global float *lightmap,global uint *lpmap,uint mapSz,uint lmapSz,
float red,float green,float blue,
float ax,float ay,float az,float pos){
    float3 dir = normalize((float3){ax,ay,az});
    RAY ray = rayCreate((float3){pos,(rnd()-1.0)*mapSz,(rnd()-1.0)*mapSz},dir);
    rayItterate(&ray);
    while(ray.ipos.x >= 0 && ray.ipos.y >= 0 && ray.ipos.z >= 0 && ray.ipos.x < 64 && ray.ipos.y < 64 && ray.ipos.z < 64){
        uint block = ray.ipos.x+ray.ipos.y*mapSz+ray.ipos.z*mapSz*mapSz;
        switch(map[block*4]){
        case 12:{
            float3 spos = getSubCoords(ray);
            float3 mtdt = {(float)metadt[block*4]/255.0f,(float)metadt[block*4+1]/255.0f,(float)metadt[block*4+2]/255.0f};
            float3 mtdt2 = {(float)metadt2[block*4]/255.0f,(float)metadt2[block*4+1]/255.0f,(float)metadt2[block*4+2]/255.0f};
            float3 rotdir = ray.dir;
            mtdt.xy = rotVEC2(mtdt.xy,(float)metadt3[block*4]/255.0*PI);
            mtdt.xz = rotVEC2(mtdt.xz,(float)metadt3[block*4+1]/255.0*PI);
            mtdt.yz = rotVEC2(mtdt.yz,(float)metadt3[block*4+2]/255.0*PI);
            spos.xy = rotVEC2(spos.xy,(float)metadt3[block*4]/255.0*PI);
            spos.xz = rotVEC2(spos.xz,(float)metadt3[block*4+1]/255.0*PI);
            spos.yz = rotVEC2(spos.yz,(float)metadt3[block*4+2]/255.0*PI);
            rotdir.xy = rotVEC2(rotdir.xy,(float)metadt3[block*4]/255.0*PI);
            rotdir.xz = rotVEC2(rotdir.xz,(float)metadt3[block*4+1]/255.0*PI);
            rotdir.yz = rotVEC2(rotdir.yz,(float)metadt3[block*4+2]/255.0*PI);
            float d = iBox(spos-mtdt,rotdir,(float3){metadt2[block*4],metadt2[block*4+1],metadt2[block*4+2]}/255.0f);
            if(d > 0){
                spos += rotdir * d;
                float3 nspos = spos;
                nspos.yz = rotVEC2(nspos.yz,-(float)metadt3[block*4+2]/255.0f*PI);
                nspos.xz = rotVEC2(nspos.xz,-(float)metadt3[block*4+1]/255.0f*PI);
                nspos.xy = rotVEC2(nspos.xy,-(float)metadt3[block*4]/255.0f*PI);
				if(nspos.x>=0.0&&nspos.y>=0.0&&nspos.z>=0.0&&nspos.x<=1.0&&nspos.y<=1.0&&nspos.z<=1.0){
                    if(spos.x>mtdt.x-mtdt2.x-0.0001&&spos.x<mtdt.x-mtdt2.x+0.0001){
                        float mt = tmax(mtdt2.y,mtdt2.z)*2.0;
                        uint xt = (spos.y-mtdt.y+mtdt2.y)*lmapSz/mt;
                        uint yt = (spos.z-mtdt.z+mtdt2.z)*lmapSz/mt;
                        uint offset = xt + yt * lmapSz;
                        lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+0] += red   / mt / mt;
                        lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                        lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+2] += blue  / mt / mt;
                        dir = (float3){(rnd()-2.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                        for(int i = 0;i < 2;i++){
                            dir += (float3){(rnd()-2.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                        }
                    }
                    if(spos.y>mtdt.y-mtdt2.y-0.0001&&spos.y<mtdt.y-mtdt2.y+0.0001){
                        float mt = tmax(mtdt2.x,mtdt2.z)*2.0;
                        uint xt = (spos.x-mtdt.x+mtdt2.x)*lmapSz/mt;
                        uint yt = (spos.z-mtdt.z+mtdt2.z)*lmapSz/mt;
                        uint offset = xt + yt * lmapSz;
                        lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+0] += red   / mt / mt;
                        lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                        lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+2] += blue  / mt / mt;
                        dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-2.0f),(rnd()-1.5f)*2.0f};
                        for(int i = 0;i < 2;i++){
                            dir +=(float3){(rnd()-1.5f)*2.0f,(rnd()-2.0f),(rnd()-1.5f)*2.0f};
                        }
                    }
                    if(spos.z>mtdt.z-mtdt2.z-0.0001&&spos.z<mtdt.z-mtdt2.z+0.0001){
                        float mt = tmax(mtdt2.x,mtdt2.y)*2.0;
                        uint xt = (spos.x-mtdt.x+mtdt2.x)*lmapSz/mt;
                        uint yt = (spos.y-mtdt.y+mtdt2.y)*lmapSz/mt;
                        uint offset = xt + yt * lmapSz;
                        lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+0] += red   / mt / mt;
                        lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                        lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+2] += blue  / mt / mt;
                        dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-2.0f)};
                        for(int i = 0;i < 2;i++){
                            dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-2.0f)};
                        }
                    }
                    if(spos.x>mtdt.x+mtdt2.x-0.0001&&spos.x<mtdt.x+mtdt2.x+0.0001){
                        float mt = tmax(mtdt2.y,mtdt2.z)*2.0;
                        uint xt = (spos.y-mtdt.y+mtdt2.y)*lmapSz/mt;
                        uint yt = (spos.z-mtdt.z+mtdt2.z)*lmapSz/mt;
                        uint offset = xt + yt * lmapSz;
                        lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+0] += red   / mt / mt;
                        lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                        lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+2] += blue  / mt / mt;
                        dir = (float3){(rnd()-1.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                        for(int i = 0;i < 2;i++){
                            dir += (float3){(rnd()-1.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                        }
                    }
                    if(spos.y>mtdt.y+mtdt2.y-0.0001&&spos.y<mtdt.y+mtdt2.y+0.0001){
                        float mt = tmax(mtdt2.x,mtdt2.z)*2.0;
                        uint xt = (spos.x-mtdt.x+mtdt2.x)*lmapSz/mt;
                        uint yt = (spos.z-mtdt.z+mtdt2.z)*lmapSz/mt;
                        uint offset = xt + yt * lmapSz;
                        lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+0] += red   / mt / mt;
                        lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                        lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+2] += blue  / mt / mt;
                        dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.0f),(rnd()-1.5f)*2.0f};
                        for(int i = 0;i < 2;i++){
                            dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.0f),(rnd()-1.5f)*2.0f};
                        }
                    }
                    if(spos.z>mtdt.z+mtdt2.z-0.0001&&spos.z<mtdt.z+mtdt2.z+0.0001){
                        float mt = tmax(mtdt2.x,mtdt2.y)*2.0;
                        uint xt = (spos.x-mtdt.x+mtdt2.x)*lmapSz/mt;
                        uint yt = (spos.y-mtdt.y+mtdt2.y)*lmapSz/mt;
                        uint offset = xt + yt * lmapSz;
                        lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+0] += red   / mt / mt;
                        lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                        lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+2] += blue  / mt / mt;
                        dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-1.0f)};
                        for(int i = 0;i < 2;i++){
                            dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-1.0f)};
                        }
                    }
                    dir.xy = rotVEC2(dir.xy,(float){metadt3[block*4]}/255.0*PI);
                    dir.xz = rotVEC2(dir.xz,(float){metadt3[block*4+1]}/255.0*PI);
                    dir.yz = rotVEC2(dir.yz,(float){metadt3[block*4+2]}/255.0*PI);
                    ray = rayCreate((float3){ray.ipos.x,(float)ray.ipos.y,(float)ray.ipos.z}+nspos,normalize(dir));
                    red   *= sqrt((float)map[block*4+1]/265.0f);
                    green *= sqrt((float)map[block*4+2]/265.0f);
                    blue  *= sqrt((float)map[block*4+3]/265.0f);
                    if(red < 0.01f && green < 0.01f && blue < 0.01f){
                        return;
                    }
                }
            }
            break;
        }
        case 15:{
            float3 spos = getSubCoords(ray);
            float3 mtdt = {(float)metadt[block*4]/255.0f,(float)metadt[block*4+1]/255.0f,(float)metadt[block*4+2]/255.0f};
            float3 mtdt2= {(float)metadt2[block*4]/255.0f,(float)metadt2[block*4+1]/255.0f,(float)metadt2[block*4+2]/255.0f};
            float d = iBox(spos-mtdt,ray.dir,mtdt2);
            if(d > 0.0f){
                spos += ray.dir * d;
                if(spos.x>=0.0f&&spos.y>=0.0f&&spos.z>=0.0f&&spos.x<=1.0&&spos.y<=1.0f&&spos.z<=1.0f){
                    if(spos.x>mtdt.x-mtdt2.x-0.0001f&&spos.x<mtdt.x-mtdt2.x+0.0001f){
                        ray.dir = refract(ray.dir,(float3){-1.0f,0.0f,0.0f},1.0f/1.52f);
                    }
                    else if(spos.y>mtdt.y-mtdt2.y-0.0001&&spos.y<mtdt.y-mtdt2.y+0.0001f){
                        ray.dir = refract(ray.dir,(float3){0.0f,-1.0f,0.0f},1.0/1.52);
                    }
                    else if(spos.z>mtdt.z-mtdt2.z-0.0001&&spos.z<mtdt.z-mtdt2.z+0.0001){
                        ray.dir = refract(ray.dir,(float3){0.0f,0.0f,-1.0f},1.0/1.52);
                    }
                    else if(spos.x>mtdt.x+mtdt2.x-0.0001&&spos.x<mtdt.x+mtdt2.x+0.0001){
                        ray.dir = refract(ray.dir,(float3){1.0f,0.0f,0.0f},1.0/1.52);
                    }
                    else if(spos.y>mtdt.y+mtdt2.y-0.0001f&&spos.y<mtdt.y+mtdt2.y+0.0001f){
                        ray.dir = refract(ray.dir,(float3){0.0f,1.0f,0.0f},1.0f/1.52f);
                    }
                    else if(spos.z>mtdt.z+mtdt2.z-0.0001f&&spos.z<mtdt.z+mtdt2.z+0.0001f){
                        ray.dir = refract(ray.dir,(float3){0.0f,0.0f,1.0f},1.0f/1.52f);
                    }
                    spos += ray.dir * iBox2(spos-mtdt.xyz,ray.dir,mtdt2.xyz);
                    if(refractDynamicCube(spos,mtdt.xyz,mtdt2.xyz,&ray.dir)==0.0f){
                        spos += ray.dir * iBox2(spos-mtdt.xyz,ray.dir,mtdt2.xyz);
                        refractDynamicCube(spos,mtdt.xyz,mtdt2.xyz,&ray.dir);
                    }
                    ray = rayCreate((float3){ray.ipos.x+spos.x,(float)ray.ipos.y+spos.y,(float)ray.ipos.z+spos.z},ray.dir);
                    red   *= sqrt((float)map[block*4+1]/265.0f);
                    green *= sqrt((float)map[block*4+2]/265.0f);
                    blue  *= sqrt((float)map[block*4+3]/265.0f);
                    if(red < 0.01f && green < 0.01f && blue < 0.01f){
                        return;
                    }
                    break;
                }
            }
            break;
        }
        case 27:
        case 28:{
            switch(ray.sid){
            case 0:{
                wall.x = tfract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y);
                wall.y = tfract(ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z);
                uint offset = (int)(tfract(wall.y)*lmapSz)*lmapSz+(int)(tfract(wall.x)*lmapSz);
                if(ray.dir.x > 0.0f){
                    lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+0] += red;
                    lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+1] += green;
                    lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+2] += blue;
                    dir = (float3){(rnd()-2.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-2.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                    }
                    dir = normalize(dir);
                    ray = rayCreate((float3){ray.ipos.x,ray.ipos.y+tfract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y),ray.ipos.z+tfract(ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z)},dir);
                }
                else{
                    lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+0] += red;
                    lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+1] += green;
                    lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+2] += blue;
                    dir = (float3){(rnd()-1.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                    for(int i = 0;i < 2;i++){
                        dir +=(float3){(rnd()-1.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                    }
                    dir = normalize(dir);
                    ray = rayCreate((float3){ray.ipos.x+1,ray.ipos.y+tfract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y),ray.ipos.z+tfract(ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z)},dir);
                }
                break;
            }
            case 1:{
                wall.x = tfract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x);
                wall.y = tfract(ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z);
                unsigned int offset = (int)(tfract(wall.y)*lmapSz)*lmapSz+(int)(tfract(wall.x)*lmapSz);
                if(ray.dir.y > 0.0f){
                    lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+0] += red;
                    lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+1] += green;
                    lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+2] += blue;
                    dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-2.0f),(rnd()-1.5f)*2.0f};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-2.0f),(rnd()-1.5f)*2.0f};
                    }
                    dir = normalize(dir);
                    ray = rayCreate((float3){ray.ipos.x+tfract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x),ray.ipos.y,ray.ipos.z+tfract(ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z)},dir);
                }
                else{
                    lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+0] += red;
                    lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+1] += green;
                    lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+2] += blue;
                    dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.0f),(rnd()-1.5f)*2.0f};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.0f),(rnd()-1.5f)*2.0f};
                    }
                    dir = normalize(dir);
                    ray = rayCreate((float3){ray.ipos.x+tfract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x),ray.ipos.y+1,ray.ipos.z+tfract(ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z)},dir);
                }
                break;
            }
            case 2:{
                wall.x = tfract(ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x);
                wall.y = tfract(ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y);
                unsigned int offset = (int)(tfract(wall.y)*lmapSz)*lmapSz+(int)(tfract(wall.x)*lmapSz);
                if(ray.dir.z > 0.0f){
                    lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+0] += red;
                    lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+1] += green;
                    lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+2] += blue;
                    dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-2.0f)};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-2.0f)};
                    }
                    dir = normalize(dir);
                    ray = rayCreate((float3){ray.ipos.x+tfract(ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x),ray.ipos.y+tfract(ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y),ray.ipos.z},dir);
                }
                else{
                    lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+0] += red;
                    lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+1] += green;
                    lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+2] += blue;
                    dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-1.0f)};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-1.0f)};
                    }
                    dir = normalize(dir);
                    ray = rayCreate((float3){ray.ipos.x+tfract(ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x),ray.ipos.y+tfract(ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y),ray.ipos.z+1},dir);
                }
                break;
            }
            }
            red   *= sqrt((float)map[block*4+1]/265.0f);
            green *= sqrt((float)map[block*4+2]/265.0f);
            blue  *= sqrt((float)map[block*4+3]/265.0f);
            if(red < 0.01f && green < 0.01f && blue < 0.01f){
                return;
            }
            break;
        }
        }
        rayItterate(&ray);
    }
}

kernel void ambientY(global uchar *map,global uchar *metadt,global uchar *metadt2,global uchar *metadt3,
global uchar *metadt4,global uchar *metadt5,global uchar *metadt6,
global float *lightmap,global uint *lpmap,uint mapSz,uint lmapSz,
float red,float green,float blue,
float ax,float ay,float az,float pos){
    float3 dir = normalize((float3){ax,ay,az});
    RAY ray = rayCreate((float3){(rnd()-1.0)*mapSz,pos,(rnd()-1.0)*mapSz},dir);
    rayItterate(&ray);
    while(ray.ipos.x >= 0 && ray.ipos.y >= 0 && ray.ipos.z >= 0 && ray.ipos.x < 64 && ray.ipos.y < 64 && ray.ipos.z < 64){
        uint block = ray.ipos.x+ray.ipos.y*mapSz+ray.ipos.z*mapSz*mapSz;
        switch(map[block*4]){
       case 12:{
            float3 spos = getSubCoords(ray);
            float3 mtdt = {(float)metadt[block*4]/255.0f,(float)metadt[block*4+1]/255.0f,(float)metadt[block*4+2]/255.0f};
            float3 mtdt2 = {(float)metadt2[block*4]/255.0f,(float)metadt2[block*4+1]/255.0f,(float)metadt2[block*4+2]/255.0f};
            float3 rotdir = ray.dir;
            mtdt.xy = rotVEC2(mtdt.xy,(float)metadt3[block*4]/255.0*PI);
            mtdt.xz = rotVEC2(mtdt.xz,(float)metadt3[block*4+1]/255.0*PI);
            mtdt.yz = rotVEC2(mtdt.yz,(float)metadt3[block*4+2]/255.0*PI);
            spos.xy = rotVEC2(spos.xy,(float)metadt3[block*4]/255.0*PI);
            spos.xz = rotVEC2(spos.xz,(float)metadt3[block*4+1]/255.0*PI);
            spos.yz = rotVEC2(spos.yz,(float)metadt3[block*4+2]/255.0*PI);
            rotdir.xy = rotVEC2(rotdir.xy,(float)metadt3[block*4]/255.0*PI);
            rotdir.xz = rotVEC2(rotdir.xz,(float)metadt3[block*4+1]/255.0*PI);
            rotdir.yz = rotVEC2(rotdir.yz,(float)metadt3[block*4+2]/255.0*PI);
            float d = iBox(spos-mtdt,rotdir,(float3){metadt2[block*4],metadt2[block*4+1],metadt2[block*4+2]}/255.0f);
            if(d > 0){
                spos += rotdir * d;
                float3 nspos = spos;
                nspos.yz = rotVEC2(nspos.yz,-(float)metadt3[block*4+2]/255.0f*PI);
                nspos.xz = rotVEC2(nspos.xz,-(float)metadt3[block*4+1]/255.0f*PI);
                nspos.xy = rotVEC2(nspos.xy,-(float)metadt3[block*4]/255.0f*PI);
				if(nspos.x>=0.0&&nspos.y>=0.0&&nspos.z>=0.0&&nspos.x<=1.0&&nspos.y<=1.0&&nspos.z<=1.0){
                    if(spos.x>mtdt.x-mtdt2.x-0.0001&&spos.x<mtdt.x-mtdt2.x+0.0001){
                        float mt = tmax(mtdt2.y,mtdt2.z)*2.0;
                        uint xt = (spos.y-mtdt.y+mtdt2.y)*lmapSz/mt;
                        uint yt = (spos.z-mtdt.z+mtdt2.z)*lmapSz/mt;
                        uint offset = xt + yt * lmapSz;
                        lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+0] += red   / mt / mt;
                        lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                        lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+2] += blue  / mt / mt;
                        dir = (float3){(rnd()-2.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                        for(int i = 0;i < 2;i++){
                            dir += (float3){(rnd()-2.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                        }
                    }
                    if(spos.y>mtdt.y-mtdt2.y-0.0001&&spos.y<mtdt.y-mtdt2.y+0.0001){
                        float mt = tmax(mtdt2.x,mtdt2.z)*2.0;
                        uint xt = (spos.x-mtdt.x+mtdt2.x)*lmapSz/mt;
                        uint yt = (spos.z-mtdt.z+mtdt2.z)*lmapSz/mt;
                        uint offset = xt + yt * lmapSz;
                        lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+0] += red   / mt / mt;
                        lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                        lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+2] += blue  / mt / mt;
                        dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-2.0f),(rnd()-1.5f)*2.0f};
                        for(int i = 0;i < 2;i++){
                            dir +=(float3){(rnd()-1.5f)*2.0f,(rnd()-2.0f),(rnd()-1.5f)*2.0f};
                        }
                    }
                    if(spos.z>mtdt.z-mtdt2.z-0.0001&&spos.z<mtdt.z-mtdt2.z+0.0001){
                        float mt = tmax(mtdt2.x,mtdt2.y)*2.0;
                        uint xt = (spos.x-mtdt.x+mtdt2.x)*lmapSz/mt;
                        uint yt = (spos.y-mtdt.y+mtdt2.y)*lmapSz/mt;
                        uint offset = xt + yt * lmapSz;
                        lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+0] += red   / mt / mt;
                        lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                        lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+2] += blue  / mt / mt;
                        dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-2.0f)};
                        for(int i = 0;i < 2;i++){
                            dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-2.0f)};
                        }
                    }
                    if(spos.x>mtdt.x+mtdt2.x-0.0001&&spos.x<mtdt.x+mtdt2.x+0.0001){
                        float mt = tmax(mtdt2.y,mtdt2.z)*2.0;
                        uint xt = (spos.y-mtdt.y+mtdt2.y)*lmapSz/mt;
                        uint yt = (spos.z-mtdt.z+mtdt2.z)*lmapSz/mt;
                        uint offset = xt + yt * lmapSz;
                        lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+0] += red   / mt / mt;
                        lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                        lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+2] += blue  / mt / mt;
                        dir = (float3){(rnd()-1.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                        for(int i = 0;i < 2;i++){
                            dir += (float3){(rnd()-1.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                        }
                    }
                    if(spos.y>mtdt.y+mtdt2.y-0.0001&&spos.y<mtdt.y+mtdt2.y+0.0001){
                        float mt = tmax(mtdt2.x,mtdt2.z)*2.0;
                        uint xt = (spos.x-mtdt.x+mtdt2.x)*lmapSz/mt;
                        uint yt = (spos.z-mtdt.z+mtdt2.z)*lmapSz/mt;
                        uint offset = xt + yt * lmapSz;
                        lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+0] += red   / mt / mt;
                        lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                        lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+2] += blue  / mt / mt;
                        dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.0f),(rnd()-1.5f)*2.0f};
                        for(int i = 0;i < 2;i++){
                            dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.0f),(rnd()-1.5f)*2.0f};
                        }
                    }
                    if(spos.z>mtdt.z+mtdt2.z-0.0001&&spos.z<mtdt.z+mtdt2.z+0.0001){
                        float mt = tmax(mtdt2.x,mtdt2.y)*2.0;
                        uint xt = (spos.x-mtdt.x+mtdt2.x)*lmapSz/mt;
                        uint yt = (spos.y-mtdt.y+mtdt2.y)*lmapSz/mt;
                        uint offset = xt + yt * lmapSz;
                        lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+0] += red   / mt / mt;
                        lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                        lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+2] += blue  / mt / mt;
                        dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-1.0f)};
                        for(int i = 0;i < 2;i++){
                            dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-1.0f)};
                        }
                    }
                    dir.xy = rotVEC2(dir.xy,(float){metadt3[block*4]}/255.0*PI);
                    dir.xz = rotVEC2(dir.xz,(float){metadt3[block*4+1]}/255.0*PI);
                    dir.yz = rotVEC2(dir.yz,(float){metadt3[block*4+2]}/255.0*PI);
                    ray = rayCreate((float3){ray.ipos.x,(float)ray.ipos.y,(float)ray.ipos.z}+nspos,normalize(dir));
                    red   *= sqrt((float)map[block*4+1]/265.0f);
                    green *= sqrt((float)map[block*4+2]/265.0f);
                    blue  *= sqrt((float)map[block*4+3]/265.0f);
                    if(red < 0.01f && green < 0.01f && blue < 0.01f){
                        return;
                    }
                }
            }
            break;
        }
        case 15:{
            float3 spos = getSubCoords(ray);
            float3 mtdt = {(float)metadt[block*4]/255.0f,(float)metadt[block*4+1]/255.0f,(float)metadt[block*4+2]/255.0f};
            float3 mtdt2= {(float)metadt2[block*4]/255.0f,(float)metadt2[block*4+1]/255.0f,(float)metadt2[block*4+2]/255.0f};
            float d = iBox(spos-mtdt,ray.dir,mtdt2);
            if(d > 0.0f){
                spos += ray.dir * d;
                if(spos.x>=0.0f&&spos.y>=0.0f&&spos.z>=0.0f&&spos.x<=1.0&&spos.y<=1.0f&&spos.z<=1.0f){
                    if(spos.x>mtdt.x-mtdt2.x-0.0001f&&spos.x<mtdt.x-mtdt2.x+0.0001f){
                        ray.dir = refract(ray.dir,(float3){-1.0f,0.0f,0.0f},1.0f/1.52f);
                    }
                    else if(spos.y>mtdt.y-mtdt2.y-0.0001&&spos.y<mtdt.y-mtdt2.y+0.0001f){
                        ray.dir = refract(ray.dir,(float3){0.0f,-1.0f,0.0f},1.0/1.52);
                    }
                    else if(spos.z>mtdt.z-mtdt2.z-0.0001&&spos.z<mtdt.z-mtdt2.z+0.0001){
                        ray.dir = refract(ray.dir,(float3){0.0f,0.0f,-1.0f},1.0/1.52);
                    }
                    else if(spos.x>mtdt.x+mtdt2.x-0.0001&&spos.x<mtdt.x+mtdt2.x+0.0001){
                        ray.dir = refract(ray.dir,(float3){1.0f,0.0f,0.0f},1.0/1.52);
                    }
                    else if(spos.y>mtdt.y+mtdt2.y-0.0001f&&spos.y<mtdt.y+mtdt2.y+0.0001f){
                        ray.dir = refract(ray.dir,(float3){0.0f,1.0f,0.0f},1.0f/1.52f);
                    }
                    else if(spos.z>mtdt.z+mtdt2.z-0.0001f&&spos.z<mtdt.z+mtdt2.z+0.0001f){
                        ray.dir = refract(ray.dir,(float3){0.0f,0.0f,1.0f},1.0f/1.52f);
                    }
                    spos += ray.dir * iBox2(spos-mtdt.xyz,ray.dir,mtdt2.xyz);
                    if(refractDynamicCube(spos,mtdt.xyz,mtdt2.xyz,&ray.dir)==0.0f){
                        spos += ray.dir * iBox2(spos-mtdt.xyz,ray.dir,mtdt2.xyz);
                        refractDynamicCube(spos,mtdt.xyz,mtdt2.xyz,&ray.dir);
                    }
                    ray = rayCreate((float3){ray.ipos.x+spos.x,(float)ray.ipos.y+spos.y,(float)ray.ipos.z+spos.z},ray.dir);
                    red   *= sqrt((float)map[block*4+1]/265.0f);
                    green *= sqrt((float)map[block*4+2]/265.0f);
                    blue  *= sqrt((float)map[block*4+3]/265.0f);
                    if(red < 0.01f && green < 0.01f && blue < 0.01f){
                        return;
                    }
                    break;
                }
            }
            break;
        }
        case 27:
        case 28:{
            switch(ray.sid){
            case 0:{
                wall.x = tfract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y);
                wall.y = tfract(ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z);
                uint offset = (int)(tfract(wall.y)*lmapSz)*lmapSz+(int)(tfract(wall.x)*lmapSz);
                if(ray.dir.x > 0.0f){
                    lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+0] += red;
                    lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+1] += green;
                    lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+2] += blue;
                    dir = (float3){(rnd()-2.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-2.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                    }
                    dir = normalize(dir);
                    ray = rayCreate((float3){ray.ipos.x,ray.ipos.y+tfract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y),ray.ipos.z+tfract(ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z)},dir);
                }
                else{
                    lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+0] += red;
                    lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+1] += green;
                    lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+2] += blue;
                    dir = (float3){(rnd()-1.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                    for(int i = 0;i < 2;i++){
                        dir +=(float3){(rnd()-1.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                    }
                    dir = normalize(dir);
                    ray = rayCreate((float3){ray.ipos.x+1,ray.ipos.y+tfract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y),ray.ipos.z+tfract(ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z)},dir);
                }
                break;
            }
            case 1:{
                wall.x = tfract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x);
                wall.y = tfract(ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z);
                unsigned int offset = (int)(tfract(wall.y)*lmapSz)*lmapSz+(int)(tfract(wall.x)*lmapSz);
                if(ray.dir.y > 0.0f){
                    lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+0] += red;
                    lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+1] += green;
                    lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+2] += blue;
                    dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-2.0f),(rnd()-1.5f)*2.0f};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-2.0f),(rnd()-1.5f)*2.0f};
                    }
                    dir = normalize(dir);
                    ray = rayCreate((float3){ray.ipos.x+tfract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x),ray.ipos.y,ray.ipos.z+tfract(ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z)},dir);
                }
                else{
                    lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+0] += red;
                    lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+1] += green;
                    lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+2] += blue;
                    dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.0f),(rnd()-1.5f)*2.0f};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.0f),(rnd()-1.5f)*2.0f};
                    }
                    dir = normalize(dir);
                    ray = rayCreate((float3){ray.ipos.x+tfract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x),ray.ipos.y+1,ray.ipos.z+tfract(ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z)},dir);
                }
                break;
            }
            case 2:{
                wall.x = tfract(ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x);
                wall.y = tfract(ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y);
                unsigned int offset = (int)(tfract(wall.y)*lmapSz)*lmapSz+(int)(tfract(wall.x)*lmapSz);
                if(ray.dir.z > 0.0f){
                    lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+0] += red;
                    lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+1] += green;
                    lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+2] += blue;
                    dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-2.0f)};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-2.0f)};
                    }
                    dir = normalize(dir);
                    ray = rayCreate((float3){ray.ipos.x+tfract(ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x),ray.ipos.y+tfract(ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y),ray.ipos.z},dir);
                }
                else{
                    lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+0] += red;
                    lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+1] += green;
                    lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+2] += blue;
                    dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-1.0f)};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-1.0f)};
                    }
                    dir = normalize(dir);
                    ray = rayCreate((float3){ray.ipos.x+tfract(ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x),ray.ipos.y+tfract(ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y),ray.ipos.z+1},dir);
                }
                break;
            }
            }
            red   *= sqrt((float)map[block*4+1]/265.0f);
            green *= sqrt((float)map[block*4+2]/265.0f);
            blue  *= sqrt((float)map[block*4+3]/265.0f);
            if(red < 0.01f && green < 0.01f && blue < 0.01f){
                return;
            }
            break;
        }
        }
        rayItterate(&ray);
    }
}

kernel void ambientZ(global uchar *map,global uchar *metadt,global uchar *metadt2,global uchar *metadt3,
global uchar *metadt4,global uchar *metadt5,global uchar *metadt6,
global float *lightmap,global uint *lpmap,uint mapSz,uint lmapSz,
float red,float green,float blue,
float ax,float ay,float az,float pos){
    float3 dir = normalize((float3){ax,ay,az});
    RAY ray = rayCreate((float3){(rnd()-1.0)*mapSz,(rnd()-1.0)*mapSz,pos},dir);
    rayItterate(&ray);
    while(ray.ipos.x >= 0 && ray.ipos.y >= 0 && ray.ipos.z >= 0 && ray.ipos.x < 64 && ray.ipos.y < 64 && ray.ipos.z < 64){
        uint block = ray.ipos.x+ray.ipos.y*mapSz+ray.ipos.z*mapSz*mapSz;
        switch(map[block*4]){
        case 12:{
            float3 spos = getSubCoords(ray);
            float3 mtdt = {(float)metadt[block*4]/255.0f,(float)metadt[block*4+1]/255.0f,(float)metadt[block*4+2]/255.0f};
            float3 mtdt2 = {(float)metadt2[block*4]/255.0f,(float)metadt2[block*4+1]/255.0f,(float)metadt2[block*4+2]/255.0f};
            float3 rotdir = ray.dir;
            mtdt.xy = rotVEC2(mtdt.xy,(float)metadt3[block*4]/255.0*PI);
            mtdt.xz = rotVEC2(mtdt.xz,(float)metadt3[block*4+1]/255.0*PI);
            mtdt.yz = rotVEC2(mtdt.yz,(float)metadt3[block*4+2]/255.0*PI);
            spos.xy = rotVEC2(spos.xy,(float)metadt3[block*4]/255.0*PI);
            spos.xz = rotVEC2(spos.xz,(float)metadt3[block*4+1]/255.0*PI);
            spos.yz = rotVEC2(spos.yz,(float)metadt3[block*4+2]/255.0*PI);
            rotdir.xy = rotVEC2(rotdir.xy,(float)metadt3[block*4]/255.0*PI);
            rotdir.xz = rotVEC2(rotdir.xz,(float)metadt3[block*4+1]/255.0*PI);
            rotdir.yz = rotVEC2(rotdir.yz,(float)metadt3[block*4+2]/255.0*PI);
            float d = iBox(spos-mtdt,rotdir,(float3){metadt2[block*4],metadt2[block*4+1],metadt2[block*4+2]}/255.0f);
            if(d > 0){
                spos += rotdir * d;
                float3 nspos = spos;
                nspos.yz = rotVEC2(nspos.yz,-(float)metadt3[block*4+2]/255.0f*PI);
                nspos.xz = rotVEC2(nspos.xz,-(float)metadt3[block*4+1]/255.0f*PI);
                nspos.xy = rotVEC2(nspos.xy,-(float)metadt3[block*4]/255.0f*PI);
				if(nspos.x>=0.0&&nspos.y>=0.0&&nspos.z>=0.0&&nspos.x<=1.0&&nspos.y<=1.0&&nspos.z<=1.0){
                    if(spos.x>mtdt.x-mtdt2.x-0.0001&&spos.x<mtdt.x-mtdt2.x+0.0001){
                        float mt = tmax(mtdt2.y,mtdt2.z)*2.0;
                        uint xt = (spos.y-mtdt.y+mtdt2.y)*lmapSz/mt;
                        uint yt = (spos.z-mtdt.z+mtdt2.z)*lmapSz/mt;
                        uint offset = xt + yt * lmapSz;
                        lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+0] += red   / mt / mt;
                        lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                        lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+2] += blue  / mt / mt;
                        dir = (float3){(rnd()-2.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                        for(int i = 0;i < 2;i++){
                            dir += (float3){(rnd()-2.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                        }
                    }
                    if(spos.y>mtdt.y-mtdt2.y-0.0001&&spos.y<mtdt.y-mtdt2.y+0.0001){
                        float mt = tmax(mtdt2.x,mtdt2.z)*2.0;
                        uint xt = (spos.x-mtdt.x+mtdt2.x)*lmapSz/mt;
                        uint yt = (spos.z-mtdt.z+mtdt2.z)*lmapSz/mt;
                        uint offset = xt + yt * lmapSz;
                        lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+0] += red   / mt / mt;
                        lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                        lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+2] += blue  / mt / mt;
                        dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-2.0f),(rnd()-1.5f)*2.0f};
                        for(int i = 0;i < 2;i++){
                            dir +=(float3){(rnd()-1.5f)*2.0f,(rnd()-2.0f),(rnd()-1.5f)*2.0f};
                        }
                    }
                    if(spos.z>mtdt.z-mtdt2.z-0.0001&&spos.z<mtdt.z-mtdt2.z+0.0001){
                        float mt = tmax(mtdt2.x,mtdt2.y)*2.0;
                        uint xt = (spos.x-mtdt.x+mtdt2.x)*lmapSz/mt;
                        uint yt = (spos.y-mtdt.y+mtdt2.y)*lmapSz/mt;
                        uint offset = xt + yt * lmapSz;
                        lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+0] += red   / mt / mt;
                        lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                        lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+2] += blue  / mt / mt;
                        dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-2.0f)};
                        for(int i = 0;i < 2;i++){
                            dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-2.0f)};
                        }
                    }
                    if(spos.x>mtdt.x+mtdt2.x-0.0001&&spos.x<mtdt.x+mtdt2.x+0.0001){
                        float mt = tmax(mtdt2.y,mtdt2.z)*2.0;
                        uint xt = (spos.y-mtdt.y+mtdt2.y)*lmapSz/mt;
                        uint yt = (spos.z-mtdt.z+mtdt2.z)*lmapSz/mt;
                        uint offset = xt + yt * lmapSz;
                        lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+0] += red   / mt / mt;
                        lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                        lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+2] += blue  / mt / mt;
                        dir = (float3){(rnd()-1.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                        for(int i = 0;i < 2;i++){
                            dir += (float3){(rnd()-1.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                        }
                    }
                    if(spos.y>mtdt.y+mtdt2.y-0.0001&&spos.y<mtdt.y+mtdt2.y+0.0001){
                        float mt = tmax(mtdt2.x,mtdt2.z)*2.0;
                        uint xt = (spos.x-mtdt.x+mtdt2.x)*lmapSz/mt;
                        uint yt = (spos.z-mtdt.z+mtdt2.z)*lmapSz/mt;
                        uint offset = xt + yt * lmapSz;
                        lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+0] += red   / mt / mt;
                        lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                        lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+2] += blue  / mt / mt;
                        dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.0f),(rnd()-1.5f)*2.0f};
                        for(int i = 0;i < 2;i++){
                            dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.0f),(rnd()-1.5f)*2.0f};
                        }
                    }
                    if(spos.z>mtdt.z+mtdt2.z-0.0001&&spos.z<mtdt.z+mtdt2.z+0.0001){
                        float mt = tmax(mtdt2.x,mtdt2.y)*2.0;
                        uint xt = (spos.x-mtdt.x+mtdt2.x)*lmapSz/mt;
                        uint yt = (spos.y-mtdt.y+mtdt2.y)*lmapSz/mt;
                        uint offset = xt + yt * lmapSz;
                        lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+0] += red   / mt / mt;
                        lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+1] += green / mt / mt;
                        lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+2] += blue  / mt / mt;
                        dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-1.0f)};
                        for(int i = 0;i < 2;i++){
                            dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-1.0f)};
                        }
                    }
                    dir.xy = rotVEC2(dir.xy,(float){metadt3[block*4]}/255.0*PI);
                    dir.xz = rotVEC2(dir.xz,(float){metadt3[block*4+1]}/255.0*PI);
                    dir.yz = rotVEC2(dir.yz,(float){metadt3[block*4+2]}/255.0*PI);
                    ray = rayCreate((float3){ray.ipos.x,(float)ray.ipos.y,(float)ray.ipos.z}+nspos,normalize(dir));
                    red   *= sqrt((float)map[block*4+1]/265.0f);
                    green *= sqrt((float)map[block*4+2]/265.0f);
                    blue  *= sqrt((float)map[block*4+3]/265.0f);
                    if(red < 0.01f && green < 0.01f && blue < 0.01f){
                        return;
                    }
                }
            }
            break;
        }
        case 15:{
            float3 spos = getSubCoords(ray);
            float3 mtdt = {(float)metadt[block*4]/255.0f,(float)metadt[block*4+1]/255.0f,(float)metadt[block*4+2]/255.0f};
            float3 mtdt2= {(float)metadt2[block*4]/255.0f,(float)metadt2[block*4+1]/255.0f,(float)metadt2[block*4+2]/255.0f};
            float d = iBox(spos-mtdt,ray.dir,mtdt2);
            if(d > 0.0f){
                spos += ray.dir * d;
                if(spos.x>=0.0f&&spos.y>=0.0f&&spos.z>=0.0f&&spos.x<=1.0&&spos.y<=1.0f&&spos.z<=1.0f){
                    if(spos.x>mtdt.x-mtdt2.x-0.0001f&&spos.x<mtdt.x-mtdt2.x+0.0001f){
                        ray.dir = refract(ray.dir,(float3){-1.0f,0.0f,0.0f},1.0f/1.52f);
                    }
                    else if(spos.y>mtdt.y-mtdt2.y-0.0001&&spos.y<mtdt.y-mtdt2.y+0.0001f){
                        ray.dir = refract(ray.dir,(float3){0.0f,-1.0f,0.0f},1.0/1.52);
                    }
                    else if(spos.z>mtdt.z-mtdt2.z-0.0001&&spos.z<mtdt.z-mtdt2.z+0.0001){
                        ray.dir = refract(ray.dir,(float3){0.0f,0.0f,-1.0f},1.0/1.52);
                    }
                    else if(spos.x>mtdt.x+mtdt2.x-0.0001&&spos.x<mtdt.x+mtdt2.x+0.0001){
                        ray.dir = refract(ray.dir,(float3){1.0f,0.0f,0.0f},1.0/1.52);
                    }
                    else if(spos.y>mtdt.y+mtdt2.y-0.0001f&&spos.y<mtdt.y+mtdt2.y+0.0001f){
                        ray.dir = refract(ray.dir,(float3){0.0f,1.0f,0.0f},1.0f/1.52f);
                    }
                    else if(spos.z>mtdt.z+mtdt2.z-0.0001f&&spos.z<mtdt.z+mtdt2.z+0.0001f){
                        ray.dir = refract(ray.dir,(float3){0.0f,0.0f,1.0f},1.0f/1.52f);
                    }
                    spos += ray.dir * iBox2(spos-mtdt.xyz,ray.dir,mtdt2.xyz);
                    if(refractDynamicCube(spos,mtdt.xyz,mtdt2.xyz,&ray.dir)==0.0f){
                        spos += ray.dir * iBox2(spos-mtdt.xyz,ray.dir,mtdt2.xyz);
                        refractDynamicCube(spos,mtdt.xyz,mtdt2.xyz,&ray.dir);
                    }
                    ray = rayCreate((float3){ray.ipos.x+spos.x,(float)ray.ipos.y+spos.y,(float)ray.ipos.z+spos.z},ray.dir);
                    red   *= sqrt((float)map[block*4+1]/265.0f);
                    green *= sqrt((float)map[block*4+2]/265.0f);
                    blue  *= sqrt((float)map[block*4+3]/265.0f);
                    if(red < 0.01f && green < 0.01f && blue < 0.01f){
                        return;
                    }
                    break;
                }
            }
            break;
        }
        case 27:
        case 28:{
            switch(ray.sid){
            case 0:{
                wall.x = tfract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y);
                wall.y = tfract(ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z);
                uint offset = (int)(tfract(wall.y)*lmapSz)*lmapSz+(int)(tfract(wall.x)*lmapSz);
                if(ray.dir.x > 0.0f){
                    lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+0] += red;
                    lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+1] += green;
                    lightmap[(lpmap[block*12+0]*lmapSz*lmapSz+offset)*3+2] += blue;
                    dir = (float3){(rnd()-2.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-2.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                    }
                    dir = normalize(dir);
                    ray = rayCreate((float3){ray.ipos.x,ray.ipos.y+tfract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y),ray.ipos.z+tfract(ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z)},dir);
                }
                else{
                    lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+0] += red;
                    lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+1] += green;
                    lightmap[(lpmap[block*12+1]*lmapSz*lmapSz+offset)*3+2] += blue;
                    dir = (float3){(rnd()-1.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                    for(int i = 0;i < 2;i++){
                        dir +=(float3){(rnd()-1.0f),(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f};
                    }
                    dir = normalize(dir);
                    ray = rayCreate((float3){ray.ipos.x+1,ray.ipos.y+tfract(ray.pos.y + (ray.side.x - ray.delta.x) * ray.dir.y),ray.ipos.z+tfract(ray.pos.z + (ray.side.x - ray.delta.x) * ray.dir.z)},dir);
                }
                break;
            }
            case 1:{
                wall.x = tfract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x);
                wall.y = tfract(ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z);
                unsigned int offset = (int)(tfract(wall.y)*lmapSz)*lmapSz+(int)(tfract(wall.x)*lmapSz);
                if(ray.dir.y > 0.0f){
                    lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+0] += red;
                    lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+1] += green;
                    lightmap[(lpmap[block*12+2]*lmapSz*lmapSz+offset)*3+2] += blue;
                    dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-2.0f),(rnd()-1.5f)*2.0f};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-2.0f),(rnd()-1.5f)*2.0f};
                    }
                    dir = normalize(dir);
                    ray = rayCreate((float3){ray.ipos.x+tfract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x),ray.ipos.y,ray.ipos.z+tfract(ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z)},dir);
                }
                else{
                    lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+0] += red;
                    lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+1] += green;
                    lightmap[(lpmap[block*12+3]*lmapSz*lmapSz+offset)*3+2] += blue;
                    dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.0f),(rnd()-1.5f)*2.0f};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.0f),(rnd()-1.5f)*2.0f};
                    }
                    dir = normalize(dir);
                    ray = rayCreate((float3){ray.ipos.x+tfract(ray.pos.x + (ray.side.y - ray.delta.y) * ray.dir.x),ray.ipos.y+1,ray.ipos.z+tfract(ray.pos.z + (ray.side.y - ray.delta.y) * ray.dir.z)},dir);
                }
                break;
            }
            case 2:{
                wall.x = tfract(ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x);
                wall.y = tfract(ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y);
                unsigned int offset = (int)(tfract(wall.y)*lmapSz)*lmapSz+(int)(tfract(wall.x)*lmapSz);
                if(ray.dir.z > 0.0f){
                    lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+0] += red;
                    lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+1] += green;
                    lightmap[(lpmap[block*12+4]*lmapSz*lmapSz+offset)*3+2] += blue;
                    dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-2.0f)};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-2.0f)};
                    }
                    dir = normalize(dir);
                    ray = rayCreate((float3){ray.ipos.x+tfract(ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x),ray.ipos.y+tfract(ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y),ray.ipos.z},dir);
                }
                else{
                    lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+0] += red;
                    lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+1] += green;
                    lightmap[(lpmap[block*12+5]*lmapSz*lmapSz+offset)*3+2] += blue;
                    dir = (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-1.0f)};
                    for(int i = 0;i < 2;i++){
                        dir += (float3){(rnd()-1.5f)*2.0f,(rnd()-1.5f)*2.0f,(rnd()-1.0f)};
                    }
                    dir = normalize(dir);
                    ray = rayCreate((float3){ray.ipos.x+tfract(ray.pos.x + (ray.side.z - ray.delta.z) * ray.dir.x),ray.ipos.y+tfract(ray.pos.y + (ray.side.z - ray.delta.z) * ray.dir.y),ray.ipos.z+1},dir);
                }
                break;
            }
            }
            red   *= sqrt((float)map[block*4+1]/265.0f);
            green *= sqrt((float)map[block*4+2]/265.0f);
            blue  *= sqrt((float)map[block*4+3]/265.0f);
            if(red < 0.01f && green < 0.01f && blue < 0.01f){
                return;
            }
            break;
        }
        }
        rayItterate(&ray);
    }
}
