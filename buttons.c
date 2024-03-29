﻿#include <windows.h>
#include <stdio.h>
#include <intrin.h>

#include "main.h"
#include "textbox.h"
#include "network.h"
#include "ui.h"
#include "console.h"
#include "levelgen.h"

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
	closeEngine();
}

void genNewWorldButton(){
	generateEmptyWorld();
}

void EnumLevelsButton(){
	menuSel = 2;
	buttonC = 0;
	WIN32_FIND_DATAA windt;
	HANDLE h = FindFirstFileA("levels/*.lvl",&windt);
	if(h!=-1){
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
			buttonCreate((VEC2){0.05f,0.28f-(f32)fileNames.strC/14.0f},100+fileNames.strC);
			buttonCreate((VEC2){0.25f,0.28f-(f32)fileNames.strC/14.0f},110+fileNames.strC);
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
	glMes[glMesC].id = 9;
	glMesC++;
	settings ^= SETTINGS_FULLSCREEN;
	if(settings & SETTINGS_FULLSCREEN){
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
	sliderCreate((VEC2){ 0.190f,0.09f },15);
	sliderCreate((VEC2){ 0.190f,0.16f },16);
}

void multiplayerButton(){
	buttonC = 0;
	sliderCreate((VEC2){0.190f,-0.12f},17);
	sliderCreate((VEC2){0.190f,-0.19f},18);
	sliderCreate((VEC2){0.190f,-0.26f},19);
	sliderCreate((VEC2){0.190f,-0.33f},20);
	buttonCreate((VEC2){-0.059f,-0.40f},10);
	textboxCreate((VEC2){0.190f,0.02f},0);
	menuSel = 6;
}

void serverConnectButton(){
	for(u32 i = 0;i < textboxC;i++){
		if(textbox[i].id == 0){
			strcpy(playerName,textbox[i].text);
			_strset(textbox[i].text,0);
			break;
		}
	}
	networkThread = CreateThread(0,0,networking,0,0,0);
	buttonC = 0;
	sliderC = 0;
	menuSel = 7;
	textboxC = 0;
	textboxSel = -1;
	ShowCursor(0);
}

void consoleEnterButton(){
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
			break;
		}
	}
}

void chatEnterButton(){
	for(u32 i = 0;i < textboxC;i++){
		if(textbox[i].id == 2){
			if(textbox[i].text[0]){
				for(u32 i2 = CHATSZ-1;i2 > 0;i2--){
					memcpy(chat[i2].text,chat[i2-1].text,40);
					chat[i2].timer = chat[i2-1].timer;
				}
				chat[0].timer = 600;
				strcpy(chat[0].text,textbox[i].text);
				_strset(textbox[i].text,0);
				packetID = 2;
			}
			break;
		}
	}
}

void (*buttons[32])() = {quitButton,genNewWorldButton,EnumLevelsButton,saveLevelButton,decLightMap,incLightMap,vsyncButton,fullscreenButton,videoSettingsButton,multiplayerButton,serverConnectButton,consoleEnterButton,chatEnterButton};
