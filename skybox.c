#include <math.h>

#include "main.h"
#include "tmgl.h"
#include "vec2.h"
#include <GL/gl.h>

#define GL_TEXTURE4 0x84C4

RGB *skyboxTexture;
STAR *star;
u8 starC;

f32 rndS(VEC2 seed) {
	union p {
		f32 f;
		i32 u;
	}r;
	r.u = hash(hash((u32)seed.x)^(u32)seed.y);
	r.u &= 0x007fffff;
	r.u |= 0x3f800000;
	return r.f;
}

f32 mix(f32 p,f32 p2,f32 p3){
	return p * (1.0f - p3) + p2 * p3;
}

f32 tnoise(VEC2 pos,f32 detail){
    f32 intens = 0.0f;
    for(f32 i = 0.1f;i < detail;i*=1.34f){
        f32 center = 1.0f - fmaxf(fabsf(fract(pos.x * i) - 0.5f),fabsf(fract(pos.y * i) - 0.5f)) * 2.0f;
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
				for(u32 i3 = 0;i3 < starC;i3++){
					skyboxTexture[i*skyboxSz+i2].r = fminf(850.0f / powf(fminf(VEC2dist((VEC2){i,i2},star[i3].skyPos),VEC2dist((VEC2){i,i2},VEC2addVEC2R(star[i3].skyPos,(VEC2){0,skyboxSz}))),0.2f)*star[i3].col.r/512.0f,255.0f),skyboxTexture[i*skyboxSz+i2].r;
					skyboxTexture[i*skyboxSz+i2].g = fminf(850.0f / powf(fminf(VEC2dist( (VEC2){i,i2},star[i3].skyPos),VEC2dist((VEC2){i,i2},VEC2addVEC2R(star[i3].skyPos,(VEC2){0,skyboxSz}))),0.2f)*star[i3].col.g/512.0f,255.0f),skyboxTexture[i*skyboxSz+i2].g;
					skyboxTexture[i*skyboxSz+i2].b = fminf(850.0f / powf(fminf(VEC2dist((VEC2){i,i2},star[i3].skyPos),VEC2dist((VEC2){i,i2},VEC2addVEC2R(star[i3].skyPos,(VEC2){0,skyboxSz}))),0.2f)*star[i3].col.b/512.0f,255.0f),skyboxTexture[i*skyboxSz+i2].b;
				}
				skyboxTexture[i*skyboxSz+i2].r = mix(max(min(star[0].skyCol.r*2-min(max(powf(((f32)i-skyboxSz/2+90)/skyboxSz/2,0.5f)*skyboxSz/2,0),255),255),0),skyboxTexture[i*skyboxSz+i2].r,skyboxTexture[i*skyboxSz+i2].r/255.0f);
				skyboxTexture[i*skyboxSz+i2].g = mix(max(min(star[0].skyCol.g*2-min(max(powf(((f32)i-skyboxSz/2+90)/skyboxSz/2,0.5f)*skyboxSz/2,0),255),255),0),skyboxTexture[i*skyboxSz+i2].g,skyboxTexture[i*skyboxSz+i2].g/255.0f);
				skyboxTexture[i*skyboxSz+i2].b = mix(max(min(star[0].skyCol.b*2-min(max(powf(((f32)i-skyboxSz/2+90)/skyboxSz/2,0.5f)*skyboxSz/2,0),255),255),0),skyboxTexture[i*skyboxSz+i2].b,skyboxTexture[i*skyboxSz+i2].b/255.0f);
			}
		}
		for(u32 i = skyboxSz/2 - 90;i < skyboxSz;i++){
			for(u32 i2 = 0;i2 < skyboxSz;i2++){
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
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D,skyboxText);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,skyboxSz,skyboxSz,0,GL_RGB,GL_UNSIGNED_BYTE,skyboxTexture);
	glGenerateMipmap(GL_TEXTURE_2D);
	printf("generated skybox\n");
	memset(skyboxTexture,0,skyboxSz*skyboxSz*sizeof(RGB));
}