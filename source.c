#include <windows.h>
#include <math.h>
#include <intrin.h>
#include <stdio.h>

#include "main.h"
#include "tools.h"
#include "network.h"
#include "textbox.h"
#include "ui.h"
#include "console.h"
#include "blockmove.h"
#include "vec4.h"
#include "godrays.h"
#include "reflections.h"
#include "ray.h"

#pragma comment(lib,"winmm.lib")

MAP   *map;
EXRGB *bmap;

LPMAP *lpmap;

MAP   *metadt;
MAP   *metadt2;
MAP   *metadt3;
MAP   *metadt4;
MAP   *metadt5;
MAP   *metadt6;

u8 menuSel;

unsigned char tempVar[2];

u32 settings;

/*
bit
1: fly/walk   : 0x01
2: fullScreen : 0x02
3: lighting   : 0x04
4: fog		  : 0x08
5: ?		  : 0x10
6: ui		  : 0x20
7: subBlock   : 0x40
8: pauze	  : 0x80
9: gameplay   : 0x100
10:vsync      : 0x200
11:smooth     : 0x400
*/

char threadStatus;
char abilities;

PROPERTIES *properties;
PLAYERDATA *player;

RGBA colorSel;
RGBA metadtSel;
RGBA metadt2Sel;
RGBA metadt3Sel;
RGBA metadt4Sel;
RGBA metadt5Sel;
RGBA metadt6Sel;

unsigned char blockSel = 1;
unsigned char toolSel;	

f32 mousex;
f32 mousey;

BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER),resx,resy,1,32,BI_RGB };	

HANDLE renderingThread;
HANDLE consoleThread;
HANDLE physicsThread;
HANDLE entitiesThread;
HANDLE staticEntitiesThread;
HANDLE ittmapThread;
HANDLE godraysThread;
HANDLE networkThread;
HANDLE reflectThread;
HANDLE HDRthread;

const u8 name[] = "3D_engine";

HINSTANCE hInstance;
HWND window;
HDC dc;
MSG Msg;

u8 playerspawnC;
VEC3 playerspawn[16];

i16 offsetB = -1;

void playerDeath(){
	player->health = 100;
	if(playerspawnC){
		player->pos = playerspawn[abs(irnd())%(playerspawnC)];
	}
	else{
		player->pos = (VEC3){5.5f,5.5f,2.8f};
	}
	player->vel = (VEC3){0.0f,0.0f,0.0f};
	/*
	if(!connectStatus){
		entityC = 0;
	}*/
	for(u32 i = 0;i < turretC;i++){
		turret[i].cooldown = 0;
	}
}

i32 hash(i32 x) {
	x += (x << 10);
	x ^= (x >> 6);
	x += (x << 3);
	x ^= (x >> 11);
	x += (x << 15);
	return x;
}

f32 rnd() {
	union p {
		f32 f;
		i32 u;
	}r;
	r.u = hash(__rdtsc());
	r.u &= 0x007fffff;
	r.u |= 0x3f800000;
	return r.f;
}

i32 irnd(){
	return hash(__rdtsc());
}

void updateBlockLight(int pos){
	glMes[glMesC].id = 3;
	glMesC++;
}

void deleteBlock(int pos){
	map[pos].id = 1;
	glMes[glMesC].id = 3;
	glMesC++;
}

VEC3 screenUVto3D(VEC2 uv){
	VEC3 ang;
	ang.x = (player->xdir * player->xydir - player->xdir * player->zdir * uv.y) - player->ydir * uv.x;
	ang.y = (player->ydir * player->xydir - player->ydir * player->zdir * uv.y) + player->xdir * uv.x;
	ang.z = player->zdir + player->xydir * uv.y;
	return VEC3normalize(ang);
}

void levelSave(char *lname){
	char *name = HeapAlloc(GetProcessHeap(),8,strlen(lname)+18);
	memcpy(name,"levels/",7);
	memcpy(name+7,lname,strlen(lname));
	memcpy(name+strlen(lname)+7,".lvl\0",5);
	CreateDirectoryA("levels",0);
	HANDLE h = CreateFileA(name,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	WriteFile(h,&properties->lvlSz,1,0,0);
	WriteFile(h,&properties->lmapSz,4,0,0);
	WriteFile(h,map,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0,0);
	WriteFile(h,metadt,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0,0);
	WriteFile(h,metadt2,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0,0);
	WriteFile(h,metadt3,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0,0);
	WriteFile(h,metadt4,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0,0);
	WriteFile(h,metadt5,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0,0);
	WriteFile(h,metadt6,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0,0);
	if(lmapC){
		WriteFile(h,lpmap,properties->lvlSz*properties->lvlSz*properties->lvlSz*sizeof(LPMAP),0,0);
		WriteFile(h,&properties->lmapSz,1,0,0);
		WriteFile(h,&lmapC,4,0,0);
		WriteFile(h,lmap,lmapC*properties->lmapSz*properties->lmapSz*sizeof(EXRGB),0,0);
	}
	HeapFree(GetProcessHeap(),0,name);
	CloseHandle(h);
	memcpy(name+strlen(lname)+7,"Backup.lvl\0",11);
	h = CreateFileA(name,GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	WriteFile(h,&properties->lvlSz,2,0,0);
	WriteFile(h,&properties->lmapSz,1,0,0);
	WriteFile(h,map,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0,0);
	WriteFile(h,metadt,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0,0);
	WriteFile(h,metadt2,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0,0);
	WriteFile(h,metadt3,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0,0);
	WriteFile(h,metadt4,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0,0);
	WriteFile(h,metadt5,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0,0);
	WriteFile(h,metadt6,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0,0);
	CloseHandle(h);
}

void levelLoad(char *lname){
	char *name = HeapAlloc(GetProcessHeap(),8,strlen(lname)+12);
	memcpy(name,"levels/",7);
	memcpy(name+7,lname,strlen(lname));
	memcpy(name+strlen(lname)+7,".lvl\0",5);
	HANDLE h = CreateFileA(name,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	ReadFile(h,&properties->lvlSz,1,0,0);
	ReadFile(h,&properties->lmapSz,4,0,0);
	map        = HeapAlloc(GetProcessHeap(),8,properties->lvlSz*properties->lvlSz*properties->lvlSz*sizeof(MAP));
	ReadFile(h,map,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0,0);
	ReadFile(h,metadt,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0,0);
	ReadFile(h,metadt2,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0,0);
	ReadFile(h,metadt3,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0,0);
	ReadFile(h,metadt4,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0,0);
	ReadFile(h,metadt5,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0,0);
	ReadFile(h,metadt6,properties->lvlSz*properties->lvlSz*properties->lvlSz*4,0,0);
	ReadFile(h,lpmap,properties->lvlSz*properties->lvlSz*properties->lvlSz*sizeof(LPMAP),0,0);
	ReadFile(h,&properties->lmapSz,1,0,0);
	ReadFile(h,&lmapC,4,0,0);
	lmap = HeapAlloc(GetProcessHeap(),8,lmapC*properties->lmapSz*properties->lmapSz*sizeof(EXRGB));
	bmap = HeapAlloc(GetProcessHeap(),8,lmapC*properties->lmapSz*properties->lmapSz*sizeof(EXRGB));
	ReadFile(h,lmap,lmapC*properties->lmapSz*properties->lmapSz*sizeof(EXRGB),0,0);
	HeapFree(GetProcessHeap(),0,name);
	for(u32 i = 0;i < BLOCKCOUNT;i++){
		switch(map[i].id){
		case BLOCK_SPAWN:{
			CVEC3 spwncrd = map2crds(i);
			playerspawn[playerspawnC] = (VEC3){spwncrd.x+0.5f,spwncrd.y+0.5f,spwncrd.z+2.25f};
			playerspawnC++;
			break;
		}
		case BLOCK_CUBE:
			if(metadt4[i].b&&metadt4[i].b!=1){
				CVEC3 crd = map2crds(i);
				turret[turretC].pos = (VEC3){(f32)crd.x+0.5f,(f32)crd.y+0.5f,(f32)crd.z+0.5f};
				turret[turretC].id  = metadt4[i].b-2;
				turret[turretC].power = metadt4[i].g;
				turret[turretC].totalCooldown = metadt4[i].id+1;
				turretC++;
			}
			break;
		case BLOCK_AMBIENTLIGHT:
			star[starC].skyCol = (RGB){metadt2[i].r,metadt2[i].g,metadt2[i].id};
			star[starC].pos = VEC3normalize((VEC3){127.0f-metadt[i].r,127.0f-metadt[i].g,127.0f-metadt[i].id});
			star[starC].col = (RGB){map[i].r,map[i].g,map[i].b};
			starC++;
			break;
		}
	}
	for(u32 i = 0;i < lmapC*properties->lmapSz*properties->lmapSz;i++){
		bmap[i] = lmap[i];
	}
	CloseHandle(h);
	glMes[glMesC].id = 3;
	glMesC++;
	glMes[glMesC].id = 6;
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

void mainMenuLoad(){
	menuSel = 1;
	buttonCreate((VEC2){-0.059f,-0.33f},1);
	buttonCreate((VEC2){-0.059f,-0.40f},0);
	buttonCreate((VEC2){-0.059f,-0.26f},2);
	buttonCreate((VEC2){-0.059f,-0.19f},3);
	buttonCreate((VEC2){-0.059f,-0.12f},8);
	buttonCreate((VEC2){-0.059f,-0.05f},9);
	if((settings&SETTINGS_GAMEPLAY)==0){
		buttonCreate((VEC2){-0.059f,0.02f},5);
		buttonCreate((VEC2){-0.019f,0.02f},4);
	}
}

void closeEngine(){
	if(~settings&SETTINGS_GAMEPLAY&&~networkSettings&SETTINGS_MOVEMENT){
		levelSave("level");
	}
	settings &= ~SETTINGS_PAUZE;
	HANDLE h = CreateFileA("config.cfg",GENERIC_WRITE,0,0,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	WriteFile(h,&settings,4,0,0);
	WriteFile(h,&player->fov.y,4,0,0);
	WriteFile(h,&properties->sensitivity,4,0,0);
	WriteFile(h,&sliderValues.serverIP,sizeof(IPADDRESS),0,0);
	WriteFile(h,playerName,20,0,0);
	CloseHandle(h);
	ExitProcess(0);
}

long _stdcall proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
	case WM_QUIT:
	case WM_CLOSE:
	case WM_DESTROY:
		closeEngine();
	case WM_ACTIVATE:
		switch(wParam){
		case WA_INACTIVE:
			SuspendThread(physicsThread);
			SuspendThread(entitiesThread);
			settings ^= SETTINGS_PAUZE;
			break;
		default:
			ResumeThread(entitiesThread);
			ResumeThread(physicsThread);
			settings ^= SETTINGS_PAUZE;
			break;
		}
		break;
	case WM_MOVE:
		properties->windowOffsetX = lParam & 0xffff;
		properties->windowOffsetY = lParam >> 16;
		break;
	case WM_SIZE:
		properties->xres = lParam & 0xffff;
		properties->yres = lParam >> 16;
		player->fov.x = player->fov.y*((f32)properties->xres/properties->yres);
		if(renderingThread){
			glMes[glMesC].id = 0;
			glMesC++;
		}
		break;
	case WM_KEYDOWN:
		if(wParam >= 0x30 && wParam <= 0x5a){
			switch(menuSel){
			case 3:
				if(wParam>0x2f&&wParam<0x39){
					inputStr[strlen(inputStr)] = wParam;
				}
				else{
					inputStr[strlen(inputStr)] = wParam-0x20;
				}
				break;
			}
			if(textboxSel!=-1&& strlen(textbox[textboxSel].text) < 20){
				if(wParam>0x2f&&wParam<0x3a){
					textbox[textboxSel].text[strlen(textbox[textboxSel].text)] = wParam;
				}
				else{
					textbox[textboxSel].text[strlen(textbox[textboxSel].text)] = wParam+0x20;
				}
			}
		}
		switch(wParam){
		case 0x54:
			if(menuSel==0){
				menuSel = 9;
				ShowCursor(1);
				textboxSel = textboxC;
				textboxCreate((VEC2){-0.69f,0.8f},2);
				buttonCreate((VEC2){-0.37f,0.76f},12);
			}
			break;
		case VK_ADD:
			blockSel++;
			break;
		case VK_SUBTRACT:
			blockSel--;
			break;
		case VK_UP:
			if(chatLineSel < CHATSZ){
				switch(menuSel){
				case 8:
					chatLineSel++;
					for(u32 i = 0;i < textboxSel;i++){
						if(textbox[i].id == 1){
							memcpy(textbox[i].text,chat[chatLineSel].text,20);
							break;
						}
					}
					break;
				}
			}
			break;
		case VK_DOWN:
			if(chatLineSel > 0){
				switch(menuSel){
				case 8:
					chatLineSel--;
					for(u32 i = 0;i < textboxSel;i++){
						if(textbox[i].id == 1){
							memcpy(textbox[i].text,chat[chatLineSel].text,20);
							break;
						}
					}
					break;
				}
			}
			break;
		case VK_SPACE:
			if(textboxSel!=-1&&strlen(textbox[textboxSel].text) < 20){
				textbox[textboxSel].text[strlen(textbox[textboxSel].text)] = wParam;
			}
			break;
		case 0x52:
			if(GetKeyState(VK_LCONTROL)&0x80){
				do{
					metadtSel.r = irnd();
				}
				while(metadtSel.r - metadtSel.b < 0 || metadtSel.r + metadtSel.b > 255);
				do{
					metadtSel.a = irnd();
				}
				while(metadtSel.a - metadtSel.b < 0 || metadtSel.a + metadtSel.b > 255);
			}
			else{
				colorSel.r = irnd();
				colorSel.g = irnd();
				colorSel.b = irnd();
			}
			break;
		case VK_OEM_PERIOD:
			if(textboxSel!=-1&& strlen(textbox[textboxSel].text) < 20){
				textbox[textboxSel].text[strlen(textbox[textboxSel].text)] = '.';
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
			case 8:
				for(u32 i = 0;i < textboxC;i++){
					if(textbox[i].id == 1){
						if(textbox[i].text[0]){
							for(u32 i2 = CHATSZ-1;i2 > 0;i2--){
								memcpy(chat[i2].text,chat[i2-1].text,40);
								chat[i2].timer = chat[i2-1].timer;
							}
							executeCommand(textbox[i].text);
							strcpy(chat[0].text,textbox[i].text);
							_strset(textbox[i].text,0);
						}
					}
				}
				break;
			case 9:
				for(u32 i = 0;i < textboxC;i++){
					if(textbox[i].id == 2){
						if(textbox[i].text[0]){
							for(u32 i2 = CHATSZ-1;i2 > 0;i2--){
								memcpy(chat[i2].text,chat[i2-1].text,40);
								chat[i2].timer = chat[i2-1].timer;
							}
							chat[0].timer = 1200;
							strcpy(chat[0].text,textbox[i].text);
							_strset(textbox[i].text,0);
						}
					}
				}
				break;
			}
			break;
		case VK_BACK:
			switch(menuSel){
			case 3:
				inputStr[strlen(inputStr)-1] = 0;
				break;
			}
			if(textboxSel!=-1 && strlen(textbox[textboxSel].text)){
				textbox[textboxSel].text[strlen(textbox[textboxSel].text)-1] = 0;
			}
			break;
		case VK_F1:
			settings ^= SETTINGS_MOVEMENT;
			break;
		case VK_F2:
			CreateThread(0,0,updateLight2,0,0,0);
			break;
		case VK_F3:
			settings ^= SETTINGS_LIGHTING;
			break;
		case VK_F4:
			settings ^= SETTINGS_GAMEPLAY;
			break;
		case VK_F5:
			settings ^= SETTINGS_SUBBLOCK;
			break;
		case VK_F6:
			settings ^= SETTINGS_SMOOTH;
			break;
		case VK_F11:
			settings ^= SETTINGS_UI;
			break;
		case VK_ESCAPE:
			switch(menuSel){
			case 0:
				ShowCursor(1);
				mainMenuLoad();
				SetCursorPos(properties->xres/2+properties->windowOffsetX,properties->yres/2+properties->windowOffsetY);
				break;
			case 4:
				SetCursorPos(properties->xres/2+properties->windowOffsetX,properties->yres/2+properties->windowOffsetY);
			case 1:
				sliderId = -1;
				sliderC = 0;
				menuSel = 0;
				buttonC = 0;
				ShowCursor(0);
				break;
			case 2:
				menuSel = 1;
				buttonC = 0;
				sliderC = 0;

				fileNames.strC = 0;
				break;
			case 3:
				buttonC = 0;
				mainMenuLoad();
				ZeroMemory(inputStr,255);
				break;
			case 5:
				buttonC = 0;
				sliderC = 0;
				mainMenuLoad();
				break;
			case 6:
				for(u32 i = 0;i < textboxC;i++){
					if(textbox[i].id==0){
						strcpy(playerName,textbox[i].text);
						_strset(textbox[i].text,0);
						break;
					}
				}
				buttonC = 0;
				sliderC = 0;
				textboxC = 0;
				textboxSel = -1;
				mainMenuLoad();
				break;
			case 7:
				if(connectStatus == 1){
					connectStatus = 0;
					sliderCreate((VEC2){ 0.190f,-0.12f },17);
					sliderCreate((VEC2){ 0.190f,-0.19f },18);
					sliderCreate((VEC2){ 0.190f,-0.26f },19);
					sliderCreate((VEC2){ 0.190f,-0.33f },20);
					buttonCreate((VEC2){ -0.059f,-0.40 },10);
					menuSel = 6;
					ShowCursor(1);
				}
				break;
			case 8:
				menuSel = 0;
				buttonC = 0;
				textboxC = 0;
				textboxSel = -1;
				chatLineSel = -1;
				ShowCursor(0);
				SetCursorPos(properties->xres/2+properties->windowOffsetX,properties->yres/2+properties->windowOffsetY);
				break;
			case 9:
				menuSel = 0;
				buttonC = 0;
				textboxC = 0;
				textboxSel = -1;
				chatLineSel = -1;
				ShowCursor(0);
				SetCursorPos(properties->xres/2+properties->windowOffsetX,properties->yres/2+properties->windowOffsetY);
				break;
			}
			break;
		case VK_CONTROL:
			player->hitboxWantedHeight = 0.7f;
			player->stamina = 0.0f;
			break;
		case VK_OEM_3:
			menuSel = 8;
			ShowCursor(1);
			textboxSel = textboxC;
			textboxCreate((VEC2){-0.69f,0.8f},1);
			buttonCreate((VEC2){-0.37f,0.76f},11);
			break;
		case VK_PRIOR:
			toolSel++;
			break;
		case VK_NEXT:
			toolSel--;
			break;
		}
		break;
	case WM_KEYUP:
		switch(wParam){
		case VK_SPACE:
			touchedSpace = 0;
			break;
		case VK_CONTROL:
			player->hitboxWantedHeightQueued = 1.7f;
			break;
		}
		break;
	case WM_MOUSEMOVE:
		if(!menuSel){
			POINT curp;
			GetCursorPos(&curp);
			mousex = ((f32)curp.x - properties->xres/2 - properties->windowOffsetX) / 80.0f;
			mousey = ((f32)curp.y - properties->yres/2 - properties->windowOffsetY) / 80.0f;
			SetCursorPos(properties->xres/2 +properties->windowOffsetX,properties->yres/2 +properties->windowOffsetY);
			player->xangle += mousex * properties->sensitivity;
			player->yangle -= mousey * properties->sensitivity;
			if(player->yangle < -1.6f){
				player->yangle = -1.6f;
			}
			if(player->yangle > 1.6f){
				player->yangle = 1.6f;
			}
		}
		break;
	case WM_MBUTTONDOWN:
		switch(toolSel){
		case 6:{
			RAY3D ray = ray3dCreate(player->pos,(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
			i32 lmapLoc = getLmapLocation(&ray);
			if(lmapLoc!=-1){
				lmapLoc /= properties->lmapSz*properties->lmapSz;
				lmapLoc *= properties->lmapSz*properties->lmapSz;
				for(u32 i = 0;i < properties->lmapSz*properties->lmapSz;i++){
					VEC3 bcolor = {bmap[lmapLoc+i].r,bmap[lmapLoc+i].g,bmap[lmapLoc+i].b};
					VEC3 lcolor = {lmap[lmapLoc+i].r,lmap[lmapLoc+i].g,lmap[lmapLoc+i].b};
					f32 mcol = fmaxf(VEC3max(lcolor),VEC3max(bcolor));
					VEC3 color = VEC3mulR(VEC3divVEC3R(lcolor,bcolor),255.0f);
					textureBuf[i].r = color.r;
					textureBuf[i].g = color.g;
					textureBuf[i].b = color.b;
				}
			}
			break;
		}
		case 7:{
			RAY3D ray = ray3dCreate(player->pos,(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
			i32 lmapLoc = getLmapLocation(&ray);
			if(lmapLoc!=-1){
				VEC3 bcolor = {bmap[lmapLoc].r,bmap[lmapLoc].g,bmap[lmapLoc].b};
				VEC3 lcolor = {lmap[lmapLoc].r,lmap[lmapLoc].g,lmap[lmapLoc].b};
				f32 mcol = fmaxf(VEC3max(lcolor),VEC3max(bcolor));
				VEC3 color = VEC3mulR(VEC3divVEC3R(lcolor,bcolor),255.0f);
				colorSel = (RGBA){color.r,color.g,color.b};
			}
			break;
		}
		default:{
			RAY3D ray = ray3dCreate(player->pos,(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
			while(ray.roundPos.x>=0&&ray.roundPos.x<properties->lvlSz&&ray.roundPos.y>=0&&ray.roundPos.y<properties->lvlSz&&ray.roundPos.z>=0&&ray.roundPos.z<properties->lvlSz){
				u32 block = crds2map(ray.roundPos.x,ray.roundPos.y,ray.roundPos.z);
				if(map[block].id!=1){
					blockSel   = map[block].id;
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
					break;
				}
				ray3dItterate(&ray);
			}
			}
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		switch(menuSel){
		case 0:
			if(settings & SETTINGS_GAMEPLAY){
				RAY3D ray = ray3dCreate(player->pos,(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
				while(ray.roundPos.x>=0&&ray.roundPos.x<properties->lvlSz&&ray.roundPos.y>=0&&ray.roundPos.y<properties->lvlSz&&ray.roundPos.z>=0&&ray.roundPos.z<properties->lvlSz){
					u32 block = crds2map(ray.roundPos.x,ray.roundPos.y,ray.roundPos.z);
					switch(map[block].id){
					case BLOCK_CUBE:
						if(metadt4[block].b==1){
							for(u32 i = 0;i < blockmoveC;i++){
								if(block == blockmove[i].block){
									goto end;
								}
							}
							createMoveBlock(block);
							goto end;
						}
						break;
					}
					ray3dItterate(&ray);
					continue;
				end:
					break;
				}
			}
			else{
				tools();
			}
			break;
		default:
			for(u32 i = 0;i < textboxC;i++){
				if(mousePos.x > textbox[i].pos.x - 0.2666666667f && mousePos.x < textbox[i].pos.x + 0.26666667f
					&& mousePos.y > textbox[i].pos.y - 0.09f && mousePos.y < textbox[i].pos.y){
					textboxSel = i;
					goto foundTextbox;
				}
			}
			textboxSel = -1;
		foundTextbox:
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
			break;
		}
		break;
	case WM_RBUTTONDOWN:
		if(settings & ~SETTINGS_GAMEPLAY){
			switch(toolSel){
			case 0:	
			case 1:{
				RAY3D ray = ray3dCreate(player->pos,(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
				while(ray.roundPos.x>=0&&ray.roundPos.x<properties->lvlSz&&ray.roundPos.y>=0&&ray.roundPos.y<properties->lvlSz&&ray.roundPos.z>=0&&ray.roundPos.z<properties->lvlSz){
					int block = crds2map(ray.roundPos.x,ray.roundPos.y,ray.roundPos.z);
					if(map[block].id!=1){
						deleteBlock(block);
						break;
					}
					ray3dItterate(&ray);
				}
				break;
			}
			}
		}
		break;
	}
	return DefWindowProcA(hwnd,msg,wParam,lParam);
}
WNDCLASS wndclass = {0,proc,0,0,0,0,0,0,name,name};
void physics(){
	for (;;){
		if(GetKeyState(VK_LBUTTON)&0x80){
			switch(toolSel){
			case 7:{
				RAY3D ray = ray3dCreate(player->pos,(VEC3){player->xdir*player->xydir,player->ydir*player->xydir,player->zdir});
				i32 l = getLmapLocation(&ray);
				if(l!=-1){
					lmap[l].r = (f32)bmap[l].r * colorSel.r / 255.0f;
					lmap[l].g = (f32)bmap[l].g * colorSel.g / 255.0f;
					lmap[l].b = (f32)bmap[l].b * colorSel.b / 255.0f;
					glMes[glMesC].data1 = l;
					glMes[glMesC].id = 7;
					glMesC++;
				}
				break;
			}
			}
		}
		player->xydir = cosf(player->yangle);
		player->xdir  = cosf(player->xangle);
	 	player->ydir  = sinf(player->xangle);
	 	player->zdir  = sinf(player->yangle);
		if(settings & SETTINGS_MOVEMENT){
			if(menuSel == 0){
				int amp = 1;
				if(GetKeyState(VK_CONTROL) & 0x80){
					amp = 3;
				}
				if (GetKeyState(0x57) & 0x80){
					if(GetKeyState(0x44) & 0x80 || GetKeyState(0x41) & 0x80){
						player->pos.x += player->xdir * player->flightSpeed * amp * 0.7071f;
						player->pos.y += player->ydir * player->flightSpeed * amp * 0.7071f;
					}
					else{
						player->pos.x += player->xdir * player->flightSpeed * amp;
						player->pos.y += player->ydir * player->flightSpeed * amp;
					}
				}
				if (GetKeyState(0x53) & 0x80){
					if(GetKeyState(0x44) & 0x80 || GetKeyState(0x41) & 0x80){
						player->pos.x -= player->xdir * player->flightSpeed * amp * 0.7071f;
						player->pos.y -= player->ydir * player->flightSpeed * amp * 0.7071f;
					}
					else{
						player->pos.x -= player->xdir * player->flightSpeed * amp;
						player->pos.y -= player->ydir * player->flightSpeed * amp;
					}
				}
				if (GetKeyState(0x44) & 0x80){
					if(GetKeyState(0x53) & 0x80 || GetKeyState(0x57) & 0x80){
						player->pos.x += cosf(player->xangle + PI_05) * player->flightSpeed * amp * 0.7071f;
						player->pos.y += sinf(player->xangle + PI_05) * player->flightSpeed * amp * 0.7071f;
					}
					else{
						player->pos.x += cosf(player->xangle + PI_05) * player->flightSpeed * amp;
						player->pos.y += sinf(player->xangle + PI_05) * player->flightSpeed * amp;
					}
				}
				if (GetKeyState(0x41) & 0x80){
					if(GetKeyState(0x53) & 0x80 || GetKeyState(0x57) & 0x80){
						player->pos.x -= cosf(player->xangle + PI_05) * player->flightSpeed * amp * 0.7071f;
						player->pos.y -= sinf(player->xangle + PI_05) * player->flightSpeed * amp * 0.7071f;
					}
					else{
						player->pos.x -= cosf(player->xangle + PI_05) * player->flightSpeed * amp;
						player->pos.y -= sinf(player->xangle + PI_05) * player->flightSpeed * amp;
					}
				}
				if (GetKeyState(VK_SPACE) & 0x80){
					player->pos.z += player->flightSpeed * amp;
				} 
				if (GetKeyState(VK_LSHIFT) & 0x80){
					player->pos.z -= player->flightSpeed * amp;
				}
			}
		}
		else{
			if(settings & SETTINGS_GAMEPLAY){
				if(player->aniTime){
					switch(player->aniType){
					default:
						player->aniTime--;
						break;
					}
				}
				if(GetKeyState(VK_LBUTTON)&0x80&&(menuSel==0)){
					switch(player->weaponEquiped){
					case 1:
						if(!player->shotCooldown){
							player->recoil.y += 0.1f;
							player->recoil.x += (rnd()-1.5f)/15.0f;
							RAY3D ray = ray3dCreate(player->pos,(VEC3){player->xdir*player->xydir*0.5f,player->ydir*player->xydir*0.5f,player->zdir*0.5f});
							getLmapLocation(&ray);
							for(u32 i = 0;i < entityC;i++){
								if(entity.cpu[i].id==4){
									spawnEntity((VEC3){player->pos.x,player->pos.y,player->pos.z-0.1f},VEC3mulR((VEC3){player->xdir*player->xydir*0.5f,player->ydir*player->xydir*0.5f,player->zdir*0.5f},1.5f),0);
									VEC3mul(&entity.gpu[i].color,10.0f);
									entity.cpu[i].aniTime = 11;
									entity.cpu[i].aniType = 2;
									break;
								}
							}
							spawnEntityEx(VEC3subVEC3R(player->pos,(VEC3){-player->xdir,-player->ydir,0.2f}),VEC3subVEC3R(getCoords(ray),player->pos),(VEC3){0.0f,0.0f,0.0f},10,(VEC3){0.5f,0.2f,0.2f});
							player->shotCooldown = 50;
							if(connectStatus){
								packetID = 1;
								packetdata.pos1 = player->pos;
								packetdata.pos2 = VEC3subVEC3R(getCoords(ray),player->pos);
							}
						}
						break;	
					}
				}
				if(player->shotCooldown){
					player->shotCooldown--;
				}
			}
			if(menuSel==0){
				f32 ampli = 1.0f;
				if(player->hitboxHeight<1.7f){
					ampli = 0.33f;
				}
				if(GetKeyState(0x57) & 0x80){
					if(GetKeyState(0x44) & 0x80 || GetKeyState(0x41) & 0x80){
						player->vel.x += player->xdir * player->movementSpeed * ampli * 0.7071f;
						player->vel.y += player->ydir * player->movementSpeed * ampli * 0.7071f;
					}
					else{
						player->vel.x += player->xdir * player->movementSpeed * ampli;
						player->vel.y += player->ydir * player->movementSpeed * ampli;
					}
				}
				if(GetKeyState(0x53) & 0x80){
					if(GetKeyState(0x44) & 0x80 || GetKeyState(0x41) & 0x80){
						player->vel.x -= player->xdir * player->movementSpeed * ampli * 0.7071f;
						player->vel.y -= player->ydir * player->movementSpeed * ampli * 0.7071f;
					}
					else{
						player->vel.x -= player->xdir * player->movementSpeed * ampli;
						player->vel.y -= player->ydir * player->movementSpeed * ampli;
					} 
				}
				if(GetKeyState(0x44) & 0x80){
					if(GetKeyState(0x53) & 0x80 || GetKeyState(0x57) & 0x80){
						player->vel.x += cosf(player->xangle + PI_05) * player->movementSpeed * ampli * 0.7071f;
						player->vel.y += sinf(player->xangle + PI_05) * player->movementSpeed * ampli * 0.7071f;
					}
					else{
						player->vel.x += cosf(player->xangle + PI_05) * player->movementSpeed * ampli;
						player->vel.y += sinf(player->xangle + PI_05) * player->movementSpeed * ampli;
					}
				}
				if(GetKeyState(0x41) & 0x80){
					if(GetKeyState(0x53) & 0x80 || GetKeyState(0x57) & 0x80){
						player->vel.x -= cosf(player->xangle + PI_05) * player->movementSpeed * ampli * 0.7071f;
						player->vel.y -= sinf(player->xangle + PI_05) * player->movementSpeed * ampli * 0.7071f;
					}
					else{
						player->vel.x -= cosf(player->xangle + PI_05) * player->movementSpeed * ampli;
						player->vel.y -= sinf(player->xangle + PI_05) * player->movementSpeed * ampli;
					}
				}
			}
			player->vel.z -= properties->gravity;

			player->yangle += player->recoil.y / 4.0f;
			player->recoil.y *= 0.75f; 
			player->xangle += player->recoil.x / 4.0f;
			player->recoil.x *= 0.75f; 

			VEC3addVEC3(&player->pos,player->vel);
			
			playerWorldCollision();
		}
		if(player->pos.x < 0.0f){
			player->pos.x = 0.0f;
		}
		if(player->pos.y < 0.0f){
			player->pos.y = 0.0f;
		}
		if(player->pos.z < 0.0f){
			player->pos.z = 0.0f;
		}
		if(player->pos.x > properties->lvlSz){
			player->pos.x = properties->lvlSz;
		}
		if(player->pos.y > properties->lvlSz){
			player->pos.y = properties->lvlSz;
		}
		if(player->pos.z > properties->lvlSz){
			player->pos.z = properties->lvlSz;
		}
		player->vel.z *= properties->airFrictionVert;
		tick++;
		Sleep(3);
	}
}


void main(){
	timeBeginPeriod(1);

	player     = HeapAlloc(GetProcessHeap(),8,sizeof(PLAYERDATA));
	properties = HeapAlloc(GetProcessHeap(),8,sizeof(PROPERTIES));

	properties->lvlSz           = MAPSZ;
	properties->lmapSzb         = LMAPSZ;
	properties->gravity         = 0.0013f;
	properties->airFrictionHor  = 0.987f;
	properties->airFrictionVert = 0.998f;
	properties->groundFriction  = 0.967f;
	properties->godrayAmm       = 1.0f;
	properties->godrayRes       = 64;
	properties->reflectRes      = 128;
	properties->rayAcceleration = 1;

	player->flightSpeed = 0.025f;
	player->hitboxHeight = 1.7f;
	player->hitboxWantedHeight = 1.7f;
	player->health = 100;
	player->jumpHeight = 0.06f;
	player->movementSpeed = 0.0008f;

	lpmap      = HeapAlloc(GetProcessHeap(),8,sizeof(LPMAP)*BLOCKCOUNT);
	map        = HeapAlloc(GetProcessHeap(),8,sizeof(MAP)*BLOCKCOUNT);
	metadt     = HeapAlloc(GetProcessHeap(),8,sizeof(MAP)*BLOCKCOUNT);
	metadt2    = HeapAlloc(GetProcessHeap(),8,sizeof(MAP)*BLOCKCOUNT);
	metadt3    = HeapAlloc(GetProcessHeap(),8,sizeof(MAP)*BLOCKCOUNT);
	metadt4    = HeapAlloc(GetProcessHeap(),8,sizeof(MAP)*BLOCKCOUNT);
	metadt5    = HeapAlloc(GetProcessHeap(),8,sizeof(MAP)*BLOCKCOUNT);
	metadt6    = HeapAlloc(GetProcessHeap(),8,sizeof(MAP)*BLOCKCOUNT);
	entity.gpu = HeapAlloc(GetProcessHeap(),8,sizeof(GPUDATA) * 128);
	entity.cpu = HeapAlloc(GetProcessHeap(),8,sizeof(CPUDATA) * 128);
	button     = HeapAlloc(GetProcessHeap(),8,sizeof(BUTTON) * 256);
	slider     = HeapAlloc(GetProcessHeap(),8,sizeof(BUTTON) * 256);
	inputStr   = HeapAlloc(GetProcessHeap(),8,256);
	godraymap  = HeapAlloc(GetProcessHeap(),8,sizeof(VEC3)*properties->godrayRes*properties->godrayRes);
	godraymapB = HeapAlloc(GetProcessHeap(),8,sizeof(VEC3)*properties->godrayRes*properties->godrayRes);
	reflectmap = HeapAlloc(GetProcessHeap(),8,sizeof(VEC3)*properties->reflectRes*properties->reflectRes);
	turret     = HeapAlloc(GetProcessHeap(),8,sizeof(TURRET) * 1024);
	star       = HeapAlloc(GetProcessHeap(),8,sizeof(STAR)*3);
	textbox    = HeapAlloc(GetProcessHeap(),8,sizeof(TEXTBOX)*16);
	playerName = HeapAlloc(GetProcessHeap(),8,20);
	blockmove  = HeapAlloc(GetProcessHeap(),8,sizeof(BLOCKMOVE)*64);
	textureBuf = HeapAlloc(GetProcessHeap(),8,sizeof(RGB)*64*64);

	for(u32 i = 0;i < CHATSZ;i++){
		chat[i].text = HeapAlloc(GetProcessHeap(),8,40);
	}

	skyboxTexture = HeapAlloc(GetProcessHeap(),8,skyboxSz*skyboxSz*sizeof(RGB));

	entityTexture = HeapAlloc(GetProcessHeap(),8,ENTITYTEXTSZ*ENTITYTEXTSZ*sizeof(RGB)*128);

	properties->xres = GetSystemMetrics(SM_CXSCREEN);
	properties->yres = GetSystemMetrics(SM_CYSCREEN);

	wndclass.hInstance = GetModuleHandleA(0);
	RegisterClassA(&wndclass);
	window = CreateWindowExA(0,name,name,0x90080000,0,0,properties->xres,properties->yres,0,0,wndclass.hInstance,0);
	hInstance = wndclass.hInstance;
	dc = GetDC(window);

	HICON hIcon = LoadImageA(0,"textures/bol.ico",IMAGE_ICON,48,48,LR_LOADFROMFILE);
	SendMessageA(window,WM_SETICON,ICON_SMALL,(long int)hIcon);

	renderingThread = CreateThread(0,0,openGL,0,0,0);
	
	while(!openglINIT){
		Sleep(1);
	}

	HANDLE h = CreateFileA("config.cfg",GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if(h!=-1){
		ReadFile(h,&settings,4,0,0);
		ReadFile(h,&player->fov.y,4,0,0);
		ReadFile(h,&properties->sensitivity,4,0,0);
		ReadFile(h,&sliderValues.serverIP,sizeof(IPADDRESS),0,0);
		ReadFile(h,playerName,20,0,0);

		sliderValues.fov = player->fov.y*127.5f;
		sliderValues.sensitivity = properties->sensitivity*255.0f;

		glMes[glMesC].id = 10;
		glMesC++;
		glMes[glMesC].id = 13;
		glMesC++;

		player->fov.x = player->fov.y*16.0f/9.0f;
	}
	else{
		settings = SETTINGS_UI | SETTINGS_LIGHTING | SETTINGS_MOVEMENT;
		player->fov.x   = 16.0f/9.0f;
		player->fov.y   = 1.0f;
		properties->sensitivity = 0.5f;
	}

	CloseHandle(h);

	levelgen();

	if(playerspawnC){
		player->pos = playerspawn[abs(irnd())%(playerspawnC)];
	}
	else{
		player->pos = (VEC3){5.5f,5.5f,2.8f};
	}

	ShowCursor(0);

	initSound();
	initOpenCL();

	physicsThread        = CreateThread(0,0,physics,0,0,0);
	entitiesThread       = CreateThread(0,0,entities,0,0,0);
	ittmapThread         = CreateThread(0,0,ittmap,0,0,0);
	godraysThread        = CreateThread(0,0,genGodraysMap,0,0,0);
	reflectThread        = CreateThread(0,0,genReflectMap,0,0,0);

	HDRthread            = CreateThread(0,0,HDR,0,0,0);

	while(GetMessageA(&Msg,window,0,0)>0){
		TranslateMessage(&Msg);
		DispatchMessageW(&Msg);
	}
}
