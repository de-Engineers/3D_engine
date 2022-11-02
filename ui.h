#pragma once

#define CHATSZ 5

typedef struct{
	u8 *text;
	u16 timer;
}CHATMSG;

extern CHATMSG chat[CHATSZ];