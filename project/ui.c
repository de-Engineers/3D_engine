#include "main.h"
#include <math.h>
#include <intrin.h>
#include <stdio.h>
#include <GL/gl.h>

#include "tmgl.h"

#define GL_ARRAY_BUFFER 0x8892
#define GL_DYNAMIC_DRAW 0x88E8

const char *words[]  = {"air","rgb","water","mirror","sphere","tower","tegeltjes","white",
	"light","mist","customslope","glass","bounce","mirror","spikes","normal",
	"donut","shape1","greekpillar",
	
	};
const char *words2[] = {"normal","cube","generatelight","entities","colorselect","lighting"};

unsigned int totalCar;
VEC2 mousePos;

float distance(VEC2 p1,VEC2 p2){
	float r1 = p1.x-p2.x;
	float r2 = p1.y-p2.y;
	return sqrtf(r1*r1+r2*r2);
}

void drawChar(int c,float x,float y,float z,float id,float xsize,float ysize){

	xsize /= 1.7777778;
	quad[totalCar * 36 + 36]    = x + xsize;
	quad[totalCar * 36 + 36+1]  = y;
	quad[totalCar * 36 + 36+2]  = z;
	quad[totalCar * 36 + 36+3]  = 0.1 + (float)(c % 10) / 10;
	quad[totalCar * 36 + 36+4]  = 0.25 + (float)(c / 10) / 4;
	quad[totalCar * 36 + 36+5]  = id;
	quad[totalCar * 36 + 36+6]  = x;
	quad[totalCar * 36 + 36+7]  = y;
	quad[totalCar * 36 + 36+8]  = z;
	quad[totalCar * 36 + 36+9] = (float)(c % 10) / 10;
	quad[totalCar * 36 + 36+10] = 0.25 + (float)(c / 10) / 4;
	quad[totalCar * 36 + 36+11] = id;
	quad[totalCar * 36 + 36+12] = x + xsize;
	quad[totalCar * 36 + 36+13] = y + ysize;
	quad[totalCar * 36 + 36+14] = z;
	quad[totalCar * 36 + 36+15] = 0.1 + (float)(c % 10) / 10;
	quad[totalCar * 36 + 36+16] = 0.0 + (float)(c / 10) / 4;
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
	quad[totalCar * 36 + 36+28] = 0.25 + (float)(c / 10) / 4;
	quad[totalCar * 36 + 36+29] = id;
	quad[totalCar * 36 + 36+30] = x + xsize;
	quad[totalCar * 36 + 36+31] = y + ysize;
	quad[totalCar * 36 + 36+32] = z;
	quad[totalCar * 36 + 36+33] = 0.1 + (float)(c % 10) / 10;
	quad[totalCar * 36 + 36+34] = 0.0 + (float)(c / 10) / 4;
	quad[totalCar * 36 + 36+35] = id;
	glBufferData(GL_ARRAY_BUFFER,(totalCar * 36 + 36) * sizeof(float),quad,GL_DYNAMIC_DRAW);
	totalCar++;
}

void drawSprite(float x,float y,float z,float id,float xsize,float ysize){
	quad[totalCar * 36 + 36]    = x + xsize;
	quad[totalCar * 36 + 36+1]  = y;
	quad[totalCar * 36 + 36+2]  = z;
	quad[totalCar * 36 + 36+3]  = 1.0;
	quad[totalCar * 36 + 36+4]  = 0.0;
	quad[totalCar * 36 + 36+5]  = id;
	quad[totalCar * 36 + 36+6]  = x;
	quad[totalCar * 36 + 36+7]  = y;
	quad[totalCar * 36 + 36+8]  = z;
	quad[totalCar * 36 + 36+9]  = 0.0;
	quad[totalCar * 36 + 36+10] = 0.0;
	quad[totalCar * 36 + 36+11] = id;
	quad[totalCar * 36 + 36+12] = x + xsize;
	quad[totalCar * 36 + 36+13] = y + ysize;
	quad[totalCar * 36 + 36+14] = z;
	quad[totalCar * 36 + 36+15] = 1.0;
	quad[totalCar * 36 + 36+16] = 1.0;
	quad[totalCar * 36 + 36+17] = id;
	quad[totalCar * 36 + 36+18] = x;
	quad[totalCar * 36 + 36+19] = y + ysize;
	quad[totalCar * 36 + 36+20] = z;
	quad[totalCar * 36 + 36+21] = 0.0;
	quad[totalCar * 36 + 36+22] = 1.0;
	quad[totalCar * 36 + 36+23] = id;
	quad[totalCar * 36 + 36+24] = x;
	quad[totalCar * 36 + 36+25] = y;
	quad[totalCar * 36 + 36+26] = z;
	quad[totalCar * 36 + 36+27] = 0.0;
	quad[totalCar * 36 + 36+28] = 0.0;
	quad[totalCar * 36 + 36+29] = id;
	quad[totalCar * 36 + 36+30] = x + xsize;
	quad[totalCar * 36 + 36+31] = y + ysize;
	quad[totalCar * 36 + 36+32] = z;
	quad[totalCar * 36 + 36+33] = 1.0;
	quad[totalCar * 36 + 36+34] = 1.0;
	quad[totalCar * 36 + 36+35] = id;
	glBufferData(GL_ARRAY_BUFFER,(totalCar * 36 + 36) * sizeof(float),quad,GL_DYNAMIC_DRAW);
	totalCar++;
}

void drawVar(float x,float y,int val){
	if(val == 0){
		drawChar(0,x+0.03,y,-0.99,0,0.04,0.04);
		return;
	}
	int size = 0;
	int tval = val;
	while(tval > 0){
		tval /= 10;
		size++;
	}
	if(!size){
		drawChar(26,x + 1.0 / 30,y,-0.99,0,0.04,0.04);
	}
	else{
		for(int i = 0;i < size;i++){
			drawChar(val % 10,x + (float)(size - i) / 30,y,-0.99,0,0.04,0.04);
			val /= 10;
		}
	}
}

void drawWord(char *str,float x,float y,float id){
	for(int i = 0;i < strlen(str);i++){
		if(str[i] == ' '){
			continue;
		}
		if(str[i] > 0x2f && str[i] < 0x3a){
			drawChar(str[i] - 0x30,x+i*0.0235,y,-0.1,id,0.04,0.04);
		}
		else{
			drawChar(str[i] - 87,x+i*0.0235,y,-0.1,id,0.04,0.04);
		}

	}
}

void drawUI(){
	drawChar(33,-0.9,0.9,-0.99,0,0.04,0.04);
	drawVar(-0.9,0.9,player->xpos);
	drawChar(34,-0.75,0.9,-0.99,0,0.04,0.04);
	drawVar(-0.75,0.9,player->ypos);
	drawChar(35,-0.6,0.9,-0.99,0,0.04,0.04);
	drawVar(-0.6,0.9,player->zpos);
	drawVar(-0.9,0.8,fabs(player->xvel * 100));
	drawVar(-0.75,0.8,fabsf(player->yvel * 100));
	drawVar(-0.6,0.8,fabsf(player->zvel * 100));
	drawChar(15,-0.96,0.7,-0.99,0,0.04,0.04);
	drawChar(25,-0.93,0.7,-0.99,0,0.04,0.04);
	drawChar(28,-0.9,0.7,-0.99,0,0.04,0.04);
	drawVar(-0.9,0.7,fps);

	if(blockSel < 16){
		drawWord(words[blockSel],-0.9,-0.9,0.0);
	}
	if(toolSel < 5){
		drawWord(words2[toolSel],-0.9,-0.78,0.0);
	}
	drawVar(0.8,-0.90,colorSel.r);
	drawVar(0.8,-0.85,colorSel.g);
	drawVar(0.8,-0.80,colorSel.b);
	drawVar(0.8,-0.75,colorSel.a);
	for(int i = 0;i < buttonC;i++){
		VEC2 buttonMiddle = {button[i].pos.x+0.015f,button[i].pos.y+0.025f};
		if(distance(mousePos,buttonMiddle)<0.03f){
			buttonId = button[i].id;
			drawSprite(button[i].pos.x,button[i].pos.y,-0.05f,4,0.03f,0.05f);
			for(;i < buttonC;i++){
				drawSprite(button[i].pos.x,button[i].pos.y,-0.05f,3,0.03f,0.05f);
			}
			goto foundButton;
		}
		else{
			drawSprite(button[i].pos.x,button[i].pos.y,-0.05f,3,0.03f,0.05f);
		}
	}
	buttonId = -1;
foundButton:
	drawSprite(0.8,-0.65,-0.2,7,0.1,0.1);
	switch(menuSel){
	case 0:
		drawChar(36,-0.01,-0.02,-0.99,0,0.04,0.04);
		break;
	case 1:{
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(window,&p);
		mousePos.x = (float)p.x/properties->xres*2.0-1.0;
		mousePos.y = -((float)p.y/properties->yres*2.0-1.0);
		drawSprite(mousePos.x-0.0075f,mousePos.y-0.0125f,-0.2,6,0.015,0.025);
		drawWord("settings",-0.11f,0.42f,0.0f);
		drawWord("save world",-0.45f,-0.21f,0.0f);
		drawWord("load world",-0.45f,-0.28f,0.0f);
		drawWord("create world",-0.45f,-0.35f,0.0f);
		drawWord("quit",-0.45f,-0.42f,0.0f);
		drawWord("lightmapsize",-0.45f,-0.07f,-0.0f);
		drawVar(-0.15f,-0.07f,properties->lmapSz);
		drawSprite(-0.5f,-0.5f,0.0f,2,1.0f,1.0f);
		break;
		}
	case 2:{
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(window,&p);
		mousePos.x = (float)p.x/properties->xres*2.0-1.0;
		mousePos.y = -((float)p.y/properties->yres*2.0-1.0);
		drawSprite(mousePos.x-0.0075f,mousePos.y-0.0125f,-0.2,6,0.015,0.025);
		drawWord("worlds",-0.11f,0.42f,0.0f);
		drawWord("load",0.02f,0.35f,0.0f);
		drawWord("delete",0.19f,0.35f,0.0f);
		for(int i = 0;i < fileNames.strC;i++){
			drawWord(fileNames.str[i],-0.45f,0.28f-(float)i/15.0f,0.0f);
		}
		drawSprite(-0.5,-0.5,0.0,2,1.0,1.0);
		break;
		}
	case 3:{
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(window,&p);
		mousePos.x = (float)p.x/properties->xres*2.0-1.0;
		mousePos.y = -((float)p.y/properties->yres*2.0-1.0);
		drawSprite(mousePos.x-0.0075f,mousePos.y-0.0125f,-0.2,6,0.015,0.025);
		drawWord("save",-0.11f,0.42f,0.0f);
		drawWord(inputStr,-0.45f,0.0f,0.0f);
		drawSprite(-0.5,-0.5,0.0,2,1.0,1.0);
		}
	}

}
