#include "main.h"
#include <math.h>

CVEC3 selarea;

RAY rayCreate(VEC3 pos,VEC3 dir){
	RAY ray;
	ray.x  		= pos.x;
	ray.y  		= pos.y;
	ray.z  		= pos.z;
	ray.vx		= dir.x;
	ray.vy 		= dir.y;
	ray.vz 		= dir.z;
	
	ray.deltax  = fabsf(1.0/ray.vx);
	ray.deltay  = fabsf(1.0/ray.vy);
	ray.deltaz  = fabsf(1.0/ray.vz);

	if(ray.vx < 0.0){
		ray.stepx = -1;
		ray.sidex = (ray.x-(int)ray.x) * ray.deltax;
	}
	else{
		ray.stepx = 1;
		ray.sidex = ((int)ray.x + 1.0 - ray.x) * ray.deltax;
	}
	if(ray.vy < 0.0){
		ray.stepy = -1;
		ray.sidey = (ray.y-(int)ray.y) * ray.deltay;
	}
	else{
		ray.stepy = 1;
		ray.sidey = ((int)ray.y + 1.0 - ray.y) * ray.deltay;
	}
	if(ray.vz < 0.0){
		ray.stepz = -1;
		ray.sidez = (ray.z-(int)ray.z) * ray.deltaz;
	}
	else{
		ray.stepz = 1;
		ray.sidez = ((int)ray.z + 1.0 - ray.z) * ray.deltaz;
	}
	ray.ix = ray.x;
	ray.iy = ray.y;
	ray.iz = ray.z;
	return ray;
}

void tools(){
	switch(toolSel){
	case 0:{
		RAY ray = rayCreate((VEC3){player->xpos,player->ypos,player->zpos},(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
		while(ray.ix>=0&&ray.ix<=properties->lvlSz&&ray.iy>=0&&ray.iy<=properties->lvlSz&&ray.iz>=0&&ray.iz<=properties->lvlSz){
			int block = crds2map(ray.ix,ray.iy,ray.iz);
			if(map[block].id && map[block].id != 9){
				switch(ray.side){
				case 0:
					if(ray.vx < 0.0){
						ray.x+=1.0;
						updateBlock(block + 1,blockSel);
					}
					else{
						ray.x-=1.0;
						updateBlock(block - 1,blockSel);
					}
					break;
				case 1:
					if(ray.vy < 0.0){
						ray.y+=1.0;
						updateBlock(block + properties->lvlSz,blockSel);
					}
					else{
						ray.y-=1.0;
						updateBlock(block - properties->lvlSz,blockSel);
					}
					break;
				case 2:
					if(ray.vz < 0.0){
						ray.z+=1.0;
						updateBlock(block + properties->lvlSz * properties->lvlSz,blockSel);
					}
					else{
						ray.z-=1.0;
						updateBlock(block - properties->lvlSz * properties->lvlSz,blockSel);
					}
					break;
				}
				switch(blockSel){
				case 67:
					toolSel = 4;
					selarea.x = ray.x;
					selarea.y = ray.y;
					selarea.z = ray.z;
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
					unsigned char x = ray.ix;
					unsigned char y = ray.iy;
					unsigned char z = ray.iz;
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
	case 2:{
		RAY ray = rayCreate((VEC3){player->xpos,player->ypos,player->zpos},(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
		while(ray.ix>=0&&ray.ix<=properties->lvlSz&&ray.iy>=0&&ray.iy<=properties->lvlSz&&ray.iz>=0&&ray.iz<=properties->lvlSz){
			int block = crds2map(ray.ix,ray.iy,ray.iz);
			switch(map[block].id){
			case 0:
				break;
			case 8:
				CreateThread(0,0,updateLightSingle,&block,0,0);
			default:
				goto end;
			}
			rayItterate(&ray);
			continue;
		end:
			break;
		}
		break;
	}
	case 3:
		break;		
	case 4:{
		break;
	}
	case 5:{
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
	case 6:{
		RAY ray = rayCreate((VEC3){player->xpos,player->ypos,player->zpos},(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
		for(int i = 0;i < 254;i++){
			rayItterate(&ray);
			int block = crds2map(ray.ix,ray.iy,ray.iz);
			if(map[block].id){
				switch(ray.side){
				case 0:
					if(ray.vx < 0.0){
						ray.x+=1.0;
						updateBlock(block + 1 * 4,9);
					}
					else{
						ray.x-=1.0;
						updateBlock(block - 1 * 4,9);
					}
					break;
				case 1:
					if(ray.vy < 0.0){
						ray.y+=1.0;
						updateBlock(block + properties->lvlSz * 4,9);
					}
					else{
						ray.y-=1.0;
						updateBlock(block - properties->lvlSz * 4,9);
					}
					break;
				case 2:
					if(ray.vz < 0.0){
						ray.z+=1.0;
						updateBlock(block + properties->lvlSz * properties->lvlSz * 4,9);
					}
					else{
						ray.z-=1.0;
						updateBlock(block - properties->lvlSz * properties->lvlSz * 4,9);
					}
					break;
				}
				break;
			}
		}
		break;
		}
	}
}
