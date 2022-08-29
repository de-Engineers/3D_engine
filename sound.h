#pragma once

#include "smallTypes.h"

typedef struct{
	u16 *data;
	u32 sz;
}SOUNDFILE;

extern SOUNDFILE stepSound;
extern SOUNDFILE stepEchoSound;
extern SOUNDFILE landSound;
extern SOUNDFILE landEchoSound;
extern SOUNDFILE boneBreakSound;

void playSound(SOUNDFILE name,u8 rate,i32 volume);
