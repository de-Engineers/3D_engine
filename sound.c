#include <windows.h>
#include <dsound.h>

#include "main.h"
#include "sound.h"

#pragma comment(lib,"DSound.lib")

#define DSSCL_PRIORITY 0x00000002

WAVEHDR wavehdr;
WAVEFORMATEX waveformat  = {WAVE_FORMAT_PCM,2,44100*2,44100*8,4,16,0};
WAVEFORMATEX waveformatS = {WAVE_FORMAT_PCM,2,44100,44100*4,4,16,0};

u8 outSelect;
u8 outSelectS;
HWAVEOUT soundOut;

SOUNDFILE stepSound;
SOUNDFILE stepEchoSound;
SOUNDFILE landSound;
SOUNDFILE landEchoSound;
SOUNDFILE boneBreakSound;

LPDIRECTSOUND       directSoundH;
LPDIRECTSOUNDBUFFER directSoundBP[16];
LPDIRECTSOUNDBUFFER directSoundBS[16];
LPDIRECTSOUNDBUFFER directSoundBPS[16];
LPDIRECTSOUNDBUFFER directSoundBSS[16];
DSBUFFERDESC        bufferDescP;
DSBUFFERDESC        bufferDescS;
DSBUFFERDESC        bufferDescSS;

u32 sbufp1Sz;
u32 sbufp2Sz;
u32 sCursor;
u32 wCursor;

u16 *sbufp1;
u16 *sbufp2;

SOUNDFILE loadSoundFile(char *name){
	SOUNDFILE file;
	HANDLE h = CreateFileA(name,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	file.sz = GetFileSize(h,0);
	file.data = HeapAlloc(GetProcessHeap(),8,file.sz+1);
	ReadFile(h,file.data,file.sz+1,0,0);
	CloseHandle(h);
	return file;
}

void initSound(){
	stepSound = loadSoundFile("sfx/step.wav");
	stepEchoSound = loadSoundFile("sfx/stepecho.wav");
	landSound = loadSoundFile("sfx/land.wav");
	landEchoSound = loadSoundFile("sfx/landecho.wav");
	boneBreakSound = loadSoundFile("sfx/boneBreak.wav");
	DirectSoundCreate(0,&directSoundH,0);
	directSoundH->lpVtbl->SetCooperativeLevel(directSoundH,window,DSSCL_PRIORITY);
	bufferDescP.dwFlags = DSBCAPS_PRIMARYBUFFER;
	bufferDescP.dwSize  = sizeof(DSBUFFERDESC);

	for(u32 i = 0;i < 16;i++){
		directSoundH->lpVtbl->CreateSoundBuffer(directSoundH,&bufferDescP,&directSoundBPS[i],0);
		directSoundBPS[i]->lpVtbl->SetFormat(directSoundBPS[i],&waveformat);
	}

	for(u32 i = 0;i < 16;i++){
		directSoundH->lpVtbl->CreateSoundBuffer(directSoundH,&bufferDescP,&directSoundBP[i],0);
		directSoundBP[i]->lpVtbl->SetFormat(directSoundBP[i],&waveformatS);
	}

	bufferDescS.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDescS.dwSize  = sizeof(DSBUFFERDESC);
	bufferDescS.dwBufferBytes = 953470;
	bufferDescS.lpwfxFormat = &waveformat;

	bufferDescSS.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDescSS.dwSize  = sizeof(DSBUFFERDESC);
	bufferDescSS.dwBufferBytes = 953470;
	bufferDescSS.lpwfxFormat = &waveformatS;

	for(u32 i = 0;i < 16;i++){
		directSoundH->lpVtbl->CreateSoundBuffer(directSoundH,&bufferDescSS,&directSoundBSS[i],0);
	}

	for(u32 i = 0;i < 16;i++){
		directSoundH->lpVtbl->CreateSoundBuffer(directSoundH,&bufferDescS,&directSoundBS[i],0);
	}
}

void playSound(SOUNDFILE name,u8 rate,i32 volume){
	switch(rate){
	case 0:
		directSoundBSS[outSelectS]->lpVtbl->SetVolume(directSoundBSS[outSelectS],volume);
		directSoundBSS[outSelectS]->lpVtbl->SetCurrentPosition(directSoundBSS[outSelectS],0);
		directSoundBSS[outSelectS]->lpVtbl->Lock(directSoundBSS[outSelectS],0,name.sz,&sbufp1,&sbufp1Sz,&sbufp2,&sbufp2Sz,0);
		for(u32 i = 0;i < name.sz/2-1000;i++){
			sbufp1[i] = name.data[i+100]; 
		}
		directSoundBSS[outSelectS]->lpVtbl->Unlock(directSoundBSS[outSelectS],sbufp1,sbufp1Sz,sbufp2,sbufp2Sz);
		directSoundBSS[outSelectS]->lpVtbl->Play(directSoundBSS[outSelectS],0,0,0);
		outSelectS++;
		if(outSelectS == 16){
			outSelectS = 0;
		}
		break;
	case 1:
		directSoundBSS[outSelectS]->lpVtbl->SetVolume(directSoundBSS[outSelectS],volume);
		directSoundBS[outSelect]->lpVtbl->SetCurrentPosition(directSoundBS[outSelect],0);
		directSoundBS[outSelect]->lpVtbl->Lock(directSoundBS[outSelect],0,name.sz,&sbufp1,&sbufp1Sz,&sbufp2,&sbufp2Sz,0);
		for(u32 i = 0;i < name.sz/2-1000;i++){
			sbufp1[i] = name.data[i+100]; 
		}
		directSoundBS[outSelect]->lpVtbl->Unlock(directSoundBS[outSelect],sbufp1,sbufp1Sz,sbufp2,sbufp2Sz);
		directSoundBS[outSelect]->lpVtbl->Play(directSoundBS[outSelect],0,0,0);
		outSelect++;
		if(outSelect == 16){
			outSelect = 0;
		}
		break;
	}
}
