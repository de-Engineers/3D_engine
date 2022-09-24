#pragma once

#include "vec2.h"
#include "smallTypes.h"

typedef struct{
	VEC2 pos;
	u8 id;
	u8 text[20];
}TEXTBOX;

extern u8 textboxC;
extern TEXTBOX *textbox;

extern i8 textboxSel;

void textboxCreate(VEC2 pos,u8 id);