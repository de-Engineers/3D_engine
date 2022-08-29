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
		for(u32 i = 0;i < BLOCKCOUNT;i++){
			map[i].id = 1;
		}
		map[crds2map(32,32,7)].r  = 190;
		map[crds2map(32,32,7)].g  = 190;
		map[crds2map(32,32,7)].b  = 190;
		metadt2[crds2map(32,32,7)].r   = 127;
		metadt2[crds2map(32,32,7)].g   = 127;
		metadt2[crds2map(32,32,7)].id  = 127;
		map[crds2map(32,32,7)].id = BLOCK_LIGHT5;
		metadt[crds2map(32,32,7)].b = 255;
	}
}
