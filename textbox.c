#include <string.h>

#include "textbox.h"
#include "network.h"

u8 textboxC;
TEXTBOX *textbox;

i8 textboxSel = -1;

void textboxCreate(VEC2 pos,u8 id){
	switch(id){
	case 0:
		if(strlen(playerName)){
			strcpy(textbox[textboxC].text,playerName);
		}
		else{
			strcpy(textbox[textboxC].text,"guest");
			textbox[textboxC].text[5] = irnd()%10+0x30;
			textbox[textboxC].text[6] = irnd()%10+0x30;
			textbox[textboxC].text[7] = irnd()%10+0x30;
			textbox[textboxC].text[8] = irnd()%10+0x30;
		}
		break;
	}
	textbox[textboxC].pos = pos;
	textbox[textboxC].id = id;
	textboxC++;
}