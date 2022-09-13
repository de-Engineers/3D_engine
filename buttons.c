#include <windows.h>
#include <stdio.h>
#include <intrin.h>

#include "main.h"

i8 buttonId = -1;

BUTTON *button;

unsigned char buttonC;

STRINGS fileNames;

void buttonCreate(VEC2 pos,unsigned char id){
	button[buttonC].pos = pos;
	button[buttonC].id  = id;
	buttonC++;
}

void quitButton(){
	levelSave("level");
	ExitProcess(0);	
}

void genNewWorldButton(){
	memset(map,0,properties->lvlSz*properties->lvlSz*properties->lvlSz*4);
	for(int i = 0;i < properties->lvlSz*properties->lvlSz;i++){
		map[i].id = 28;
		map[i].r = 190;
		map[i].g = 190;
		map[i].b = 190;
	}
	for(int i = properties->lvlSz*properties->lvlSz*4;i < properties->lvlSz*properties->lvlSz*properties->lvlSz;i++){
		map[i].r = 128;
		map[i].g = 128;
		map[i].b = 128;
		map[i].id = 0;
	}
	map[crds2map(32,32,7)].r  = 25;
	map[crds2map(32,32,7)].g  = 25;
	map[crds2map(32,32,7)].b  = 25;
	map[crds2map(32,32,7)].id = 6;
	metadt[crds2map(32,32,7)].b = 255;
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

void vsyncButton(){
	settings ^= SETTINGS_VSYNC;
	glMes[glMesC].id = 9;
	glMesC++;
}

void fullscreenButton(){
	settings &= ~SETTINGS_FULLSCREEN;
	glMes[glMesC].id = 9;
	glMesC++;
	settings ^= SETTINGS_FULLSCREEN;
	if(settings & 0x02){
		SetWindowLongPtrA(window,GWL_STYLE,WS_VISIBLE|WS_POPUP);
		SetWindowPos(window,0,0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),0);
	}
	else{
		SetWindowLongPtrA(window,GWL_STYLE,WS_VISIBLE|WS_POPUP|WS_TILED|WS_MAXIMIZEBOX|WS_MINIMIZEBOX|WS_SYSMENU|WS_CAPTION|WS_SIZEBOX);
		SetWindowPos(window,0,100,100,1000,1000,0);
	}
	glMes[glMesC].id = 0;
	glMesC++;
}

void videoSettingsButton(){
	buttonC = 0;
	menuSel = 5;
	buttonCreate((VEC2){ -0.059f,0.24f },7);
	buttonCreate((VEC2){ -0.059f,0.02f },6);
	sliderCreate((VEC2){ 0.190f,0.09f },12);
	sliderCreate((VEC2){ 0.190f,0.16f },13);
}

void multiplayerButton(){
	buttonC = 0;
	sliderCreate((VEC2){0.190f,-0.12f},14);
	sliderCreate((VEC2){0.190f,-0.19f},15);
	sliderCreate((VEC2){0.190f,-0.26f},16);
	sliderCreate((VEC2){0.190f,-0.33f},17);
	buttonCreate((VEC2){-0.059f,-0.40},10);
	menuSel = 6;
}

void serverConnectButton(){
	networkThread = CreateThread(0,0,networking,0,0,0);
	buttonC = 0;
	sliderC = 0;
	menuSel = 7;
	ShowCursor(0);
}

void (*buttons[32])() = {quitButton,genNewWorldButton,EnumLevelsButton,saveLevelButton,decLightMap,incLightMap,vsyncButton,fullscreenButton,videoSettingsButton,multiplayerButton,serverConnectButton};
