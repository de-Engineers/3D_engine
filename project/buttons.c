#include <main.h>
#include <windows.h>

char buttonId;
BUTTON *button;
unsigned char buttonC;

void quitButton(){
	HANDLE h = CreateFile("level.lvl",GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
	WriteFile(h,map,MAPRAM,0,0);
	WriteFile(h,mapdata,MAPRAM,0,0);
	CloseHandle(h);
	ExitProcess(0);	
}

void genNewWorldButton(){
	memset(map,0,properties->lvlSz*properties->lvlSz*properties->lvlSz*4);
	memset(mapdata,0,properties->lvlSz*properties->lvlSz*properties->lvlSz*4);
	for(int i = 0;i < properties->lvlSz*properties->lvlSz*4;i+=4){
		map[i] = 28;
		map[i+1] = 255;
		map[i+2] = 255;
		map[i+3] = 255;
		mapdata[i] = 255;
		mapdata[i+1] = 255;
		mapdata[i+2] = 255;
	}
	for(int i = properties->lvlSz*properties->lvlSz*4;i < properties->lvlSz*properties->lvlSz*properties->lvlSz*4;i+=4){
		map[i+1] = 128;
		map[i+2] = 128;
		map[i+3] = 128;
	}
	glMes[glMesC].id = 3;
	glMesC++;
}

void (*buttons[2])() = {quitButton,genNewWorldButton};
