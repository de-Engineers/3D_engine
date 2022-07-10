#include "main.h"
#include <windows.h>
#include <intrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <shlwapi.h>

void levelgen(){
	HANDLE h = CreateFileA("levels/level.lvl",GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if((int)h != -1){
		CloseHandle(h);
		levelLoad("level");
	}
	else{
		for(int i = 0;i < properties->lvlSz*properties->lvlSz;i++){
			map[i].id = 28;
			map[i].r = 255;
			map[i].g = 255;
			map[i].b = 255;
		}
	}

}
