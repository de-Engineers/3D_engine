#version 460 core

out vec4 FragColor;

in vec4 gl_FragCoord;

uniform mat3 cameraMatrix;

uniform sampler2D skybox;

uniform sampler3D map;
uniform usampler3D lpmap;

uniform sampler3D lmap;
uniform sampler3D lmap2;
uniform sampler3D lmap3;
uniform sampler3D lmap4;

uniform int tex3DszLimit;

uniform sampler3D metadt;
uniform sampler3D metadt2;
uniform sampler3D metadt3;
uniform sampler3D metadt4;
uniform sampler3D metadt5;
uniform sampler3D metadt6;

uniform int entityC;
uniform sampler1D entity;
uniform sampler3D entityTextures;

uniform sampler2D godraymap;
uniform sampler2D reflectmap;

uniform vec3 Pos;

uniform vec2 angle;	
uniform vec2 fov;
uniform vec4 dir;
uniform ivec2 reso;

uniform int tick;
uniform int health;

uniform float brightness;

uniform int mapSz;

uniform int renderDistance;

uniform int state;

uniform float lmapsz;

int x,y,z;
vec3 pos;
vec3 ang;
vec3 tr;
vec3 sid;
int stepX,stepY,stepZ;
float vx,vy,vz;
float deltaX,deltaY,deltaZ;
int side;
vec2 wall,awall;
int effect;
vec4 fog;
vec3 ePos;
vec3 bAng;
ivec2 pon;
vec3 filtr;
vec4 block;

#define PI_05  1.57079632679
#define PI_2   6.28318530718
#define PI     3.14159265358
#define PI_025 0.78539816339

uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}

uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y)                         ); }
uint hash( uvec3 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z)             ); }
uint hash( uvec4 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w) ); }


float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu; 
    const uint ieeeOne      = 0x3F800000u; 

    m &= ieeeMantissa;                   
    m |= ieeeOne;                      

    float  f = uintBitsToFloat( m );      
    return f - 1.0;                       
}

float random( float x ) { return floatConstruct(hash(floatBitsToUint(x))); }
float random( vec2  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec3  v ) { return floatConstruct(hash(floatBitsToUint(v))); }

float tnoise2(vec2 pos){
	vec2 fpos = floor(pos); 
	vec2 fpos2 = fract(pos);
	float r1 = random(fpos);
	float r2 = random(fpos+vec2(1.0,0.0));
	float r3 = random(fpos+vec2(0.0,1.0));
	float r4 = random(fpos+vec2(1.0,1.0));
	float m1 = mix(r1,r2,fpos2.x);
	float m2 = mix(r3,r4,fpos2.x);
	return mix(m1,m2,fpos2.y);
}

float rdist(vec2 p1,vec2 p2){
    return abs(p1.x-p2.x)+abs(p1.y-p2.y);
}	

vec3 getSubCoords(){
	switch(side){
	case 0:
		if(ang.x > 0.0){
			return vec3(0.0,wall);
		}
		else{
			return vec3(1.0,wall);
		}
	case 1:
		if(ang.y > 0.0){
			return vec3(wall.x,0.0,wall.y);
		}
		else{
			return vec3(wall.x,1.0,wall.y);
		}
	case 2:
		if(ang.z > 0.0){
			return vec3(wall,0.0);
		}
		else{
			return vec3(wall,1.0);
		}
	}
}

vec3 getRayPos(){
	switch(side){
	case 0:
		if(ang.x > 0.0){
			return vec3(x,awall);
		}
		else{
			return vec3(x+1,awall);
		}
	case 1:
		if(ang.y > 0.0){
			return vec3(awall.x,y,awall.y);
		}
		else{
			return vec3(awall.x,y+1,awall.y);
		}
	case 2:
		if(ang.z > 0.0){
			return vec3(awall,z);
		}
		else{
			return vec3(awall,z+1);
		}
	}
}

float iPlane(vec3 ro,vec3 rd,vec4 p){
	return -(dot(ro,p.xyz)+p.w)/dot(rd,p.xyz);
}

float iSphere(vec3 ro,vec3 rd,float ra){
    float b = dot(ro,rd);
    float c = dot(ro,ro) - ra*ra;
    float h = b*b - c;
    if(h<0.0){
        return -1.0;
    }
    return -b-sqrt(h);
}

float iSphere2(vec3 ro,vec3 rd,float ra){
    float b = dot(ro,rd);
    float c = dot(ro,ro) - ra*ra;
    float h = b*b - c;
    if(h<0.0){
        return -1.0;
    }
    return -b+sqrt(h);
}

float iTorus(vec3 ro,vec3 rd,vec2 tor){
    float po = 1.0;
    float Ra2 = tor.x*tor.x;
    float ra2 = tor.y*tor.y;
    float m = dot(ro,ro);
    float n = dot(ro,rd);
    float k = (m + Ra2 - ra2)/2.0;
    float k3 = n;
    float k2 = n*n - Ra2*dot(rd.xy,rd.xy) + k;
    float k1 = n*k - Ra2*dot(rd.xy,ro.xy);
    float k0 = k*k - Ra2*dot(ro.xy,ro.xy);
    
    if(abs(k3*(k3*k3-k2)+k1) < 0.01 )
    {
        po = -1.0;
        float tmp=k1; k1=k3; k3=tmp;
        k0 = 1.0/k0;
        k1 = k1*k0;
        k2 = k2*k0;
        k3 = k3*k0;
    }
    
    float c2 = k2*2.0 - 3.0*k3*k3;
    float c1 = k3*(k3*k3-k2)+k1;
    float c0 = k3*(k3*(c2+2.0*k2)-8.0*k1)+4.0*k0;
    c2 /= 3.0;
    c1 *= 2.0;
    c0 /= 3.0;
    float Q = c2*c2 + c0;
    float R = c2*c2*c2 - 3.0*c2*c0 + c1*c1;
    float h = R*R - Q*Q*Q;
    
    if( h>=0.0 )  
    {
        h = sqrt(h);
        float v = sign(R+h)*pow(abs(R+h),1.0/3.0); // cube root
        float u = sign(R-h)*pow(abs(R-h),1.0/3.0); // cube root
        vec2 s = vec2( (v+u)+4.0*c2, (v-u)*sqrt(3.0));
        float y = sqrt(0.5*(length(s)+s.x));
        float x = 0.5*s.y/y;
        float r = 2.0*c1/(x*x+y*y);
        float t1 =  x - r - k3; t1 = (po<0.0)?2.0/t1:t1;
        float t2 = -x - r - k3; t2 = (po<0.0)?2.0/t2:t2;
        float t = 1e20;
        if( t1>0.0 ) t=t1;
        if( t2>0.0 ) t=min(t,t2);
        return t;
    }
    
    float sQ = sqrt(Q);
    float w = sQ*cos( acos(-R/(sQ*Q)) / 3.0 );
    float d2 = -(w+c2); if( d2<0.0 ) return -1.0;
    float d1 = sqrt(d2);
    float h1 = sqrt(w - 2.0*c2 + c1/d1);
    float h2 = sqrt(w - 2.0*c2 - c1/d1);
    float t1 = -d1 - h1 - k3; t1 = (po<0.0)?2.0/t1:t1;
    float t2 = -d1 + h1 - k3; t2 = (po<0.0)?2.0/t2:t2;
    float t3 =  d1 - h2 - k3; t3 = (po<0.0)?2.0/t3:t3;
    float t4 =  d1 + h2 - k3; t4 = (po<0.0)?2.0/t4:t4;
    float t = 1e20;
    if( t1>0.0 ) t=t1;
    if( t2>0.0 ) t=min(t,t2);
    if( t3>0.0 ) t=min(t,t3);
    if( t4>0.0 ) t=min(t,t4);
    return t;
}

float sphDensity( vec3 ro, vec3 rd, float rad)
{
    float ndbuffer = 1000.0/rad;
    vec3  rc = ro/rad;
	
    float b = dot(rd,rc);
    float c = dot(rc,rc) - 1.0;
    float h = b*b - c;
    if( h<0.0 ) return 0.0;
    h = sqrt( h );
    float t1 = -b - h;
    float t2 = -b + h;

    if( t2<0.0 || t1>ndbuffer ) return 0.0;
    t1 = max( t1, 0.0 );
    t2 = min( t2, ndbuffer );

    float i1 = -(c*t1 + b*t1*t1 + t1*t1*t1/3.0);
    float i2 = -(c*t2 + b*t2*t2 + t2*t2*t2/3.0);
    return (i2-i1)*(3.0/4.0);
}

float iBox(vec3 ro,vec3 rd,vec3 boxSize){
    vec3 m = 1.0/rd;
    vec3 n = m*ro;
    vec3 k = abs(m)*boxSize;
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;
    float tN = max( max( t1.x, t1.y ), t1.z );
    float tF = min( min( t2.x, t2.y ), t2.z );
    if( tN>tF || tF<0.0) return -1.0;
    return tN;
}

float iBox2(vec3 ro,vec3 rd,vec3 boxSize){
    vec3 m = 1.0/rd;
    vec3 n = m*ro;
    vec3 k = abs(m)*boxSize;
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;
    float tN = max( max( t1.x, t1.y ), t1.z );
    float tF = min( min( t2.x, t2.y ), t2.z );
    if( tN>tF || tF<0.0) return -1.0;
    return tF;
}

vec3 permute(vec3 x) { return mod(((x*34.0)+1.0)*x, 289.0); }

float snoise(vec2 v){
  const vec4 C = vec4(0.211324865405187, 0.366025403784439,
           -0.577350269189626, 0.024390243902439);
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);
  vec2 i1;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;
  i = mod(i, 289.0);
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
  + i.x + vec3(0.0, i1.x, 1.0 ));
  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy),
    dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;
  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}

float iCylinder(vec3 ro,vec3 rd,vec3 pb, float ra){
	vec3 pa = -pb;
    vec3 ca = pb-pa;
    vec3 oc = ro-pa;
    float caca = dot(ca,ca);
    float card = dot(ca,rd);
    float caoc = dot(ca,oc);
    float a = caca - card*card;
    float b = caca*dot( oc, rd) - caoc*card;
    float c = caca*dot( oc, oc) - caoc*caoc - ra*ra*caca;
    float h = b*b - a*c;
    if( h<0.0 ) return -1.0;
    h = sqrt(h);
    float t = (-b-h)/a;
    float y = caoc + t*card;
    if( y>0.0 && y<caca ) return t;
    t = (((y<0.0)?0.0:caca) - caoc)/card;
    if( abs(b+a*t)<h ) return t;
    return -1.0;
}

float iLaser2(vec3 ro,vec3 rd,vec3 pb,float r){
    vec3  oa = ro - pb;
    float baba = dot(pb,pb);
    float bard = dot(pb,rd);
    float baoa = dot(pb,oa);
    float rdoa = dot(rd,oa);
    float oaoa = dot(oa,oa);
    float a = baba      - bard*bard;
    float b = baba*rdoa - baoa*bard;
    float c = baba*oaoa - baoa*baoa - r*r*baba;
    float h = b*b - a*c;
    if( h>=0.0 ){
        float t = (-b+sqrt(h))/a;
        float y = baoa + t*bard;
        if( y>0.0 && y<baba ) return t;
        vec3 oc = (y<=0.0) ? oa : ro - pb;
        b = dot(rd,oc);
        c = dot(oc,oc) - r*r;
        h = b*b - c;
        if( h>0.0 ) return -b + sqrt(h);
    }
    return -1.0;
}

float iLaser(vec3 ro,vec3 rd,vec3 pb,float r){
    vec3  oa = ro - pb;
    float baba = dot(pb,pb);
    float bard = dot(pb,rd);
    float baoa = dot(pb,oa);
    float rdoa = dot(rd,oa);
    float oaoa = dot(oa,oa);
    float a = baba      - bard*bard;
    float b = baba*rdoa - baoa*bard;
    float c = baba*oaoa - baoa*baoa - r*r*baba;
    float h = b*b - a*c;
    if( h>=0.0 ){
        float t = (-b-sqrt(h))/a;
        float y = baoa + t*bard;
        if( y>0.0 && y<baba ) return t;
        vec3 oc = (y<=0.0) ? oa : ro - pb;
        b = dot(rd,oc);
        c = dot(oc,oc) - r*r;
        h = b*b - c;
        if( h>0.0 ) return -b - sqrt(h);
    }
    return -1.0;
}

vec2 rotVEC2(vec2 p,float rot){
    vec2 r;
    r.x = cos(rot) * p.x - sin(rot) * p.y;
    r.y = sin(rot) * p.x + cos(rot) * p.y;
    return r;
}

float sdVerticalCapsule(vec3 p, float h, float r ){
  p.z -= clamp( p.z, 0.0, h );
  return max(length( p ) - r,0.0);
}

float sdSegment(vec3 p,vec3 b,float rad ){
    float h = clamp( dot(p,b)/dot(b,b), 0.0, 1.0 );
    return length(p - b*h) - rad;
}

void calcFog(){
	FragColor.rgb *= filtr;
	FragColor.rgb = mix(FragColor.rgb,fog.rgb,clamp(fog.a,0.0,1.0));
	float d = distance(Pos,ePos);
	for(int i = 0;i < entityC;i++){
		vec3 enP = texelFetch(entity,i*5,0).xyz;
		float id = texelFetch(entity,i*5,0).w;
		switch(int(id)){
		case 0:{
			float rad = texelFetch(entity,i*5+1,0).w;
			float e = iSphere(Pos-enP,bAng,rad);
			if(e > 0.0 && e < d){
				vec3 col = texelFetch(entity,i*5+1,0).xyz;
				vec3 spos = (Pos + bAng * e) - enP;
				vec2 rot = texelFetch(entity,i*5+2,0).xy;
				spos.xz = rotVEC2(spos.xz,rot.x);
				spos.yz = rotVEC2(spos.yz,rot.y);
				FragColor.rgb = texelFetch(entityTextures,ivec3((atan(spos.x,spos.y)/PI+1.0)*16.0,(spos.z+rad)/rad*16.0,texelFetch(entity,i*5+2,0).w),0).rgb * col;
				return;
			}
			}
			break;
		case 1:{
			vec3 pos2 = texelFetch(entity,i*5+2,0).xyz;
			float rad = texelFetch(entity,i*5+1,0).w;
			float e = iLaser(Pos-enP+pos2,bAng,pos2,rad);
			vec3 col = texelFetch(entity,i*5+1,0).xyz;
			if(e > 0.0 && e < d){
				vec3 spos = (Pos + bAng * e);
				FragColor.rgb += pow(iLaser2(spos-enP+pos2,bAng,pos2,rad),2.0)*4.0*col;
			}
			FragColor.rgb += vec3(0.2)/sdSegment(ePos-enP,pos2,rad)*col;
			break;
		}
		case 2:{
			vec3 pos2 = texelFetch(entity,i*5+2,0).xyz;
			float rad = texelFetch(entity,i*5+1,0).w;
			float e = iCylinder(Pos-enP-pos2*0.5,bAng,pos2*0.5,rad);
			vec3 col = texelFetch(entity,i*5+1,0).xyz;
			if(e > 0.0 && e < d){
				vec3 spos = (Pos + bAng * e);
				FragColor.rgb = col*10.0;
				return;
			}
			FragColor.rgb += vec3(2.0)/sdSegment(ePos-enP,pos2,rad)*col;
			break;
		}
		case 3:{
			vec3 pos2 = texelFetch(entity,i*5+2,0).xyz;
			float rad = texelFetch(entity,i*5+1,0).w;
			float e = iCylinder(Pos-enP-pos2*0.5,bAng,pos2*0.5,rad);
			if(e > 0.0 && e < d){
				vec3 col = texelFetch(entity,i*5+1,0).xyz;
				vec3 spos = (Pos + bAng * e) - enP;
				spos.xy = rotVEC2(spos.xy,texelFetch(entity,i*5+4,0).x);
				vec2 textN = vec2((atan(spos.x,spos.y)/PI+1.0)*16.0,(spos.z+1.8)*16.0);
				FragColor.rgb = texelFetch(entityTextures,ivec3(textN,texelFetch(entity,i*5+2,0).w),0).rgb*col;
				return;
			}
			FragColor.rgb -= vec3(0.05)/sqrt(sdSegment(ePos-enP,pos2,rad));
			break;
		}
		}
	}
	FragColor.rgb /= clamp(0.1/sdVerticalCapsule(Pos-ePos-vec3(0.0,0.0,0.3),1.2,0.1),-1.0,1.0)+1.0;

	vec2 kon = vec2((gl_FragCoord.x/reso.x),gl_FragCoord.y/reso.y);
	vec4 gd = texture(godraymap,kon);
	FragColor.rgb = mix(FragColor.rgb,gd.rgb,distance(pos,ePos)*0.03);
	
	FragColor.rgb = pow(FragColor.rgb,vec3(0.75));
	FragColor.r += 0.0006 * distance(gl_FragCoord.xy,vec2(reso)/2.0) * float(health) / 75.0;
}

void rayItterate(){
	if(sid.x < sid.y){
        if(sid.x < sid.z){
			x += stepX;
			sid.x += deltaX;
			side = 0;
        }
        else{
			z += stepZ;
			sid.z += deltaZ;
			side = 2;
        }
    }
	else{
		if(sid.y < sid.z){
			y += stepY;
			sid.y += deltaY;
			side = 1;
		}
		else{
			z += stepZ;
			sid.z += deltaZ;
			side = 2;
		}
	}
}
void rayInitialize(vec3 spos){
	vec3 apos;
	apos.x = x + spos.x;
	apos.y = y + spos.y;
	apos.z = z + spos.z;
	deltaX = abs(1.0 / ang.x);
	deltaY = abs(1.0 / ang.y);
	deltaZ = abs(1.0 / ang.z);
	pos = apos;
	if(ang.x < 0){
		stepX = -1;
		sid.x= fract(apos.x) * deltaX;
	}
	else{
		stepX = 1;
		sid.x = (int(apos.x) + 1.0 - apos.x) * deltaX;
	}
	if(ang.y < 0){
		stepY = -1;	
		sid.y = fract(apos.y) * deltaY;
	}
	else{
		stepY = 1;
		sid.y = (int(apos.y) + 1.0 - apos.y) * deltaY;
	}
	if(ang.z < 0){
		stepZ = -1;
		sid.z = fract(apos.z) * deltaZ;
	}
	else{
		stepZ = 1;
		sid.z = (int(apos.z) + 1.0 - apos.z) * deltaZ;
	}
}

void setWall(){
	switch(side){
	case 0:
		awall.x = pos.y + (sid.x - deltaX) * ang.y;
		awall.y = pos.z + (sid.x - deltaX) * ang.z;
		break;
	case 1:
		awall.x = pos.x + (sid.y - deltaY) * ang.x;
		awall.y = pos.z + (sid.y - deltaY) * ang.z;
		break;
	case 2:
		awall.x = pos.x + (sid.z - deltaZ) * ang.x;
		awall.y = pos.y + (sid.z - deltaZ) * ang.y;
		break;
	}
	wall = fract(awall);
}

float refractDynamicCube(vec3 spos,vec3 mtdt,vec3 mtdt2){
	vec3 tang;
	if(spos.x>mtdt.x-mtdt2.x-0.0001&&spos.x<mtdt.x-mtdt2.x+0.0001){
		tang = refract(ang,vec3(1.0,0.0,0.0),1.52);
		if(tang.x == 0.0 && tang.y == 0.0 && tang.z == 0.0){
			ang = reflect(ang,vec3(1.0,0.0,0.0));
			return 0.0;
		}
	}
	if(spos.y>mtdt.y-mtdt2.y-0.0001&&spos.y<mtdt.y-mtdt2.y+0.0001){
		tang = refract(ang,vec3(0.0,1.0,0.0),1.52);
		if(tang.x == 0.0 && tang.y == 0.0 && tang.z == 0.0){
			ang = reflect(ang,vec3(0.0,1.0,0.0));
			return 0.0;
		}
	}
	if(spos.z>mtdt.z-mtdt2.z-0.0001&&spos.z<mtdt.z-mtdt2.z+0.0001){
		tang = refract(ang,vec3(0.0,0.0,1.0),1.52);
		if(tang.x == 0.0 && tang.y == 0.0 && tang.z == 0.0){
			ang = reflect(ang,vec3(0.0,0.0,1.0));
			return 0.0;
		}
	}
	if(spos.x>mtdt.x+mtdt2.x-0.0001&&spos.x<mtdt.x+mtdt2.x+0.0001){
		tang = refract(ang,vec3(-1.0,0.0,0.0),1.52);
		if(tang.x == 0.0 && tang.y == 0.0 && tang.z == 0.0){
			ang = reflect(ang,vec3(-1.0,0.0,0.0));
			return 0.0;
		}
	}
	if(spos.y>mtdt.y+mtdt2.y-0.0001&&spos.y<mtdt.y+mtdt2.y+0.0001){
		tang = refract(ang,vec3(0.0,-1.0,0.0),1.52);
		if(tang.x == 0.0 && tang.y == 0.0 && tang.z == 0.0){
			ang = reflect(ang,vec3(0.0,-1.0,0.0));
			return 0.0;
		}
	}
	if(spos.z>mtdt.z+mtdt2.z-0.0001&&spos.z<mtdt.z+mtdt2.z+0.0001){
		tang = refract(ang,vec3(0.0,0.0,-1.0),1.52);
		if(tang.x == 0.0 && tang.y == 0.0 && tang.z == 0.0){
			ang = reflect(ang,vec3(0.0,0.0,-1.0));
			return 0.0;
		}
	}
	ang = tang;
	return 1.0;
}

void main(){
	FragColor.a = 1.0;
	filtr = vec3(1.0);
	fog.rgb = vec3(1.0);

	vec2 rg;

	float pixelOffsetY = fov.y * ((gl_FragCoord.y * 2.0 / reso.y) - 1);
	float pixelOffsetX = fov.x * ((gl_FragCoord.x * 2.0 / reso.x) - 1);

    ang.x = (dir.x * dir.w - dir.x * dir.z * pixelOffsetY) - dir.y * pixelOffsetX;
    ang.y = (dir.y * dir.w - dir.y * dir.z * pixelOffsetY) + dir.x * pixelOffsetX;
    ang.z = dir.z + dir.w * pixelOffsetY;

	ang = normalize(ang);

	bAng = ang;

	pos = Pos;
	
	deltaX = abs(1 / ang.x);
	deltaY = abs(1 / ang.y);
	deltaZ = abs(1 / ang.z);

	if(ang.x < 0){
		stepX = -1;
		sid.x = fract(pos.x) * deltaX;
	}
	else{
		stepX = 1;
		sid.x = (int(pos.x) + 1.0 - pos.x) * deltaX;
	}
	if(ang.y < 0){
		stepY = -1;
		sid.y = fract(pos.y) * deltaY;
	}
	else{
		stepY = 1;
		sid.y = (int(pos.y) + 1.0 - pos.y) * deltaY;
	}
	if(ang.z < 0){
		stepZ = -1;
		sid.z = fract(pos.z) * deltaZ;
	}
	else{
		stepZ = 1;
		sid.z = (int(pos.z) + 1.0 - pos.z) * deltaZ;
	}
	x = int(pos.x);
	y = int(pos.y);
	z = int(pos.z);
	rayItterate();
    for(;;){
        block = texelFetch(map,ivec3(x,y,z),0);
		if(int(block[0]*255)!=1){
			int bside;
			switch(side){
			case 0:
				if(ang.x > 0.0){
					bside = 0;
				}
				else{
					bside = 1;
				}
				awall.x = pos.y + (sid.x - deltaX) * ang.y;
				awall.y = pos.z + (sid.x - deltaX) * ang.z;
				break;
			case 1:
				awall.x = pos.x + (sid.y - deltaY) * ang.x;
				awall.y = pos.z + (sid.y - deltaY) * ang.z;
				if(ang.y > 0.0){
					bside = 2;
				}
				else{
					bside = 3;
				}
				break;
			case 2:
				awall.x = pos.x + (sid.z - deltaZ) * ang.x;
				awall.y = pos.y + (sid.z - deltaZ) * ang.y;
				if(ang.z > 0.0){
					bside = 4;
				}
				else{
					bside = 5;
				}
				break;
			}
			wall = fract(awall);
			block[0] *= 255;
			switch(int(block[0])){
			case 0:
				FragColor.rgb = texelFetch(skybox,ivec2((atan(ang.x,ang.y)+PI)/PI_2*1023.0,(ang.z+1.0)*511.0),0).rgb;
				ePos = pos + vec3(64.0,0.0,0.0);
				calcFog();
				return;
			case 9:{
				vec3 spos = getSubCoords();
				vec4 mtdt = texelFetch(metadt,ivec3(x,y,z),0);
				float d = iSphere(spos-mtdt.xyz,ang,mtdt.a);
				if(d > 0.0){
					spos += ang * d;
					if(spos.x > 0.0 && spos.y > 0.0 && spos.z > 0.0 && spos.x < 1.0 && spos.y < 1.0 && spos.z < 1.0){
						float xcpos = (atan(spos.x-mtdt.x,spos.y-mtdt.y)+PI)/PI;
						float minz,maxz;
						maxz = mtdt.z+mtdt.a;
						minz = mtdt.z-mtdt.a;
						if(maxz > 1.0){
							maxz = 1.0;
						}
						if(minz < 0.0){
							minz = 0.0;
						}
						float rspos = spos.z - minz;
						rspos /= maxz;
						rspos *= 2.0;
						uint pointer = texelFetch(lpmap,ivec3(x*6+int(xcpos)+int(rspos)*2,y,z),0).x;
						FragColor.rgb = block.gba * texelFetch(lmap,ivec3(fract(xcpos)*lmapsz,fract(rspos)*lmapsz,pointer),0).rgb * 65535.0 / float(brightness+1);
						calcFog();
						return;
					}
				}
				break;
			}
			case 10:{
				vec3 spos = getSubCoords();
				vec4 mtdt = texelFetch(metadt,ivec3(x,y,z),0);
				float d = iSphere(spos-mtdt.xyz,ang,mtdt.a);
				if(d > 0.0){
					spos += ang * d;
					if(spos.x > 0.0 && spos.y > 0.0 && spos.z > 0.0 && spos.x < 1.0 && spos.y < 1.0 && spos.z < 1.0){
						ang = refract(ang,normalize(spos-mtdt.xyz),0.65789473684);
						rayInitialize(spos);
						spos += ang * iSphere2(spos-mtdt.xyz,ang,mtdt.a);
						ang = normalize(refract(ang,-normalize(spos-mtdt.xyz),1.52));
						rayInitialize(spos);
						fog.rgb = block.gba;
						fog.a = 0.5;
					}
				}
				break;
			}
			case 11:{
				vec3 spos = getSubCoords();
				vec4 mtdt = texelFetch(metadt,ivec3(x,y,z),0);
				float d = iSphere(spos-mtdt.xyz,ang,mtdt.a);
				if(d > 0.0){
					spos += ang * d;
					if(spos.x > 0.0 && spos.y > 0.0 && spos.z > 0.0 && spos.x < 1.0 && spos.y < 1.0 && spos.z < 1.0){
						ang = reflect(ang,normalize(spos-mtdt.xyz));
						fog.rgb = block.gba;
						ang.xy = rotVEC2(ang.xy,snoise(spos.xy*20.0)/5.0);
						ang.xz = rotVEC2(ang.xz,snoise(spos.xz*20.0)/5.0);
						ang.yz = rotVEC2(ang.yz,snoise(spos.yz*20.0)/5.0);
						rayInitialize(spos);
					}
				}
				break;
			}
			case 12:{
				vec3 spos = getSubCoords();
				vec4 mtdt = texelFetch(metadt,ivec3(x,y,z),0);
				vec4 mtdt2= texelFetch(metadt2,ivec3(x,y,z),0);
				vec4 mtdt3= texelFetch(metadt3,ivec3(x,y,z),0);
				vec3 angr = ang;
				angr.xy = rotVEC2(angr.xy,mtdt3.x*PI);
				angr.xz = rotVEC2(angr.xz,mtdt3.y*PI);
				angr.yz = rotVEC2(angr.yz,mtdt3.z*PI);
				spos.xy = rotVEC2(spos.xy,mtdt3.x*PI);
				spos.xz = rotVEC2(spos.xz,mtdt3.y*PI);
				spos.yz = rotVEC2(spos.yz,mtdt3.z*PI);
				mtdt.xy = rotVEC2(mtdt.xy,mtdt3.x*PI);
				mtdt.xz = rotVEC2(mtdt.xz,mtdt3.y*PI);
				mtdt.yz = rotVEC2(mtdt.yz,mtdt3.z*PI);
				float d = iBox(spos-mtdt.xyz,angr,mtdt2.xyz);
				if(d > 0.0){
					spos += angr * d;
					vec3 nspos = spos;
					nspos.yz = rotVEC2(nspos.yz,-mtdt3.z*PI);
					nspos.xz = rotVEC2(nspos.xz,-mtdt3.y*PI);
					nspos.xy = rotVEC2(nspos.xy,-mtdt3.x*PI);
					if(nspos.x>=0.0&&nspos.y>=0.0&&nspos.z>=0.0&&nspos.x<=1.0&&nspos.y<=1.0&&nspos.z<=1.0){
						uint sd;
						ivec2 tcrd;
						if(spos.x>mtdt.x-mtdt2.x-0.0001&&spos.x<mtdt.x-mtdt2.x+0.0001){
							float mt = max(mtdt2.y,mtdt2.z)*2.0;
							tcrd = ivec2((spos.yz-mtdt.yz+mtdt2.yz)*lmapsz/mt);
							sd = 0;
						}
						if(spos.y>mtdt.y-mtdt2.y-0.0001&&spos.y<mtdt.y-mtdt2.y+0.0001){
							float mt = max(mtdt2.x,mtdt2.z)*2.0;
							tcrd = ivec2((spos.xz-mtdt.xz+mtdt2.xz)*lmapsz/mt);
							sd = 1;
						}
						if(spos.z>mtdt.z-mtdt2.z-0.0001&&spos.z<mtdt.z-mtdt2.z+0.0001){
							float mt = max(mtdt2.x,mtdt2.y)*2.0;
							tcrd = ivec2((spos.xy-mtdt.xy+mtdt2.xy)*lmapsz/mt);
							sd = 2;
						}
						if(spos.x>mtdt.x+mtdt2.x-0.0001&&spos.x<mtdt.x+mtdt2.x+0.0001){
							float mt = max(mtdt2.y,mtdt2.z)*2.0;
							tcrd = ivec2((spos.yz-mtdt.yz+mtdt2.yz)*lmapsz/mt);
							sd = 3;
						}
						if(spos.y>mtdt.y+mtdt2.y-0.0001&&spos.y<mtdt.y+mtdt2.y+0.0001){
							float mt = max(mtdt2.x,mtdt2.z)*2.0;
							tcrd = ivec2((spos.xz-mtdt.xz+mtdt2.xz)*lmapsz/mt);
							sd = 4;
						}
						if(spos.z>mtdt.z+mtdt2.z-0.0001&&spos.z<mtdt.z+mtdt2.z+0.0001){
							float mt = max(mtdt2.x,mtdt2.y)*2.0;
							tcrd = ivec2((spos.xy-mtdt.xy+mtdt2.xy)*lmapsz/mt);
							sd = 5;
						}
						uint pointer = texelFetch(lpmap,ivec3(x*12,y,z),0).x;
						switch(pointer/tex3DszLimit){
						case 0:
							FragColor.rgb += block.gba * texelFetch(lmap,ivec3(tcrd,pointer%tex3DszLimit+sd),0).rgb * 32767.0 / (brightness+1.0);
							break;
						case 1:
							FragColor.rgb += block.gba * texelFetch(lmap2,ivec3(tcrd,pointer%tex3DszLimit+sd),0).rgb * 32767.0 / (brightness+1.0);
							break;
						case 2:
							FragColor.rgb += block.gba * texelFetch(lmap3,ivec3(tcrd,pointer%tex3DszLimit+sd),0).rgb * 32767.0 / (brightness+1.0);
							break;
						case 3:
							FragColor.rgb += block.gba * texelFetch(lmap4,ivec3(tcrd,pointer%tex3DszLimit+sd),0).rgb * 32767.0 / (brightness+1.0);
							break;
						}
						ePos = getRayPos();
						calcFog();
						return;
					}

				}
				break;
			}
			case 13:{
				vec3 spos = getSubCoords();
				vec4 mtdt = texelFetch(metadt,ivec3(x,y,z),0);
				vec4 mtdt2= texelFetch(metadt2,ivec3(x,y,z),0);
				float d = iTorus(spos-mtdt.xyz,ang,mtdt2.xy);
				if(d > 0.0){
					spos += ang * d;
					if(spos.x > 0.0 && spos.y > 0.0 && spos.z > 0.0 && spos.x < 1.0 && spos.y < 1.0 && spos.z < 1.0){
						uint pointer = texelFetch(lpmap,ivec3(x*12,y,z),0).x;
						uint ty = uint((atan((spos.z-mtdt.z),length(spos.xy-mtdt.xy)-mtdt2.x)+PI)/PI_2*lmapsz);
						uint tx = uint((atan(spos.x-mtdt.x,spos.y-mtdt.y)+PI)/PI_2*lmapsz*2);
						uint txt = uint(tx / lmapsz);
						tx = uint(mod(tx,lmapsz));
						FragColor.rgb = texelFetch(lmap,ivec3(tx,ty,pointer+txt),0).rgb * block.gba * 32767.0 / (brightness+1.0);
						return;
					}
				}
				break;
			}
			case 14:{
				vec3 spos = getSubCoords();
				vec4 mtdt  = texelFetch(metadt,ivec3(x,y,z),0);
				vec4 mtdt2 = texelFetch(metadt2,ivec3(x,y,z),0);
				vec4 mtdt3 = texelFetch(metadt3,ivec3(x,y,z),0);
				float d = iCylinder(spos-mtdt.xyz,ang,mtdt3.xyz,mtdt2.x);
				if(d > 0.0){
					spos += ang * d;
					if(spos.x > 0.0 && spos.y > 0.0 && spos.z > 0.0 && spos.x < 1.0 && spos.y < 1.0 && spos.z < 1.0){
						vec3 w = normalize(mtdt3.xyz);
						vec3 u = normalize(cross(w,vec3(1.0,1.0,1.0)));
						vec3 v = normalize(cross(u,w));
						vec3 q = (spos+mtdt3.xyz-mtdt.xyz)*mat3(u,v,w);
						uint pointer = texelFetch(lpmap,ivec3(x*12,y,z),0).x;
						uint tx = uint((atan(q.y,q.x)+PI)/PI_2*lmapsz*2);
						uint txt = uint(tx/lmapsz);
						tx = uint(mod(tx,lmapsz));
						uint ty = uint(q.z*lmapsz);
						FragColor.rgb = texelFetch(lmap,ivec3(tx,ty,pointer+txt),0).rgb * block.gba * 32767.0 / (brightness+1.0);
						calcFog();
						return;
					}
				}
				break;
			}
			case 15:{
				vec3 spos  = getSubCoords();
				vec4 mtdt  = texelFetch(metadt,ivec3(x,y,z),0);
				vec4 mtdt2 = texelFetch(metadt2,ivec3(x,y,z),0);
				vec4 mtdt3 = texelFetch(metadt3,ivec3(x,y,z),0);
				float d = iBox(spos-mtdt.xyz,ang,mtdt2.xyz);
				if(d > 0.0){
					spos += ang * d;
					if(spos.x>=0.0&&spos.y>=0.0&&spos.z>=0.0&&spos.x<=1.0&&spos.y<=1.0&&spos.z<=1.0){
						if(spos.x>mtdt.x-mtdt2.x-0.0001&&spos.x<mtdt.x-mtdt2.x+0.0001){
							ang = refract(ang,vec3(-1.0,0.0,0.0),1.0/1.52);
						}
						else if(spos.y>mtdt.y-mtdt2.y-0.0001&&spos.y<mtdt.y-mtdt2.y+0.0001){
							ang = refract(ang,vec3(0.0,-1.0,0.0),1.0/1.52);
						}
						else if(spos.z>mtdt.z-mtdt2.z-0.0001&&spos.z<mtdt.z-mtdt2.z+0.0001){
							ang = refract(ang,vec3(0.0,0.0,-1.0),1.0/1.52);
						}
						else if(spos.x>mtdt.x+mtdt2.x-0.0001&&spos.x<mtdt.x+mtdt2.x+0.0001){
							ang = refract(ang,vec3(1.0,0.0,0.0),1.0/1.52);
						}
						else if(spos.y>mtdt.y+mtdt2.y-0.0001&&spos.y<mtdt.y+mtdt2.y+0.0001){
							ang = refract(ang,vec3(0.0,1.0,0.0),1.0/1.52);
						}
						else if(spos.z>mtdt.z+mtdt2.z-0.0001&&spos.z<mtdt.z+mtdt2.z+0.0001){
							ang = refract(ang,vec3(0.0,0.0,1.0),1.0/1.52);
						}
						float dstc = iBox2(spos-mtdt.xyz,ang,mtdt2.xyz);
						spos += ang * dstc;
						if(refractDynamicCube(spos,mtdt.xyz,mtdt2.xyz)==0.0){
							float dstc2 =  iBox2(spos-mtdt.xyz,ang,mtdt2.xyz);
							spos += ang * dstc2;	
							refractDynamicCube(spos,mtdt.xyz,mtdt2.xyz);
						}
						rayInitialize(spos);
						filtr -= (1.0-block.gba) ;
						break;
					}
				}
				break;
			}
			case 16:{
				vec3 spos  = getSubCoords();

                vec4 mtdt  = texelFetch(metadt,ivec3(x,y,z),0);
                vec4 mtdt2 = texelFetch(metadt2,ivec3(x,y,z),0);
                vec4 mtdt3 = texelFetch(metadt3,ivec3(x,y,z),0);
                vec4 mtdt4 = texelFetch(metadt4,ivec3(x,y,z),0);
                vec4 mtdt5 = texelFetch(metadt5,ivec3(x,y,z),0);
                vec4 mtdt6 = texelFetch(metadt6,ivec3(x,y,z),0);

                float d  = iBox(spos-mtdt.xyz,ang,mtdt2.xyz);
                float d2 = iBox(spos-mtdt4.xyz,ang,mtdt5.xyz);
                
                if(d < 0.0){
                    d = 999999.0;
                }
                if(d2 < 0.0){
                    d2 = 999999.0;
                }
                if(d < d2 && d != 999999.0){
					uint pointer = texelFetch(lpmap,ivec3(x*12,y,z),0).x;
					spos += ang * d;
					if(spos.x>mtdt.x-mtdt2.x-0.0001&&spos.x<mtdt.x-mtdt2.x+0.0001){
						float mt = max(mtdt2.y,mtdt2.z)*2.0;
						ivec2 tcrd = ivec2((spos.yz-mtdt.yz+mtdt2.yz)*lmapsz/mt);
						FragColor.rgb += block.gba * texelFetch(lmap,ivec3(tcrd,pointer),0).rgb * 32767.0 / (brightness+1.0);
					}
					if(spos.y>mtdt.y-mtdt2.y-0.0001&&spos.y<mtdt.y-mtdt2.y+0.0001){
						float mt = max(mtdt2.x,mtdt2.z)*2.0;
						ivec2 tcrd = ivec2((spos.xz-mtdt.xz+mtdt2.xz)*lmapsz/mt);
						FragColor.rgb += block.gba * texelFetch(lmap,ivec3(tcrd,pointer+1),0).rgb * 32767.0 / (brightness+1.0);
					}
					if(spos.z>mtdt.z-mtdt2.z-0.0001&&spos.z<mtdt.z-mtdt2.z+0.0001){
						float mt = max(mtdt2.x,mtdt2.y)*2.0;
						ivec2 tcrd = ivec2((spos.xy-mtdt.xy+mtdt2.xy)*lmapsz/mt);
						FragColor.rgb += block.gba * texelFetch(lmap,ivec3(tcrd,pointer+2),0).rgb * 32767.0 / (brightness+1.0);
					}
					if(spos.x>mtdt.x+mtdt2.x-0.0001&&spos.x<mtdt.x+mtdt2.x+0.0001){
						float mt = max(mtdt2.y,mtdt2.z)*2.0;
						ivec2 tcrd = ivec2((spos.yz-mtdt.yz+mtdt2.yz)*lmapsz/mt);
						FragColor.rgb += block.gba * texelFetch(lmap,ivec3(tcrd,pointer+3),0).rgb * 32767.0 / (brightness+1.0);
					}
					if(spos.y>mtdt.y+mtdt2.y-0.0001&&spos.y<mtdt.y+mtdt2.y+0.0001){
						float mt = max(mtdt2.x,mtdt2.z)*2.0;
						ivec2 tcrd = ivec2((spos.xz-mtdt.xz+mtdt2.xz)*lmapsz/mt);
						FragColor.rgb += block.gba * texelFetch(lmap,ivec3(tcrd,pointer+4),0).rgb * 32767.0 / (brightness+1.0);
					}
					if(spos.z>mtdt.z+mtdt2.z-0.0001&&spos.z<mtdt.z+mtdt2.z+0.0001){
						float mt = max(mtdt2.x,mtdt2.y)*2.0;
						ivec2 tcrd = ivec2((spos.xy-mtdt.xy+mtdt2.xy)*lmapsz/mt);
						FragColor.rgb += block.gba * texelFetch(lmap,ivec3(tcrd,pointer+5),0).rgb * 32767.0 / (brightness+1.0);
					}
					calcFog();
					return;
				}
				else if(d2 != 999999.0){
					uint pointer = texelFetch(lpmap,ivec3(x*12,y,z),0).x;
					spos += ang * d2;
					if(spos.x>mtdt4.x-mtdt5.x-0.0001&&spos.x<mtdt4.x-mtdt5.x+0.0001){
						float mt = max(mtdt5.y,mtdt5.z)*2.0;
						ivec2 tcrd = ivec2((spos.yz-mtdt4.yz+mtdt5.yz)*lmapsz/mt);
						FragColor.rgb += block.gba * texelFetch(lmap,ivec3(tcrd,pointer+6),0).rgb * 32767.0 / (brightness+1.0);
					}
					if(spos.y>mtdt4.y-mtdt5.y-0.0001&&spos.y<mtdt4.y-mtdt5.y+0.0001){
						float mt = max(mtdt5.x,mtdt5.z)*2.0;
						ivec2 tcrd = ivec2((spos.xz-mtdt4.xz+mtdt5.xz)*lmapsz/mt);
						FragColor.rgb += block.gba * texelFetch(lmap,ivec3(tcrd,pointer+7),0).rgb * 32767.0 / (brightness+1.0);
					}
					if(spos.z>mtdt4.z-mtdt5.z-0.0001&&spos.z<mtdt4.z-mtdt5.z+0.0001){
						float mt = max(mtdt5.x,mtdt5.y)*2.0;
						ivec2 tcrd = ivec2((spos.xy-mtdt4.xy+mtdt5.xy)*lmapsz/mt);
						FragColor.rgb += block.gba * texelFetch(lmap,ivec3(tcrd,pointer+8),0).rgb * 32767.0 / (brightness+1.0);
					}
					if(spos.x>mtdt4.x+mtdt5.x-0.0001&&spos.x<mtdt4.x+mtdt5.x+0.0001){
						float mt = max(mtdt5.y,mtdt5.z)*2.0;
						ivec2 tcrd = ivec2((spos.yz-mtdt4.yz+mtdt5.yz)*lmapsz/mt);
						FragColor.rgb += block.gba * texelFetch(lmap,ivec3(tcrd,pointer+9),0).rgb * 32767.0 / (brightness+1.0);
					}
					if(spos.y>mtdt4.y+mtdt5.y-0.0001&&spos.y<mtdt4.y+mtdt5.y+0.0001){
						float mt = max(mtdt5.x,mtdt5.z)*2.0;
						ivec2 tcrd = ivec2((spos.xz-mtdt4.xz+mtdt5.xz)*lmapsz/mt);
						FragColor.rgb += block.gba * texelFetch(lmap,ivec3(tcrd,pointer+10),0).rgb * 32767.0 / (brightness+1.0);
					}
					if(spos.z>mtdt4.z+mtdt5.z-0.0001&&spos.z<mtdt4.z+mtdt5.z+0.0001){
						float mt = max(mtdt5.x,mtdt5.y)*2.0;
						ivec2 tcrd = ivec2((spos.xy-mtdt4.xy+mtdt5.xy)*lmapsz/mt);
						FragColor.rgb += block.gba * texelFetch(lmap,ivec3(tcrd,pointer+11),0).rgb * 32767.0 / (brightness+1.0);
					}
					calcFog();
					return;
				}
				break;
			}
			case 18:{
				uint pointer = texelFetch(lpmap,ivec3(x*12+bside,y,z),0).x;
				FragColor.rgb = texelFetch(lmap,ivec3(wall.x*lmapsz,wall.y*lmapsz,pointer),0).rgb * 32767.0 / (brightness+1.0);
				vec2 kon = vec2((gl_FragCoord.x/reso.x),gl_FragCoord.y/reso.y);
				vec4 gd = texture(reflectmap,kon);
				FragColor.rgb = mix(FragColor.rgb,gd.rgb,0.2);
				return;
			}
			case 27:{
				uint pointer = texelFetch(lpmap,ivec3(x*12+bside,y,z),0).x;
				vec4 mtdt  = texelFetch(metadt,ivec3(x,y,z),0);
				vec4 mtdt2 = texelFetch(metadt2,ivec3(x,y,z),0);
				if(fog.a == 0.0){
					vec3 spos = getSubCoords();
					fog.rgb = texelFetch(lmap,ivec3(wall.x*lmapsz,wall.y*lmapsz,pointer),0).rgb * 32767.0 / (brightness+1.0);
					switch(side){
					case 0:
						if(ang.x < 0.0){
							fog.a = pow(-ang.x,mtdt.x);
							x++;
						}
						else{
							fog.a = pow(ang.x,mtdt.x);
						}
						ang.x = -ang.x;
						ang.xy = rotVEC2(ang.xy,(tnoise2((spos.yz+vec2(y,z))*16.0*mtdt.z+float(tick)*mtdt2.x)-0.5)    *mtdt.y);
						ang.xz = rotVEC2(ang.xz,(tnoise2((spos.yz+vec2(y,z))*16.0*mtdt.z+0.1+float(tick)*mtdt2.y)-0.5)*mtdt.y);
						ang.yz = rotVEC2(ang.yz,(tnoise2((spos.yz+vec2(y,z))*16.0*mtdt.z+0.2+float(tick)*mtdt2.z)-0.5)*mtdt.y);
						break;
					case 1:
						if(ang.y < 0.0){
							fog.a = pow(-ang.y,mtdt.x);
							y++;
						}
						else{
							fog.a = pow(ang.y,mtdt.x);
						}
						ang.y = -ang.y;
						ang.xy = rotVEC2(ang.xy,(tnoise2((spos.xz+vec2(x,z))*16.0*mtdt.z+float(tick)*mtdt2.x)-0.5)*mtdt.y);
						ang.xz = rotVEC2(ang.xz,(tnoise2((spos.xz+vec2(x,z))*16.0*mtdt.z+0.1+float(tick)*mtdt2.y)-0.5)*mtdt.y);
						ang.yz = rotVEC2(ang.yz,(tnoise2((spos.xz+vec2(x,z))*16.0*mtdt.z+0.2+float(tick)*mtdt2.z)-0.5)*mtdt.y);
						break;
					case 2:
						if(ang.z < 0.0){
							fog.a = pow(-ang.z,mtdt.x);
							z++;
						}
						else{
							fog.a = pow(ang.z,mtdt.x);
						}
						ang.z = -ang.z;
						ang.xy = rotVEC2(ang.xy,(tnoise2((spos.xy+vec2(x,y))*16.0*mtdt.z+float(tick)*mtdt2.x)-0.5)*mtdt.y);
						ang.xz = rotVEC2(ang.xz,(tnoise2((spos.xy+vec2(x,y))*16.0*mtdt.z+0.1+float(tick)*mtdt2.y)-0.5)*mtdt.y);
						ang.yz = rotVEC2(ang.yz,(tnoise2((spos.xy+vec2(x,y))*16.0*mtdt.z+0.2+float(tick)*mtdt2.z)-0.5)*mtdt.y);
						break;
					}
					rayInitialize(spos);
				}
				else{
					FragColor.rgb = texelFetch(lmap,ivec3(wall.x*lmapsz,wall.y*lmapsz,pointer),0).rgb * 32767.0 / (brightness+1.0);
					ePos = getRayPos();
					calcFog();
					return;
				}
				break;
			}
			case 28:{
				uint pointer = texelFetch(lpmap,ivec3(x*12+bside,y,z),0).x;
				ivec3 crd = ivec3(wall.x*lmapsz,wall.y*lmapsz,pointer%tex3DszLimit);
				switch(pointer/tex3DszLimit){
				case 0:
					FragColor.rgb = texelFetch(lmap,crd,0).rgb * 32767.0 / (brightness+1.0);
					break;
				case 1:
					FragColor.rgb = texelFetch(lmap2,crd,0).rgb * 32767.0 / (brightness+1.0);
					break;
				case 2:
					FragColor.rgb = texelFetch(lmap3,crd,0).rga * 32767.0 / (brightness+1.0);
					break;
				case 3:
					FragColor.rgb = texelFetch(lmap4,crd,0).rgb * 32767.0 / (brightness+1.0);
					break;
				}
				ePos = getRayPos();
				calcFog();
				return;
			}
			default:
				break;
			}
		}
		rayItterate();
    }
}





