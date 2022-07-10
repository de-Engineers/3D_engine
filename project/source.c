#include "main.h"
#include <windows.h>
#include <math.h>
#include <intrin.h>
#include <stdio.h>

#pragma comment(lib,"winmm.lib");

#define resx 512
#define resy 512

#define RENDERDISTANCE 32

#define MAPSZ 64

#define MAPRAM MAPSZ*MAPSZ*MAPSZ*4

#define VRAM resx*resy*4

#define LMAPSZ 4
#define LMAPSZT LMAPSZ*LMAPSZ*LMAPSZ

MAP   *map;
LPMAP *lpmap;

unsigned char menuSel;

unsigned char tempVar[2];

int settings;

/*
bit
1: fly/walk
2: fullScreen
3: lighting
4: fog
5: ?
6: ui
7:
8: pauze
*/

char touchStatus;
char threadStatus;
char abilities;

PROPERTIES *properties;
PLAYERDATA *player;

RGBA colorSel;

unsigned char blockSel = 1;
unsigned char toolSel;	

char specialBlock[2];
float specialBlockcrd[3];

float mousex;
float mousey;
float stamina = 1.0f;
float brightness;
float averageBrightness;
float maxBrightness;

BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER),resx,resy,1,32,BI_RGB };	

HANDLE renderingThread;
HANDLE consoleThread;
HANDLE physicsThread;
HANDLE entitiesThread;
HANDLE staticEntitiesThread;
HANDLE ittmapThread;

const char name[] = "3D_engine";

HINSTANCE hInstance;
HWND window;
HDC dc;
MSG Msg;

void playerDeath(){
	player->xpos = player->xspawn;
	player->ypos = player->yspawn;
	player->zpos = player->zspawn;
	player->xvel = 0;
	player->yvel = 0;
	player->zvel = 0;
}

inline void buttonCreate(VEC2 pos,unsigned char id){
	button[buttonC].pos.x = pos.x;
	button[buttonC].pos.y = pos.y;
	button[buttonC].id    = id;
	buttonC++;
}

void blockDetection(float x,float y,float z,int axis){
	int block = crds2map(x,y,z);
	switch(map[block].id){
	case 3:
		break;
	case 9:
		break;
	case 12:
		specialBlock[0] = map[block].id;
		specialBlock[1] |= axis;
		break;
	case 14:
		playerDeath();
		break;
	case 49:
		if(axis & 0x21){
			break;
		}
		touchStatus |= axis;
		break;
	case 50:
		if(axis & 0x11){
			break;
		}
		touchStatus |= axis;
	case 51:
		if(axis & 0x9){
			break;
		}
		touchStatus |= axis;
	case 67:
		break;
	default:
		touchStatus |= axis;
		break;
	}
}

void specialBlockDetection(float x,float y,float z,int axis){
	int block = crds2map(x,y,z);
	switch(map[block].id){
	case 3:
		break;
	case 9:
		break;
	case 49:
		if(touchStatus){
			break;
		}
		if(y-(int)y>z-(int)z){
			specialBlock[0] = map[block].id;
			specialBlock[1] |= axis;
		}
		break;
	case 50:
		if(touchStatus){
			break;	
		}
		if(y-(int)y<z-(int)z){
			specialBlock[0] = map[block].id;
			specialBlock[1] |= axis;
		}
		break;
	case 51:
		if(touchStatus){
			break;	
		}
		if(x-(int)x<z-(int)z){
			specialBlock[0] = map[block].id;
			specialBlock[1] |= axis;
		}
		break;
	case 67:
		break;
	default:
		touchStatus |= 64;
		break;
	}
}

void hitboxZdown(float x,float y,float z){
	if(map[((int)x + (int)y * properties->lvlSz + (int)z * properties->lvlSz * properties->lvlSz)].id){
		z -= player->zvel;
		if(!map[((int)x + (int)y * properties->lvlSz + (int)z * properties->lvlSz * properties->lvlSz)].id){
			z += player->zvel;
			blockDetection(x,y,z,1);
		}
		else{
			z += player->zvel;
			specialBlockDetection(x,y,z,1);
		}
	}
}

void hitboxZup(float x,float y,float z){
	if(z > properties->lvlSz || map[((int)x + (int)y * properties->lvlSz + (int)z * properties->lvlSz * properties->lvlSz)].id){
		z -= player->zvel;
		if(!map[((int)x + (int)y * properties->lvlSz + (int)z * properties->lvlSz * properties->lvlSz)].id){
			z += player->zvel;
			blockDetection(x,y,z,2);
		}
		else{
			z += player->zvel;
			specialBlockDetection(x,y,z,2);
		}
	}
}

void hitboxXdown(float x,float y,float z){
	if(x < 0 || x > properties->lvlSz || map[((int)x + (int)y * properties->lvlSz + (int)z * properties->lvlSz * properties->lvlSz)].id){
		x -= player->xvel;
		if(!map[((int)x + (int)y * properties->lvlSz + (int)z * properties->lvlSz * properties->lvlSz)].id){
			x += player->xvel;
			blockDetection(x,y,z,4);
		}
		else{
			x += player->xvel;
			specialBlockDetection(x,y,z,4);
		}
	}
}

void hitboxXup(float x,float y,float z){
	if(x < 0 || x > properties->lvlSz || map[((int)x + (int)y * properties->lvlSz + (int)z * properties->lvlSz * properties->lvlSz)].id){
		x -= player->xvel;
		if(!map[((int)x + (int)y * properties->lvlSz + (int)z * properties->lvlSz * properties->lvlSz)].id){
			x += player->xvel;
			blockDetection(x,y,z,8);
		}
		else{
			x += player->xvel;
			specialBlockDetection(x,y,z,8);
		}
	}
}

void hitboxYdown(float x,float y,float z){
	if(y < 0 || y > properties->lvlSz || map[((int)x + (int)y * properties->lvlSz + (int)z * properties->lvlSz * properties->lvlSz)].id){
		y -= player->yvel;
		if(!map[((int)x + (int)y * properties->lvlSz + (int)z * properties->lvlSz * properties->lvlSz)].id){
			y += player->yvel;
			blockDetection(x,y,z,16);
		}
		else{
			y += player->yvel;
			specialBlockDetection(x,y,z,16);
		}
	}
}

void hitboxYup(float x,float y,float z){
	if(y < 0 || y > properties->lvlSz || map[((int)x + (int)y * properties->lvlSz + (int)z * properties->lvlSz * properties->lvlSz)].id){
		y -= player->yvel;
		if(!map[((int)x + (int)y * properties->lvlSz + (int)z * properties->lvlSz * properties->lvlSz)].id){
			y += player->yvel;
			blockDetection(x,y,z,32);
		}
		else{
			y += player->yvel;
			specialBlockDetection(x,y,z,32);
		}
	}
}

void rayItterate(RAY *ray){
    if(ray->sidex < ray->sidey){
        if(ray->sidex < ray->sidez){
			ray->ix += ray->stepx;
			ray->sidex += ray->deltax;
			ray->side = 0;
        }
        else{
			ray->iz += ray->stepz;
			ray->sidez += ray->deltaz;
			ray->side = 2;
        }
    }
    else if(ray->sidey < ray->sidez){
		ray->iy += ray->stepy;
		ray->sidey += ray->deltay;
		ray->side = 1;
    }
    else{
		ray->iz += ray->stepz;
		ray->sidez += ray->deltaz;
		ray->side = 2;
    }
}

inline int max3(int val1,int val2,int val3){
	if(val1 > val2){
		if(val1 > val3){
			return val1;
		}
		else{
			return val3;
		}
	}
	else if(val2 > val3){
		return val2;
	}
	else{
		return val3;
	}
}

void updateBlock(int pos,int val){
	map[pos].id = val;
	map[pos].r = colorSel.r;
	map[pos].g = colorSel.g;
	map[pos].b = colorSel.b;
	glMes[glMesC].id = 3;
	glMesC++;
}

void updateBlockLight(int pos){
	glMes[glMesC].id = 3;
	glMesC++;
}

void deleteBlock(int pos){
	map[pos].id = 0;
	glMes[glMesC].id = 3;
	glMesC++;
}


void spawnEntity(float x,float y,float z,float vx,float vy,float vz,float sz,int id){
	entity[entityC].x  = x;
	entity[entityC].y  = y;
	entity[entityC].z  = z;
	entity[entityC].vx = vx;
	entity[entityC].vy = vy;
	entity[entityC].vz = vz;
	entity[entityC].sz = sz;
	entity[entityC].id = id;
	entityC++;
}

void levelSave(char *lname){
	char *name = HeapAlloc(GetProcessHeap(),8,strlen(lname)+12);
	memcpy(name,"levels/",7);
	memcpy(name+7,lname,strlen(lname));
	memcpy(name+strlen(lname)+7,".lvl\0",5);
	CreateDirectoryA("levels",0);
	HANDLE h = CreateFileA(name,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	WriteFile(h,&properties->lvlSz,1,0,0);
	WriteFile(h,&properties->lmapSz,1,0,0);
	WriteFile(h,map,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0,0);
	HeapFree(GetProcessHeap(),0,name);
	CloseHandle(h);
}

void levelLoad(char *lname){
	char *name = HeapAlloc(GetProcessHeap(),8,strlen(lname)+12);
	memcpy(name,"levels/",7);
	memcpy(name+7,lname,strlen(lname));
	memcpy(name+strlen(lname)+7,".lvl\0",5);
	HANDLE h = CreateFileA(name,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	SetFilePointer(h,2,0,FILE_CURRENT);
	properties->lmapSz3 = properties->lmapSz*properties->lmapSz*properties->lmapSz;
	properties->lmapSz2  = properties->lmapSz*properties->lvlSz;
	map        = HeapAlloc(GetProcessHeap(),8,properties->lvlSz*properties->lvlSz*properties->lvlSz*4);
	ReadFile(h,map,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0,0);
	HeapFree(GetProcessHeap(),0,name);
	CloseHandle(h);
	glMes[glMesC].id = 3;
	glMesC++;
}

void levelDelete(char *lname){
	char *name = HeapAlloc(GetProcessHeap(),8,strlen(lname)+12);
	memcpy(name,"levels/",7);
	memcpy(name+7,lname,strlen(lname));
	memcpy(name+strlen(lname)+7,".lvl\0",5);
	DeleteFileA(name);
	HeapFree(GetProcessHeap(),0,name);
}

long _stdcall proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
	case WM_QUIT:
	case WM_CLOSE:
	case WM_DESTROY:
		levelSave("level");
		ExitProcess(0);
	case WM_ACTIVATE:
		switch(wParam){
		case WA_INACTIVE:
			SuspendThread(physicsThread);
			settings ^= 0x80;
			break;
		default:
			ResumeThread(physicsThread);
			settings ^= 0x80;
			break;
		}
		break;
	case WM_KEYDOWN:
		switch(menuSel){
		case 3:
			if(wParam>0x2f&&wParam<0x3a){
				inputStr[strlen(inputStr)] = wParam-0x33;
			}
			else if(wParam>0x40&&wParam<0x60){
				inputStr[strlen(inputStr)] = wParam-0x3a;
			}
			break;
		}
		switch(wParam){
		case VK_NUMPAD0:
			colorSel.r = 255;
			colorSel.g = 0;
			colorSel.b = 0;
			if(GetKeyState(VK_RCONTROL)&0x80){
				colorSel.r -= 127;
			}
			break;
		case VK_NUMPAD1:
			colorSel.r = 0;
			colorSel.g = 255;
			colorSel.b = 0;
			if(GetKeyState(VK_RCONTROL)&0x80){
				colorSel.g -= 127;
			}
			break;
		case VK_NUMPAD2:
			colorSel.r = 0;
			colorSel.g = 0;
			colorSel.b = 255;
			if(GetKeyState(VK_RCONTROL)&0x80){
				colorSel.b -= 127;
			}
			break;
		case VK_NUMPAD3:
			colorSel.r = 255;
			colorSel.g = 255;
			colorSel.b = 0;
			if(GetKeyState(VK_RCONTROL)&0x80){
				colorSel.r -= 127;
				colorSel.g -= 127;
			}
			break;
		case VK_NUMPAD4:
			colorSel.r = 0;
			colorSel.g = 255;
			colorSel.b = 255;
			if(GetKeyState(VK_RCONTROL)&0x80){
				colorSel.g -= 127;
				colorSel.b -= 127;
			}
			break;
		case VK_NUMPAD5:
			colorSel.r = 255;
			colorSel.g = 0;
			colorSel.b = 255;
			if(GetKeyState(VK_RCONTROL)&0x80){
				colorSel.r -= 127;
				colorSel.b -= 127;
			}
			break;
		case VK_NUMPAD6:
			colorSel.r = 255;
			colorSel.g = 255;
			colorSel.b = 255;
			if(GetKeyState(VK_RCONTROL)&0x80){
				colorSel.r -= 127;
				colorSel.g -= 127;
				colorSel.b -= 127;
			}
			break;
		case VK_RETURN:
			switch(menuSel){
			case 3:
				for(int i = 0;i < strlen(inputStr);i++){
					if(inputStr[i]<10){
						inputStr[i]+=0x33;
					}
					else{
						inputStr[i]+=0x5a;
					}
				}
				levelSave(inputStr);
				menuSel = 1;
				buttonC = 0;
				buttonCreate((VEC2){0.05f,-0.35f},1);
				buttonCreate((VEC2){0.05f,-0.42f},0);
				buttonCreate((VEC2){0.05f,-0.28f},2);
				buttonCreate((VEC2){0.05f,-0.21f},3);
				break;
			}
			break;
		case VK_BACK:
			switch(menuSel){
			case 3:
				inputStr[strlen(inputStr)-1] = 0;
				break;
			}
			break;
		case VK_F2:
			CreateThread(0,0,updateLight2,0,0,0);
			break;
		case VK_F3:
			settings ^= 0x04;
			break;
		case VK_ESCAPE:
			switch(menuSel){
			case 0:
				menuSel = 1;
				buttonCreate((VEC2){0.05f,-0.35f},1);
				buttonCreate((VEC2){0.05f,-0.42f},0);
				buttonCreate((VEC2){0.05f,-0.28f},2);
				buttonCreate((VEC2){0.05f,-0.21f},3);
				buttonCreate((VEC2){-0.158f,-0.07f},4);
				buttonCreate((VEC2){-0.052f,-0.07f},5);
				SetCursorPos(properties->xres/2,properties->yres/2);
				break;
			case 1:
				menuSel = 0;
				buttonC = 0;
				break;
			case 2:
				menuSel = 1;
				buttonC = 0;
				buttonCreate((VEC2){0.05f,-0.35f},1);
				buttonCreate((VEC2){0.05f,-0.42f},0);
				buttonCreate((VEC2){0.05f,-0.28f},2);
				buttonCreate((VEC2){0.05f,-0.21f},3);
				buttonCreate((VEC2){-0.158f,-0.07f},4);
				buttonCreate((VEC2){-0.052f,-0.07f},5);
				fileNames.strC = 0;
				break;
			case 3:
				menuSel = 1;
				buttonCreate((VEC2){0.05f,-0.35f},1);
				buttonCreate((VEC2){0.05f,-0.42f},0);
				buttonCreate((VEC2){0.05f,-0.28f},2);
				buttonCreate((VEC2){0.05f,-0.21f},3);				
				buttonCreate((VEC2){-0.158f,-0.07f},4);
				buttonCreate((VEC2){-0.052f,-0.07f},5);
				ZeroMemory(inputStr,255);
				break;
			}
			break;
		case VK_LCONTROL:
			if(!abilities & 0x01){ 
				player->xvel *= 2.5;
				player->yvel *= 2.5;
				player->zvel -= 0.3;
				abilities ^= 0x01;
			}
			break;
		case VK_F11:
			settings ^= 0x20;
			break;
		case VK_F1:
			settings ^= 0x01;
			break;
		case VK_F5:
			settings ^= 0x08;
			if(settings & 0x08){
				for(int i = 0;i < MAPRAM;i+=4){
					if(map[i].id == 0){
						map[i].id = 9;
					}
				}
				glMes[glMesC].id = 3;
				glMesC++;
			}
			else{
				for(int i = 0;i < MAPRAM;i+=4){
					if(map[i].id == 9){
						map[i].id = 0;
					}
				}
				glMes[glMesC].id = 3;
				glMesC++;
			}
			break;
		}
		if(GetKeyState(VK_F6) & 0x80){
			for(int i = 0;i < properties->lvlSz*properties->lvlSz*properties->lvlSz*4;i+=4){
				if(map[i].id == 1){
					map[i].id = 0;
				}
			}
		}
		if(GetKeyState(VK_F7) & 0x80){
			for(int i = 0;i < properties->lvlSz*properties->lvlSz*properties->lvlSz*4;i+=4){
				if(map[i].id < 2){
					int x = i%(properties->lvlSz*4)-20;
					int y = i/(properties->lvlSz*4)%(properties->lvlSz*4)*properties->lvlSz*4-20*properties->lvlSz;
					int z = i/properties->lvlSz/properties->lvlSz/4*properties->lvlSz*properties->lvlSz*4-20*properties->lvlSz*properties->lvlSz;
					if(x < 0){
						x = 0;
					}
					if(y < 0){
						y = 0;
					}
					if(z < 0){
						z = 0;
					}
					if(z > properties->lvlSz*properties->lvlSz*properties->lvlSz*4-properties->lvlSz*properties->lvlSz*40){
						z = properties->lvlSz*properties->lvlSz*properties->lvlSz*4-properties->lvlSz*properties->lvlSz*40;
					}
					if(y > properties->lvlSz*properties->lvlSz*4-properties->lvlSz*40){
						y = properties->lvlSz*properties->lvlSz*4-properties->lvlSz*40;
					}
					if(x > properties->lvlSz*4-40){
						x = properties->lvlSz*4-40;
					}
					int hit = 0;
					for(int i2 = z;i2 < z + 10*properties->lvlSz*properties->lvlSz*4;i2+=properties->lvlSz*properties->lvlSz*4){
						for(int i3 = y;i3 < y + 10*properties->lvlSz*4;i3+=properties->lvlSz*4){
							for(int i4 = x;i4 < x + 40;i4+=4){
								if(map[i2+i3+i4].id > 1){
									i2 = 0x0fffffff;
									i3 = 0x0fffffff;
									i4 = 0x0fffffff;
									hit = 1;
									break;
								}
							}
						}	
					}
					if(!hit){
						map[i].id = 1;
					}
				}
			}
		}
		if(GetKeyState(VK_PRIOR) & 0x80){
			toolSel++;
		}
		if(GetKeyState(VK_NEXT) & 0x80){
			toolSel--;
		}
		if(GetKeyState(0x46) & 0x80){
			switch(menuSel){
			case 3:
				break;
			default:
				settings ^= 2;
				if(settings & 2){
					SetWindowPos(window,0,0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),0);	
					properties->xres = GetSystemMetrics(SM_CXSCREEN);
					properties->yres = GetSystemMetrics(SM_CYSCREEN);
					glMes[glMesC].id = 0;
					glMesC++;
				}
				else{
					SetWindowPos(window,0,0,0,256,256,0);	
					properties->xres = 256;
					properties->yres = 256;
					glMes[glMesC].id = 0;
					glMesC++;
				}
				break;
			}

		}
		if(GetKeyState(VK_ADD) & 0x80){
			selarea.x = 0;
			selarea.y = 0;
			selarea.z = 0;
			if(toolSel == 2 || toolSel == 5){
				if(GetKeyState(VK_RSHIFT) & 0x80){
					if(GetKeyState(VK_RCONTROL) & 0x80){
						colorSel.a++;
					}
					else{
						colorSel.b++;
					}
				}
				else{
					if(GetKeyState(VK_RCONTROL) & 0x80){
						colorSel.g++;
					}
					else{
						colorSel.r++;
					}
				}
			}
			else{
				if(GetKeyState(VK_LCONTROL) & 0x80){
					if(GetKeyState(VK_RSHIFT) & 0x80){
						if(GetKeyState(VK_RCONTROL) & 0x80){
							colorSel.a++;
						}
						else{
							colorSel.b++;
						}
					}
					else{
						if(GetKeyState(VK_RCONTROL) & 0x80){
							colorSel.g++;
						}
						else{
							colorSel.r++;
						}
					}
				}
				else{
					blockSel++;
				}
			}
		}
		if(GetKeyState(VK_SUBTRACT) & 0x80){
			selarea.x = 0;
			selarea.y = 0;
			selarea.z = 0;
			if(toolSel == 2 || toolSel == 5){
				if(GetKeyState(VK_RSHIFT) & 0x80){
					if(GetKeyState(VK_RCONTROL) & 0x80){
						colorSel.a--;
					}
					else{
						colorSel.b--;
					}
				}
				else{
					if(GetKeyState(VK_RCONTROL) & 0x80){
						colorSel.g--;
					}
					else{
						colorSel.r--;
					}
				}
			}
			else{
				if(GetKeyState(VK_LCONTROL) & 0x80){
					if(GetKeyState(VK_RSHIFT) & 0x80){
						if(GetKeyState(VK_RCONTROL) & 0x80){
							colorSel.a--;
						}
						else{
							colorSel.b--;
						}
					}
					else{
						if(GetKeyState(VK_RCONTROL) & 0x80){
							colorSel.g--;
						}
						else{
							colorSel.r--;
						}
					}
				}
				else{
					if(blockSel != 0){
						blockSel--;
					}
				}

			}
		}
		if(GetKeyState(0x4b) & 0x80){
			entity[entityC].x  = player->xpos+1;
			entity[entityC].y  = player->ypos;
			entity[entityC].z  = player->zpos;
			entity[entityC].vx = player->xdir / 20;
			entity[entityC].vy = player->ydir / 20;
			entity[entityC].vz = player->zdir / 20;
			entityC++;
		}
		if(settings & 0x01){
			if(GetKeyState(0x45) & 0x80){
				map[(int)player->xpos + (int)player->ypos * properties->lvlSz + ((int)player->zpos - 1) * properties->lvlSz * properties->lvlSz].id = blockSel;
				glMes[glMesC].id = 1;
				glMes[glMesC].data1 = player->xpos;
				glMes[glMesC].data2 = player->ypos;
				glMes[glMesC].data3 = player->zpos - 1;
				glMesC++;
			}
		}
		break;
	case WM_KEYUP:
		switch(wParam){
		case VK_CONTROL:
			abilities ^= 0x01;
			if(map[((int)(player->xpos - 0.2) + (int)(player->ypos - 0.2) * properties->lvlSz + (int)( player->zpos - 1.69) * properties->lvlSz * properties->lvlSz)].id){
				player->zvel = 0.211;
			}
			else if(map[((int)(player->xpos + 0.2) + (int)(player->ypos - 0.2) * properties->lvlSz + (int)( player->zpos - 1.69) * properties->lvlSz * properties->lvlSz)].id){
				player->zvel = 0.211;
			}
			else if(map[((int)(player->xpos - 0.2) + (int)(player->ypos + 0.2) * properties->lvlSz + (int)( player->zpos - 1.69) * properties->lvlSz * properties->lvlSz)].id){
				player->zvel = 0.211;
			}
			else if(map[((int)(player->xpos + 0.2) + (int)(player->ypos + 0.2) * properties->lvlSz + (int)( player->zpos - 1.69) * properties->lvlSz * properties->lvlSz)].id){
				player->zvel = 0.211;
			}
			break;
		}
		break;
	case WM_MOUSEMOVE:
		if(!menuSel){
			POINT curp;
			GetCursorPos(&curp);
			mousex = (float)(curp.x - 50) / 250;
			mousey = (float)(curp.y - 50) / 250;
			SetCursorPos(50,50);
			if(mousex > 0.5 || mousey > 0.5){
				break;
			}
			player->xangle += mousex;
			player->yangle -= mousey;
			if(player->yangle < -1.6){
				player->yangle = -1.6;
			}
			if(player->yangle > 1.6){
				player->yangle = 1.6;
			}
			updateCamera();
		}
		break;
	case WM_MBUTTONDOWN:
		if((settings & 0x10) == 0){
			RAY ray = rayCreate((VEC3){player->xpos,player->ypos,player->zpos},(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
			while(ray.ix>=0&&ray.ix<=properties->lvlSz&&ray.iy>=0&&ray.iy<=properties->lvlSz&&ray.iz>=0&&ray.iz<=properties->lvlSz){
				int block = crds2map(ray.ix,ray.iy,ray.iz);
				if(map[block].id){
					switch(toolSel){
					case 4:{
						CVEC3 scrd = map2crds(block);
						switch(ray.side){
						case 0:
							if(ray.vx < 0.0){
								scrd.x--;
							}
							else{
								scrd.x++;
							}
							break;
						case 1:
							if(ray.vy < 0.0){
								scrd.y--;
							}
							else{
								scrd.y++;
							}
							break;
						case 2:
							if(ray.vz < 0.0){
								scrd.z--;
							}
							else{
								scrd.z++;
							}
							break;
						}
						colorSel.r = scrd.x;
						colorSel.g = scrd.y;
						colorSel.b = scrd.z;
						break;
					}
					case 5:
						colorSel.r = map[block].r;
						colorSel.g = map[block].g;
						colorSel.b = map[block].b;
						break;
					default:
						blockSel   = map[block].id;
						colorSel.r = map[block].r;
						colorSel.g = map[block].g;
						colorSel.b = map[block].b;
						break;
					}
					break;
				}
				rayItterate(&ray);
			}
			break;
		}
	case WM_LBUTTONDOWN:{
			if(!menuSel){
				tools();
			}
			else{
				if(buttonId!=-1){
					if(buttonId<100){
						buttons[buttonId]();
					}
					else if (buttonId < 110){
						levelLoad(fileNames.str[buttonId-100]);
					}
					else{
						levelDelete(fileNames.str[buttonId-110]);
						menuSel = 1;
						buttonC = 0;
						buttonCreate((VEC2){0.05f,-0.35f},1);
						buttonCreate((VEC2){0.05f,-0.42f},0);
						buttonCreate((VEC2){0.05f,-0.28f},2);
						buttonCreate((VEC2){0.05f,-0.21f},3);
					}
				}
			}
		break;
		}
	case WM_RBUTTONDOWN:
			if((settings & 0x10) == 0){
				RAY ray = rayCreate((VEC3){player->xpos,player->ypos,player->zpos},(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
			while(ray.ix>=0&&ray.ix<=properties->lvlSz&&ray.iy>=0&&ray.iy<=properties->lvlSz&&ray.iz>=0&&ray.iz<=properties->lvlSz){
				int block = crds2map(ray.ix,ray.iy,ray.iz);
				if(map[block].id){
					deleteBlock(block);
					break;
				}
				rayItterate(&ray);
			}
			break;
		}
	}
	return DefWindowProc(hwnd,msg,wParam,lParam);
}
WNDCLASS wndclass = {0,proc,0,0,0,0,0,0,name,name};
void physics()
{
	player->xfov = 2;
	for (;;)
	{
		player->xydir = cosf(player->yangle);
		player->xdir  = cosf(player->xangle);
	 	player->ydir  = sinf(player->xangle);
	 	player->zdir  = sinf(player->yangle);
		if (settings & 0x01)
		{
			int amp = 1;
			if(GetKeyState(VK_CONTROL) & 0x80){
				amp = 3;
			}
			if (GetKeyState(0x57) & 0x80){
				player->xpos += player->xdir / 8 * amp;
				player->ypos += player->ydir / 8 * amp;
			}
			if (GetKeyState(0x53) & 0x80){
				player->xpos -= player->xdir / 8 * amp;
				player->ypos -= player->ydir / 8 * amp;
			}
			if (GetKeyState(0x44) & 0x80){
				player->xpos += cosf(player->xangle + PI_05) / 8 * amp;
				player->ypos += sinf(player->xangle + PI_05) / 8 * amp;
			}
			if (GetKeyState(0x41) & 0x80){
				player->xpos -= cosf(player->xangle + PI_05) / 8 * amp;
				player->ypos -= sinf(player->xangle + PI_05) / 8 * amp;
			}
			if (GetKeyState(VK_SPACE) & 0x80){
				player->zpos += 0.15 * amp;
			} 
			if (GetKeyState(VK_LSHIFT) & 0x80){
				player->zpos -= 0.15 * amp;
			}
			if(player->xpos < 0.0){
				player->xpos = 0.0;
			}
			if(player->ypos < 0.0){
				player->ypos = 0.0;
			}
			if(player->zpos < 0.0){
				player->zpos = 0.0;
			}
			if(player->xpos > properties->lvlSz){
				player->xpos = properties->lvlSz;
			}
			if(player->ypos > properties->lvlSz){
				player->ypos = properties->lvlSz;
			}
			if(player->zpos > properties->lvlSz){
				player->zpos = properties->lvlSz;
			}
		}
		else
		{
			if(GetKeyState(VK_LBUTTON) & 0x80){
				spawnEntity(player->xpos,player->ypos,player->zpos,0,0,0,0.3,1);
			}
			if(stamina < 1.0){
				stamina+=0.045;
			}
			else{
				stamina = 1.0;
			}
			float amp = 1;
			if(GetKeyState(VK_LCONTROL) & 0x80){
				amp = 0.3;
			}
			if (GetKeyState(0x57) & 0x80){
				player->xvel += player->xdir / 50 * amp;
				player->yvel += player->ydir / 50 * amp;

			}
			if (GetKeyState(0x53) & 0x80){
				player->xvel -= player->xdir / 50 * amp;
				player->yvel -= player->ydir / 50 * amp;
			}
			if (GetKeyState(0x44) & 0x80){
				player->xvel += cosf(player->xangle + PI_05) / 50 * amp;
				player->yvel += sinf(player->xangle + PI_05) / 50 * amp;
			}
			if (GetKeyState(0x41) & 0x80){
				player->xvel -= cosf(player->xangle + PI_05) / 50 * amp;
				player->yvel -= sinf(player->xangle + PI_05) / 50 * amp;
			}
			player->zvel -= 0.015;

			player->xpos += player->xvel;
			player->ypos += player->yvel;
			player->zpos += player->zvel;

			if (player->zvel < 0){
				if(GetKeyState(VK_LCONTROL) & 0x80){
					hitboxZdown(player->xpos - 0.2, player->ypos - 0.2, player->zpos - 0.7);
					hitboxZdown(player->xpos + 0.2, player->ypos - 0.2, player->zpos - 0.7);
					hitboxZdown(player->xpos - 0.2, player->ypos + 0.2, player->zpos - 0.7);
					hitboxZdown(player->xpos + 0.2, player->ypos + 0.2, player->zpos - 0.7);
				}
				else{
					hitboxZdown(player->xpos - 0.2, player->ypos - 0.2, player->zpos - 1.7);
					hitboxZdown(player->xpos + 0.2, player->ypos - 0.2, player->zpos - 1.7);
					hitboxZdown(player->xpos - 0.2, player->ypos + 0.2, player->zpos - 1.7);
					hitboxZdown(player->xpos + 0.2, player->ypos + 0.2, player->zpos - 1.7);
				}
			}
			else{
				hitboxZup(player->xpos - 0.2, player->ypos - 0.2, player->zpos + 0.2);
				hitboxZup(player->xpos + 0.2, player->ypos - 0.2, player->zpos + 0.2);
				hitboxZup(player->xpos - 0.2, player->ypos + 0.2, player->zpos + 0.2);
				hitboxZup(player->xpos + 0.2, player->ypos + 0.2, player->zpos + 0.2);

			}
			if (player->xvel < 0){
				if(GetKeyState(VK_LCONTROL) & 0x80){
					for(float i = -0.7;i <= 0.2;i+=0.1){
						hitboxXdown(player->xpos - 0.2, player->ypos + 0.2, player->zpos + i);
						hitboxXdown(player->xpos - 0.2, player->ypos - 0.2, player->zpos + i);
					}
				}
				else{
					for(float i = -1.7;i <= 0.2;i+=0.1){
						hitboxXdown(player->xpos - 0.2, player->ypos + 0.2, player->zpos + i);
						hitboxXdown(player->xpos - 0.2, player->ypos - 0.2, player->zpos + i);
					}
				}

			}
			else{
				if(GetKeyState(VK_LCONTROL) & 0x80){
					for(float i = -0.7;i <= 0.2;i+=0.1){
						hitboxXup(player->xpos + 0.2, player->ypos + 0.2, player->zpos + i);
						hitboxXup(player->xpos + 0.2, player->ypos - 0.2, player->zpos + i);
					}
				}
				else{
					for(float i = -1.7;i <= 0.2;i+=0.1){
						hitboxXup(player->xpos + 0.2, player->ypos + 0.2, player->zpos + i);
						hitboxXup(player->xpos + 0.2, player->ypos - 0.2, player->zpos + i);
					}
				}

			}
			if (player->yvel < 0){
				if(GetKeyState(VK_CONTROL) & 0x80){
					for(float i = -0.7;i <= 0.2;i+=0.1){
						hitboxYdown(player->xpos + 0.2, player->ypos - 0.2, player->zpos + i);
						hitboxYdown(player->xpos - 0.2, player->ypos - 0.2, player->zpos + i);
					}
				}
				else{
					for(float i = -1.7;i <= 0.2;i+=0.1){
						hitboxYdown(player->xpos + 0.2, player->ypos - 0.2, player->zpos + i);
						hitboxYdown(player->xpos - 0.2, player->ypos - 0.2, player->zpos + i);
					}
				}

			}
			else{
				if(GetKeyState(VK_LCONTROL) & 0x80){
					for(float i = -0.7;i <= 0.2;i+=0.1){
						hitboxYup(player->xpos + 0.2, player->ypos + 0.2, player->zpos + i);
						hitboxYup(player->xpos - 0.2, player->ypos + 0.2, player->zpos + i);
					}
				}
				else{
					for(float i = -1.7;i <= 0.2;i+=0.1){
						hitboxYup(player->xpos + 0.2, player->ypos + 0.2, player->zpos + i);
						hitboxYup(player->xpos - 0.2, player->ypos + 0.2, player->zpos + i);
					}
				}

			}
			touchStatus &= ~specialBlock[1];
			switch(specialBlock[0]){
			case 12:
				switch(specialBlock[1]){
				case 1:
					player->zvel = 0.5;
					break;
				case 2:
					player->zvel = -0.5;
					break;
				case 4:
					player->xvel = 0.5;
					break;
				case 8:
					player->xvel = -0.5;
					break;
				case 16:
					player->yvel = 0.5;
					break;
				case 32:
					player->yvel = -0.5;
					break;
				}
				break;
			case 49: 
				player->zpos -= player->zvel;
				player->zpos += player->yvel;
				break;
			case 50:
				player->zpos -= player->zvel;
				player->zpos += player->yvel;
				break;
			case 51:
				player->zpos -= player->zvel;
				player->zpos += player->xvel;
				break;
			}
			if (touchStatus & 0x01){
				player->zpos -= player->zvel;
				player->zvel = 0;
				if (GetKeyState(VK_SPACE) & 0x80){
					sound(1);
					player->zvel += 0.2 * stamina;
					player->xvel *= 1.7 * stamina;
					player->yvel *= 1.7 * stamina;
					stamina = 0.0;
				}
			}
			if (touchStatus & 0x02){
				player->zpos -= player->zvel;
				player->zvel = 0;
			}
			if (touchStatus & 0x04){
				player->xpos -= player->xvel;
				player->xvel = 0;
				if(player->zvel < -0.05){
					player->zvel = -0.05;
				}
				if(GetKeyState(VK_SPACE) & 0x80){
					player->zvel += 0.25 * stamina;
					player->xvel += 0.25 * stamina;
					stamina = 0.0;
				}
			}
			if (touchStatus & 0x08){
				player->xpos -= player->xvel;
				player->xvel = 0;
				if(player->zvel < -0.05){
					player->zvel = -0.05;
				}
				if(GetKeyState(VK_SPACE) & 0x80){
					player->zvel += 0.25 * stamina;
					player->xvel += -0.25 * stamina;
					stamina = 0.0;
				}
			}
			if (touchStatus & 0x10){
				player->ypos -= player->yvel;
				player->yvel = 0;
				if(player->zvel < -0.05){
					player->zvel = -0.05;
				}
				if(GetKeyState(VK_SPACE) & 0x80){
					player->zvel += 0.25 * stamina;
					player->yvel += 0.25 * stamina;
					stamina = 0.0;
				}
			}
			if (touchStatus & 0x20){
				player->ypos -= player->yvel;
				player->yvel = 0;
				if(player->zvel < -0.05){
					player->zvel = -0.05;
				}
				if(GetKeyState(VK_SPACE) & 0x80){
					player->zvel += 0.25 * stamina;
					player->yvel += -0.25 * stamina;
					stamina = 0.0;
				}
			}
			if(touchStatus == 64){
				player->zvel = 0;
				player->zpos += 0.1;
			}
			player->xvel /= 1.08;
			player->yvel /= 1.08;
		}
		touchStatus = 0;
		specialBlock[0] = 0;
		specialBlock[1] = 0;
		player->zvel /= 1.003;	
		tick++;
		
		Sleep(15);
		while(settings & 0x10){
			Sleep(1);
		}
	}
}


void main(){
	timeBeginPeriod(1);

	lpmap      = HeapAlloc(GetProcessHeap(),8,sizeof(LPMAP)*MAPRAM);
	map        = HeapAlloc(GetProcessHeap(),8,MAPRAM);
	player     = HeapAlloc(GetProcessHeap(),8,sizeof(PLAYERDATA));
	properties = HeapAlloc(GetProcessHeap(),8,sizeof(PROPERTIES));
	entity     = HeapAlloc(GetProcessHeap(),8,sizeof(ENTITY) * 512);
	button     = HeapAlloc(GetProcessHeap(),8,sizeof(BUTTON) * 256);
	inputStr   = HeapAlloc(GetProcessHeap(),8,256);

	wndclass.hInstance = GetModuleHandleA(0);
	RegisterClassA(&wndclass);
	window = CreateWindowExA(0,name,name,0x90080000,0,0,resy + 16,resx + 39,0,0,wndclass.hInstance,0);
	hInstance = wndclass.hInstance;
	dc = GetDC(window);

	HICON hIcon = LoadImageA(0,"textures/bol.ico",IMAGE_ICON,48,48,LR_LOADFROMFILE);
	SendMessageA(window,WM_SETICON,ICON_SMALL,(long int)hIcon);

	settings = 0x21;

	player->xfov   = 16/9;
	player->yfov   = 1;
	player->xspawn = 5.5;
	player->yspawn = 5.5;
	player->zspawn = 2.5;
	player->zpos   = player->zspawn;
	player->xpos   = player->xspawn;
	player->ypos   = player->yspawn;

	properties->lvlSz          = MAPSZ;
	properties->renderDistance = RENDERDISTANCE;
	properties->xres           = resx;
	properties->yres           = resy;
	properties->fog            = 0.5;
	properties->lmapSz         = LMAPSZ;
	properties->lmapSz2        = MAPSZ*LMAPSZ;
	properties->lmapSz3        = LMAPSZT;

	ShowCursor(0);

	initSound();

	renderingThread      = CreateThread(0,0,openGL,0,0,0);
	physicsThread        = CreateThread(0,0,physics,0,0,0);
	entitiesThread       = CreateThread(0,0,entities,0,0,0);
	ittmapThread         = CreateThread(0,0,ittmap,0,0,0);

	levelgen();

	for(;;){
		while(PeekMessageA(&Msg,window,0,0,0)){
			GetMessageA(&Msg,window,0,0);
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
		Sleep(1);
	}
}
