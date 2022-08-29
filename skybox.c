#include <math.h>

#include "main.h"
#include "tmgl.h"
#include "vec2.h"
#include <GL/gl.h>

#define GL_TEXTURE4 0x84C4

RGB *skyboxTexture;
STAR *star;
u8 starC;

inline i32 hash(i32 x) {
	x += (x << 10);
	x ^= (x >> 6);
	x += (x << 3);
	x ^= (x >> 11);
	x += (x << 15);
	return x;
}

inline f32 rndS(VEC2 seed) {
	union p {
		float f;
		i32 u;
	}r;
	r.u = hash(hash((u32)seed.x)^(u32)seed.y);
	r.u &= 0x007fffff;
	r.u |= 0x3f800000;
	return r.f;
}

inline f32 mix(f32 p,f32 p2,f32 p3){
	return p * (1.0f - p3) + p2 * p3;
}

float tnoise(VEC2 pos,float detail){
    float intens = 0.0f;
    for(float i = 0.1f;i < detail;i*=1.34f){
        float center = 1.0f - fmaxf(fabsf(fract(pos.x * i) - 0.5f),fabsf(fract(pos.y * i) - 0.5f)) * 2.0f;
        intens += mix(0.5f,rndS(VEC2floorR(VEC2mulR(pos,i)))-1.0f,center) - 0.5f;
		intens *= 1.1f;
    }
    return intens;
}

void generateSkyBox(){
	VEC2 cloudRandomOffset = {(rnd()-1.0f)*999.0f,(rnd()-1.0f)*999.0f};
	VEC2 cloudRandomOffset2 = {(rnd()-1.0f)*999.0f,(rnd()-1.0f)*999.0f};
	for(u32 i = 0;i < starC;i++){
		star[i].skyPos = (VEC2){skyboxSz-(star[i].pos.z*skyboxSz/2+skyboxSz/2),(atan2f(star[i].pos.x,star[i].pos.y))/PI_2*skyboxSz};
	}
	if(!starC){
		for(u32 i = 0;i < 500;i++){
			u32 r = abs(irnd()) % (skyboxSz*skyboxSz);
			u8 luminance = abs(irnd()) & 0xff;
			VEC3 col = {rnd()/4.0f+0.5f,rnd()/4.0f+0.5f,rnd()/4.0f+0.5f};
			skyboxTexture[r].r = luminance * col.x * col.x;
			skyboxTexture[r].g = luminance * col.y * col.y;
			skyboxTexture[r].b = luminance * col.z * col.z;
		}
	}
	else{
		for(u32 i = 1;i < skyboxSz;i++){
			for(u32 i2 = 0;i2 < skyboxSz;i2++){
				skyboxTexture[i*skyboxSz+i2].r = max(min(star[0].highCol.r-star[0].lowCol.r+255-min(max(powf(((f32)i-skyboxSz/2+90)/skyboxSz/2,0.5f)*skyboxSz/2,0),255),255),0);
				skyboxTexture[i*skyboxSz+i2].g = max(min(star[0].highCol.g-star[0].lowCol.g+255-min(max(powf(((f32)i-skyboxSz/2+90)/skyboxSz/2,0.5f)*skyboxSz/2,0),255),255),0);
				skyboxTexture[i*skyboxSz+i2].b = max(min(star[0].highCol.b-star[0].lowCol.b+255-min(max(powf(((f32)i-skyboxSz/2+90)/skyboxSz/2,0.5f)*skyboxSz/2,0),255),255),0);
				for(u32 i3 = 0;i3 < starC;i3++){
					skyboxTexture[i*skyboxSz+i2].r += fminf(1680.0f / powf(fminf(VEC2dist((VEC2){i,i2},star[i3].skyPos),VEC2dist((VEC2){i,i2},VEC2addVEC2R(star[i3].skyPos,(VEC2){0,skyboxSz}))),0.55f)*star[i3].col.r/512.0f,255.0f-skyboxTexture[i*skyboxSz+i2].r);
					skyboxTexture[i*skyboxSz+i2].g += fminf(1680.0f / powf(fminf(VEC2dist( (VEC2){i,i2},star[i3].skyPos),VEC2dist((VEC2){i,i2},VEC2addVEC2R(star[i3].skyPos,(VEC2){0,skyboxSz}))),0.55f)*star[i3].col.g/512.0f,255.0f-skyboxTexture[i*skyboxSz+i2].g);
					skyboxTexture[i*skyboxSz+i2].b += fminf(1680.0f / powf(fminf(VEC2dist((VEC2){i,i2},star[i3].skyPos),VEC2dist((VEC2){i,i2},VEC2addVEC2R(star[i3].skyPos,(VEC2){0,skyboxSz}))),0.55f)*star[i3].col.b/512.0f,255.0f-skyboxTexture[i*skyboxSz+i2].b);
				}
				f32 cloud = (fmaxf(tnoise(VEC2addVEC2R(VEC2divR((VEC2){i,i2},1000.0f),cloudRandomOffset),200.0f)*(1.0f-fabsf((f32)i2-skyboxSz/2)/skyboxSz*2.0f),0.0f));
				skyboxTexture[i*skyboxSz+i2].r = max(min(cloud*10.0f + skyboxTexture[i*skyboxSz+i2].r,255),0);
				skyboxTexture[i*skyboxSz+i2].g = max(min(cloud*10.0f + skyboxTexture[i*skyboxSz+i2].g,255),0);
				skyboxTexture[i*skyboxSz+i2].b = max(min(cloud*10.0f + skyboxTexture[i*skyboxSz+i2].b,255),0);
				cloud = (fmaxf(tnoise(VEC2addVEC2R(VEC2divR((VEC2){i,i2},1000.0f),cloudRandomOffset2),200.0f)*(1.0f-fabsf((f32)i2-skyboxSz/2)/skyboxSz*2.0f),0.0f));
				skyboxTexture[i*skyboxSz+i2-skyboxSz/2].r = max(min(cloud*10.0f + skyboxTexture[i*skyboxSz+i2-skyboxSz/2].r,255),0);
				skyboxTexture[i*skyboxSz+i2-skyboxSz/2].g = max(min(cloud*10.0f + skyboxTexture[i*skyboxSz+i2-skyboxSz/2].g,255),0);
				skyboxTexture[i*skyboxSz+i2-skyboxSz/2].b = max(min(cloud*10.0f + skyboxTexture[i*skyboxSz+i2-skyboxSz/2].b,255),0);
			}
		}
	}
	cloudRandomOffset.x = (rnd()-1.0f)*999.0f;
	cloudRandomOffset.y = (rnd()-1.0f)*999.0f;
	/*
	for(u32 i = 0;i < skyboxSz/2;i++){
		for(u32 i2 = 0;i2 < skyboxSz;i2++){
			skyboxTexture[i*skyboxSz+i2].r = 100;
			skyboxTexture[i*skyboxSz+i2].g = 66;
			skyboxTexture[i*skyboxSz+i2].b = 46;
		}
	}
	f32 seed = rnd()*200.0f;
	for(u32 i = skyboxSz/2-40;i < skyboxSz/2+40;i++){
		for(u32 i2 = 0;i2 < skyboxSz;i2++){
			f32 n = 0.0f;
			for(f32 i3 = 3.0f;i3 < 30.0f;i3*=1.34f){
				n += cosf((f32)i2/i3+seed);
			}
			if(n-((f32)i-skyboxSz/2)/5.0f > -4.0f){
				skyboxTexture[i*skyboxSz+i2].r = 150.0f*i/skyboxSz*2.0f;
				skyboxTexture[i*skyboxSz+i2].g = 100.0f*i/skyboxSz*2.0f;
				skyboxTexture[i*skyboxSz+i2].b = 70.0f*i/skyboxSz*2.0f;
			}
		}
	}
	seed = rnd()*200.0f;
	for(u32 i = skyboxSz/2-120;i < skyboxSz/2+120;i++){
		for(u32 i2 = 0;i2 < skyboxSz;i2++){
			f32 n = 0.0f;
			for(f32 i3 = 3.0f;i3 < 30.0f;i3*=1.34f){
				n += cosf((f32)i2/i3/3.0f+seed) * 3.0;
			}
			if(n-((f32)i-skyboxSz/2)/5.0f > -4.0f){
				skyboxTexture[i*skyboxSz+i2].r = 120.0f*i/skyboxSz*2.0f;
				skyboxTexture[i*skyboxSz+i2].g = 80.0f*i/skyboxSz*2.0f;
				skyboxTexture[i*skyboxSz+i2].b = 55.0f*i/skyboxSz*2.0f;
			}
		}
	}
	seed = rnd()*200.0f;
	for(u32 i = skyboxSz/2-360;i < skyboxSz/2+360;i++){
		for(u32 i2 = 0;i2 < skyboxSz;i2++){
			f32 n = 0.0f;
			for(f32 i3 = 3.0f;i3 < 30.0f;i3*=1.34f){
				n += cosf((f32)i2/i3/9.0+seed) * 9.0f;
			}
			if(n-((f32)i-skyboxSz/2)/5.0f > -4.0f){
				skyboxTexture[i*skyboxSz+i2].r = 100.0f*i/skyboxSz*2.0f*fminf(fabsf((f32)i2-200),fabsf((f32)i2-200+skyboxSz))/350.0f;
				skyboxTexture[i*skyboxSz+i2].g = 66.0f*i/skyboxSz*2.0f*fminf(fabsf((f32)i2-200),fabsf((f32)i2-200+skyboxSz))/350.0f;
				skyboxTexture[i*skyboxSz+i2].b = 46.0f*i/skyboxSz*2.0f*fminf(fabsf((f32)i2-200),fabsf((f32)i2-200+skyboxSz))/350.0f;
			}
		}
	}*/
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D,skyboxText);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,skyboxSz,skyboxSz,0,GL_RGB,GL_UNSIGNED_BYTE,skyboxTexture);
	glGenerateMipmap(GL_TEXTURE_2D);
	printf("generated skybox\n");
	memset(skyboxTexture,0,skyboxSz*skyboxSz*sizeof(RGB));
}