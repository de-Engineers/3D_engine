#version 460 core

out vec4 FragColor;

in vec4 gl_FragCoord;

uniform mat3 cameraMatrix;

uniform sampler3D map;
uniform usampler3D lpmap;
uniform sampler3D lmap;
uniform sampler3D metadt;
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

float x,y,z;
vec3 pos;
vec3 ang;
vec3 tr;
float sideX,sideY,sideZ;
float stepX,stepY,stepZ;
float vx,vy,vz;
float deltaX,deltaY,deltaZ;
int side;
vec2 wall,awall;
int effect;
vec3 fog;

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

float tnoise(vec2 pos,float itt){
	pos.x += float(rngUni);
	pos.y += 300.0;
    float intens;
    for(float i = 1.0;i < itt;i*=1.34){
        float center = 1.0 - max(abs(fract(pos.x * i) - 0.5),abs(fract(pos.y * i) - 0.5)) * 2.0;
        intens += mix(0.5,random(floor(pos * i)),center) - 0.5;
        intens *= 1.1;
    }
    return intens;
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

vec3 getSide(vec3 tr){
	vec3 si;
	if(ang.x < 0){
		si.x = fract(tr.x) * deltaX;
	}
	else{
		si.x = (int(tr.x) + 1 - tr.x) * deltaX;
	}
	if(ang.y < 0){
		si.y = fract(tr.y) * deltaY;
	}
	else{
		si.y = (int(tr.y) + 1 - tr.y) * deltaY;
	}
	if(ang.z < 0){
		si.z = fract(tr.z) * deltaZ;
	}
	else{
		si.z = (int(tr.z) + 1 - tr.z) * deltaZ;
	}
	return si;
}

void reinitializeRay(vec3 apos){
	deltaX = abs(1.0 / ang.x);
	deltaY = abs(1.0 / ang.y);
	deltaZ = abs(1.0 / ang.z);
	x = floor(apos.x);
	y = floor(apos.y);
	z = floor(apos.z);
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
    
    if( abs(k3*(k3*k3-k2)+k1) < 0.01 )
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
    if(tN<tF){
        return tN;
    }
    else{
        return tF;
    }
}

void main(){
	float pixelOffsetY = fov.y * ((gl_FragCoord.y * 2.0 / reso.y) - 1);
	float pixelOffsetX = fov.x * ((gl_FragCoord.x * 2.0 / reso.x) - 1);

    ang.x = (dir.x * dir.w - dir.x * dir.z * pixelOffsetY) - dir.y * pixelOffsetX;
    ang.y = (dir.y * dir.w - dir.y * dir.z * pixelOffsetY) + dir.x * pixelOffsetX;
    ang.z = dir.z + dir.w * pixelOffsetY;
	ang = normalize(ang);

	FragColor.a = 1.0;
	vec3 pos = Pos;
	float reflectP = 1.0;
	fog = vec3(1.0);
	
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

    for(int i = 0;i < 64;i++){
        vec4 block = texelFetch(map,ivec3(x,y,z),0);
        if(block[0] > 0.0){
			int bside;
			switch(side){
			case 0:
				awall.x = pos.y + (sideX - deltaX) * ang.y;
				awall.y = pos.z + (sideX - deltaX) * ang.z;
				if(ang.x > 0.0){
					bside = 0;
				}
				else{
					bside = 1;
				}
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
			if(state / 4 % 2 == 1){
				if(wall.x < 0.02 || wall.y < 0.02 || wall.x > 0.98 || wall.y > 0.98){
					FragColor.rgb = vec3(random(block[0]),random(block[0]+0.1),random(block[0]+0.2));
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
							vec3 apos;
							apos.x = x + spos.x;
							apos.y = y + spos.y;
							apos.z = z + spos.z;
							deltaX = abs(1.0 / ang.x);
							deltaY = abs(1.0 / ang.y);
							deltaZ = abs(1.0 / ang.z);
							x = floor(apos.x);
							y = floor(apos.y);
							z = floor(apos.z);
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
							spos += ang * sphDensity(spos,ang,mtdt.a);
							ang = refract(ang,normalize(spos-mtdt.xyz),0.6591349);
							apos.x = x + spos.x;
							apos.y = y + spos.y;
							apos.z = z + spos.z;
							deltaX = abs(1.0 / ang.x);
							deltaY = abs(1.0 / ang.y);
							deltaZ = abs(1.0 / ang.z);
							x = floor(apos.x);
							y = floor(apos.y);
							z = floor(apos.z);
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
							fog = block.gba;
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
							fog = block.gba;
							vec3 apos;
							apos.x = x + spos.x;
							apos.y = y + spos.y;
							apos.z = z + spos.z;
							deltaX = abs(1.0 / ang.x);
							deltaY = abs(1.0 / ang.y);
							deltaZ = abs(1.0 / ang.z);
							x = floor(apos.x);
							y = floor(apos.y);
							z = floor(apos.z);
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
					}
					break;
				}
				case 12:{
					vec3 spos = getSubCoords();
					vec4 mtdt = texelFetch(metadt,ivec3(x,y,z),0);
					float d = iBox(spos-mtdt.xyz,ang,vec3(mtdt.a));
					if(d > 0.0){
						spos += ang * d;
						if(spos.x>mtdt.x-mtdt.a-0.0001&&spos.x<mtdt.x-mtdt.a+0.0001){
							if(fract(floor(spos.y/mtdt.a*4.0)/2.0+floor(spos.z/mtdt.a*4.0)/2.0) < 0.5){
								FragColor.rgb = block.gba * 0.8;
							}
							else{
								FragColor.rgb = block.gba * 1.2;
							}
						}
						if(spos.y>mtdt.y-mtdt.a-0.0001&&spos.y<mtdt.y-mtdt.a+0.0001){
							if(fract(floor(spos.x/mtdt.a*4.0)/2.0+floor(spos.z/mtdt.a*4.0)/2.0) < 0.5){
								FragColor.rgb = block.gba * 0.8;
							}
							else{
								FragColor.rgb = block.gba * 1.2;
							}
						}
						if(spos.z>mtdt.z-mtdt.a-0.0001&&spos.z<mtdt.z-mtdt.a+0.0001){
							if(fract(floor(spos.x/mtdt.a*4.0)/2.0+floor(spos.y/mtdt.a*4.0)/2.0) < 0.5){
								FragColor.rgb = block.gba * 0.8;
							}
							else{
								FragColor.rgb = block.gba * 1.2;
							}
						}
						if(spos.x>mtdt.x+mtdt.a-0.0001&&spos.x<mtdt.x+mtdt.a+0.0001){
							if(fract(floor(spos.y/mtdt.a*4.0)/2.0+floor(spos.z/mtdt.a*4.0)/2.0) < 0.5){
								FragColor.rgb = block.gba * 0.8;
							}
							else{
								FragColor.rgb = block.gba * 1.2;
							}
						}
						if(spos.y>mtdt.y+mtdt.a-0.0001&&spos.y<mtdt.y+mtdt.a+0.0001){
							if(fract(floor(spos.x/mtdt.a*4.0)/2.0+floor(spos.z/mtdt.a*4.0)/2.0) < 0.5){
								FragColor.rgb = block.gba * 0.8;
							}
							else{
								FragColor.rgb = block.gba * 1.2;
							}
						}
						if(spos.z>mtdt.z+mtdt.a-0.0001&&spos.z<mtdt.z+mtdt.a+0.0001){
							if(fract(floor(spos.x/mtdt.a*4.0)/2.0+floor(spos.y/mtdt.a*4.0)/2.0) < 0.5){
								FragColor.rgb = block.gba * 0.8;
							}
							else{
								FragColor.rgb = block.gba * 1.2;
							}
						}
						return;
					}
					break;
				}
				default:
					FragColor.rgb = block.gba;
					FragColor.rgb /= fog;
					return;
				}
			}
			else{
				switch(int(block[0])){
				case 1:{
					uint pointer = texelFetch(lpmap,ivec3(x*6+bside,y,z),0).x;
					FragColor.rgb = normalize(block.gba);
					return;
				}
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
							ang = refract(ang,normalize(spos-mtdt.xyz),0.6591349);
							vec3 apos;
							apos.x = x + spos.x;
							apos.y = y + spos.y;
							apos.z = z + spos.z;
							deltaX = abs(1.0 / ang.x);
							deltaY = abs(1.0 / ang.y);
							deltaZ = abs(1.0 / ang.z);
							x = floor(apos.x);
							y = floor(apos.y);
							z = floor(apos.z);
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
							spos += ang * sphDensity(spos,ang,mtdt.a);
							ang = refract(ang,normalize(spos-mtdt.xyz),0.6591349);
							apos;
							apos.x = x + spos.x;
							apos.y = y + spos.y;
							apos.z = z + spos.z;
							deltaX = abs(1.0 / ang.x);
							deltaY = abs(1.0 / ang.y);
							deltaZ = abs(1.0 / ang.z);
							x = floor(apos.x);
							y = floor(apos.y);
							z = floor(apos.z);
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
							fog = block.gba;
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
							fog = block.gba;
							vec3 apos;
							apos.x = x + spos.x;
							apos.y = y + spos.y;
							apos.z = z + spos.z;
							deltaX = abs(1.0 / ang.x);
							deltaY = abs(1.0 / ang.y);
							deltaZ = abs(1.0 / ang.z);
							x = floor(apos.x);
							y = floor(apos.y);
							z = floor(apos.z);
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
					}
					break;
				}
				case 12:{
					vec3 spos = getSubCoords();
					vec4 mtdt = texelFetch(metadt,ivec3(x,y,z),0);
					float d = iBox(spos-mtdt.xyz,ang,vec3(mtdt.a));
					if(d > 0.0){
						uint pointer = texelFetch(lpmap,ivec3(x*6,y,z),0).x;
						spos += ang * d;
						if(spos.x>mtdt.x-mtdt.a-0.0001&&spos.x<mtdt.x-mtdt.a+0.0001){
							ivec2 tcrd = ivec2(((spos.yz-mtdt.yz)/mtdt.a+1.0)*0.5*lmapsz);
							FragColor.rgb += block.gba * texelFetch(lmap,ivec3(tcrd,pointer),0).rgb * 32767.0 / (brightness+1.0);
						}
						if(spos.y>mtdt.y-mtdt.a-0.0001&&spos.y<mtdt.y-mtdt.a+0.0001){
							ivec2 tcrd = ivec2(((spos.xz-mtdt.xz)/mtdt.a+1.0)*0.5*lmapsz);
							FragColor.rgb += block.gba * texelFetch(lmap,ivec3(tcrd,pointer+1),0).rgb * 32767.0 / (brightness+1.0);
						}
						if(spos.z>mtdt.z-mtdt.a-0.0001&&spos.z<mtdt.z-mtdt.a+0.0001){
							ivec2 tcrd = ivec2(((spos.xy-mtdt.xy)/mtdt.a+1.0)*0.5*lmapsz);
							FragColor.rgb += block.gba * texelFetch(lmap,ivec3(tcrd,pointer+2),0).rgb * 32767.0 / (brightness+1.0);
						}
						if(spos.x>mtdt.x+mtdt.a-0.0001&&spos.x<mtdt.x+mtdt.a+0.0001){
							ivec2 tcrd = ivec2(((spos.yz-mtdt.yz)/mtdt.a+1.0)*0.5*lmapsz);
							FragColor.rgb += block.gba * texelFetch(lmap,ivec3(tcrd,pointer+3),0).rgb * 32767.0 / (brightness+1.0);
						}
						if(spos.y>mtdt.y+mtdt.a-0.0001&&spos.y<mtdt.y+mtdt.a+0.0001){
							ivec2 tcrd = ivec2(((spos.xz-mtdt.xz)/mtdt.a+1.0)*0.5*lmapsz);
							FragColor.rgb += block.gba * texelFetch(lmap,ivec3(tcrd,pointer+4),0).rgb * 32767.0 / (brightness+1.0);
						}
						if(spos.z>mtdt.z+mtdt.a-0.0001&&spos.z<mtdt.z+mtdt.a+0.0001){
							ivec2 tcrd = ivec2(((spos.xy-mtdt.xy)/mtdt.a+1.0)*0.5*lmapsz);
							FragColor.rgb += block.gba * texelFetch(lmap,ivec3(tcrd,pointer+5),0).rgb * 32767.0 / (brightness+1.0);
						}
						return;
					}
					break;
				}
				case 28:{
					uint pointer = texelFetch(lpmap,ivec3(x*6+bside,y,z),0).x;
					float cnt = 1.0;
					vec3 col;
					ivec3 tpos = ivec3(wall.x*lmapsz,wall.y*lmapsz,pointer);
					FragColor.rgb = block.gba * texelFetch(lmap,tpos,0).rgb * 65535.0 / float(brightness+1);
					if(tpos.x > 0){
						FragColor.rgb += block.gba * texelFetch(lmap,tpos+ivec3(-1,0,0),0).rgb * 32767.0 / (brightness+1.0);
						cnt += 0.5;
					}
					if(tpos.y > 0){
						FragColor.rgb += block.gba * texelFetch(lmap,tpos+ivec3(0,-1,0),0).rgb * 32767.0 / (brightness+1.0);
						cnt += 0.5;
					}
					if(tpos.x < lmapsz-1.0){
						FragColor.rgb += block.gba * texelFetch(lmap,tpos+ivec3(1,0,0),0).rgb * 32767.0 / (brightness+1.0);
						cnt += 0.5;
					}
					if(tpos.y < lmapsz-1.0){
						FragColor.rgb += block.gba * texelFetch(lmap,tpos+ivec3(0,1,0),0).rgb * 32767.0 / (brightness+1.0);
						cnt += 0.5;
					}
					FragColor.rgb /= cnt;
					FragColor.rgb *= fog;
					return;
				}
				default:
					break;
				}
			}
		}
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
}





