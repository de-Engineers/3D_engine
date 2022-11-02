#include <windows.h>
#include <intrin.h>
#include <stdio.h>

#include "main.h"

void generateEmptyWorld(){
	for(u32 i = 0;i < BLOCKCOUNT;i++){
		map[i].id = 1;
	}
	map[crds2map(32,32,7)].id = BLOCK_LIGHT5;
	map[crds2map(32,32,5)].r  = 190;
	map[crds2map(32,32,5)].g  = 190;
	map[crds2map(32,32,5)].b  = 190;
	metadt2[crds2map(32,32,7)].r   = 128;
	metadt2[crds2map(32,32,7)].g   = 128;
	metadt2[crds2map(32,32,7)].id  = 128;

	map[crds2map(32,32,0)].id = BLOCK_SOLID;
	map[crds2map(32,32,0)].r  = 190;
	map[crds2map(32,32,0)].g  = 190;
	map[crds2map(32,32,0)].b  = 190;

	metadt[crds2map(32,32,7)].b = 255;
	glMes[glMesC].id = 8;
	glMesC++;
	glMes[glMesC].id = 3;
	glMesC++;
	glMes[glMesC].id = 6;
	glMesC++;
}

void levelgen(){
	HANDLE h = CreateFileA("levels/level.lvl",GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	if(h != -1){
		CloseHandle(h);
		levelLoad("level");
	}
	else{
		generateEmptyWorld();
	}
}
