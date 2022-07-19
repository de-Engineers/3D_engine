#include "main.h"
#include <math.h>

CVEC3 selarea;

inline RAY rayCreate(VEC3 pos,VEC3 dir){
	RAY ray;

	ray.pos = pos;
	ray.dir = dir;
	
	ray.delta = VEC3absR(VEC3divFR(ray.dir,1.0f));

	if(ray.dir.x < 0.0f){
		ray.stepx = -1;
		ray.side.x = (ray.pos.x-(int)ray.pos.x) * ray.delta.x;
	}
	else{
		ray.stepx = 1;
		ray.side.x = ((int)ray.pos.x + 1.0f - ray.pos.x) * ray.delta.x;
	}
	if(ray.dir.y < 0.0f){
		ray.stepy = -1;
		ray.side.y = (ray.pos.y-(int)ray.pos.y) * ray.delta.y;
	}
	else{
		ray.stepy = 1;
		ray.side.y = ((int)ray.pos.y + 1.0f - ray.pos.y) * ray.delta.y;
	}
	if(ray.dir.z < 0.0f){
		ray.stepz = -1;
		ray.side.z = (ray.pos.z-(int)ray.pos.z) * ray.delta.z;
	}
	else{
		ray.stepz = 1;
		ray.side.z = ((int)ray.pos.z + 1.0f - ray.pos.z) * ray.delta.z;
	}
	ray.ix = ray.pos.x;
	ray.iy = ray.pos.y;
	ray.iz = ray.pos.z;
	return ray;
}

void tools(){
	switch(toolSel){
	case 0:{
		RAY ray = rayCreate((VEC3){player->xpos,player->ypos,player->zpos},(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
		while(ray.ix>=0&&ray.ix<=properties->lvlSz&&ray.iy>=0&&ray.iy<=properties->lvlSz&&ray.iz>=0&&ray.iz<=properties->lvlSz){
			int block = crds2map(ray.ix,ray.iy,ray.iz);
			if(map[block].id){
				switch(ray.sid){
				case 0:
					if(ray.dir.x < 0.0f){
						ray.pos.x+=1.0f;
						updateBlock(block + 1,blockSel);
					}
					else{
						ray.pos.x-=1.0f;
						updateBlock(block - 1,blockSel);
					}
					break;
				case 1:
					if(ray.dir.y < 0.0f){
						ray.pos.y+=1.0f;
						updateBlock(block + properties->lvlSz,blockSel);
					}
					else{
						ray.pos.y-=1.0f;
						updateBlock(block - properties->lvlSz,blockSel);
					}
					break;
				case 2:
					if(ray.dir.z < 0.0f){
						ray.pos.z+=1.0f;
						updateBlock(block + properties->lvlSz * properties->lvlSz,blockSel);
					}
					else{
						ray.pos.z-=1.0f;
						updateBlock(block - properties->lvlSz * properties->lvlSz,blockSel);
					}
					break;
				}
				switch(blockSel){
				case 67:
					toolSel = 4;
					selarea.x = ray.pos.x;
					selarea.y = ray.pos.y;
					selarea.z = ray.pos.z;
					break;
				}
				break;
			}
			rayItterate(&ray);
		}
		break;
		}
	case 1:{
		RAY ray = rayCreate((VEC3){player->xpos,player->ypos,player->zpos},(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
		while(ray.ix>=0&&ray.ix<=properties->lvlSz&&ray.iy>=0&&ray.iy<=properties->lvlSz&&ray.iz>=0&&ray.iz<=properties->lvlSz){
			int block = crds2map(ray.ix,ray.iy,ray.iz);
			if(map[block].id){
				if(!selarea.x && !selarea.y && !selarea.z){
					selarea.x = ray.ix;
					selarea.y = ray.iy;
					selarea.z = ray.iz;
				}
				else{
					u8 x = ray.ix;
					u8 y = ray.iy;
					u8 z = ray.iz;
					if(selarea.z > ray.iz){
						selarea.z ^= z;
						z ^= selarea.z;
						selarea.z ^= z;
					}
					if(selarea.x > ray.ix){
						selarea.x ^= x;
						x ^= selarea.x;
						selarea.x ^= x;
					}
					if(selarea.y > ray.iy){
						selarea.y ^= y;
						y ^= selarea.y;
						selarea.y ^= y;
					}
					for(int i = selarea.x;i <= x;i++){
						for(int i2 = selarea.y;i2 <= y;i2++){
							for(int i3 = selarea.z;i3 <= z;i3++){
								int block = (i + i2 * properties->lvlSz + i3 * properties->lvlSz * properties->lvlSz);
								map[block].id = blockSel;
								map[block].r  = colorSel.r;
								map[block].g  = colorSel.g;
								map[block].b  = colorSel.b;

								metadt[block].r = metadtSel.r;
								metadt[block].g = metadtSel.g;
								metadt[block].b = metadtSel.b;
								metadt[block].id = metadtSel.a;

								metadt2[block].r = metadtSel.r;
								metadt2[block].g = metadtSel.g;
								metadt2[block].b = metadtSel.b;
								metadt2[block].id = metadtSel.a;

								metadt3[block].r = metadtSel.r;
								metadt3[block].g = metadtSel.g;
								metadt3[block].b = metadtSel.b;
								metadt3[block].id = metadtSel.a;
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
			rayItterate(&ray);
		}
		break;
	}
	case 2:
		break;
	case 3:{
		RAY ray = rayCreate((VEC3){player->xpos,player->ypos,player->zpos},(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
		while(ray.ix>=0&&ray.ix<=properties->lvlSz&&ray.iy>=0&&ray.iy<=properties->lvlSz&&ray.iz>=0&&ray.iz<=properties->lvlSz){
			int block = crds2map(ray.ix,ray.iy,ray.iz);
			if(map[block].id){
				map[block].r = colorSel.r;
				map[block].g = colorSel.g;
				map[block].b = colorSel.b;
				glMes[glMesC].id = 3;
				glMesC++;
				break;
			}
			rayItterate(&ray);
		}
		break;	
	}
	case 4:{
		RAY ray = rayCreate((VEC3){player->xpos,player->ypos,player->zpos},(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
		while(ray.ix>=0&&ray.ix<=properties->lvlSz&&ray.iy>=0&&ray.iy<=properties->lvlSz&&ray.iz>=0&&ray.iz<=properties->lvlSz){
			int block = crds2map(ray.ix,ray.iy,ray.iz);
			if(map[block].id){
				metadt[block].r = metadtSel.r;
				metadt[block].g = metadtSel.g;
				metadt[block].b = metadtSel.b;
				metadt[block].id= metadtSel.a;
				glMes[glMesC].id = 3;
				glMesC++;
				break;
			}
			rayItterate(&ray);
		}
		break;
	}
	case 5:{
		RAY ray = rayCreate((VEC3){player->xpos,player->ypos,player->zpos},(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
		while(ray.ix>=0&&ray.ix<=properties->lvlSz&&ray.iy>=0&&ray.iy<=properties->lvlSz&&ray.iz>=0&&ray.iz<=properties->lvlSz){
			int block = crds2map(ray.ix,ray.iy,ray.iz);
			if(map[block].id){
				metadt[block].r = metadt2Sel.r;
				metadt[block].g = metadt2Sel.g;
				metadt[block].b = metadt2Sel.b;
				metadt[block].id= metadt2Sel.a;
				glMes[glMesC].id = 3;
				glMesC++;
				break;
			}
			rayItterate(&ray);
		}
		break;
		}
	case 6:{
		RAY ray = rayCreate((VEC3){player->xpos,player->ypos,player->zpos},(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
		for(int i = 0;i < 254;i++){
			rayItterate(&ray);
			int block = crds2map(ray.ix,ray.iy,ray.iz);
			if(map[block].id){
				metadt[block].r = metadt3Sel.r;
				metadt[block].g = metadt3Sel.g;
				metadt[block].b = metadt3Sel.b;
				metadt[block].id= metadt3Sel.a;
				glMes[glMesC].id = 3;
				glMesC++;
				break;
			}
		}
		break;
		}
	}
}
