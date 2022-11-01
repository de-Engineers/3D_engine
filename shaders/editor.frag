#version 460 core

out vec4 FragColor;

uniform sampler3D map;
uniform usampler3D lpmap;
uniform sampler3D lmap;

uniform sampler2D skybox;

uniform sampler3D metadt;
uniform sampler3D metadt2;
uniform sampler3D metadt3;
uniform sampler3D metadt4;
uniform sampler3D metadt5;
uniform sampler3D metadt6;

uniform sampler3D chessModels;
uniform sampler3D models8;

uniform sampler2D epicTexture;
uniform sampler2D slope;
uniform sampler2D spikes;

uniform vec3 Pos;

uniform ivec2 rngUni;

uniform vec2 angle;	
uniform vec2 fov;
uniform vec4 dir;
uniform ivec2 reso;

uniform int tick;

uniform float brightness;

uniform int mapSz;

uniform int renderDistance;

uniform int state;

uniform float lmapsz;

int x,y,z;
vec3 pos;
vec3 ang;
vec3 tr;
float sideX,sideY,sideZ;
int stepX,stepY,stepZ;
float vx,vy,vz;
float deltaX,deltaY,deltaZ;
int side;
vec2 wall,awall;
int effect;
vec4 fog;

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

vec2 rotVEC2(vec2 p,float rot){
    vec2 r;
    r.x = cos(rot) * p.x - sin(rot) * p.y;
    r.y = sin(rot) * p.x + cos(rot) * p.y;
    return r;
}

void calcFog(){
	FragColor.rgb *= fog.rgb;
}

void rayItterate(){
	if(sideX < sideY){
        if(sideX < sideZ){
			x += stepX;
			sideX += deltaX;
            side = 0;
        }
        else{
			z += stepZ;
			sideZ += deltaZ;
            side = 2;
        }
    }
    else if(sideY < sideZ){
		y += stepY;
		sideY += deltaY;
        side = 1;
    }
    else{
		z += stepZ;
		sideZ += deltaZ;
        side = 2;
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
		sideX = fract(apos.x) * deltaX;
	}
	else{
		stepX = 1;
		sideX = (int(apos.x) + 1.0 - apos.x) * deltaX;
	}
	if(ang.y < 0){
		stepY = -1;	
		sideY = fract(apos.y) * deltaY;
	}
	else{
		stepY = 1;
		sideY = (int(apos.y) + 1.0 - apos.y) * deltaY;
	}
	if(ang.z < 0){
		stepZ = -1;
		sideZ = fract(apos.z) * deltaZ;
	}
	else{
		stepZ = 1;
		sideZ = (int(apos.z) + 1.0 - apos.z) * deltaZ;
	}
}

void setWall(){
	switch(side){
	case 0:
		awall.x = pos.y + (sideX - deltaX) * ang.y;
		awall.y = pos.z + (sideX - deltaX) * ang.z;
		break;
	case 1:
		awall.x = pos.x + (sideY - deltaY) * ang.x;
		awall.y = pos.z + (sideY - deltaY) * ang.z;
		break;
	case 2:
		awall.x = pos.x + (sideZ - deltaZ) * ang.x;
		awall.y = pos.y + (sideZ - deltaZ) * ang.y;
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
	fog.rgb = vec3(1.0);
	float pixelOffsetY = fov.y * ((gl_FragCoord.y * 2.0 / reso.y) - 1);
	float pixelOffsetX = fov.x * ((gl_FragCoord.x * 2.0 / reso.x) - 1);

    ang.x = (dir.x * dir.w - dir.x * dir.z * pixelOffsetY) - dir.y * pixelOffsetX;
    ang.y = (dir.y * dir.w - dir.y * dir.z * pixelOffsetY) + dir.x * pixelOffsetX;
    ang.z = dir.z + dir.w * pixelOffsetY;
	ang = normalize(ang);

	FragColor.a = 1.0;
	pos = Pos;
	float reflectP = 1.0;
	
	deltaX = abs(1 / ang.x);
	deltaY = abs(1 / ang.y);
	deltaZ = abs(1 / ang.z);

	if(ang.x < 0){
		stepX = -1;
		sideX = fract(pos.x) * deltaX;
	}
	else{
		stepX = 1;
		sideX = (int(pos.x) + 1.0 - pos.x) * deltaX;
	}
	if(ang.y < 0){
		stepY = -1;
		sideY = fract(pos.y) * deltaY;
	}
	else{
		stepY = 1;
		sideY = (int(pos.y) + 1.0 - pos.y) * deltaY;
	}
	if(ang.z < 0){
		stepZ = -1;
		sideZ = fract(pos.z) * deltaZ;
	}
	else{
		stepZ = 1;
		sideZ = (int(pos.z) + 1.0 - pos.z) * deltaZ;
	}
	x = int(pos.x);
	y = int(pos.y);
	z = int(pos.z);
	rayItterate();
    for(;;){
        vec4 block = texelFetch(map,ivec3(x,y,z),0);
        switch(int(block[0]*255)){
		case 0:
            FragColor.rgb = texelFetch(skybox,ivec2((atan(ang.x,ang.y)+PI)/PI_2*1023.0,(ang.z+1.0)*511.0),0).rgb;
            return;
        case 1:
            break;
		default:
			int bside;
			switch(side){
			case 0:
				if(ang.x > 0.0){
					bside = 0;
				}
				else{
					bside = 1;
				}
				awall.x = pos.y + (sideX - deltaX) * ang.y;
				awall.y = pos.z + (sideX - deltaX) * ang.z;
				break;
			case 1:
				awall.x = pos.x + (sideY - deltaY) * ang.x;
				awall.y = pos.z + (sideY - deltaY) * ang.z;
				if(ang.y > 0.0){
					bside = 2;
				}
				else{
					bside = 3;
				}
				break;
			case 2:
				awall.x = pos.x + (sideZ - deltaZ) * ang.x;
				awall.y = pos.y + (sideZ - deltaZ) * ang.y;
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
            if(wall.x < 0.02 || wall.y < 0.02 || wall.x > 0.98 || wall.y > 0.98){
				vec2 dtc = (abs(0.5-wall)-0.48) * 50.0;
				FragColor.rgb = vec3(random(block[0]),random(block[0]+0.1),random(block[0]+0.2)) * max(dtc.x,dtc.y);
				return;
			}
            switch(int(block[0])){
            case 9:{
                vec3 spos = getSubCoords();
                vec4 mtdt = texelFetch(metadt,ivec3(x,y,z),0);
                float d = iSphere(spos-mtdt.xyz,ang,mtdt.a);
                if(d > 0.0){
                    spos += ang * d;
                    if(spos.x > 0.0 && spos.y > 0.0 && spos.z > 0.0 && spos.x < 1.0 && spos.y < 1.0 && spos.z < 1.0){
                        FragColor.rgb = block.gba;
                        return;
                    }
                }
                calcFog();
                break;
            }
            case 10:{
                vec3 spos = getSubCoords();
                vec4 mtdt = texelFetch(metadt,ivec3(x,y,z),0);
                float d = iSphere(spos-mtdt.xyz,ang,mtdt.a);
                if(d > 0.0){
                    spos += ang * d;
                    if(spos.x > 0.0 && spos.y > 0.0 && spos.z > 0.0 && spos.x < 1.0 && spos.y < 1.0 && spos.z < 1.0){
                        ang = refract(ang,normalize(spos-mtdt.xyz),0.6591349);
                        rayInitialize(spos);
                        spos += ang * iSphere2(spos-mtdt.xyz,ang,mtdt.a);
                        ang = refract(ang,normalize(spos-mtdt.xyz),1.52);
                        rayInitialize(spos);
                        fog.rgb = block.gba;
                        fog.a = 0.5;
                    }
                }
                calcFog();
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
                        rayInitialize(spos);
                    }
                }
                calcFog();
                break;
            }
            case 12:{
                vec3 spos  = getSubCoords();
                vec4 mtdt  = texelFetch(metadt,ivec3(x,y,z),0);
                vec4 mtdt2 = texelFetch(metadt2,ivec3(x,y,z),0);
                vec4 mtdt3 = texelFetch(metadt3,ivec3(x,y,z),0);
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
                        if(spos.x>mtdt.x-mtdt2.x-0.0001&&spos.x<mtdt.x-mtdt2.x+0.0001){
                            float mt = max(mtdt2.y,mtdt2.z)*8.0;
                            if(fract(floor(spos.y*mt)/2.0+floor(spos.z*mt)/2.0) < 0.5){
                                FragColor.rgb = block.gba * 0.8;
                            }
                            else{
                                FragColor.rgb = block.gba * 1.2;
                            }
                        }
                        if(spos.y>mtdt.y-mtdt2.y-0.0001&&spos.y<mtdt.y-mtdt2.y+0.0001){
                            float mt = max(mtdt2.x,mtdt2.z)*8.0;
                            if(fract(floor(spos.x*mt)/2.0+floor(spos.z*mt)/2.0) < 0.5){
                                FragColor.rgb = block.gba * 0.8;
                            }
                            else{
                                FragColor.rgb = block.gba * 1.2;
                            }
                        }
                        if(spos.z>mtdt.z-mtdt2.z-0.0001&&spos.z<mtdt.z-mtdt2.z+0.0001){
                            float mt = max(mtdt2.x,mtdt2.y)*8.0;
                            if(fract(floor(spos.x*mt)/2.0+floor(spos.y*mt)/2.0) < 0.5){
                                FragColor.rgb = block.gba * 0.8;
                            }
                            else{
                                FragColor.rgb = block.gba * 1.2;
                            }
                        }
                        if(spos.x>mtdt.x+mtdt2.x-0.0001&&spos.x<mtdt.x+mtdt2.x+0.0001){
                            float mt = max(mtdt2.y,mtdt2.z)*8.0;
                            if(fract(floor(spos.y*mt)/2.0+floor(spos.z*mt)/2.0) < 0.5){
                                FragColor.rgb = block.gba * 0.8;
                            }
                            else{
                                FragColor.rgb = block.gba * 1.2;
                            }
                        }
                        if(spos.y>mtdt.y+mtdt2.y-0.0001&&spos.y<mtdt.y+mtdt2.y+0.0001){
                            float mt = max(mtdt2.x,mtdt2.z)*8.0;
                            if(fract(floor(spos.x*mt)/2.0+floor(spos.z*mt)/2.0) < 0.5){
                                FragColor.rgb = block.gba * 0.8;
                            }
                            else{
                                FragColor.rgb = block.gba * 1.2;
                            }
                        }
                        if(spos.z>mtdt.z+mtdt2.z-0.0001&&spos.z<mtdt.z+mtdt2.z+0.0001){
                            float mt = max(mtdt2.x,mtdt2.y)*8.0;
                            if(fract(floor(spos.x*mt)/2.0+floor(spos.y*mt)/2.0) < 0.5){
                                FragColor.rgb = block.gba * 0.8;
                            }
                            else{
                                FragColor.rgb = block.gba * 1.2;
                            }
                        }
                        calcFog();
                        return;
                    }
                }
                break;
            }
            case 13:{
                vec3 spos  = getSubCoords();
                vec4 mtdt  = texelFetch(metadt,ivec3(x,y,z),0);
                vec4 mtdt2 = texelFetch(metadt2,ivec3(x,y,z),0);
                float d = iTorus(spos-mtdt.xyz,ang,mtdt2.xy);
                if(d > 0.0){
                    spos += ang * d;
                    if(spos.x > 0.0 && spos.y > 0.0 && spos.z > 0.0 && spos.x < 1.0 && spos.y < 1.0 && spos.z < 1.0){
                        if(fract(floor(mod((atan(spos.x-mtdt.x,spos.y-mtdt.y)+PI)/PI_2*8.0,2.0))/2.0+floor(mod((atan(length(spos.xy-mtdt.xy)*mtdt2.y,(spos.z-mtdt.z)*mtdt2.y)+PI)/PI_2*16.0,2.0))/2.0)<0.5){
                            FragColor.rgb = block.gba * 1.2;
                        }
                        else{
                            FragColor.rgb = block.gba * 0.8;
                        }
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
                        if(fract((atan(q.y,q.x)+PI)/PI_2*8.0)<0.5){
                            FragColor.rgb = block.gba * 1.2;
                        }
                        else{
                            FragColor.rgb = block.gba * 0.8;
                        }
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
                        spos += ang * iBox2(spos-mtdt.xyz,ang,mtdt2.xyz);
                        if(refractDynamicCube(spos,mtdt.xyz,mtdt2.xyz)==0.0){
                            spos += ang * iBox2(spos-mtdt.xyz,ang,mtdt2.xyz);
                            refractDynamicCube(spos,mtdt.xyz,mtdt2.xyz);
                        }
                        rayInitialize(spos);
                        fog.rgb = block.gba;
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
                    spos += ang * d;
                    if(spos.x>mtdt.x-mtdt2.x-0.0001&&spos.x<mtdt.x-mtdt2.x+0.0001){
                        float mt = max(mtdt2.y,mtdt2.z)*8.0;
                        if(fract(floor(spos.y*mt)/2.0+floor(spos.z*mt)/2.0) < 0.5){
                            FragColor.rgb = block.gba * 0.8;
                        }
                        else{
                            FragColor.rgb = block.gba * 1.2;
                        }
                    }
                    if(spos.y>mtdt.y-mtdt2.y-0.0001&&spos.y<mtdt.y-mtdt2.y+0.0001){
                        float mt = max(mtdt2.x,mtdt2.z)*8.0;
                        if(fract(floor(spos.x*mt)/2.0+floor(spos.z*mt)/2.0) < 0.5){
                            FragColor.rgb = block.gba * 0.8;
                        }
                        else{
                            FragColor.rgb = block.gba * 1.2;
                        }
                    }
                    if(spos.z>mtdt.z-mtdt2.z-0.0001&&spos.z<mtdt.z-mtdt2.z+0.0001){
                        float mt = max(mtdt2.x,mtdt2.y)*8.0;
                        if(fract(floor(spos.x*mt)/2.0+floor(spos.y*mt)/2.0) < 0.5){
                            FragColor.rgb = block.gba * 0.8;
                        }
                        else{
                            FragColor.rgb = block.gba * 1.2;
                        }
                    }
                    if(spos.x>mtdt.x+mtdt2.x-0.0001&&spos.x<mtdt.x+mtdt2.x+0.0001){
                        float mt = max(mtdt2.y,mtdt2.z)*8.0;
                        if(fract(floor(spos.y*mt)/2.0+floor(spos.z*mt)/2.0) < 0.5){
                            FragColor.rgb = block.gba * 0.8;
                        }
                        else{
                            FragColor.rgb = block.gba * 1.2;
                        }
                    }
                    if(spos.y>mtdt.y+mtdt2.y-0.0001&&spos.y<mtdt.y+mtdt2.y+0.0001){
                        float mt = max(mtdt2.x,mtdt2.z)*8.0;
                        if(fract(floor(spos.x*mt)/2.0+floor(spos.z*mt)/2.0) < 0.5){
                            FragColor.rgb = block.gba * 0.8;
                        }
                        else{
                            FragColor.rgb = block.gba * 1.2;
                        }
                    }
                    if(spos.z>mtdt.z+mtdt2.z-0.0001&&spos.z<mtdt.z+mtdt2.z+0.0001){
                        float mt = max(mtdt2.x,mtdt2.y)*8.0;
                        if(fract(floor(spos.x*mt)/2.0+floor(spos.y*mt)/2.0) < 0.5){
                            FragColor.rgb = block.gba * 0.8;
                        }
                        else{
                            FragColor.rgb = block.gba * 1.2;
                        }
                    }
                    calcFog();
                    return;
                }
                else if(d2 != 999999.0){
                    spos += ang * d2;
                    if(spos.x>mtdt4.x-mtdt5.x-0.0001&&spos.x<mtdt4.x-mtdt5.x+0.0001){
                        float mt = max(mtdt5.y,mtdt5.z)*8.0;
                        if(fract(floor(spos.y*mt)/2.0+floor(spos.z*mt)/2.0) < 0.5){
                            FragColor.rgb = block.gba * 0.8;
                        }
                        else{
                            FragColor.rgb = block.gba * 1.2;
                        }
                    }
                    if(spos.y>mtdt4.y-mtdt5.y-0.0001&&spos.y<mtdt4.y-mtdt5.y+0.0001){
                        float mt = max(mtdt5.x,mtdt5.z)*8.0;
                        if(fract(floor(spos.x*mt)/2.0+floor(spos.z*mt)/2.0) < 0.5){
                            FragColor.rgb = block.gba * 0.8;
                        }
                        else{
                            FragColor.rgb = block.gba * 1.2;
                        }
                    }
                    if(spos.z>mtdt4.z-mtdt5.z-0.0001&&spos.z<mtdt4.z-mtdt5.z+0.0001){
                        float mt = max(mtdt5.x,mtdt5.y)*8.0;
                        if(fract(floor(spos.x*mt)/2.0+floor(spos.y*mt)/2.0) < 0.5){
                            FragColor.rgb = block.gba * 0.8;
                        }
                        else{
                            FragColor.rgb = block.gba * 1.2;
                        }
                    }
                    if(spos.x>mtdt4.x+mtdt5.x-0.0001&&spos.x<mtdt4.x+mtdt5.x+0.0001){
                        float mt = max(mtdt5.y,mtdt5.z)*8.0;
                        if(fract(floor(spos.y*mt)/2.0+floor(spos.z*mt)/2.0) < 0.5){
                            FragColor.rgb = block.gba * 0.8;
                        }
                        else{
                            FragColor.rgb = block.gba * 1.2;
                        }
                    }
                    if(spos.y>mtdt4.y+mtdt5.y-0.0001&&spos.y<mtdt4.y+mtdt5.y+0.0001){
                        float mt = max(mtdt5.x,mtdt5.z)*8.0;
                        if(fract(floor(spos.x*mt)/2.0+floor(spos.z*mt)/2.0) < 0.5){
                            FragColor.rgb = block.gba * 0.8;
                        }
                        else{
                            FragColor.rgb = block.gba * 1.2;
                        }
                    }
                    if(spos.z>mtdt4.z+mtdt5.z-0.0001&&spos.z<mtdt4.z+mtdt5.z+0.0001){
                        float mt = max(mtdt5.x,mtdt5.y)*8.0;
                        if(fract(floor(spos.x*mt)/2.0+floor(spos.y*mt)/2.0) < 0.5){
                            FragColor.rgb = block.gba * 0.8;
                        }
                        else{
                            FragColor.rgb = block.gba * 1.2;
                        }
                    }
                    calcFog();
                    return;
                }
                break;
            }
            case 29:{
                switch(side){
                case 0:
                    ang.x = -ang.x;
                    break;
                case 1:
                    ang.y = -ang.y;
                    break;
                case 2:
                    ang.z = -ang.z;
                    break;
                }
                pos = getRayPos();
                if(ang.x < 0){
                    stepX = -1;
                    sideX = fract(pos.x) * deltaX;
                }
                else{
                    stepX = 1;
                    sideX = (int(pos.x) + 1.0 - pos.x) * deltaX;
                }
                if(ang.y < 0){
                    stepY = -1;	
                    sideY = fract(pos.y) * deltaY;
                }
                else{
                    stepY = 1;
                    sideY = (int(pos.y) + 1.0 - pos.y) * deltaY;
                }
                if(ang.z < 0){
                    stepZ = -1;
                    sideZ = fract(pos.z) * deltaZ;
                }
                else{
                    stepZ = 1;
                    sideZ = (int(pos.z) + 1.0 - pos.z) * deltaZ;
                }
                calcFog();
                break;
            }
            case 32:{
                vec2 spos = abs(0.5-fract(wall*4.0));
                FragColor.rgb = vec3(distance(spos.x,spos.y));
                return;
            }
            default:
                if(fract(floor(wall.x*4.0)/2.0+floor(wall.y*4.0)/2.0) < 0.5){
                    FragColor.rgb = block.gba * 1.2;
                }
                else{
                    FragColor.rgb = block.gba * 0.8;
                }
                calcFog();
                return;
			}
		}
        rayItterate();
    }
}
