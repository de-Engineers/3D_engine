#include "main.h"
#include "vec2.h"
#include "vec3.h"
#include "ivec2.h"
#include "ray.h"

f32 brightness;

#define HDR_RES 60

void HDR(){
	for(;;){
		if(GetKeyState(VK_LBUTTON)&0x80){
			for(u32 i = 0;i < sliderC;i++){
				if(mousePos.x > slider[i].pos.x - 0.266666667f && mousePos.x < slider[i].pos.x + 0.26666667f
					&& mousePos.y > slider[i].pos.y - 0.015f && mousePos.y < slider[i].pos.y + 0.015f){
					sliderId = slider[i].id;
					sliderPos = (mousePos.x-slider[i].pos.x+0.266666667f)*480.0f;
				}
			}
			if(sliderId!=-1){
				sliders[sliderId](sliderPos);
				sliderId = -1;
				glMes[glMesC].id = 13;
				glMesC++;
			}
		}
		u32 hits = 0;
		f32 polBrightness = 0;
		for(u32 i = 0;i < HDR_RES;i++){
			for(u32 i2 = 0;i2 < HDR_RES;i2++){
				VEC2 px = {player->fov.x*(((f32)i2/HDR_RES*2.0f)-1.0f),player->fov.y*(((f32)i/HDR_RES*2.0f)-1.0f)};
				VEC3 ang = screenUVto3D(px);
				RAY3D ray = ray3dCreate(player->pos,ang);
				i32 lmapLoc = getLmapLocation(&ray);
				if(lmapLoc!=-1){
					polBrightness += max(lmap[lmapLoc].r,max(lmap[lmapLoc].g,lmap[lmapLoc].b));
					hits++;
				}
			}
		}
		if(hits){
			polBrightness /= hits * 1024.0f / (HDR_RES * HDR_RES);
			brightness = (brightness * 199.0f + polBrightness) / 200.0f;
			brightness = (brightness * 199.0f + 512.0f) / 200.0f;
		}
		Sleep(15);
	}
}