#include <main.h>
#include <windows.h>

WAVEHDR wavehdr;
WAVEFORMATEX waveformat = {WAVE_FORMAT_PCM,2,22100,44200,2,8,0};
HWAVEOUT soundOut;

char *soundfile;

void initSound(){
	soundfile = loadFile("jump.wav");
	waveOutOpen(&soundOut,WAVE_MAPPER,&waveformat,0,0,0);
	waveOutPrepareHeader(soundOut,&wavehdr,sizeof(WAVEHDR));
}

void sound(int type){
	switch(type){
	case 1:
		/*
		wavehdr.dwBufferLength = 1430;
		wavehdr.dwBytesRecorded = 1430;
		wavehdr.lpData = soundfile;
		waveOutWrite(soundOut,&wavehdr,sizeof(WAVEHDR));
		*/
		break;
	}
}
