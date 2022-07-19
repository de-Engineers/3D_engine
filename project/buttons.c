#include <windows.h>
#include <stdio.h>
#include <intrin.h>
#include "main.h"

char buttonId;

BUTTON *button;

unsigned char buttonC;

STRINGS fileNames;

inline void buttonCreate(VEC2 pos,unsigned char id){
	button[buttonC].pos.x = pos.x;
	button[buttonC].pos.y = pos.y;
	button[buttonC].id    = id;
	buttonC++;
}

void quitButton(){
	levelSave("level");
	ExitProcess(0);	
}

void genNewWorldButton(){
	memset(map,0,properties->lvlSz*properties->lvlSz*properties->lvlSz*4);
	for(int i = 0;i < properties->lvlSz*properties->lvlSz*4;i+=4){
		map[i].id = 28;
		map[i].r = 255;
		map[i].g = 255;
		map[i].b = 255;
	}
	for(int i = properties->lvlSz*properties->lvlSz*4;i < properties->lvlSz*properties->lvlSz*properties->lvlSz*4;i+=4){
		map[i].r = 128;
		map[i].g = 128;
		map[i].b = 128;
	}
	glMes[glMesC].id = 3;
	glMesC++;
}

void EnumLevelsButton(){
	menuSel = 2;
	buttonC = 0;
	WIN32_FIND_DATAA windt;
	HANDLE h = FindFirstFileA("levels/*.lvl",&windt);
	if((int)h!=-1){
		fileNames.str = HeapAlloc(GetProcessHeap(),8,sizeof(char*)*(fileNames.strC+1));
		fileNames.str[fileNames.strC] = HeapAlloc(GetProcessHeap(),8,strlen(windt.cFileName)+1);
		memcpy(fileNames.str[fileNames.strC],windt.cFileName,strlen(windt.cFileName)-4);
		fileNames.strC++;
		buttonCreate((VEC2){0.05f,0.28f},100);
		buttonCreate((VEC2){0.25f,0.28f},110);
		while(FindNextFileA(h,&windt)){
			fileNames.str = HeapReAlloc(GetProcessHeap(),8,fileNames.str,sizeof(char*)*(fileNames.strC+1));
			fileNames.str[fileNames.strC] = HeapAlloc(GetProcessHeap(),8,strlen(windt.cFileName)+1);
			memcpy(fileNames.str[fileNames.strC],windt.cFileName,strlen(windt.cFileName)-4);
			buttonCreate((VEC2){0.05f,0.28f-(float)fileNames.strC/14.0f},100+fileNames.strC);
			buttonCreate((VEC2){0.25f,0.28f-(float)fileNames.strC/14.0f},110+fileNames.strC);
			fileNames.strC++;
		}
	}
}

void decLightMap(){
	properties->lmapSzb = _rotl8(properties->lmapSzb,1);
}

void incLightMap(){
	properties->lmapSzb = _rotr8(properties->lmapSzb,1);
}

void saveLevelButton(){
	menuSel = 3;
	buttonC = 0;
}

void (*buttons[6])() = {quitButton,genNewWorldButton,EnumLevelsButton,saveLevelButton,decLightMap,incLightMap};
