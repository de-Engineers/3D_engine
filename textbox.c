#include "textbox.h"

u8 textboxC;
TEXTBOX *textbox;

i8 textboxSel = -1;

void textboxCreate(VEC2 pos,u8 id){
	textbox[textboxC].pos = pos;
	textbox[textboxC].id = id;
	textboxC++;
}