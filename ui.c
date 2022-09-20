#include "main.h"
#include <math.h>
#include <intrin.h>
#include <stdio.h>
#include <GL/gl.h>

#include "tmgl.h"
#include "network.h"
#include "textbox.h"
#include "ui.h"

#define GL_ARRAY_BUFFER 0x8892
#define GL_DYNAMIC_DRAW 0x88E8

const char *words[]  = {"air","rgb","ambientlight","light1","light2","light3","light4","light5",
	"light6","sphere","customslope","glass","cube","mirror","spikes","normal",
	"donut","shape1","greekpillar","","","","","","","playerspawn","clipblock","reflectiveblock","block"
	};
const char *words2[] = {"normal","cube","blockedit","changelight","changeproperties","changeproperties2","changeproperties3","paintpixel",
"paintside","paintsiderandom"};

unsigned int totalCar;
VEC2 mousePos;

CHATMSG chat1;
CHATMSG chat2;
CHATMSG chat3;

void drawChar(int c,float x,float y,float z,float id,float xsize,float ysize){
	xsize /= 1.7777778;
	quad[totalCar * 36 + 36]    = x + xsize;
	quad[totalCar * 36 + 36+1]  = y;
	quad[totalCar * 36 + 36+2]  = z;
	quad[totalCar * 36 + 36+3]  = 0.1f + (float)(c % 10) / 10;
	quad[totalCar * 36 + 36+4]  = 0.25f + (float)(c / 10) / 4;
	quad[totalCar * 36 + 36+5]  = id;
	quad[totalCar * 36 + 36+6]  = x;
	quad[totalCar * 36 + 36+7]  = y;
	quad[totalCar * 36 + 36+8]  = z;
	quad[totalCar * 36 + 36+9] = (float)(c % 10) / 10;
	quad[totalCar * 36 + 36+10] = 0.25f + (float)(c / 10) / 4;
	quad[totalCar * 36 + 36+11] = id;
	quad[totalCar * 36 + 36+12] = x + xsize;
	quad[totalCar * 36 + 36+13] = y + ysize;
	quad[totalCar * 36 + 36+14] = z;
	quad[totalCar * 36 + 36+15] = 0.1f + (float)(c % 10) / 10;
	quad[totalCar * 36 + 36+16] = 0.0f + (float)(c / 10) / 4;
	quad[totalCar * 36 + 36+17] = id;
	quad[totalCar * 36 + 36+18] = x;
	quad[totalCar * 36 + 36+19] = y + ysize;
	quad[totalCar * 36 + 36+20] = z;
	quad[totalCar * 36 + 36+21] = (float)(c % 10) / 10;
	quad[totalCar * 36 + 36+22] = 0.0 + (float)(c / 10) / 4;
	quad[totalCar * 36 + 36+23] = id;
	quad[totalCar * 36 + 36+24] = x;
	quad[totalCar * 36 + 36+25] = y;
	quad[totalCar * 36 + 36+26] = z;
	quad[totalCar * 36 + 36+27] = (float)(c % 10) / 10;
	quad[totalCar * 36 + 36+28] = 0.25f + (float)(c / 10) / 4;
	quad[totalCar * 36 + 36+29] = id;
	quad[totalCar * 36 + 36+30] = x + xsize;
	quad[totalCar * 36 + 36+31] = y + ysize;
	quad[totalCar * 36 + 36+32] = z;
	quad[totalCar * 36 + 36+33] = 0.1f + (float)(c % 10) / 10;
	quad[totalCar * 36 + 36+34] = 0.0f + (float)(c / 10) / 4;
	quad[totalCar * 36 + 36+35] = id;
	glBufferData(GL_ARRAY_BUFFER,(totalCar * 36 + 36) * sizeof(float),quad,GL_DYNAMIC_DRAW);
	totalCar++;
}

void drawSprite(VEC3 pos,VEC2 size,float id){
	quad[totalCar * 36 + 36]    = pos.x + size.x;
	quad[totalCar * 36 + 36+1]  = pos.y - size.y;
	quad[totalCar * 36 + 36+2]  = pos.z;
	quad[totalCar * 36 + 36+3]  = 1.0f;
	quad[totalCar * 36 + 36+4]  = 0.0f;
	quad[totalCar * 36 + 36+5]  = id;
	quad[totalCar * 36 + 36+6]  = pos.x - size.x;
	quad[totalCar * 36 + 36+7]  = pos.y - size.y;
	quad[totalCar * 36 + 36+8]  = pos.z;
	quad[totalCar * 36 + 36+9]  = 0.0f;
	quad[totalCar * 36 + 36+10] = 0.0f;
	quad[totalCar * 36 + 36+11] = id;
	quad[totalCar * 36 + 36+12] = pos.x + size.x;
	quad[totalCar * 36 + 36+13] = pos.y + size.y;
	quad[totalCar * 36 + 36+14] = pos.z;
	quad[totalCar * 36 + 36+15] = 1.0f;
	quad[totalCar * 36 + 36+16] = 1.0f;
	quad[totalCar * 36 + 36+17] = id;
	quad[totalCar * 36 + 36+18] = pos.x - size.x;
	quad[totalCar * 36 + 36+19] = pos.y + size.y;
	quad[totalCar * 36 + 36+20] = pos.z;
	quad[totalCar * 36 + 36+21] = 0.0f;
	quad[totalCar * 36 + 36+22] = 1.0f;
	quad[totalCar * 36 + 36+23] = id;
	quad[totalCar * 36 + 36+24] = pos.x - size.x;
	quad[totalCar * 36 + 36+25] = pos.y - size.y;
	quad[totalCar * 36 + 36+26] = pos.z;
	quad[totalCar * 36 + 36+27] = 0.0f;
	quad[totalCar * 36 + 36+28] = 0.0f;
	quad[totalCar * 36 + 36+29] = id;
	quad[totalCar * 36 + 36+30] = pos.x + size.x;
	quad[totalCar * 36 + 36+31] = pos.y + size.y;
	quad[totalCar * 36 + 36+32] = pos.z;
	quad[totalCar * 36 + 36+33] = 1.0f;
	quad[totalCar * 36 + 36+34] = 1.0f;
	quad[totalCar * 36 + 36+35] = id;
	glBufferData(GL_ARRAY_BUFFER,(totalCar * 36 + 36) * sizeof(float),quad,GL_DYNAMIC_DRAW);
	totalCar++;
}

void drawVar(f32 x,f32 y,u32 val){
	if(val == 0){
		drawChar(0,x+0.03f,y,-0.99f,0,0.04f,0.04f);
		return;
	}
	u32 size = 0;
	u32 tval = val;
	while(tval > 0){
		tval /= 10;
		size++;
	}
	if(!size){
		drawChar(26,x + 1.0f / 30,y,-0.99f,0,0.04f,0.04f);
	}
	else{
		for(u32 i = 0;i < size;i++){
			drawChar(val % 10,x + (f32)(size - i) / 30,y,-0.99f,0,0.04f,0.04f);
			val /= 10;
		}
	}
}

void drawWord(u8 *str,f32 x,f32 y,f32 id){
	for(int i = 0;i < strlen(str);i++){
		if(str[i] == ' '){
			continue;
		}
		if(str[i] > 0x2f && str[i] < 0x3a){
			drawChar(str[i] - 0x30,x+i*0.0235f,y,-0.1f,id,0.04f,0.04f);
		}
		else{
			drawChar(str[i] - 87,x+i*0.0235f,y,-0.1f,id,0.04f,0.04f);
		}

	}
}

void drawDescription(f32 offset){
	switch(blockSel){
	case BLOCK_AMBIENTLIGHT:
		drawWord("angle x",offset+0.0235f*3,-0.50f,0.0f);
		drawWord("angle y",offset+0.0235f*3,-0.45f,0.0f);
		drawWord("angle z",offset+0.0235f*3,-0.40f,0.0f);

		drawWord("skycolor x",offset,-0.20f,0.0f);
		drawWord("skycolor y",offset,-0.15f,0.0f);
		drawWord("skycolor z",offset,-0.10f,0.0f);
		break;
	case BLOCK_LIGHT1:
	case BLOCK_LIGHT2:
	case BLOCK_LIGHT3:
	case BLOCK_LIGHT4:
	case BLOCK_LIGHT5:
	case BLOCK_LIGHT6:
		drawWord("areasize x",offset,-0.50f,0.0f);
		drawWord("areasize y",offset,-0.45f,0.0f);
		drawWord("areasize z",offset,-0.40f,0.0f);

		drawWord("areapos x",offset+0.0235f*1,-0.20f,0.0f);
		drawWord("areapos y",offset+0.0235f*1,-0.15f,0.0f);
		drawWord("areapos z",offset+0.0235f*1,-0.10f,0.0f);

		drawWord("size",offset+0.0235f*6,0.15f,0.0f);
		break;
	case BLOCK_GLASS:
		drawWord("position x",offset,-0.50f,0.0f);
		drawWord("position y",offset,-0.45f,0.0f);
		drawWord("position z",offset,-0.40f,0.0f);

		drawWord("size x",offset+0.0235f*4,-0.20f,0.0f);
		drawWord("size y",offset+0.0235f*4,-0.15f,0.0f);
		drawWord("size z",offset+0.0235f*4,-0.10f,0.0f);
		break;
	case BLOCK_CUBE:
		drawWord("position x",offset,-0.50f,0.0f);
		drawWord("position y",offset,-0.45f,0.0f);
		drawWord("position z",offset,-0.40f,0.0f);

		drawWord("size x",offset+0.0235f*4,-0.20f,0.0f);
		drawWord("size y",offset+0.0235f*4,-0.15f,0.0f);
		drawWord("size z",offset+0.0235f*4,-0.10f,0.0f);

		drawWord("rotation x",offset,0.10f,0.0f);
		drawWord("rotation y",offset,0.15f,0.0f);
		drawWord("rotation z",offset,0.20f,0.0f);
		break;
	}
	drawWord("red",offset+0.0235f*7,-0.90f,0.0f);
	drawWord("green",offset+0.0235f*5,-0.85f,0.0f);
	drawWord("blue",offset+0.0235f*6,-0.80f,0.0f);
}

void updateMouse(){
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(window,&p);
	mousePos.x = (float)p.x/properties->xres*2.0f-1.0f;
	mousePos.y = -((float)p.y/properties->yres*2.0f-1.0f);
}

void drawUI(){
	if(~settings & SETTINGS_GAMEPLAY){
		drawChar(33,-0.9f,0.9f,-0.99f,0,0.04f,0.04f);
		drawVar(-0.9f,0.9f,player->pos.x);
		drawChar(34,-0.75,0.9,-0.99f,0,0.04f,0.04f);
		drawVar(-0.75,0.9,player->pos.y);
		drawChar(35,-0.6f,0.9f,-0.99f,0,0.04f,0.04f);
		drawVar(-0.6f,0.9f,player->pos.z);
		drawChar(15,-0.96f,0.7f,-0.99f,0,0.04f,0.04f);
		drawChar(25,-0.93f,0.7f,-0.99f,0,0.04f,0.04f);
		drawChar(28,-0.9f,0.7f,-0.99f,0 ,0.04f,0.04f);
		drawVar(-0.9f,0.7f,10000000/fps);
		if(blockSel < 29){
			drawWord(words[blockSel],-0.9f,-0.9f,0.0f);
		}
		if(toolSel < 10){
			drawWord(words2[toolSel],-0.9f,-0.78f,0.0f);
		}
		drawVar(0.85f,-0.90f,colorSel.r);
		drawVar(0.85f,-0.85f,colorSel.g);
		drawVar(0.85f,-0.80f,colorSel.b);
		drawVar(0.85f,-0.75f,colorSel.a);
		if(settings & SETTINGS_SUBBLOCK){
			drawWord("sub block 2",-0.9f,-0.66f,0.0f);
			drawVar(0.85f,-0.50f,metadt4Sel.r);
			drawVar(0.85f,-0.45f,metadt4Sel.g);
			drawVar(0.85f,-0.40f,metadt4Sel.a);
			drawVar(0.85f,-0.35f,metadt4Sel.b);

			drawVar(0.85f,-0.20f,metadt5Sel.r);
			drawVar(0.85f,-0.15f,metadt5Sel.g);
			drawVar(0.85f,-0.10f,metadt5Sel.a);
			drawVar(0.85f,-0.05f,metadt5Sel.b);

			drawVar(0.85f,0.10f,metadt6Sel.r);
			drawVar(0.85f,0.15f,metadt6Sel.g);
			drawVar(0.85f,0.20f,metadt6Sel.a);
			drawVar(0.85f,0.25f,metadt6Sel.b);
		}
		else{
			drawWord("sub block 1",-0.9f,-0.66f,0.0f);
			drawVar(0.85f,-0.50f,metadtSel.r);
			drawVar(0.85f,-0.45f,metadtSel.g);
			drawVar(0.85f,-0.40f,metadtSel.a);
			drawVar(0.85f,-0.35f,metadtSel.b);

			drawVar(0.85f,-0.20f,metadt2Sel.r);
			drawVar(0.85f,-0.15f,metadt2Sel.g);
			drawVar(0.85f,-0.10f,metadt2Sel.a);
			drawVar(0.85f,-0.05f,metadt2Sel.b);

			drawVar(0.85f,0.10f,metadt3Sel.r);
			drawVar(0.85f,0.15f,metadt3Sel.g);
			drawVar(0.85f,0.20f,metadt3Sel.a);
			drawVar(0.85f,0.25f,metadt3Sel.b);

			switch(menuSel){
			case 0:
				drawDescription(0.62f);
				break;
			case 4:
				drawDescription(0.055f);
				break;							
			}
		}
		drawSprite((VEC3){0.927f,-0.603f,-0.2f},(VEC2){0.05f,0.05f},7);
	}
	else{
		if(chat1.timer){
			drawWord(chat1.text,-0.94f,-0.9f,0.0);
			chat1.timer--;
		}
		if(chat2.timer){
			drawWord(chat2.text,0.0f,0.0f,0.0);
			chat2.timer--;
		}
		if(chat3.timer){
			drawWord(chat3.text,0.0f,0.0f,0.0);
			chat3.timer--;
		}
	}
	switch(menuSel){
	case 0:
		drawChar(36,-0.01f,-0.02f,-0.99f,0,0.04f,0.04f);
		break;
	case 1:{
		drawSprite((VEC3){0.0f,0.0f,0.0f},(VEC2){0.5f,0.5f},2);
		drawWord("settings",-0.45f,0.42f,0.0f);
		drawWord("video settings",-0.45f,-0.14f,0.0f);
		drawWord("save world",-0.45f,-0.21f,0.0f);
		drawWord("load world",-0.45f,-0.28f,0.0f);
		drawWord("create world",-0.45f,-0.35f,0.0f);
		drawWord("quit",-0.45f,-0.42f,0.0f);
		drawWord("multiplayer",-0.45f,-0.07f,0.0f);
		if((settings&SETTINGS_GAMEPLAY)==0){
			drawWord("lightmapsize",-0.45f,0.0f,-0.0f);
			drawVar(-0.025f,0.0f,properties->lmapSzb);
		}
		updateMouse();
		break;
		}
	case 2:{
		drawSprite((VEC3){0.0f,0.0f,0.0f},(VEC2){0.5f,0.5f},2);
		drawWord("worlds",-0.11f,0.42f,0.0f);
		drawWord("load",0.02f,0.35f,0.0f);
		drawWord("delete",0.19f,0.35f,0.0f);
		for(int i = 0;i < fileNames.strC;i++){
			drawWord(fileNames.str[i],-0.45f,0.28f-(float)i/15.0f,0.0f);
		}
		updateMouse();
		break;
		}
	case 3:{
		drawSprite((VEC3){0.0f,0.0f,0.0},(VEC2){0.5f,0.5f},2);
		drawWord("save",-0.11f,0.42f,0.0f);
		drawWord(inputStr,-0.45f,0.0f,0.0f);
		updateMouse();
		}
		break;
	case 4:{
		updateMouse();
		break;
	case 5:
		drawSprite((VEC3){ 0.0f,0.0f,0.0f },(VEC2){ 0.5f,0.5f },2);
		drawWord("video settings",-0.45f,0.42f,0.0f);
		if(settings & SETTINGS_VSYNC){
			drawWord("vsync on",-0.45f,0.0f,-0.0f);
		}
		else{
			drawWord("vsync off",-0.45f,0.0f,-0.0f);
		}
		drawWord("fov",-0.45f,0.07f,-0.0f);
		drawWord("sensitivity",-0.45f,0.14f,-0.0f);
		if(settings & SETTINGS_FULLSCREEN){
			drawWord("fullscreen on",-0.45f,0.21f,0.0f);
		}
		else{
			drawWord("fullscreen off",-0.45f,0.21f,0.0f);
		}
		updateMouse();
		break;
	case 6:{
		drawSprite((VEC3){ 0.0f,0.0f,0.0f },(VEC2){ 0.5f,0.5f },2);
		drawWord("multiplayer",-0.45f,0.42f,0.0f);
		drawWord("name",-0.45f,-0.04f,0.0f);
		drawWord("ipaddress 1",-0.45f,-0.14f,0.0f);
		drawWord("ipaddress 2",-0.45f,-0.21f,0.0f);
		drawWord("ipaddress 3",-0.45f,-0.28f,0.0f);
		drawWord("ipaddress 4",-0.45f,-0.35f,0.0f);
		drawWord("connect",-0.45f,-0.42,0.0f);
		f32 offset = 0.0f;
		drawVar(-0.06f,-0.415f,sliderValues.serverIP.p1);
		if(sliderValues.serverIP.p1 >= 100){
			offset += 0.06f;
		}
		else if(sliderValues.serverIP.p1 >= 10){
			offset += 0.03f;
		}
		drawWord("x",0.002f+offset,-0.415f,0.0f);
		drawVar(0.0f+offset,-0.415f,sliderValues.serverIP.p2);
		if(sliderValues.serverIP.p2 >= 100){
			offset += 0.06f;
		}
		else if(sliderValues.serverIP.p2 >= 10){
			offset += 0.03f;
		}
		drawWord("x",0.062f+offset,-0.415f,0.0f);
		drawVar(0.06f+offset,-0.415f,sliderValues.serverIP.p3);
		if(sliderValues.serverIP.p3 >= 100){
			offset += 0.06f;
		}
		else if(sliderValues.serverIP.p3 >= 10){
			offset += 0.03f;
		}
		drawWord("x",0.122f+offset,-0.415f,0.0f);
		drawVar(0.12f+offset,-0.415f,sliderValues.serverIP.p4);
		if(sliderValues.serverIP.p4 >= 100){
			offset += 0.06f;
		}
		else if(sliderValues.serverIP.p4 >= 10){
			offset += 0.03f;
		}
		updateMouse();
		break;
	}
	case 7:
		drawSprite((VEC3){ 0.0f,0.0f,0.0f },(VEC2){ 0.5f,0.5f },2);
		drawWord("joining server",-0.45f,0.42f,0.0f);
		switch(connectStatus){
		case 1:
			drawWord("connecting",-0.45f,0.35f,0.0f);
			break;
		case 2:
			drawWord("downloading miscelanious data",-0.45f,0.35f,0.0f);
			break;
		case 3:
			drawWord("downloading map",-0.45f,0.35f,0.0f);
			break;
		case 4:
			drawWord("downloading metadatamap1",-0.45f,0.35f,0.0f);
			break;
		case 5:
			drawWord("downloading metadatamap2",-0.45f,0.35f,0.0f);
			break;
		case 6:
			drawWord("downloading metadatamap3",-0.45f,0.35f,0.0f);
			break;
		case 7:
			drawWord("downloading metadatamap4",-0.45f,0.35f,0.0f);
			break;
		case 8:
			drawWord("downloading metadatamap5",-0.45f,0.35f,0.0f);
			break;
		case 9:
			drawWord("downloading metadatamap6",-0.45f,0.35f,0.0f);
			break;
		case 10:
			drawWord("downloading pointermap",-0.45f,0.35f,0.0f);
			break;
		case 11:
			drawWord("downloading lightmap",-0.45f,0.35f,0.0f);
			break;
		case 12:
			drawWord("connected",-0.45f,0.35f,0.0f);
			break;
		}
		updateMouse();
		break;
	}
	}
	for(u32 i = 0;i < buttonC;i++){
		VEC2 buttonMiddle = {button[i].pos.x+0.0075f,button[i].pos.y+0.0125f};
		if(VEC2dist(mousePos,buttonMiddle)<0.03f){
			buttonId = button[i].id;
			drawSprite((VEC3){button[i].pos.x,button[i].pos.y,-0.05f},(VEC2){0.015f,0.025f},4);
			for(;i < buttonC;i++){
				drawSprite((VEC3){button[i].pos.x,button[i].pos.y,-0.05f},(VEC2){0.015f,0.025f},3);
			}
			goto foundButton;
		}
		else{
			drawSprite((VEC3){button[i].pos.x,button[i].pos.y,0.0f},(VEC2){0.015f,0.025f},3);
		}
	}
	buttonId = -1;
	foundButton:
	for(u32 i = 0;i < sliderC;i++){
		drawSprite((VEC3){slider[i].pos.x,slider[i].pos.y,0.0f},(VEC2){0.266666667f,0.015f},8+slider[i].id);
	}
	for(u32 i = 0;i < textboxC;i++){
		drawSprite((VEC3){textbox[i].pos.x,textbox[i].pos.y,0.0f},(VEC2){0.266666667f,0.004f},5);
		drawSprite((VEC3){textbox[i].pos.x-0.2646666667f,textbox[i].pos.y-0.044f,0.0f},(VEC2){0.0028f,0.05f},5);
		drawSprite((VEC3){textbox[i].pos.x,textbox[i].pos.y-0.089f,0.0f},(VEC2){0.266666667f,0.004f},5);
		drawSprite((VEC3){textbox[i].pos.x+0.2636666667f,textbox[i].pos.y-0.044f,0.0f},(VEC2){0.0028f,0.05f},5);
		drawWord(textbox[i].text,textbox[i].pos.x-0.2546666667f,textbox[i].pos.y-0.062f,0.0f);
	}
	if(textboxSel!=-1 && GetTickCount() & 0x200){
		drawSprite((VEC3){textbox[textboxSel].pos.x-0.24666667f+0.0233f*strlen(textbox[textboxSel].text),textbox[textboxSel].pos.y-0.0425f,0.0f},(VEC2){0.002f,0.03f},5);
	}
}
