#version 460 core

out vec4 FragColor;

in vec4 gl_FragCoord;

uniform mat3 cameraMatrix;

uniform sampler3D map;
uniform sampler3D mapdata;
uniform sampler3D chessModels;
uniform sampler3D models8;

uniform sampler2D epicTexture;
uniform sampler2D slope;
uniform sampler2D spikes;

uniform vec3 Pos;

uniform ivec2 rngUni;

uniform vec3 playerDir;
uniform vec3 playerDirRight;
uniform vec3 playerDirUp;

uniform vec2 angle;	
uniform vec2 fov;
uniform vec4 dir;
uniform ivec2 reso;

uniform int tick;

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
float wallXa,wallYa,wallX,wallY;
vec4 fog;
int effect;

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
			return vec3(0.0,wallX,wallY);
		}
		else{
			return vec3(1.0,wallX,wallY);
		}
	case 1:
		if(ang.y > 0.0){
			return vec3(wallX,0.0,wallY);
		}
		else{
			return vec3(wallX,1.0,wallY);
		}
	case 2:
		if(ang.z > 0.0){
			return vec3(wallX,wallY,0.0);
		}
		else{
			return vec3(wallX,wallY,1.0);
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

void reinitializeRay(){
	deltaX = abs(1 / ang.x);
	deltaY = abs(1 / ang.y);
	deltaZ = abs(1 / ang.z);
	if(ang.x < 0){
		stepX = -1;
		sideX = fract(x) * deltaX;
	}
	else{
		stepX = 1;
		sideX = (int(x) + 1.0 - x) * deltaX;
	}
	if(ang.y < 0){
		stepY = -1;	
		sideY = fract(y) * deltaY;
	}
	else{
		stepY = 1;
		sideY = (int(y) + 1.0 - y) * deltaY;
	}
	if(ang.z < 0){
		stepZ = -1;
		sideZ = fract(z) * deltaZ;
	}
	else{
		stepZ = 1;
		sideZ = (int(z) + 1.0 - z) * deltaZ;
	}
	x = int(x);
	y = int(y);
	z = int(z);
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

float revertnoise(vec2 rp){
    float c1 = 0.5;
    float c2 = 0.5;
    float c3 = 0.5;
    float c4 = 0.5;
    c1 *= clamp(1.0-distance(vec2(1.0,1.0),rp),0.0,1.0);
    c2 *= clamp(1.0-distance(vec2(1.0,0.0),rp),0.0,1.0);
    c3 *= clamp(1.0-distance(vec2(0.0,1.0),rp),0.0,1.0);
    c4 *= clamp(1.0-distance(vec2(0.0,0.0),rp),0.0,1.0);
    return (c1+c2+c3+c4+1.0)/4.0;
}

vec3 blockLight(){
	vec4 t,e1,e2,e3,e4,c1,c2,c3,c4;
	if(side == 0){
		if(ang.x<0.0){
			t  = texelFetch(mapdata,ivec3(vec3(x,wallXa,wallYa)*lmapsz)+ivec3( lmapsz, 0, 0),0);
		  	e1 = texelFetch(mapdata,ivec3(vec3(x,wallXa,wallYa)*lmapsz)+ivec3( lmapsz, 1, 0),0);
			e2 = texelFetch(mapdata,ivec3(vec3(x,wallXa,wallYa)*lmapsz)+ivec3( lmapsz,-1, 0),0);
			e3 = texelFetch(mapdata,ivec3(vec3(x,wallXa,wallYa)*lmapsz)+ivec3( lmapsz, 0, 1),0);
			e4 = texelFetch(mapdata,ivec3(vec3(x,wallXa,wallYa)*lmapsz)+ivec3( lmapsz, 0,-1),0);
			c1 = texelFetch(mapdata,ivec3(vec3(x,wallXa,wallYa)*lmapsz)+ivec3( lmapsz, 1, 1),0);
			c2 = texelFetch(mapdata,ivec3(vec3(x,wallXa,wallYa)*lmapsz)+ivec3( lmapsz, 1,-1),0);
			c3 = texelFetch(mapdata,ivec3(vec3(x,wallXa,wallYa)*lmapsz)+ivec3( lmapsz,-1, 1),0);
			c4 = texelFetch(mapdata,ivec3(vec3(x,wallXa,wallYa)*lmapsz)+ivec3( lmapsz,-1,-1),0);
		}
		else{
			t  = texelFetch(mapdata,ivec3(vec3(x,wallXa,wallYa)*lmapsz)+ivec3(-1, 0, 0),0);
		  	e1 = texelFetch(mapdata,ivec3(vec3(x,wallXa,wallYa)*lmapsz)+ivec3(-1, 1, 0),0);
			e2 = texelFetch(mapdata,ivec3(vec3(x,wallXa,wallYa)*lmapsz)+ivec3(-1,-1, 0),0);
			e3 = texelFetch(mapdata,ivec3(vec3(x,wallXa,wallYa)*lmapsz)+ivec3(-1, 0, 1),0);
			e4 = texelFetch(mapdata,ivec3(vec3(x,wallXa,wallYa)*lmapsz)+ivec3(-1, 0,-1),0);
			c1 = texelFetch(mapdata,ivec3(vec3(x,wallXa,wallYa)*lmapsz)+ivec3(-1, 1, 1),0);
			c2 = texelFetch(mapdata,ivec3(vec3(x,wallXa,wallYa)*lmapsz)+ivec3(-1, 1,-1),0);
			c3 = texelFetch(mapdata,ivec3(vec3(x,wallXa,wallYa)*lmapsz)+ivec3(-1,-1, 1),0);
			c4 = texelFetch(mapdata,ivec3(vec3(x,wallXa,wallYa)*lmapsz)+ivec3(-1,-1,-1),0);
		}
	}
	else if(side == 1){
		if(ang.y<0.0){
			t  = texelFetch(mapdata,ivec3(vec3(wallXa,y,wallYa)*lmapsz)+ivec3( 0, lmapsz, 0),0);
		  	e1 = texelFetch(mapdata,ivec3(vec3(wallXa,y,wallYa)*lmapsz)+ivec3( 1, lmapsz, 0),0);
			e2 = texelFetch(mapdata,ivec3(vec3(wallXa,y,wallYa)*lmapsz)+ivec3(-1, lmapsz, 0),0);
			e3 = texelFetch(mapdata,ivec3(vec3(wallXa,y,wallYa)*lmapsz)+ivec3( 0, lmapsz, 1),0);
			e4 = texelFetch(mapdata,ivec3(vec3(wallXa,y,wallYa)*lmapsz)+ivec3( 0, lmapsz,-1),0);
			c1 = texelFetch(mapdata,ivec3(vec3(wallXa,y,wallYa)*lmapsz)+ivec3( 1, lmapsz, 1),0);
			c2 = texelFetch(mapdata,ivec3(vec3(wallXa,y,wallYa)*lmapsz)+ivec3( 1, lmapsz,-1),0);
			c3 = texelFetch(mapdata,ivec3(vec3(wallXa,y,wallYa)*lmapsz)+ivec3(-1, lmapsz, 1),0);
			c4 = texelFetch(mapdata,ivec3(vec3(wallXa,y,wallYa)*lmapsz)+ivec3(-1, lmapsz,-1),0);
		}
		else{
			t  = texelFetch(mapdata,ivec3(vec3(wallXa,y,wallYa)*lmapsz)+ivec3( 0,-1, 0),0);
		  	e1 = texelFetch(mapdata,ivec3(vec3(wallXa,y,wallYa)*lmapsz)+ivec3( 1,-1, 0),0);
			e2 = texelFetch(mapdata,ivec3(vec3(wallXa,y,wallYa)*lmapsz)+ivec3(-1,-1, 0),0);
			e3 = texelFetch(mapdata,ivec3(vec3(wallXa,y,wallYa)*lmapsz)+ivec3( 0,-1, 1),0);
			e4 = texelFetch(mapdata,ivec3(vec3(wallXa,y,wallYa)*lmapsz)+ivec3( 0,-1,-1),0);
			c1 = texelFetch(mapdata,ivec3(vec3(wallXa,y,wallYa)*lmapsz)+ivec3( 1,-1, 1),0);
			c2 = texelFetch(mapdata,ivec3(vec3(wallXa,y,wallYa)*lmapsz)+ivec3( 1,-1,-1),0);
			c3 = texelFetch(mapdata,ivec3(vec3(wallXa,y,wallYa)*lmapsz)+ivec3(-1,-1, 1),0);
			c4 = texelFetch(mapdata,ivec3(vec3(wallXa,y,wallYa)*lmapsz)+ivec3(-1,-1,-1),0);
		}
	}
	else{
		if(ang.z<0.0){
			t  = texelFetch(mapdata,ivec3(vec3(wallXa,wallYa,z)*lmapsz)+ivec3( 0, 0, lmapsz),0);
		  	e1 = texelFetch(mapdata,ivec3(vec3(wallXa,wallYa,z)*lmapsz)+ivec3( 1, 0, lmapsz),0);
			e2 = texelFetch(mapdata,ivec3(vec3(wallXa,wallYa,z)*lmapsz)+ivec3(-1, 0, lmapsz),0);
			e3 = texelFetch(mapdata,ivec3(vec3(wallXa,wallYa,z)*lmapsz)+ivec3( 0, 1, lmapsz),0);
			e4 = texelFetch(mapdata,ivec3(vec3(wallXa,wallYa,z)*lmapsz)+ivec3( 0,-1, lmapsz),0);
			c1 = texelFetch(mapdata,ivec3(vec3(wallXa,wallYa,z)*lmapsz)+ivec3( 1, 1, lmapsz),0);
			c2 = texelFetch(mapdata,ivec3(vec3(wallXa,wallYa,z)*lmapsz)+ivec3( 1,-1, lmapsz),0);
			c3 = texelFetch(mapdata,ivec3(vec3(wallXa,wallYa,z)*lmapsz)+ivec3(-1, 1, lmapsz),0);
			c4 = texelFetch(mapdata,ivec3(vec3(wallXa,wallYa,z)*lmapsz)+ivec3(-1,-1, lmapsz),0);
		}
		else{
			t  = texelFetch(mapdata,ivec3(vec3(wallXa,wallYa,z)*lmapsz)+ivec3( 0, 0,-1),0);
		  	e1 = texelFetch(mapdata,ivec3(vec3(wallXa,wallYa,z)*lmapsz)+ivec3( 1, 0,-1),0);
			e2 = texelFetch(mapdata,ivec3(vec3(wallXa,wallYa,z)*lmapsz)+ivec3(-1, 0,-1),0);
			e3 = texelFetch(mapdata,ivec3(vec3(wallXa,wallYa,z)*lmapsz)+ivec3( 0, 1,-1),0);
			e4 = texelFetch(mapdata,ivec3(vec3(wallXa,wallYa,z)*lmapsz)+ivec3( 0,-1,-1),0);
			c1 = texelFetch(mapdata,ivec3(vec3(wallXa,wallYa,z)*lmapsz)+ivec3( 1, 1,-1),0);
			c2 = texelFetch(mapdata,ivec3(vec3(wallXa,wallYa,z)*lmapsz)+ivec3( 1,-1,-1),0);
			c3 = texelFetch(mapdata,ivec3(vec3(wallXa,wallYa,z)*lmapsz)+ivec3(-1, 1,-1),0);
			c4 = texelFetch(mapdata,ivec3(vec3(wallXa,wallYa,z)*lmapsz)+ivec3(-1,-1,-1),0);
		}
	} 
    e1 *= fract(wallX*lmapsz);
    e2 *= 1.0-fract(wallX*lmapsz);
    e3 *= fract(wallY*lmapsz);
    e4 *= 1.0-fract(wallY*lmapsz);
    c1 *= clamp(1.0-distance(vec2(1.0,1.0),vec2(fract(wallX*lmapsz),fract(wallY*lmapsz))),0.0,1.0);
    c2 *= clamp(1.0-distance(vec2(1.0,0.0),vec2(fract(wallX*lmapsz),fract(wallY*lmapsz))),0.0,1.0);
    c3 *= clamp(1.0-distance(vec2(0.0,1.0),vec2(fract(wallX*lmapsz),fract(wallY*lmapsz))),0.0,1.0);
    c4 *= clamp(1.0-distance(vec2(0.0,0.0),vec2(fract(wallX*lmapsz),fract(wallY*lmapsz))),0.0,1.0);
	return (c1.rgb+c2.rgb+c3.rgb+c4.rgb+e1.rgb+e2.rgb+e3.rgb+e4.rgb+t.rgb)/2.0 * (1.0-revertnoise(vec2(fract(wallX*lmapsz),fract(wallY*lmapsz))));
}

void lightEffect(){
}
	
void main(){

	vec2 offset;
	offset.y = (gl_FragCoord.y * 2.0 / reso.y - 1.0) * fov.y;
	offset.x = (gl_FragCoord.x * 2.0 / reso.x - 1.0) * fov.x;

    ang = normalize(playerDir + playerDirUp * offset.y + playerDirRight * offset.x);
	
	FragColor.a = 1.0;
	vec3 pos = Pos;
	float reflectP = 1.0;

    x = pos.x;
    y = pos.y;
    z = pos.z;

	deltaX = abs(1.0 / ang.x);
	deltaY = abs(1.0 / ang.y);
	deltaZ = abs(1.0 / ang.z);

	if(ang.x < 0){
		stepX = -1;
		sideX = fract(x) * deltaX;
	}
	else{
		stepX = 1;
		sideX = (int(x) + 1.0 - x) * deltaX;
	}
	if(ang.y < 0){
		stepY = -1;
		sideY = fract(y) * deltaY;
	}
	else{
		stepY = 1;
		sideY = (int(y) + 1.0 - y) * deltaY;
	}
	if(ang.z < 0){
		stepZ = -1;
		sideZ = fract(z) * deltaZ;
	}
	else{
		stepZ = 1;
		sideZ = (int(z) + 1.0 - z) * deltaZ;
	}
	x = int(x);
	y = int(y);
	z = int(z);
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
			if(side == 0){
				wallXa = pos.y + (sideX - deltaX) * ang.y;
				wallYa = pos.z + (sideX - deltaX) * ang.z;
			}
			else if(side == 1){
				wallXa = pos.x + (sideY - deltaY) * ang.x;
				wallYa = pos.z + (sideY - deltaY) * ang.z;
			}
			else{
				wallXa = pos.x + (sideZ - deltaZ) * ang.x;
				wallYa = pos.y + (sideZ - deltaZ) * ang.y;
			} 
			wallX = fract(wallXa);
			wallY = fract(wallYa);
			block[0] *= 255;
			float pattern;
			if(state / 4 % 2 == 1 &&i<15&&(wallX < 0.02 || wallY < 0.02 || wallX > 0.98 || wallY > 0.98)){
				FragColor.rgb = vec3(random(block[0]),random(block[0]+0.1),random(block[0]+0.2));
				return;
			}
			switch(int(block[0])){
			case 1:	
				for(int i2 = 0;i2 < 15.0;i2++){
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
				i+=15;
				break;
			case 2:{
				vec3 tp = vec3(x,y,z);
				vec3 ts = vec3(sideX,sideY,sideZ);
				for(int i2 = 0;i2 < 5;i2++){
			        if(ts.x < ts.y){
			            if(ts.x < ts.z){
							tp.x += stepX;
							ts.x += deltaX;
			            }
			            else{
							tp.z += stepZ;
							ts.z += deltaZ;
			            }
			        }
			        else if(ts.y < ts.z){
						tp.y += stepY;
						ts.y += deltaY;
			        }
			        else{
						tp.z += stepZ;
						ts.z += deltaZ;
			        }
					if(texelFetch(map,ivec3(tp),0).x*255!= 2.0){	
						//FragColor.r += float(i2)/5.0;
						break;
					}
				}
				vec3 tr = getSubCoords();
				while(tr.x >= 0.0 && tr.x <= 1.0 && tr.y >=0.0 && tr.y <= 1.0 && tr.z >=  0.0 && tr.z <= 1.0){
					float dst = tr.z - sin((tr.x+x+float(tick)*0.01)*5.0)/45.0 - 0.5;
					if(dst < 0.0001){
						float f1 = cos((tr.x+x+float(tick)*0.01)*5.0)/45.0;
						ang.xz = reflect(ang.xz,normalize(vec2(f1,-1)));
						for(;;){
							dst = min(min(min(tr.x,1.0-tr.x),min(tr.y,1.0-tr.y)),min(tr.z,1.0-tr.z));
							if(dst < 0.001){
								break;
							}
							tr += ang * dst;
						}
						x = x+tr.x;
					    y = y+tr.y;
					    z = z+tr.z;
						pos.x = x;
						pos.y = y;	
						pos.z = z;
						reinitializeRay();
						break;
					}
					tr += ang * dst;
				}
				break;
				}
			case 3:{
				break;
				}
			case 4:{
				vec3 tr = getSubCoords();
				float r = iSphere(tr-0.5,ang,0.4);
				if(r > 0.0){
					tr += ang * r;
					ang = reflect(ang,normalize(tr-0.5));
					tr += ang;
					tr = normalize(tr);
					if(tr.x == 1.0){
						wallXa = tr.y+y;
						wallYa = tr.z+z;
					}
					else if(tr.y == 1.0){
						wallXa = tr.x+x;
						wallYa = tr.z+z;
					}
					else if(tr.z == 1.0){
						wallXa = tr.x+x;
						wallYa = tr.y+y;
					}
					FragColor.rgb = blockLight();
					return;
				}	
				break;
			}
			case 5:{
				vec3 tr = getSubCoords();
				float d = iTorus(tr,ang,vec2(0.1,0.3));
				if(d > 0.0){
					FragColor.r = 1.0;
					return;
				}
				break;
			}
			case 6:{
				vec4 exdt = texelFetch(mapdata,ivec3(x,y,z),0);
				float s = exdt.a*255.0;
				vec2 rpos = vec2(wallXa,wallYa)*(exdt.a*255.0);
				vec2 m = floor(rpos)+0.5;
				float d = 1.0-max(abs(rpos.x-m.x),abs(rpos.y-m.y));
				vec2 s3 = fract(rpos);
                vec2 s2 = floor(rpos);
				float r = floor(random(s2)*3.0) + exdt.r;
	            FragColor.rgb += vec3(random(r),random(r+0.01),random(r+0.02)) * blockLight();
				if(s3.x < 0.025 || s3.x > 0.975 || s3.y < 0.025 || s3.y > 0.975){
					FragColor.rgb /= 2.0;
				}
				lightEffect();
				return;
			}
			case 7:
				float X,Y;
				switch(side){
				case 0:
					X = (y + wallX) / 1024 + 0.2;
					Y = (z + wallY) / 1024 + 0.4;
					break;
				case 1:
					X = (x + wallX) / 1024 + 0.2;
					Y = (z + wallY) / 1024 + 0.4;
					break;
				case 2:
					X = (x + wallX) / 1024 + 0.2;
					Y = (y + wallY) / 1024 + 0.4;
					break;
				}
				float VX = 0;
				float VY = 0;
				int itt = 0;
				while(VX + VY < 512 && itt < 256){
					float xtemp = VX*VX-VY*VY+X;
					VY = 2*VX*VY+Y;
					VX = xtemp;
					itt++;
				}
				FragColor.rgb = (float(itt) / 256) * blockLight();
				return;
			case 8:{
				vec3 tr = getSubCoords();
				float s = iSphere(tr-0.5,ang,0.4);
				if(s != -1.0){
					tr += ang * s;
					FragColor.rgb += block.gba*10.0 * pow(-dot(tr-0.5,ang),8.0) * 1000.0;
				}
				break;
				}
			case 9:{
				vec3 tr = getSubCoords()+vec3(x,y,z); 
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
				if(side == 0){
					wallX = fract(pos.y + (sideX - deltaX) * ang.y);
					wallY = fract(pos.z + (sideX - deltaX) * ang.z);
				}
				else if(side == 1){
					wallX = fract(pos.x + (sideY - deltaY) * ang.x);
					wallY = fract(pos.z + (sideY - deltaY) * ang.z);
				}
				else{
					wallX = fract(pos.x + (sideZ - deltaZ) * ang.x);
					wallY = fract(pos.y + (sideZ - deltaZ) * ang.y);
				} 
				vec3 tr2 = getSubCoords()+vec3(x,y,z);
				float d = distance(tr,tr2);
				fog.a += 0.05 * d;
				fog.rgb += block.gba * d * clamp(1.0-fog.a,0.0,1.0) * 0.1;
				continue;
				}
			case 10:{
				vec3 tr = getSubCoords();
				vec4 angle = texelFetch(mapdata,ivec3(x,y,z),0);
				if(tr.x < angle.w-angle.x||tr.y < angle.w-angle.y||tr.z < angle.w-angle.z){
					FragColor.rgb += blockLight();
					return;
				}
				float p = iPlane(tr,ang,vec4(normalize(angle.xyz),-angle.w));
				if(p>0.0){
					tr += ang * p;
					side = 2;
					wallX = tr.x;
					wallY = tr.y;
					FragColor.rgb += blockLight();
					return;
				}
				break;
				}
			case 11:{
				vec3 btr = getSubCoords()*30.0;
				vec3 si = getSide(btr);
				vec3 tr = floor(btr);
         		while(tr.x >= 0.0 && tr.x <= 30.0-stepX && tr.y >=0.0 && tr.y <= 30.0-stepY && tr.z >= 0.0 && tr.z <= 30.0-stepZ){   
					if(tr.y == 20.0){
						FragColor.rgb = vec3(1.0);	
						return;
					}
					if(si.x < si.y){
			            if(si.x < si.z){
							tr.x += stepX;
							si.x += deltaX;
							side = 0;
			            }
			            else{
							tr.z += stepZ;
							si.z += deltaZ;
							side = 2;
			            }
			        }
			        else if(si.y < si.z){
						tr.y += stepY;
						si.y += deltaY;
						side = 1;
			        }
			        else{
						tr.z += stepZ;
						si.z += deltaZ;
						side = 2;
			        }
          		}
				break;
				}
			case 12:{
				return;
			}
			case 13:
				switch(side){
				case 0:{
					if(ang.x < 0){
						x++;
					}
					ang.x = -ang.x;
					y = pos.y + (sideX - deltaX) * ang.y;
					z = pos.z + (sideX - deltaX) * ang.z;
					break;
					}
				case 1:
					if(ang.y < 0){
						y++;
					}
					ang.y = -ang.y;
					x = pos.x + (sideY - deltaY) * ang.x;
					z = pos.z + (sideY - deltaY) * ang.z;
					break;
				case 2:
					if(ang.z < 0){
						z++;
					}
					ang.z = -ang.z;
					x = pos.x + (sideZ - deltaZ) * ang.x;
					y = pos.y + (sideZ - deltaZ) * ang.y;
					break;
				}
				if(ang.x < 0){
					stepX = -1;
					sideX = fract(x) * deltaX;
				}
				else{
					stepX = 1;
					sideX = (int(x) + 1.0 - x) * deltaX;
				}
				if(ang.y < 0){
					stepY = -1;
					sideY = fract(y) * deltaY;
				}
				else{
					stepY = 1;
					sideY = (int(y) + 1.0 - y) * deltaY;
				}
				if(ang.z < 0){
					stepZ = -1;
					sideZ = fract(z) * deltaZ;
				}
				else{
					stepZ = 1;
					sideZ = (int(z) + 1.0 - z) * deltaZ;
				}
				pos.x = x;
				pos.y = y;
				pos.z = z;
				break;
			case 14:
				vec4 Block = texelFetch(epicTexture,ivec2(wallX*1024,wallY*1024),0);
				FragColor.rgb += Block.rgb * blockLight();
				lightEffect();
				return;
			case 15:{
				vec4 Block = texelFetch(epicTexture,ivec2(wallX*1024+1024,wallY*1024),0);
				FragColor.rgb += Block.rgb * blockLight();
				lightEffect();
				return;
				}
			case 16:
				break;
			case 17:{
				vec3 tr = getSubCoords();
				float dst = iTorus(tr-vec3(0.5,0.5,0.1),ang,vec2(0.3,0.1));
				if(dst > 0.0){
					tr += ang * dst;
					if(tr.x >= 0.0 && tr.x <= 1.0 && tr.y >=0.0 && tr.y <= 1.0 && tr.z >=  0.0 && tr.z <= 1.0){
						FragColor.rgb = normalize(tr);
						lightEffect();
						return;
					}
				}
				break;
			}
			case 18:{
				vec3 tr = getSubCoords();
				while(tr.x >= 0.0 && tr.x <= 1.0 && tr.y >=0.0 && tr.y <= 1.0 && tr.z >=  0.0 && tr.z <= 1.0){
					float dst = distance(tr.xy,vec2(0.5)) - 0.45 + cos(atan(tr.x-0.5,tr.y-0.5)*20.0) / 40.0;
					if(dst < 0.001){
						FragColor.rgb = (0.7 - vec3(cos(atan(tr.x-0.5,tr.y-0.5)*20.0)) / 5.0);
						vec3 e1 = texelFetch(map,ivec3(x+1.0,y,z),0).gba * tr.x;
						vec3 e2 = texelFetch(map,ivec3(x+1.0,y,z+1.0),0).gba * tr.x;
						vec3 e3 = texelFetch(map,ivec3(x+1.0,y,z-1.0),0).gba * tr.x;
						vec3 e4 = texelFetch(map,ivec3(x-1.0,y,z),0).gba * (1.0-tr.x);
						vec3 e5 = texelFetch(map,ivec3(x-1.0,y,z+1.0),0).gba * (1.0-tr.x);
						vec3 e6 = texelFetch(map,ivec3(x-1.0,y,z-1.0),0).gba * (1.0-tr.x);
						vec3 e7 = texelFetch(map,ivec3(x,y+1.0,z),0).gba * tr.y;
						vec3 e8 = texelFetch(map,ivec3(x,y+1.0,z+1.0),0).gba * tr.y;
						vec3 e9 = texelFetch(map,ivec3(x,y+1.0,z-1.0),0).gba * tr.y;
						vec3 e10 = texelFetch(map,ivec3(x,y-1.0,z),0).gba * (1.0-tr.y);
						vec3 e11 = texelFetch(map,ivec3(x,y-1.0,z+1.0),0).gba * (1.0-tr.y);
						vec3 e12 = texelFetch(map,ivec3(x,y-1.0,z-1.0),0).gba * (1.0-tr.y);
						e2 *= tr.z;
						e3 *= 1.0-tr.z;
						e5 *= tr.z;
						e6 *= 1.0-tr.z;
						e8 *= tr.z;
						e9 *= 1.0-tr.z;
						e11 *= tr.z;
						e12 *= 1.0-tr.z;
						FragColor.rgb *= (e1+e2+e3+e4+e5+e6+e7+e8+e9+e10+e11+e12)/3.0;
						lightEffect();
						return;
					}
					tr += ang * dst;
				}
				break;
			}
			case 19:{
				vec3 tr = getSubCoords();
				while(tr.x >= 0.0 && tr.x <= 1.0 && tr.y >=0.0 && tr.y <= 1.0 && tr.z >=  0.0 && tr.z <= 1.0){
					float dst = distance(tr.xz,vec2(0.5)) - 0.5;
					if(dst < 0.001){
						FragColor.rgb = vec3(tnoise(vec2(atan(tr.x-0.5,tr.z-0.5),tr.y+y),100.0));
						lightEffect();
						return;
					}
					tr += ang * dst;
				}
				break;
			}
			case 20:
				vec3 btr = getSubCoords()*16.0;
				vec3 si = getSide(btr);
				vec3 tr = floor(btr);
				vec4 bdt = texelFetch(mapdata,ivec3(x,y,z),0);
				while(tr.x >= 0.0 && tr.x <= 16.0-stepX && tr.y >=0.0 && tr.y <= 16.0-stepY && tr.z >= 0.0 && tr.z < 16.0-stepZ){ 
					if(si.x < si.y){
			            if(si.x < si.z){
							tr.x += stepX;
							si.x += deltaX;
							side = 0;
			            }
			            else{
							tr.z += stepZ;
							si.z += deltaZ;
							side = 2;
			            }
			        }
			        else if(si.y < si.z){
						tr.y += stepY;
						si.y += deltaY;
						side = 1;
			        }
			        else{
						tr.z += stepZ;
						si.z += deltaZ;
						side = 2;
			        }
					float h = texelFetch(chessModels,ivec3(tr.xy,tr.z+bdt.a*4096),0).x;
					if(h > 0.0){
						switch(side){
						case 0:
							wallX = tr.y/16.0;
							wallY = tr.z/16.0;
							break;
						case 1:
							wallX = tr.x/16.0;
							wallY = tr.z/16.0;
							break;
						case 2:
							wallX = tr.x/16.0;
							wallY = tr.y/16.0;
							break;
						}
						switch(int(h*255.0)){
						case 1:
							FragColor.rgb = blockLight() * bdt.rgb;
							return;
						case 2:
							FragColor.rgb = blockLight() * (1.0-bdt.rgb);
							return;
						}
					}
				}
				break;
			case 21:{
				vec3 tr = getSubCoords();
				vec4 xdt = texelFetch(mapdata,ivec3(x,y,z),0);
				vec2 glscrd;
				float d;
				switch(int(xdt.a*255.0)){
				case 0:
					d = iPlane(tr,ang,vec4(1.0,0.0,0.0,-0.5));
					tr += ang * d;
					glscrd = tr.yz+vec2(x,z);
					break;
				case 1:
					d = iPlane(tr,ang,vec4(0.0,1.0,0.0,-0.5));
					tr += ang * d;
					glscrd = tr.xz+vec2(x,z);
					break;
				case 2:
					d = iPlane(tr,ang,vec4(0.0,0.0,1.0,-0.999));
					tr += ang * d;
					glscrd = tr.xy+vec2(x,y);
				}
				if(tr.x >= 0.0 && tr.x <= 1.0 && tr.y >=0.0 && tr.y <= 1.0 && tr.z >=  0.0 && tr.z <= 1.0){
					vec2 fp = glscrd;
					fog.a += max(max(xdt.r,xdt.g),xdt.b);
					fog.rgb += xdt.rgb;

				}
				break;
			}
			case 22:{
				vec3 btr = getSubCoords()*8.0;
				vec3 si = getSide(btr);
				vec3 tr = floor(btr);
				vec4 bdt = texelFetch(mapdata,ivec3(x,y,z),0);
				while(tr.x >= 0.0 && tr.x <= 8.0-stepX && tr.y >=0.0 && tr.y <= 8.0-stepY && tr.z >= 0.0 && tr.z <= 8.0){ 
					float h = texelFetch(models8,ivec3(tr.xy,tr.z+bdt.a*512),0).x;
					if(h > 0.0){
						switch(side){
						case 0:
							wallX = tr.y/8.0;
							wallY = tr.z/8.0;
							break;
						case 1:
							wallX = tr.x/8.0;
							wallY = tr.z/8.0;
							break;
						case 2:
							wallX = tr.x/8.0;
							wallY = tr.y/8.0;
							break;
						}
						switch(int(h*255.0)){
						case 1:
							FragColor.rgb = blockLight() * bdt.rgb;
							return;
						case 2:
							FragColor.rgb = blockLight() * (1.0-bdt.rgb);
							return;
						}
					}
					if(si.x < si.y){
			            if(si.x < si.z){
							tr.x += stepX;
							si.x += deltaX;
							side = 0;
			            }
			            else{
							tr.z += stepZ;
							si.z += deltaZ;
							side = 2;
			            }
			        }
			        else if(si.y < si.z){
						tr.y += stepY;
						si.y += deltaY;
						side = 1;
			        }
			        else{
						tr.z += stepZ;
						si.z += deltaZ;
						side = 2;
			        }
				}
				break;
			}
			case 23:{
				vec2 tr = floor(vec2(wallXa*16.0,wallYa*16.0));
				vec3 col = vec3(random(tr),random(tr+0.1),random(tr+0.2)) * vec3(0.5,0.75,0.0);
				col += 0.25;
				col = pow(col,vec3(2.0));
				FragColor.rgb += blockLight() * col;
				lightEffect();
				return;
			}
			case 24:
				break;
			case 25:
				break;
			case 26:{
				vec3 tr = getSubCoords();	
				vec4 bldt = texelFetch(mapdata,ivec3(x,y,z),0);
				while(tr.x >= 0.0 && tr.x <= 1.0 && tr.y >=0.0 && tr.y <= 1.0 && tr.z >=  0.0 && tr.z <= 1.0){
					vec3 tp = fract(tr*4.0);
					vec2 ct = vec2(random(floor(tr.xy*4.0)),random(floor(tr.xy*4.0)))*0.5+0.25;
					float dst = distance(tp.xy,ct)-0.2;
					if(dst<0.001){
						wallX = tr.x;
						wallY = tr.y;
						ang.xy = tp.xy-ct;
						side = 0;
						vec3 b1 = blockLight();
						side = 1;
						vec3 b2 = blockLight();
						vec2 v1 = abs(ct-tp.xy);
						float v2 = v1.x+v1.y;
						v1 /= v2;
						b1 *= v1.x;
						b2 *= v1.y;
						FragColor.rgb += (b1+b2) * bldt.rgb;
						return;
					}
					tr += ang * dst * 0.25;
				}
				break;
			}
			case 27:{
				vec4 bldt = texelFetch(mapdata,ivec3(x,y,z),0);
				FragColor.rgb += clamp(1.0-pow(abs(wallY-0.5),2.0)*3.0,0.0,1.0) * bldt.rgb;
				FragColor.rgb += blockLight() * 0.5;
				return;
			}
			case 28:{
				vec4 bldt = texelFetch(map,ivec3(x,y,z),0);
				FragColor.rgb += blockLight() * bldt.gba;
				lightEffect();
				return;
			}
			case 29:
				pattern = clamp(tnoise(vec2(wallXa,wallYa)*0.2,200) / 80.0 + 0.9,0.0,1.0);
				FragColor.rgb += vec3(1.0,0.1,0.1) * blockLight() * pattern;
				return;
			case 30:{
				vec3 tr = getSubCoords();
				if(tr.x < tr.y){
					pattern = clamp(tnoise(vec2(wallXa,wallYa)*0.2,200) / 80.0 + 0.9,0.0,1.0);
					FragColor.rgb += blockLight();
					lightEffect();
					return;
				}
				float p = iPlane(tr,ang,vec4(1.0,-1.0,0.0,0.0));
				if(p>0.0){
					tr += ang * p;
					if(tr.x >= 0.0 && tr.x <= 1.0 && tr.y >=0.0 && tr.y <= 1.0 && tr.z >=  0.0 && tr.z <= 1.0){
						vec3 t,t2,e1,e2,e3,e4,c1,c2,c3,c4;
						t  = texelFetch(map,ivec3(x+1,y,z),0).gba;
						t2 = texelFetch(map,ivec3(x,y-1,z),0).gba;	
					    e1 = texelFetch(map,ivec3(x+1,y,z+1),0).gba;
						e2 = texelFetch(map,ivec3(x+1,y,z-1),0).gba;
						e3 = texelFetch(map,ivec3(x,y-1,z+1),0).gba;
						e4 = texelFetch(map,ivec3(x,y-1,z-1),0).gba;
						t  *= tr.x;
						t2 *= 1.0-tr.y;
					    e1 *= clamp(1.0-distance(vec2(1.0,1.0),tr.xz),0.0,1.0);
					    e2 *= clamp(1.0-distance(vec2(1.0,0.0),tr.xz),0.0,1.0);
					    e3 *= clamp(1.0-distance(vec2(0.0,1.0),tr.yz),0.0,1.0);
					    e4 *= clamp(1.0-distance(vec2(0.0,0.0),tr.yz),0.0,1.0);
						switch(int(texelFetch(map,ivec3(x+1,y+1,z),0).x*255.0)){
						case 0:
						case 30:
							break;
						default:
							t /= 2.0;
							e1/= 2.0;
							e2/= 2.0;
							break;
						}
						switch(int(texelFetch(map,ivec3(x-1,y-1,z),0).x*255.0)){
						case 0:
						case 30:
							break;
						default:
							t2 /= 2.0;
							e3/= 2.0;
							e4/= 2.0;
							break;
						}
						FragColor.rgb += (t+t2+e1+e2+e3+e4) * 1.425 * (1.0-revertnoise(tr.xz)*1.5);
						lightEffect();
						return;
					}
				}
				break;
			}
			case 31:{
				vec3 tr = getSubCoords();
				if(tr.y < tr.x){
					pattern = clamp(tnoise(vec2(wallXa,wallYa)*0.2,200) / 80.0 + 0.9,0.0,1.0);
					FragColor.rgb += blockLight();
					lightEffect();
					return;
				}
				float p = iPlane(tr,ang,vec4(-1.0,1.0,0.0,0.0));
				if(p>0.0){
					tr += ang * p;
					if(tr.x >= 0.0 && tr.x <= 1.0 && tr.y >=0.0 && tr.y <= 1.0 && tr.z >=  0.0 && tr.z <= 1.0){
						vec3 t,t2,e1,e2,e3,e4,c1,c2,c3,c4;
						t  = texelFetch(map,ivec3(x,y+1,z),0).gba;
						t2 = texelFetch(map,ivec3(x-1,y,z),0).gba;	
					    e1 = texelFetch(map,ivec3(x,y+1,z+1),0).gba;
						e2 = texelFetch(map,ivec3(x,y+1,z-1),0).gba;
						e3 = texelFetch(map,ivec3(x-1,y,z+1),0).gba;
						e4 = texelFetch(map,ivec3(x-1,y,z-1),0).gba;
						t  *= tr.y;
						t2 *= 1.0-tr.x;
					    e1 *= clamp(1.0-distance(vec2(1.0,1.0),tr.yz),0.0,1.0);
					    e2 *= clamp(1.0-distance(vec2(1.0,0.0),tr.yz),0.0,1.0);
					    e3 *= clamp(1.0-distance(vec2(0.0,1.0),tr.xz),0.0,1.0);
					    e4 *= clamp(1.0-distance(vec2(0.0,0.0),tr.xz),0.0,1.0);
						switch(int(texelFetch(map,ivec3(x+1,y+1,z),0).x*255.0)){
						case 0:
						case 31:
							break;
						default:
							t /= 2.0;
							e1/= 2.0;
							e2/= 2.0;
							break;
						}
						switch(int(texelFetch(map,ivec3(x-1,y-1,z),0).x*255.0)){
						case 0:
						case 31:
							break;
						default:
							t2 /= 2.0;
							e3/= 2.0;
							e4/= 2.0;
							break;
						}
						FragColor.rgb += (t+t2+e1+e2+e3+e4) * 1.425 * (1.0-revertnoise(tr.xz)*1.5);
						lightEffect();
						return;
					}
				}
				break;
			}
			case 32:{
				vec3 tr = getSubCoords();
				if(tr.x < tr.z){
					pattern = clamp(tnoise(vec2(wallXa,wallYa)*0.2,200) / 80.0 + 0.9,0.0,1.0);
					FragColor.rgb += blockLight();
					lightEffect();
					return;
				}
				float p = iPlane(tr,ang,vec4(1.0,-1.0,0.0,0.0));
				if(p>0.0){
					tr += ang * p;
					if(tr.x >= 0.0 && tr.x <= 1.0 && tr.y >=0.0 && tr.y <= 1.0 && tr.z >=  0.0 && tr.z <= 1.0){
						vec3 t,t2,e1,e2,e3,e4,c1,c2,c3,c4;
						t  = texelFetch(map,ivec3(x+1,y,z),0).gba;
						t2 = texelFetch(map,ivec3(x,y,z-1),0).gba;	
					    e1 = texelFetch(map,ivec3(x+1,y+1,z),0).gba;
						e2 = texelFetch(map,ivec3(x+1,y-1,z),0).gba;
						e3 = texelFetch(map,ivec3(x,y+1,z-1),0).gba;
						e4 = texelFetch(map,ivec3(x,y-1,z-1),0).gba;
						t  *= tr.x;
						t2 *= 1.0-tr.z;
					    e1 *= clamp(1.0-distance(vec2(1.0,1.0),tr.xy),0.0,1.0);
					    e2 *= clamp(1.0-distance(vec2(1.0,0.0),tr.xy),0.0,1.0);
					    e3 *= clamp(1.0-distance(vec2(0.0,1.0),tr.yz),0.0,1.0);
					    e4 *= clamp(1.0-distance(vec2(0.0,0.0),tr.yz),0.0,1.0);
						switch(int(texelFetch(map,ivec3(x+1,y,z+1),0).x*255.0)){
						case 0:
						case 32:
							break;
						default:
							t /= 2.0;
							e1/= 2.0;
							e2/= 2.0;
							break;
						}
						switch(int(texelFetch(map,ivec3(x-1,y,z-1),0).x*255.0)){
						case 0:
						case 32:
							break;
						default:
							t2 /= 2.0;
							e3/= 2.0;
							e4/= 2.0;
							break;
						}
						FragColor.rgb += (t+t2+e1+e2+e3+e4) * 1.425 * (1.0-revertnoise(tr.xz)*1.5);
						lightEffect();
						return;
					}
				}
				break;
			}
			case 33:{
				vec3 tr = getSubCoords();
				if(tr.x > tr.z){
					pattern = clamp(tnoise(vec2(wallXa,wallYa)*0.2,200) / 80.0 + 0.9,0.0,1.0);
					FragColor.rgb += blockLight();
					lightEffect();
					return;
				}
				float p = iPlane(tr,ang,vec4(1.0,0.0,-1.0,0.0));
				if(p>0.0){
					tr += ang * p;
					if(tr.x >= 0.0 && tr.x <= 1.0 && tr.y >=0.0 && tr.y <= 1.0 && tr.z >=  0.0 && tr.z <= 1.0){
						vec3 t,t2,e1,e2,e3,e4,c1,c2,c3,c4;
						t  = texelFetch(map,ivec3(x,y,z+1),0).gba;
						t2 = texelFetch(map,ivec3(x-1,y,z),0).gba;	
					    e1 = texelFetch(map,ivec3(x,y+1,z+1),0).gba;
						e2 = texelFetch(map,ivec3(x,y-1,z+1),0).gba;
						e3 = texelFetch(map,ivec3(x-1,y+1,z),0).gba;
						e4 = texelFetch(map,ivec3(x-1,y-1,z),0).gba;
						t  *= tr.x;
						t2 *= 1.0-tr.z;
					    e1 *= clamp(1.0-distance(vec2(1.0,1.0),tr.xz),0.0,1.0);
					    e2 *= clamp(1.0-distance(vec2(1.0,0.0),tr.xz),0.0,1.0);
					    e3 *= clamp(1.0-distance(vec2(0.0,1.0),tr.xy),0.0,1.0);
					    e4 *= clamp(1.0-distance(vec2(0.0,0.0),tr.xy),0.0,1.0);
						switch(int(texelFetch(map,ivec3(x+1,y,z+1),0).x*255.0)){
						case 0:
						case 33:
							break;
						default:
							t /= 2.0;
							e1/= 2.0;
							e2/= 2.0;
							break;
						}
						switch(int(texelFetch(map,ivec3(x-1,y,z-1),0).x*255.0)){
						case 0:
						case 33:
							break;
						default:
							t2 /= 2.0;
							e3/= 2.0;
							e4/= 2.0;
							break;
						}
						FragColor.rgb += (t+t2+e1+e2+e3+e4) * 1.425 * (1.0-revertnoise(tr.xz)*1.5);
						lightEffect();
						return;
					}
				}
				break;
			}
			case 34:{
				vec3 tr = getSubCoords();
				while(tr.x >= 0.0 && tr.x <= 1.0 && tr.y >=0.0 && tr.y <= 1.0 && tr.z >=  0.0 && tr.z <= 1.0){
					float dst = tr.y - tr.z;
					dst /= 2.0;
					if(dst < 0.0001){
						if(tr.x == 0.0 || tr.x == 1.0){
							FragColor = texelFetch(spikes,ivec2(vec2(1.0-tr.z,tr.y) * 1024.0),0);
						}
						else if(tr.z == 1.0){
							FragColor = texelFetch(epicTexture,ivec2(vec2(tr.x,tr.y) * 1024.0+ivec2(1024,0)),0);
						}
						else if(tr.y == 0.0){
							FragColor = texelFetch(epicTexture,ivec2(vec2(tr.x,tr.z) * 1024.0+ivec2(1024,0)),0);
						}	
						else{
							FragColor = texelFetch(epicTexture,ivec2(vec2((tr.z+tr.y)/2.0,tr.x) * 1024.0+ivec2(1024,0)),0);
						}
						return;
					}
					tr += ang * dst;
				}
				break;
			}
			case 35:{
				vec3 tr = getSubCoords();
				while(tr.x >= 0.0 && tr.x <= 1.0 && tr.y >=0.0 && tr.y <= 1.0 && tr.z >=  0.0 && tr.z <= 1.0){
					float dst = tr.x - tr.z;
					dst /= 2.0;
					if(dst < 0.0001){
						if(tr.y == 0.0 || tr.y == 1.0){
							FragColor = texelFetch(spikes,ivec2(vec2(1.0-tr.z,tr.x) * 1024.0),0);
						}
						else if(tr.z == 1.0){
							FragColor = texelFetch(epicTexture,ivec2(vec2(tr.x,tr.y) * 1024.0+ivec2(1024,0)),0);
						}
						else if(tr.x == 0.0){
							FragColor = texelFetch(epicTexture,ivec2(vec2(tr.y,tr.z) * 1024.0+ivec2(1024,0)),0);
						}	
						else{
							FragColor = texelFetch(epicTexture,ivec2(vec2((tr.z+tr.x)/2.0,tr.y) * 1024.0+ivec2(1024,0)),0);
						}
						return;
					}
					tr += ang * dst;
				}
				break;
			}
			case 45:
				vec2 p = vec2(wallXa,wallYa) * 10.0;
				vec2 fp = fract(p);
				if(random(floor(p)) < 0.5){
			        if(fp.x > 0.33 && fp.x < 0.67 && fp.y > 0.33 && fp.y < 0.67){       
						FragColor.r = 1.0;
						return;
			        }
			        if(random(floor(vec2(p.x+1.0,p.y))) < 0.5 && fp.x > 0.67 && fp.y > 0.33 && fp.y < 0.67 ){
			            FragColor.r = 1.0;
						return;
			        }
			        if(random(floor(vec2(p.x-1.0,p.y))) < 0.5 && fp.x < 0.33 && fp.y > 0.33 && fp.y < 0.67 ){
			            FragColor.r = 1.0;
						return;
			        }
			        if(random(floor(vec2(p.x,p.y+1.0))) < 0.5 && fp.x > 0.33 && fp.x < 0.67 && fp.y > 0.67 ){
			            FragColor.r = 1.0;
						return;
			        }
			        if(random(floor(vec2(p.x,p.y-1.0))) < 0.5 && fp.x > 0.33 && fp.x < 0.67 && fp.y < 0.33 ){
			            FragColor.r = 1.0;
						return;
			        }
			        if(random(floor(vec2(p.x+1.0,p.y+1.0))) < 0.5 && fp.x > 0.67 && fp.y > 0.67 ){
			            FragColor.r = 1.0;
						return;
			        }
			        if(random(floor(vec2(p.x+1.0,p.y-1.0))) < 0.5 && fp.x > 0.67 && fp.y < 0.33 ){
			            FragColor.r = 1.0;
						return;
			        }
			        if(random(floor(vec2(p.x-1.0,p.y+1.0))) < 0.5 && fp.x < 0.33 && fp.y > 0.67 ){
			            FragColor.r = 1.0;
						return;
			        }
			        if(random(floor(vec2(p.x-1.0,p.y-1.0))) < 0.5 && fp.x < 0.33 && fp.y < 0.33 ){
			            FragColor.r = 1.0;
						return;
			        }
				}
				return;
			case 46:{
				break;
				}
			case 47:{
				vec3 tr = getSubCoords();
				while(tr.x >= 0.0 && tr.x <= 1.0 && tr.y >=0.0 && tr.y <= 1.0 && tr.z >=  0.0 && tr.z <= 1.0){
					float dst = distance(tr,vec3(0.5))-0.3;
					if(dst < 0.001){
						FragColor = texelFetch(spikes,ivec2((atan(tr.x-0.5,tr.y-0.5)+3.14)*32.0/6.28,tr.z*32.0),0);
						return;
					}
					tr += ang * dst;
				}
				break;
			}
			case 48:{
				vec3 tr = getSubCoords();
				while(tr.x >= 0.0 && tr.x <= 1.0 && tr.y >=0.0 && tr.y <= 1.0 && tr.z >=  0.0 && tr.z <= 1.0){
					float dst = distance(tr.xy,vec2(0.5))-0.3;
					if(dst < 0.001){
						FragColor.rgb = normalize(tr);
						return;
					}
					tr += ang * dst;
				}
				break;
			}
			case 49:{
                vec3 btr = getSubCoords()*4.0;
				vec3 si = getSide(btr);
				vec3 tr = floor(btr);
                while(tr.x >= 0.0 && tr.x <= 4.0-stepX && tr.y >=0.0 && tr.y <= 4.0-stepY && tr.z >= 0.0 && tr.z <= 4.0-stepZ){   
					if(tr.z - tr.y < 1.0){	
						vec4 t,t2,e1,e2,e3,e4;
						vec2 uWall;
						if(tr.z == tr.y){
							switch(side){
							case 0:
								FragColor.rgb = blockLight();
								break;
							case 1:
								uWall.x = (btr.x + (si.y - deltaY) * ang.x)*0.25;
								uWall.y = (btr.z + (si.y - deltaY) * ang.z)*0.25;
								if(tr.y==0.0&&texelFetch(map,ivec3(x,y-1,z-1),0).r==0.0){
									FragColor.rgb = blockLight();
									break;
								}
								t  = texelFetch(map,ivec3(x,y-1,z),0);
								t2 = texelFetch(map,ivec3(x,y,z+1),0);
								e1 = texelFetch(map,ivec3(x+1,y-1,z),0);
								e2 = texelFetch(map,ivec3(x-1,y-1,z),0);
								e3 = texelFetch(map,ivec3(x+1,y,z+1),0);
								e4 = texelFetch(map,ivec3(x-1,y,z+1),0);
								t  *= 1.0-uWall.y;
								t2 *= uWall.y;
								e1 *= uWall.x*(1.0-uWall.y);
								e2 *= (1.0-uWall.x)*(1.0-uWall.y);
								e3 *= uWall.x*uWall.y;
								e4 *= (1.0-uWall.x)*uWall.y;
								FragColor.rgb = (t.gba+t2.gba+e1.gba+e2.gba+e3.gba+e4.gba)/2.0;
								FragColor.rgb *= fract((btr.z + (si.y - deltaY) * ang.z))*0.25+0.75;
								break;
							case 2:
								uWall.x = (btr.x + (si.z - deltaZ) * ang.x)*0.25;
								uWall.y = (btr.y + (si.z - deltaZ) * ang.y)*0.25;
								if(tr.z==3.0&&texelFetch(map,ivec3(x,y+1,z+1),0).r==0.0){
									FragColor.rgb = blockLight();
									break;
								}
								t  = texelFetch(map,ivec3(x,y-1,z),0);
								t2 = texelFetch(map,ivec3(x,y,z+1),0);
								e1 = texelFetch(map,ivec3(x+1,y-1,z),0);
								e2 = texelFetch(map,ivec3(x-1,y-1,z),0);
								e3 = texelFetch(map,ivec3(x+1,y,z+1),0);
								e4 = texelFetch(map,ivec3(x-1,y,z+1),0);
								t  *= 1.0-uWall.y;
								t2 *= uWall.y;
								e1 *= uWall.x*(1.0-uWall.y);
								e2 *= (1.0-uWall.x)*(1.0-uWall.y);
								e3 *= uWall.x*uWall.y;
								e4 *= (1.0-uWall.x)*uWall.y;
								FragColor.rgb = (t.gba+t2.gba+e1.gba+e2.gba+e3.gba+e4.gba)/2.0;
								FragColor.rgb *= 1.0-fract((btr.y + (si.z - deltaZ) * ang.y))*0.25;
								break;
							}
						}
						else{
							FragColor.rgb = blockLight();
						}
						lightEffect();
						return;
					}  
					if(si.x < si.y){
			            if(si.x < si.z){
							tr.x += stepX;
							si.x += deltaX;
							side = 0;
			            }
			            else{
							tr.z += stepZ;
							si.z += deltaZ;
							side = 2;
			            }
			        }
			        else if(si.y < si.z){
						tr.y += stepY;
						si.y += deltaY;
						side = 1;
			        }
			        else{
						tr.z += stepZ;
						si.z += deltaZ;
						side = 2;
			        }
                }
                break;
            }
			case 50:{
                vec3 btr = getSubCoords()*4.0;
				vec3 si = getSide(btr);
				vec3 tr = floor(btr);
                while(tr.x >= 0.0 && tr.x <= 4.0-stepX && tr.y >=0.0 && tr.y <= 4.0-stepY && tr.z >= 0.0 && tr.z <= 4.0-stepZ){   
					if(tr.z - (3.0-tr.y) < 1.0){	
						vec4 t,t2,e1,e2,e3,e4;
						vec2 uWall;
						if(tr.z == 3.0-tr.y){
							switch(side){
							case 0:
								FragColor.rgb = blockLight();
								break;
							case 1:
								uWall.x = (btr.x + (si.y - deltaY) * ang.x)*0.25;
								uWall.y = (btr.z + (si.y - deltaY) * ang.z)*0.25;
								if(tr.y==0.0&&texelFetch(map,ivec3(x,y+1,z-1),0).r==0.0){
									FragColor.rgb = blockLight();
									break;
								}
								t  = texelFetch(map,ivec3(x,y+1,z),0);
								t2 = texelFetch(map,ivec3(x,y,z+1),0);
								e1 = texelFetch(map,ivec3(x+1,y+1,z),0);
								e2 = texelFetch(map,ivec3(x-1,y+1,z),0);
								e3 = texelFetch(map,ivec3(x+1,y,z+1),0);
								e4 = texelFetch(map,ivec3(x-1,y,z+1),0);
								t  *= 1.0-uWall.y;
								t2 *= uWall.y;
								e1 *= uWall.x*(1.0-uWall.y);	
								e2 *= (1.0-uWall.x)*(1.0-uWall.y);
								e3 *= uWall.x*uWall.y;
								e4 *= (1.0-uWall.x)*uWall.y;
								FragColor.rgb = (t.gba+t2.gba+e1.gba+e2.gba+e3.gba+e4.gba)/2.0;
								FragColor.rgb *= fract((btr.z + (si.y - deltaY) * ang.z))*0.25+0.75;	
								break;
							case 2:
								uWall.x = (btr.x + (si.z - deltaZ) * ang.x)*0.25;
								uWall.y = (btr.y + (si.z - deltaZ) * ang.y)*0.25;
								if(tr.z==3.0&&texelFetch(map,ivec3(x,y-1,z+1),0).r==0.0){
									FragColor.rgb = blockLight();
									break;
								}
								t  = texelFetch(map,ivec3(x,y+1,z),0);
								t2 = texelFetch(map,ivec3(x,y,z+1),0);
								e1 = texelFetch(map,ivec3(x+1,y+1,z),0);
								e2 = texelFetch(map,ivec3(x-1,y+1,z),0);
								e3 = texelFetch(map,ivec3(x+1,y,z+1),0);
								e4 = texelFetch(map,ivec3(x-1,y,z+1),0);
								t  *= uWall.y;
								t2 *= 1.0-uWall.y;
								e1 *= uWall.x*(uWall.y);
								e2 *= (1.0-uWall.x)*(uWall.y);
								e3 *= uWall.x*(1.0-uWall.y);
								e4 *= (1.0-uWall.x)*(1.0-uWall.y);
								FragColor.rgb = (t.gba+t2.gba+e1.gba+e2.gba+e3.gba+e4.gba)/2.0;
								FragColor.rgb *= fract((btr.y + (si.z - deltaZ) * ang.y))*0.25+0.75;
								break;
							}
						}
						else{
							FragColor.rgb = blockLight();
						}
						lightEffect();
						return;
					}  
					if(si.x < si.y){
			            if(si.x < si.z){
							tr.x += stepX;
							si.x += deltaX;
							side = 0;
			            }
			            else{
							tr.z += stepZ;
							si.z += deltaZ;
							side = 2;
			            }
			        }
			        else if(si.y < si.z){
						tr.y += stepY;
						si.y += deltaY;
						side = 1;
			        }
			        else{
						tr.z += stepZ;
						si.z += deltaZ;
						side = 2;
			        }
                }
                break;
			}
			case 51:{
                vec3 btr = getSubCoords()*4.0;
				vec3 si = getSide(btr);
				vec3 tr = floor(btr);
                while(tr.x >= 0.0 && tr.x <= 4.0-stepX && tr.y >=0.0 && tr.y <= 4.0-stepY && tr.z >= 0.0 && tr.z <= 4.0-stepZ){   
					if(tr.z - tr.x < 1.0){	
						vec4 t,t2,e1,e2,e3,e4;
						vec2 uWall;
						if(tr.z == tr.x){
							switch(side){
							case 0:
								uWall.x = (btr.y + (si.x - deltaX) * ang.y)*0.25;
								uWall.y = (btr.z + (si.x - deltaX) * ang.z)*0.25;
								if(tr.y==0.0&&texelFetch(map,ivec3(x-1,y,z-1),0).r==0.0){
									FragColor.rgb = blockLight();
									break;
								}
								t  = texelFetch(map,ivec3(x-1,y,z),0);
								t2 = texelFetch(map,ivec3(x,y,z+1),0);
								e1 = texelFetch(map,ivec3(x-1,y+1,z),0);
								e2 = texelFetch(map,ivec3(x-1,y-1,z),0);
								e3 = texelFetch(map,ivec3(x,y+1,z+1),0);
								e4 = texelFetch(map,ivec3(x,y-1,z+1),0);
								t  *= 1.0-uWall.y;
								t2 *= uWall.y;
								e1 *= uWall.x*(1.0-uWall.y);
								e2 *= (1.0-uWall.x)*(1.0-uWall.y);
								e3 *= uWall.x*uWall.y;
								e4 *= (1.0-uWall.x)*uWall.y;
								FragColor.rgb = (t.gba+t2.gba+e1.gba+e2.gba+e3.gba+e4.gba)/2.0;
								FragColor.rgb *= fract((btr.z + (si.x - deltaX) * ang.z))*0.25+0.75;
								break;
							case 1:
								FragColor.rgb = blockLight();
								break;
							case 2:
								uWall.x = (btr.y + (si.z - deltaZ) * ang.y)*0.25;
								uWall.y = (btr.x + (si.z - deltaZ) * ang.x)*0.25;
								if(tr.z==3.0&&texelFetch(map,ivec3(x+1,y,z+1),0).r==0.0){
									FragColor.rgb = blockLight();
									break;
								}
								t  = texelFetch(map,ivec3(x-1,y,z),0);
								t2 = texelFetch(map,ivec3(x,y,z+1),0);
								e1 = texelFetch(map,ivec3(x-1,y+1,z),0);
								e2 = texelFetch(map,ivec3(x-1,y-1,z),0);
								e3 = texelFetch(map,ivec3(x,y+1,z+1),0);
								e4 = texelFetch(map,ivec3(x,y-1,z+1),0);
								t  *= 1.0-uWall.y;
								t2 *= uWall.y;
								e1 *= uWall.x*(1.0-uWall.y);
								e2 *= (1.0-uWall.x)*(1.0-uWall.y);
								e3 *= uWall.x*uWall.y;
								e4 *= (1.0-uWall.x)*uWall.y;
								FragColor.rgb = (t.gba+t2.gba+e1.gba+e2.gba+e3.gba+e4.gba)/2.0;
								FragColor.rgb *= 1.0-fract((btr.x + (si.z - deltaZ) * ang.x))*0.25;
								break;
							}
						}
						else{
							FragColor.rgb = blockLight();
						}
						lightEffect();
						return;
					}  
					if(si.x < si.y){
			            if(si.x < si.z){
							tr.x += stepX;
							si.x += deltaX;
							side = 0;
			            }
			            else{
							tr.z += stepZ;
							si.z += deltaZ;
							side = 2;
			            }
			        }
			        else if(si.y < si.z){
						tr.y += stepY;
						si.y += deltaY;
						side = 1;
			        }
			        else{
						tr.z += stepZ;
						si.z += deltaZ;
						side = 2;
			        }
                }
                break;
			}
			case 52:{
               vec3 btr = getSubCoords()*4.0;
				vec3 si = getSide(btr);
				vec3 tr = floor(btr);
                while(tr.x >= 0.0 && tr.x <= 4.0-stepX && tr.y >=0.0 && tr.y <= 4.0-stepY && tr.z >= 0.0 && tr.z <= 4.0-stepZ){   
					if(tr.z - (3.0-tr.x) < 1.0){	
						vec4 t,t2,e1,e2,e3,e4;
						vec2 uWall;
						if(tr.z == 3.0-tr.x){
							switch(side){
							case 0:
								uWall.x = (btr.y + (si.x - deltaX) * ang.y)*0.25;
								uWall.y = (btr.z + (si.x - deltaX) * ang.z)*0.25;
								if(tr.x==0.0&&texelFetch(map,ivec3(x+1,y,z-1),0).r==0.0){
									FragColor.rgb = blockLight();
									break;
								}
								t  = texelFetch(map,ivec3(x+1,y,z),0);
								t2 = texelFetch(map,ivec3(x,y,z+1),0);
								e1 = texelFetch(map,ivec3(x+1,y+1,z),0);
								e2 = texelFetch(map,ivec3(x+1,y-1,z),0);
								e3 = texelFetch(map,ivec3(x,y+1,z+1),0);
								e4 = texelFetch(map,ivec3(x,y-1,z+1),0);
								t  *= 1.0-uWall.y;
								t2 *= uWall.y;
								e1 *= uWall.x*(1.0-uWall.y);	
								e2 *= (1.0-uWall.x)*(1.0-uWall.y);
								e3 *= uWall.x*uWall.y;
								e4 *= (1.0-uWall.x)*uWall.y;
								FragColor.rgb = (t.gba+t2.gba+e1.gba+e2.gba+e3.gba+e4.gba)/2.0;
								FragColor.rgb *= fract((btr.z + (si.x - deltaX) * ang.z))*0.25+0.75;	
								break;
							case 1:
								FragColor.rgb = blockLight();
								break;
							case 2:
								uWall.x = (btr.x + (si.z - deltaZ) * ang.x)*0.25;
								uWall.y = (btr.y + (si.z - deltaZ) * ang.y)*0.25;
								if(tr.z==3.0&&texelFetch(map,ivec3(x-1,y,z+1),0).r==0.0){
									FragColor.rgb = blockLight();
									break;
								}
								t  = texelFetch(map,ivec3(x+1,y,z),0);
								t2 = texelFetch(map,ivec3(x,y,z+1),0);
								e1 = texelFetch(map,ivec3(x+1,y+1,z),0);
								e2 = texelFetch(map,ivec3(x+1,y-1,z),0);
								e3 = texelFetch(map,ivec3(x,y+1,z+1),0);
								e4 = texelFetch(map,ivec3(x,y-1,z+1),0);
								t  *= uWall.y;
								t2 *= 1.0-uWall.y;
								e1 *= uWall.x*(uWall.y);
								e2 *= (1.0-uWall.x)*(uWall.y);
								e3 *= uWall.x*(1.0-uWall.y);
								e4 *= (1.0-uWall.x)*(1.0-uWall.y);
								FragColor.rgb = (t.gba+t2.gba+e1.gba+e2.gba+e3.gba+e4.gba)/2.0;
								FragColor.rgb *= fract((btr.x + (si.z - deltaZ) * ang.x))*0.25+0.75;
								break;
							}
						}
						else{
							FragColor.rgb = blockLight();
						}
						lightEffect();
						return;
					} 
					if(si.x < si.y){
			            if(si.x < si.z){
							tr.x += stepX;
							si.x += deltaX;
							side = 0;
			            }
			            else{
							tr.z += stepZ;
							si.z += deltaZ;
							side = 2;
			            }
			        }
			        else if(si.y < si.z){
						tr.y += stepY;
						si.y += deltaY;
						side = 1;
			        }
			        else{
						tr.z += stepZ;
						si.z += deltaZ;
						side = 2;
			        }
                }
                break;
            }
			case 60:{
				vec3 tra = getSubCoords()*120.0;
				tra += ang * deltaZ * clamp((115.0-tra.z),0.0,120.0) + 0.00001;
				vec3 si = getSide(tra);
				vec3 tr = floor(tra);
				int sid = side;
				side = 2;
              	while(tr.x >= 0.0 && tr.x <= 120.0 && tr.y >=0.0 && tr.y <= 120.0 && tr.z >= 115.0 && tr.z <= 120.0){
			  		if(mod(tr.x,6.0) < 1.0 || mod(tr.y,6.0) < 1.0){	
						wallX = tr.x/120.0;
						wallY = tr.y/120.0;
						vec4 t,e1,e2,e3,e4,c1,c2,c3,c4;
						t  = texelFetch(map,ivec3(x,y,z+1),0);
					    e1 = texelFetch(map,ivec3(x+1,y,z+1),0);
						e2 = texelFetch(map,ivec3(x-1,y,z+1),0);
						e3 = texelFetch(map,ivec3(x,y+1,z+1),0);
						e4 = texelFetch(map,ivec3(x,y-1,z+1),0);
						c1 = texelFetch(map,ivec3(x+1,y+1,z+1),0);
						c2 = texelFetch(map,ivec3(x+1,y-1,z+1),0);
						c3 = texelFetch(map,ivec3(x-1,y+1,z+1),0);
						c4 = texelFetch(map,ivec3(x-1,y-1,z+1),0);
					    e1 *= wallX;
					    e2 *= 1.0-wallX;
					    e3 *= wallY;
					    e4 *= 1.0-wallY;
					    c1 *= clamp(1.0-distance(vec2(1.0,1.0),vec2(wallX,wallY)),0.0,1.0);
					    c2 *= clamp(1.0-distance(vec2(1.0,0.0),vec2(wallX,wallY)),0.0,1.0);
					    c3 *= clamp(1.0-distance(vec2(0.0,1.0),vec2(wallX,wallY)),0.0,1.0);
					    c4 *= clamp(1.0-distance(vec2(0.0,0.0),vec2(wallX,wallY)),0.0,1.0);
						vec3 l = (c1.gba+c2.gba+c3.gba+c4.gba+e1.gba+e2.gba+e3.gba+e4.gba+t.gba)/2.0* (1.0-revertnoise(tr.xy/120.0));
						FragColor.rgb = l * (tr.z-110.0) * 0.1;
						lightEffect();
						return;
					}
					if(si.x < si.y){
			            if(si.x < si.z){
							tr.x += stepX;
							si.x += deltaX;
							sid = 0;
			            }
			            else{
							tr.z += stepZ;
							si.z += deltaZ;
							sid = 2;
			            }
			        }
			        else if(si.y < si.z){
						tr.y += stepY;
						si.y += deltaY;
						sid = 1;
			        }
			        else{
						tr.z += stepZ;
						si.z += deltaZ;
						sid = 2;
			        }
                }
                break;
            }
			case 61:{
				vec2 tr;
				switch(side){
				case 0:
					tr = vec2(wallX+y,wallY+z)*50.0;
					break;
				case 1:
					tr = vec2(wallX+x,wallY+z)*50.0;
					break;
				case 2:
					tr = vec2(wallX+x,wallY+y)*50.0;
					break;
				}
			    vec2 fpos = floor(tr);
			    vec3 col = vec3(0.0,0.0,0.0);
			    float r = 0.0;
			    for(int i = 0;i < 1600;i++){
			        fpos.y-=1.0;
			        col.g += 0.01;
			        float r2 = random(fpos)*16.0;
			        if(int(r2) == 0){
			            FragColor.rgb += col;
			            FragColor.rgb += vec3(0.1,0.43,1.0) * ((tnoise(fpos,5.0)+5.0)/5.0 - (col.r + col.g)/2.0);
			            tr.y = floor(tr.y)+float(i);
			            tr.x = floor(tr.x/3.0)+float(i);
			            FragColor.rgb *= tnoise(tr/30.0,5.0)/30.0+1.0;
						lightEffect();
			            return;
			        }
			        else if(r2 < 3.0){
			            fpos.y-=1.0;
			            fpos.x-=1.0;
			            col.r += 0.01;
			        }
			        else if(r2 < 5.0){
			            fpos.y-=1.0;
			            fpos.x+=1.0;
			            col.r += 0.01;
			        }
				}
				break;
			}
			case 62:{
				vec3 tr = getSubCoords();
				vec3 rp = texelFetch(mapdata,ivec3(x,y,z),0).xyz;
				float s = iSphere(tr-rp,ang,0.5);
				if(s != -1.0){
					tr += s * ang;
					if(tr.x >= 0.0 && tr.x <= 1.0 && tr.y >=0.0 && tr.y <= 1.0 && tr.z >= 1.0 && tr.z <= 1.0){
						ang = reflect(ang,normalize(tr-rp));
						deltaX = abs(1 / ang.x);
						deltaY = abs(1 / ang.y);
						deltaZ = abs(1 / ang.z);
						x = x+tr.x;
						y = y+tr.y;
						z = z+tr.z;
						if(ang.x < 0){
							stepX = -1;
							sideX = fract(x) * deltaX;
						}
						else{
							stepX = 1;
							sideX = (int(x) + 1.0 - x) * deltaX;
						}
						if(ang.y < 0){
							stepY = -1;
							sideY = fract(y) * deltaY;
						}
						else{
							stepY = 1;
							sideY = (int(y) + 1.0 - y) * deltaY;
						}
						if(ang.z < 0){
							stepZ = -1;
							sideZ = fract(z) * deltaZ;
						}
						else{
							stepZ = 1;
							sideZ = (int(z) + 1.0 - z) * deltaZ;
						}
						pos.x = x;
						pos.y = y;
						pos.z = z;
						break;
					}
				}
				break;
			}
			case 63:{
				break;
				}
			case 64:{
				break;
				}
			case 65:{	
				break;
				}
			case 66:{
				break;
				}
			case 67:{
				vec4 nc = vec4(ivec4(texelFetch(mapdata,ivec3(x,y,z),0)*255.0));
				vec3 tr;
				switch(int(nc.w)){
				case 1:
					x = nc.x;
					y = nc.y;
					z = nc.z;
					float tang = cos(0.5*3.14159) * ang.x - sin(0.5*3.14159) * ang.y;
					ang.y = sin(0.5*3.14159) * ang.x + cos(0.5*3.14159) * ang.y;
					ang.x = tang;
					reinitializeRay();
					break;
				default:
					x = nc.x;
					y = nc.y;
					z = nc.z;
				}
				break;
			}
			default:
				break;
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
	lightEffect();
}




