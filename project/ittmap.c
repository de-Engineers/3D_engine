#include <main.h>
#include <math.h>
#include <stdio.h>

CVEC3 map2crds(int map){
	CVEC3 m;
	map/=4;
	m.x = map % properties->lvlSz;
	m.y = map / properties->lvlSz % properties->lvlSz;
	m.z = map / (properties->lvlSz*properties->lvlSz);
	return m;
}

void ittmap(){
	for(;;){
		if((settings & 0x05) == 0){
			for(int i = 0;i < MAPRAM;i+=4){
				switch(map[i]){
				case 5:{
					if(mapdata[i] == 0){
						CVEC3 block = map2crds(i);
						float nx = player->xpos-((float)block.x+0.5);
						float ny = player->ypos-((float)block.y+0.5);
						float nz = player->zpos-((float)block.z+0.5);
						float nm = fmaxf(fabsf(nx),fmaxf(fabsf(ny),fabsf(nz)));
						nx /= nm;
						ny /= nm;
						nz /= nm;
						RAY ray = rayCreate((float)block.x+0.5,(float)block.y+0.5,(float)block.z+0.5,nx,ny,nz);
						for(;;){
							rayItterate(&ray);
							int block2 = crds2map(ray.ix,ray.iy,ray.iz);
							if(map[block2]){
								break;
							}
							if(ray.ix == (int)player->xpos && ray.iy == (int)player->ypos && ray.iz == (int)player->zpos){
								spawnEntity((float)block.x+0.5,(float)block.y+0.5,(float)block.z+0.5,nx*0.03,ny*0.03,nz*0.03,0.3,1);
								mapdata[i] = 50;
								break;
							}
						} 
						break;
					}
					else{
						mapdata[i]--;
					}
				}
				default:
					break;
				}
			}
		}
		Sleep(15);
	}
}
