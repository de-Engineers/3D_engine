#include <math.h>

#include "main.h"
#include "tools.h"
#include "ray.h"

CVEC3 selarea;

u32 editBlockSel;

i16 pixelCrdB = -1;

RGB *textureBuf;

void setBlock(u32 block){
	lpmap[block].p1 = 0;
	lpmap[block].p2 = 0;
	lpmap[block].p3 = 0;
								
	map[block].id = blockSel;
	map[block].r  = colorSel.r;
	map[block].g  = colorSel.g;
	map[block].b  = colorSel.b;

	metadt[block].r = metadtSel.r;
	metadt[block].g = metadtSel.g;
	metadt[block].b = metadtSel.b;
	metadt[block].id = metadtSel.a;

	metadt2[block].r = metadt2Sel.r;
	metadt2[block].g = metadt2Sel.g;
	metadt2[block].b = metadt2Sel.b;
	metadt2[block].id = metadt2Sel.a;

	metadt3[block].r = metadt3Sel.r;
	metadt3[block].g = metadt3Sel.g;
	metadt3[block].b = metadt3Sel.b;
	metadt3[block].id = metadt3Sel.a;

	metadt4[block].r  = metadt4Sel.r;
	metadt4[block].g  = metadt4Sel.g;
	metadt4[block].b  = metadt4Sel.b;
	metadt4[block].id = metadt4Sel.a;

	metadt5[block].r  = metadt5Sel.r;
	metadt5[block].g  = metadt5Sel.g;
	metadt5[block].b  = metadt5Sel.b;
	metadt5[block].id = metadt5Sel.a;

	metadt6[block].r  = metadt6Sel.r;
	metadt6[block].g  = metadt6Sel.g;
	metadt6[block].b  = metadt6Sel.b;
	metadt6[block].id = metadt6Sel.a;
}

void tools(){
	switch(toolSel){
	case 0:{
		if(GetKeyState(VK_LCONTROL)&0x80){
			RAY3D ray = ray3dCreate(player->pos,(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
			while(ray.roundPos.x>=0&&ray.roundPos.x<properties->lvlSz&&ray.roundPos.y>=0&&ray.roundPos.y<properties->lvlSz&&ray.roundPos.z>=0&&ray.roundPos.z<properties->lvlSz){
				u32 block = crds2map(ray.roundPos.x,ray.roundPos.y,ray.roundPos.z);
				if(map[block].id!=BLOCK_AIR){
					ShowCursor(1);
					SetCursorPos(properties->xres/2,properties->yres/2);
					menuSel = 4;
					blockSel = map[block].id;
					colorSel.r = map[block].r;
					colorSel.g = map[block].g;
					colorSel.b = map[block].b;
					metadtSel.r = metadt[block].r;
					metadtSel.g = metadt[block].g;
					metadtSel.b = metadt[block].b;
					metadtSel.a = metadt[block].id;
					metadt2Sel.r = metadt2[block].r;
					metadt2Sel.g = metadt2[block].g;
					metadt2Sel.b = metadt2[block].b;
					metadt2Sel.a = metadt2[block].id;
					metadt3Sel.r = metadt3[block].r;
					metadt3Sel.g = metadt3[block].g;
					metadt3Sel.b = metadt3[block].b;
					metadt3Sel.a = metadt3[block].id;
					metadt4Sel.r = metadt4[block].r;
					metadt4Sel.g = metadt4[block].g;
					metadt4Sel.b = metadt4[block].b;
					metadt4Sel.a = metadt4[block].id;
					metadt5Sel.r = metadt5[block].r;
					metadt5Sel.g = metadt5[block].g;
					metadt5Sel.b = metadt5[block].b;
					metadt5Sel.a = metadt5[block].id;
					metadt6Sel.r = metadt6[block].r;
					metadt6Sel.g = metadt6[block].g;
					metadt6Sel.b = metadt6[block].b;
					metadt6Sel.a = metadt6[block].id;
					editBlockSel = block;
					sliderCreate((VEC2){0.59f,-0.875f},0);
					sliderCreate((VEC2){0.59f,-0.825f},1);
					sliderCreate((VEC2){0.59f,-0.775f},2);

					sliderCreate((VEC2){0.59f,-0.475f},3);
					sliderCreate((VEC2){0.59f,-0.425f},4);
					sliderCreate((VEC2){0.59f,-0.375f},5);
					sliderCreate((VEC2){0.59f,-0.325f},6);

					sliderCreate((VEC2){0.59f,-0.175f},7);
					sliderCreate((VEC2){0.59f,-0.125f},8);
					sliderCreate((VEC2){0.59f,-0.075f},9);
					sliderCreate((VEC2){0.59f,-0.025f},10);

					sliderCreate((VEC2){0.59f, 0.125f},11);
					sliderCreate((VEC2){0.59f, 0.175f},12);
					sliderCreate((VEC2){0.59f, 0.225f},13);
					sliderCreate((VEC2){0.59f, 0.275f},14);
					break;
				}
				ray3dItterate(&ray);
			}
		}
		else{
			RAY3D ray = ray3dCreate(player->pos,(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
			while(ray.roundPos.x>=0&&ray.roundPos.x<properties->lvlSz&&ray.roundPos.y>=0&&ray.roundPos.y<properties->lvlSz&&ray.roundPos.z>=0&&ray.roundPos.z<properties->lvlSz){
				int block = crds2map(ray.roundPos.x,ray.roundPos.y,ray.roundPos.z);
				if(map[block].id!=BLOCK_AIR){
					switch(ray.hitSide){
					case 0:
						if(ray.dir.x < 0.0f){
							ray.pos.x+=1.0f;
							setBlock(block + 1);
						}
						else{
							ray.pos.x-=1.0f;
							setBlock(block - 1);
						}
						break;
					case 1:
						if(ray.dir.y < 0.0f){
							ray.pos.y+=1.0f;
							setBlock(block + properties->lvlSz);
						}
						else{
							ray.pos.y-=1.0f;
							setBlock(block - properties->lvlSz);
						}
						break;
					case 2:
						if(ray.dir.z < 0.0f){
							ray.pos.z+=1.0f;
							setBlock(block + properties->lvlSz * properties->lvlSz);
						}
						else{
							ray.pos.z-=1.0f;
							setBlock(block - properties->lvlSz * properties->lvlSz);
						}
						break;
					}
					glMes[glMesC].id = 3;
					glMesC++;
					break;
				}
				ray3dItterate(&ray);
			}
		}
		break;
	}
	case 1:{
		RAY3D ray = ray3dCreate(player->pos,(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
		while(ray.roundPos.x>=0&&ray.roundPos.x<properties->lvlSz&&ray.roundPos.y>=0&&ray.roundPos.y<properties->lvlSz&&ray.roundPos.z>=0&&ray.roundPos.z<properties->lvlSz){
			int block = crds2map(ray.roundPos.x,ray.roundPos.y,ray.roundPos.z);
			if(map[block].id!=BLOCK_AIR){
				if(!selarea.x && !selarea.y && !selarea.z){
					selarea.x = ray.roundPos.x;
					selarea.y = ray.roundPos.y;
					selarea.z = ray.roundPos.z;
				}
				else{
					u8 x = ray.roundPos.x;
					u8 y = ray.roundPos.y;
					u8 z = ray.roundPos.z;
					if(selarea.x > ray.roundPos.x){
						selarea.x ^= x;
						x ^= selarea.x;
						selarea.x ^= x;
					}
					if(selarea.y > ray.roundPos.y){
						selarea.y ^= y;
						y ^= selarea.y;
						selarea.y ^= y;
					}
					if(selarea.z > ray.roundPos.z){
						selarea.z ^= z;
						z ^= selarea.z;
						selarea.z ^= z;
					}
					for(int i = selarea.x;i <= x;i++){
						for(int i2 = selarea.y;i2 <= y;i2++){
							for(int i3 = selarea.z;i3 <= z;i3++){
								setBlock(i + i2 * properties->lvlSz + i3 * properties->lvlSz * properties->lvlSz);
							}
						}
					}
					selarea.x = 0;
					selarea.y = 0;
					selarea.z = 0;
					glMes[glMesC].id = 3;
					glMesC++;
				}
				break;
			}
			ray3dItterate(&ray);
		}
		break;
	}
	case 2:{
		RAY3D ray = ray3dCreate(player->pos,(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
		while(ray.roundPos.x>=0&&ray.roundPos.x<properties->lvlSz&&ray.roundPos.y>=0&&ray.roundPos.y<properties->lvlSz&&ray.roundPos.z>=0&&ray.roundPos.z<properties->lvlSz){
			int block = crds2map(ray.roundPos.x,ray.roundPos.y,ray.roundPos.z);
			if(map[block].id!=BLOCK_AIR){
				if(!selarea.x && !selarea.y && !selarea.z){
					selarea.x = ray.roundPos.x;
					selarea.y = ray.roundPos.y;
					selarea.z = ray.roundPos.z;
				}
				else{
					u8 x = ray.roundPos.x;
					u8 y = ray.roundPos.y;
					u8 z = ray.roundPos.z;
					if(selarea.x > ray.roundPos.x){
						selarea.x ^= x;
						x ^= selarea.x;
						selarea.x ^= x;
					}
					if(selarea.y > ray.roundPos.y){
						selarea.y ^= y;
						y ^= selarea.y;
						selarea.y ^= y;
					}
					if(selarea.z > ray.roundPos.z){
						selarea.z ^= z;
						z ^= selarea.z;
						selarea.z ^= z;
					}
					for(int i = selarea.x;i <= x;i++){
						for(int i2 = selarea.y;i2 <= y;i2++){
							for(int i3 = selarea.z;i3 <= z;i3++){
								int block = (i + i2 * properties->lvlSz + i3 * properties->lvlSz * properties->lvlSz);
								if(rnd() > 1.0f+(f32)metadtSel.r/255.0f){
									setBlock(block);
								}
								else{
									lpmap[block].p1 = 1;
									lpmap[block].p2 = 1;
									lpmap[block].p3 = 1;
								
									map[block].id = 1;
									map[block].r  = 1;
									map[block].g  = 1;
									map[block].b  = 1;
								}
							}
						}
					}
					selarea.x = 0;
					selarea.y = 0;
					selarea.z = 0;
					glMes[glMesC].id = 3;
					glMesC++;
				}
				break;
			}
			ray3dItterate(&ray);
		}
		break;
		break;
	}
	case 3:{
		RAY3D ray = ray3dCreate(player->pos,(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
		while(ray.roundPos.x>=0&&ray.roundPos.x<properties->lvlSz&&ray.roundPos.y>=0&&ray.roundPos.y<properties->lvlSz&&ray.roundPos.z>=0&&ray.roundPos.z<properties->lvlSz){
			int block = crds2map(ray.roundPos.x,ray.roundPos.y,ray.roundPos.z);
			if(map[block].id!=BLOCK_AIR){
				map[block].r = colorSel.r;
				map[block].g = colorSel.g;
				map[block].b = colorSel.b;
				glMes[glMesC].id = 3;
				glMesC++;
				break;
			}
			ray3dItterate(&ray);
		}
		break;	
	}
	case 4:{
		RAY3D ray = ray3dCreate(player->pos,(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
		while(ray.roundPos.x>=0&&ray.roundPos.x<properties->lvlSz&&ray.roundPos.y>=0&&ray.roundPos.y<properties->lvlSz&&ray.roundPos.z>=0&&ray.roundPos.z<properties->lvlSz){
			int block = crds2map(ray.roundPos.x,ray.roundPos.y,ray.roundPos.z);
			if(map[block].id!=BLOCK_AIR){
				if(settings & SETTINGS_SUBBLOCK){
					metadt4[block].r = metadt4Sel.r;
					metadt4[block].g = metadt4Sel.g;
					metadt4[block].b = metadt4Sel.b;
					metadt4[block].id= metadt4Sel.a;
				}
				else{
					metadt[block].r = metadtSel.r;
					metadt[block].g = metadtSel.g;
					metadt[block].b = metadtSel.b;
					metadt[block].id= metadtSel.a;
				}
				glMes[glMesC].id = 3;
				glMesC++;
				break;
			}
			ray3dItterate(&ray);
		}
		break;
	}
	case 5:{
		RAY3D ray = ray3dCreate(player->pos,(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
		while(ray.roundPos.x>=0&&ray.roundPos.x<properties->lvlSz&&ray.roundPos.y>=0&&ray.roundPos.y<properties->lvlSz&&ray.roundPos.z>=0&&ray.roundPos.z<properties->lvlSz){
			int block = crds2map(ray.roundPos.x,ray.roundPos.y,ray.roundPos.z);
			if(map[block].id!=BLOCK_AIR){
				if(settings & SETTINGS_SUBBLOCK){
					metadt5[block].r = metadt5Sel.r;
					metadt5[block].g = metadt5Sel.g;
					metadt5[block].b = metadt5Sel.b;
					metadt5[block].id= metadt5Sel.a;
				}
				else{
					metadt2[block].r = metadt2Sel.r;
					metadt2[block].g = metadt2Sel.g;
					metadt2[block].b = metadt2Sel.b;
					metadt2[block].id= metadt2Sel.a;
				}
				glMes[glMesC].id = 3;
				glMesC++;
				break;
			}
			ray3dItterate(&ray);
		}
		break;
		}
	case 6:{
		RAY3D ray = ray3dCreate(player->pos,(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
		i32 l = getLmapLocation(&ray);
		if(l!=-1){
			l /= properties->lmapSz*properties->lmapSz;
			l *= properties->lmapSz*properties->lmapSz;
			for(u32 i = 0;i < properties->lmapSz*properties->lmapSz;i++){
				lmap[l+i].r = (f32)bmap[l+i].r * textureBuf[i].r / 255.0f;
				lmap[l+i].g = (f32)bmap[l+i].g * textureBuf[i].g / 255.0f;
				lmap[l+i].b = (f32)bmap[l+i].b * textureBuf[i].b / 255.0f;
			}
			glMes[glMesC].data1 = l;
			glMes[glMesC].id = 7;
			glMesC++;
		}
		}
		break;
	case 8:{
		RAY3D ray = ray3dCreate(player->pos,(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
		i32 l = getLmapLocation(&ray);
		if(l!=-1){
			OpenClipboard(window);
			HANDLE h = GetClipboardData(CF_DIB);
			u8 *data = GlobalLock(h);
			BITMAPINFO bmp;
			memcpy(&bmp,data,sizeof(BITMAPINFO));
			RGBA *clipTexture = HeapAlloc(GetProcessHeap(),8,bmp.bmiHeader.biWidth*bmp.bmiHeader.biHeight*sizeof(RGBA));
			memcpy(clipTexture,data+sizeof(BITMAPINFO),bmp.bmiHeader.biWidth*bmp.bmiHeader.biHeight*sizeof(RGBA));
			GlobalUnlock(h);
			CloseHandle(h);
			CloseClipboard();
			l /= properties->lmapSz*properties->lmapSz;
			l *= properties->lmapSz*properties->lmapSz;
			for(u32 i = 0;i < bmp.bmiHeader.biHeight/properties->lmapSz;i++){
				for(u32 i2 = 0;i2 < bmp.bmiHeader.biWidth/properties->lmapSz;i2++){
				switch(ray.hitSide){
				case 0:{
					u32 block = crds2map(ray.roundPos.x,ray.roundPos.y+i,ray.roundPos.z+i2);
					if(ray.dir.x>0.0f){
						for(u32 i3 = 0;i3 < properties->lmapSz;i3++){
							for(u32 i4 = 0;i4 < properties->lmapSz;i4++){
								switch(map[block].id){
								case BLOCK_SOLID:
								case BLOCK_REFLECTIVE:
									lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].r = (f32)bmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].r * clipTexture[i3*bmp.bmiHeader.biHeight+i4+i*properties->lmapSz+i2*properties->lmapSz*bmp.bmiHeader.biWidth].b / 255.0f;
									lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].g = (f32)bmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].g * clipTexture[i3*bmp.bmiHeader.biHeight+i4+i*properties->lmapSz+i2*properties->lmapSz*bmp.bmiHeader.biWidth].g / 255.0f;
									lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].b = (f32)bmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].b * clipTexture[i3*bmp.bmiHeader.biHeight+i4+i*properties->lmapSz+i2*properties->lmapSz*bmp.bmiHeader.biWidth].r / 255.0f;
									break;
								}
							}
						}
						glMes[glMesC].data1 = lpmap[block].p1*properties->lmapSz*properties->lmapSz;
					}
					else{
						for(u32 i3 = 0;i3 < properties->lmapSz;i3++){
							for(u32 i4 = 0;i4 < properties->lmapSz;i4++){
								switch(map[block].id){
								case BLOCK_SOLID:
								case BLOCK_REFLECTIVE:
									lmap[lpmap[block].p2*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].r = (f32)bmap[lpmap[block].p2*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].r * clipTexture[i3*bmp.bmiHeader.biHeight+i4+i*properties->lmapSz+i2*properties->lmapSz*bmp.bmiHeader.biWidth].b / 255.0f;
									lmap[lpmap[block].p2*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].g = (f32)bmap[lpmap[block].p2*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].g * clipTexture[i3*bmp.bmiHeader.biHeight+i4+i*properties->lmapSz+i2*properties->lmapSz*bmp.bmiHeader.biWidth].g / 255.0f;
									lmap[lpmap[block].p2*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].b = (f32)bmap[lpmap[block].p2*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].b * clipTexture[i3*bmp.bmiHeader.biHeight+i4+i*properties->lmapSz+i2*properties->lmapSz*bmp.bmiHeader.biWidth].r / 255.0f;
									break;
								}
							}
						}
						glMes[glMesC].data1 = lpmap[block].p2*properties->lmapSz*properties->lmapSz;
					}
					break;
				}
				case 1:{
					u32 block = crds2map(ray.roundPos.x+i,ray.roundPos.y,ray.roundPos.z+i2);
					if(ray.dir.y>0.0f){
						for(u32 i3 = 0;i3 < properties->lmapSz;i3++){
							for(u32 i4 = 0;i4 < properties->lmapSz;i4++){
								switch(map[block].id){
								case BLOCK_SOLID:
								case BLOCK_REFLECTIVE:
									lmap[lpmap[block].p3*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].r = (f32)bmap[lpmap[block].p3*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].r * clipTexture[i3*bmp.bmiHeader.biHeight+i4+i*properties->lmapSz+i2*properties->lmapSz*bmp.bmiHeader.biWidth].b / 255.0f;
									lmap[lpmap[block].p3*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].g = (f32)bmap[lpmap[block].p3*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].g * clipTexture[i3*bmp.bmiHeader.biHeight+i4+i*properties->lmapSz+i2*properties->lmapSz*bmp.bmiHeader.biWidth].g / 255.0f;
									lmap[lpmap[block].p3*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].b = (f32)bmap[lpmap[block].p3*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].b * clipTexture[i3*bmp.bmiHeader.biHeight+i4+i*properties->lmapSz+i2*properties->lmapSz*bmp.bmiHeader.biWidth].r / 255.0f;
									break;
								}
							}
						}
						glMes[glMesC].data1 = lpmap[block].p3*properties->lmapSz*properties->lmapSz;
					}
					else{
						for(u32 i3 = 0;i3 < properties->lmapSz;i3++){
							for(u32 i4 = 0;i4 < properties->lmapSz;i4++){
								switch(map[block].id){
								case BLOCK_SOLID:
								case BLOCK_REFLECTIVE:
									lmap[lpmap[block].p4*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].r = (f32)bmap[lpmap[block].p4*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].r * clipTexture[i3*bmp.bmiHeader.biHeight+i4+i*properties->lmapSz+i2*properties->lmapSz*bmp.bmiHeader.biWidth].b / 255.0f;
									lmap[lpmap[block].p4*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].g = (f32)bmap[lpmap[block].p4*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].g * clipTexture[i3*bmp.bmiHeader.biHeight+i4+i*properties->lmapSz+i2*properties->lmapSz*bmp.bmiHeader.biWidth].g / 255.0f;
									lmap[lpmap[block].p4*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].b = (f32)bmap[lpmap[block].p4*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].b * clipTexture[i3*bmp.bmiHeader.biHeight+i4+i*properties->lmapSz+i2*properties->lmapSz*bmp.bmiHeader.biWidth].r / 255.0f;
									break;
								}
							}
						}
						glMes[glMesC].data1 = lpmap[block].p4*properties->lmapSz*properties->lmapSz;
					}
					break;
				}
				case 2:{
					u32 block = crds2map(ray.roundPos.x+i,ray.roundPos.y+i2,ray.roundPos.z);
					if(ray.dir.z>0.0f){
						for(u32 i3 = 0;i3 < properties->lmapSz;i3++){
							for(u32 i4 = 0;i4 < properties->lmapSz;i4++){
								switch(map[block].id){
								case BLOCK_SOLID:
								case BLOCK_REFLECTIVE:
									lmap[lpmap[block].p5*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].r = (f32)bmap[lpmap[block].p5*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].r * clipTexture[i3*bmp.bmiHeader.biHeight+i4+i*properties->lmapSz+i2*properties->lmapSz*bmp.bmiHeader.biWidth].b / 255.0f;
									lmap[lpmap[block].p5*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].g = (f32)bmap[lpmap[block].p5*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].g * clipTexture[i3*bmp.bmiHeader.biHeight+i4+i*properties->lmapSz+i2*properties->lmapSz*bmp.bmiHeader.biWidth].g / 255.0f;
									lmap[lpmap[block].p5*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].b = (f32)bmap[lpmap[block].p5*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].b * clipTexture[i3*bmp.bmiHeader.biHeight+i4+i*properties->lmapSz+i2*properties->lmapSz*bmp.bmiHeader.biWidth].r / 255.0f;
									break;
								}
							}
						}
						glMes[glMesC].data1 = lpmap[block].p5*properties->lmapSz*properties->lmapSz;
					}
					else{
						for(u32 i3 = 0;i3 < properties->lmapSz;i3++){
							for(u32 i4 = 0;i4 < properties->lmapSz;i4++){
								switch(map[block].id){
								case BLOCK_SOLID:
								case BLOCK_REFLECTIVE:
									lmap[lpmap[block].p6*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].r = (f32)bmap[lpmap[block].p6*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].r * clipTexture[i3*bmp.bmiHeader.biHeight+i4+i*properties->lmapSz+i2*properties->lmapSz*bmp.bmiHeader.biWidth].b / 255.0f;
									lmap[lpmap[block].p6*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].g = (f32)bmap[lpmap[block].p6*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].g * clipTexture[i3*bmp.bmiHeader.biHeight+i4+i*properties->lmapSz+i2*properties->lmapSz*bmp.bmiHeader.biWidth].g / 255.0f;
									lmap[lpmap[block].p6*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].b = (f32)bmap[lpmap[block].p6*properties->lmapSz*properties->lmapSz+i3*properties->lmapSz+i4].b * clipTexture[i3*bmp.bmiHeader.biHeight+i4+i*properties->lmapSz+i2*properties->lmapSz*bmp.bmiHeader.biWidth].r / 255.0f;
									break;
								}
							}
						}
						glMes[glMesC].data1 = lpmap[block].p6*properties->lmapSz*properties->lmapSz;
					}
					break;
				}
				}
				glMes[glMesC].id = 7;
				glMesC++;
				}
			}
			HeapFree(GetProcessHeap(),0,clipTexture);
			glMes[glMesC].data1 = l;
			glMes[glMesC].id = 7;
			glMesC++;
			pixelCrdB = -1;
		}
		break;
	}
	case 9:{
		RAY3D ray = ray3dCreate(player->pos,(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
		i32 l = getLmapLocation(&ray);
		ray3dItterate(&ray);
		if(l!=-1){
			RGBA *clipTexture = HeapAlloc(GetProcessHeap(),8,properties->lmapSz*properties->lmapSz*sizeof(RGBA));
			OpenClipboard(window);
			HANDLE h = GetClipboardData(CF_DIB);
			u8 *data = GlobalLock(h);
			memcpy(clipTexture,data+sizeof(BITMAPINFO),properties->lmapSz*properties->lmapSz*sizeof(RGBA));
			GlobalUnlock(h);
			CloseHandle(h);
			CloseClipboard();
			switch(irnd()&3){
			case 0:
				break;
			case 1:
				for(u32 i = 0;i < properties->lmapSz;i++){
					for(u32 i2 = 0;i2 < properties->lmapSz/2;i2++){
						clipTexture[i*properties->lmapSz+i2].r ^= clipTexture[i*properties->lmapSz+properties->lmapSz-i2-1].r;
						clipTexture[i*properties->lmapSz+i2].g ^= clipTexture[i*properties->lmapSz+properties->lmapSz-i2-1].g;
						clipTexture[i*properties->lmapSz+i2].b ^= clipTexture[i*properties->lmapSz+properties->lmapSz-i2-1].b;
						clipTexture[i*properties->lmapSz+properties->lmapSz-i2-1].r ^= clipTexture[i*properties->lmapSz+i2].r;
						clipTexture[i*properties->lmapSz+properties->lmapSz-i2-1].g ^= clipTexture[i*properties->lmapSz+i2].g;
						clipTexture[i*properties->lmapSz+properties->lmapSz-i2-1].b ^= clipTexture[i*properties->lmapSz+i2].b;
						clipTexture[i*properties->lmapSz+i2].r ^= clipTexture[i*properties->lmapSz+properties->lmapSz-i2-1].r;
						clipTexture[i*properties->lmapSz+i2].g ^= clipTexture[i*properties->lmapSz+properties->lmapSz-i2-1].g;
						clipTexture[i*properties->lmapSz+i2].b ^= clipTexture[i*properties->lmapSz+properties->lmapSz-i2-1].b;
					}
				}
				break;
			case 2:
				for(u32 i = 0;i < properties->lmapSz/2;i++){
					for(u32 i2 = 0;i2 < properties->lmapSz;i2++){
						clipTexture[i*properties->lmapSz+i2].r ^= clipTexture[properties->lmapSz*properties->lmapSz-properties->lmapSz-i*properties->lmapSz+i2].r;
						clipTexture[i*properties->lmapSz+i2].g ^= clipTexture[properties->lmapSz*properties->lmapSz-properties->lmapSz-i*properties->lmapSz+i2].g;
						clipTexture[i*properties->lmapSz+i2].b ^= clipTexture[properties->lmapSz*properties->lmapSz-properties->lmapSz-i*properties->lmapSz+i2].b;
						clipTexture[properties->lmapSz*properties->lmapSz-properties->lmapSz-i*properties->lmapSz+i2].r ^= clipTexture[i*properties->lmapSz+i2].r;
						clipTexture[properties->lmapSz*properties->lmapSz-properties->lmapSz-i*properties->lmapSz+i2].g ^= clipTexture[i*properties->lmapSz+i2].g;
						clipTexture[properties->lmapSz*properties->lmapSz-properties->lmapSz-i*properties->lmapSz+i2].b ^= clipTexture[i*properties->lmapSz+i2].b;
						clipTexture[i*properties->lmapSz+i2].r ^= clipTexture[properties->lmapSz*properties->lmapSz-properties->lmapSz-i*properties->lmapSz+i2].r;
						clipTexture[i*properties->lmapSz+i2].g ^= clipTexture[properties->lmapSz*properties->lmapSz-properties->lmapSz-i*properties->lmapSz+i2].g;
						clipTexture[i*properties->lmapSz+i2].b ^= clipTexture[properties->lmapSz*properties->lmapSz-properties->lmapSz-i*properties->lmapSz+i2].b;
					}
				}
				break;
			case 3:
				for(u32 i = 0;i < properties->lmapSz;i++){
					for(u32 i2 = 0;i2 < properties->lmapSz/2;i2++){
						clipTexture[i*properties->lmapSz+i2].r ^= clipTexture[i*properties->lmapSz+properties->lmapSz-i2-1].r;
						clipTexture[i*properties->lmapSz+i2].g ^= clipTexture[i*properties->lmapSz+properties->lmapSz-i2-1].g;
						clipTexture[i*properties->lmapSz+i2].b ^= clipTexture[i*properties->lmapSz+properties->lmapSz-i2-1].b;
						clipTexture[i*properties->lmapSz+properties->lmapSz-i2-1].r ^= clipTexture[i*properties->lmapSz+i2].r;
						clipTexture[i*properties->lmapSz+properties->lmapSz-i2-1].g ^= clipTexture[i*properties->lmapSz+i2].g;
						clipTexture[i*properties->lmapSz+properties->lmapSz-i2-1].b ^= clipTexture[i*properties->lmapSz+i2].b;
						clipTexture[i*properties->lmapSz+i2].r ^= clipTexture[i*properties->lmapSz+properties->lmapSz-i2-1].r;
						clipTexture[i*properties->lmapSz+i2].g ^= clipTexture[i*properties->lmapSz+properties->lmapSz-i2-1].g;
						clipTexture[i*properties->lmapSz+i2].b ^= clipTexture[i*properties->lmapSz+properties->lmapSz-i2-1].b;
					}
				}
				for(u32 i = 0;i < properties->lmapSz/2;i++){
					for(u32 i2 = 0;i2 < properties->lmapSz;i2++){
						clipTexture[i*properties->lmapSz+i2].r ^= clipTexture[properties->lmapSz*properties->lmapSz-properties->lmapSz-i*properties->lmapSz+i2].r;
						clipTexture[i*properties->lmapSz+i2].g ^= clipTexture[properties->lmapSz*properties->lmapSz-properties->lmapSz-i*properties->lmapSz+i2].g;
						clipTexture[i*properties->lmapSz+i2].b ^= clipTexture[properties->lmapSz*properties->lmapSz-properties->lmapSz-i*properties->lmapSz+i2].b;
						clipTexture[properties->lmapSz*properties->lmapSz-properties->lmapSz-i*properties->lmapSz+i2].r ^= clipTexture[i*properties->lmapSz+i2].r;
						clipTexture[properties->lmapSz*properties->lmapSz-properties->lmapSz-i*properties->lmapSz+i2].g ^= clipTexture[i*properties->lmapSz+i2].g;
						clipTexture[properties->lmapSz*properties->lmapSz-properties->lmapSz-i*properties->lmapSz+i2].b ^= clipTexture[i*properties->lmapSz+i2].b;
						clipTexture[i*properties->lmapSz+i2].r ^= clipTexture[properties->lmapSz*properties->lmapSz-properties->lmapSz-i*properties->lmapSz+i2].r;
						clipTexture[i*properties->lmapSz+i2].g ^= clipTexture[properties->lmapSz*properties->lmapSz-properties->lmapSz-i*properties->lmapSz+i2].g;
						clipTexture[i*properties->lmapSz+i2].b ^= clipTexture[properties->lmapSz*properties->lmapSz-properties->lmapSz-i*properties->lmapSz+i2].b;
					}
				}
				break;
			}
			l /= properties->lmapSz*properties->lmapSz;
			l *= properties->lmapSz*properties->lmapSz;
			for(u32 i = 0;i < properties->lmapSz*properties->lmapSz;i++){
				lmap[l+i].r = (f32)bmap[l+i].r * (f32)clipTexture[i].r / 127.0f;
				lmap[l+i].g = (f32)bmap[l+i].g * (f32)clipTexture[i].g / 127.0f;
				lmap[l+i].b = (f32)bmap[l+i].b * (f32)clipTexture[i].b / 127.0f;
			}
			HeapFree(GetProcessHeap(),0,clipTexture);
			glMes[glMesC].data1 = l;
			glMes[glMesC].id = 7;
			glMesC++;
			pixelCrdB = -1;
		}
		break;
		}
	case 10:{
		RAY3D ray = ray3dCreate(player->pos,(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
		i32 l = getLmapLocation(&ray);
		if(l!=-1){
			if(!selarea.x && !selarea.y && !selarea.z){
				selarea.x = ray.roundPos.x;
				selarea.y = ray.roundPos.y;
				selarea.z = ray.roundPos.z;
			}
			else{
				RGBA *clipTexture = HeapAlloc(GetProcessHeap(),8,properties->lmapSz*properties->lmapSz*sizeof(RGBA));
				OpenClipboard(window);
				HANDLE h = GetClipboardData(CF_DIB);
				u8 *data = GlobalLock(h);
				memcpy(clipTexture,data+sizeof(BITMAPINFO),properties->lmapSz*properties->lmapSz*sizeof(RGBA));
				GlobalUnlock(h);
				CloseHandle(h);
				CloseClipboard();
				u8 x = ray.roundPos.x;
				u8 y = ray.roundPos.y;
				u8 z = ray.roundPos.z;
				if(selarea.x > ray.roundPos.x){
					selarea.x ^= x;
					x ^= selarea.x;
					selarea.x ^= x;
				}
				if(selarea.y > ray.roundPos.y){
					selarea.y ^= y;
					y ^= selarea.y;
					selarea.y ^= y;
				}
				if(selarea.z > ray.roundPos.z){
					selarea.z ^= z;
					z ^= selarea.z;
					selarea.z ^= z;
				}
				for(int i = selarea.x;i <= x;i++){
					for(int i2 = selarea.y;i2 <= y;i2++){
						for(int i3 = selarea.z;i3 <= z;i3++){
							if(i >= properties->lvlSz || i2 >= properties->lvlSz || i3 >= properties->lvlSz){
								goto fillLoopEnd;
							}
							u32 block = (i + i2 * properties->lvlSz + i3 * properties->lvlSz * properties->lvlSz);
							
							switch(ray.hitSide){
							case 0:
								if(ray.dir.x>0.0f){
									for(u32 i = 0;i < properties->lmapSz*properties->lmapSz;i++){
										lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+i].r = (f32)lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+i].r * (f32)clipTexture[i].r / 127.0f;
										lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+i].g = (f32)lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+i].g * (f32)clipTexture[i].g / 127.0f;
										lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+i].b = (f32)lmap[lpmap[block].p1*properties->lmapSz*properties->lmapSz+i].b * (f32)clipTexture[i].b / 127.0f;
									}
									glMes[glMesC].data1 = lpmap[block].p1*properties->lmapSz*properties->lmapSz;
								}
								else{
									for(u32 i = 0;i < properties->lmapSz*properties->lmapSz;i++){
										lmap[lpmap[block].p2*properties->lmapSz*properties->lmapSz+i].r = (f32)lmap[lpmap[block].p2*properties->lmapSz*properties->lmapSz+i].r * (f32)clipTexture[i].r / 127.0f;
										lmap[lpmap[block].p2*properties->lmapSz*properties->lmapSz+i].g = (f32)lmap[lpmap[block].p2*properties->lmapSz*properties->lmapSz+i].g * (f32)clipTexture[i].g / 127.0f;
										lmap[lpmap[block].p2*properties->lmapSz*properties->lmapSz+i].b = (f32)lmap[lpmap[block].p2*properties->lmapSz*properties->lmapSz+i].b * (f32)clipTexture[i].b / 127.0f;
									}
									glMes[glMesC].data1 = lpmap[block].p2*properties->lmapSz*properties->lmapSz;
								}
								break;
							case 1:
								if(ray.dir.y>0.0f){
									for(u32 i = 0;i < properties->lmapSz*properties->lmapSz;i++){
										lmap[lpmap[block].p3*properties->lmapSz*properties->lmapSz+i].r = (f32)lmap[lpmap[block].p3*properties->lmapSz*properties->lmapSz+i].r * (f32)clipTexture[i].r / 127.0f;
										lmap[lpmap[block].p3*properties->lmapSz*properties->lmapSz+i].g = (f32)lmap[lpmap[block].p3*properties->lmapSz*properties->lmapSz+i].g * (f32)clipTexture[i].g / 127.0f;
										lmap[lpmap[block].p3*properties->lmapSz*properties->lmapSz+i].b = (f32)lmap[lpmap[block].p3*properties->lmapSz*properties->lmapSz+i].b * (f32)clipTexture[i].b / 127.0f;
									}
									glMes[glMesC].data1 = lpmap[block].p3*properties->lmapSz*properties->lmapSz;
								}
								else{
									for(u32 i = 0;i < properties->lmapSz*properties->lmapSz;i++){
										lmap[lpmap[block].p4*properties->lmapSz*properties->lmapSz+i].r = (f32)lmap[lpmap[block].p4*properties->lmapSz*properties->lmapSz+i].r * (f32)clipTexture[i].r / 127.0f;
										lmap[lpmap[block].p4*properties->lmapSz*properties->lmapSz+i].g = (f32)lmap[lpmap[block].p4*properties->lmapSz*properties->lmapSz+i].g * (f32)clipTexture[i].g / 127.0f;
										lmap[lpmap[block].p4*properties->lmapSz*properties->lmapSz+i].b = (f32)lmap[lpmap[block].p4*properties->lmapSz*properties->lmapSz+i].b * (f32)clipTexture[i].b / 127.0f;
									}
									glMes[glMesC].data1 = lpmap[block].p4*properties->lmapSz*properties->lmapSz;
								}
								break;
							case 2:
								if(ray.dir.z>0.0f){
									for(u32 i = 0;i < properties->lmapSz*properties->lmapSz;i++){
										lmap[lpmap[block].p5*properties->lmapSz*properties->lmapSz+i].r = (f32)lmap[lpmap[block].p5*properties->lmapSz*properties->lmapSz+i].r * (f32)clipTexture[i].r / 127.0f;
										lmap[lpmap[block].p5*properties->lmapSz*properties->lmapSz+i].g = (f32)lmap[lpmap[block].p5*properties->lmapSz*properties->lmapSz+i].g * (f32)clipTexture[i].g / 127.0f;
										lmap[lpmap[block].p5*properties->lmapSz*properties->lmapSz+i].b = (f32)lmap[lpmap[block].p5*properties->lmapSz*properties->lmapSz+i].b * (f32)clipTexture[i].b / 127.0f;
									}
									glMes[glMesC].data1 = lpmap[block].p5*properties->lmapSz*properties->lmapSz;
								}
								else{
									for(u32 i = 0;i < properties->lmapSz*properties->lmapSz;i++){
										lmap[lpmap[block].p6*properties->lmapSz*properties->lmapSz+i].r = (f32)lmap[lpmap[block].p6*properties->lmapSz*properties->lmapSz+i].r * (f32)clipTexture[i].r / 127.0f;
										lmap[lpmap[block].p6*properties->lmapSz*properties->lmapSz+i].g = (f32)lmap[lpmap[block].p6*properties->lmapSz*properties->lmapSz+i].g * (f32)clipTexture[i].g / 127.0f;
										lmap[lpmap[block].p6*properties->lmapSz*properties->lmapSz+i].b = (f32)lmap[lpmap[block].p6*properties->lmapSz*properties->lmapSz+i].b * (f32)clipTexture[i].b / 127.0f;
									}
									glMes[glMesC].data1 = lpmap[block].p6*properties->lmapSz*properties->lmapSz;
								}
								break;
							}
							glMes[glMesC].id = 7;
							glMesC++;
						}
					}
				}
			fillLoopEnd:
				selarea.x = 0;
				selarea.y = 0;
				selarea.z = 0;
				HeapFree(GetProcessHeap(),0,clipTexture);
			}
		}
	}
	}
}
