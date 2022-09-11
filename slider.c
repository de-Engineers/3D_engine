#include <windows.h>
#include <stdio.h>
#include <intrin.h>

#include "network.h"
#include "main.h"

i8 sliderId = -1;
u8 sliderPos;

BUTTON *slider;

u8 sliderC;


void sliderCreate(VEC2 pos,u8 id){
	slider[sliderC].pos = pos;
	slider[sliderC].id  = id;
	sliderC++;
}

void sliderRed(u8 pos){
	colorSel.r = pos;
	map[editBlockSel].r = colorSel.r;
	sliderValues.col.r = pos;
	glMes[glMesC].id = 3;
	glMesC++;
}

void sliderGreen(u8 pos){
	colorSel.g = pos;
	map[editBlockSel].g = colorSel.g;
	sliderValues.col.g = pos;
	glMes[glMesC].id = 3;
	glMesC++;
}

void sliderBlue(u8 pos){
	colorSel.b = pos;
	map[editBlockSel].b = colorSel.b;
	sliderValues.col.b = pos;
	glMes[glMesC].id = 3;
	glMesC++;
}

void sliderMetadt1x(u8 pos){
	if(settings & 0x40){
		metadt4Sel.r = pos;
		metadt4[editBlockSel].r = metadt4Sel.r;
	}
	else{
		metadtSel.r = pos;
		metadt[editBlockSel].r = metadtSel.r;
	}
	sliderValues.metadt.x = pos;
	glMes[glMesC].id = 3;
	glMesC++;
}

void sliderMetadt1y(u8 pos){
	if(settings & 0x40){
		metadt4Sel.g = pos;
		metadt4[editBlockSel].g = metadt4Sel.g;
	}
	else{
		metadtSel.g = pos;
		metadt[editBlockSel].g = metadtSel.g;
	}
	sliderValues.metadt.y = pos;
	glMes[glMesC].id = 3;
	glMesC++;
}

void sliderMetadt1z(u8 pos){
	if(settings & 0x40){
		metadt4Sel.a = pos;
		metadt4[editBlockSel].id= metadt4Sel.a;
	}
	else{
		metadtSel.a = pos;
		metadt[editBlockSel].id= metadtSel.a;
	}
	sliderValues.metadt.z = pos;
	glMes[glMesC].id = 3;
	glMesC++;
}

void sliderMetadt2x(u8 pos){
	if(settings & 0x40){
		metadt5Sel.r = pos;
		metadt5[editBlockSel].r = metadt5Sel.r;
	}
	else{
		metadt2Sel.r = pos;
		metadt2[editBlockSel].r = metadt2Sel.r;
	}
	sliderValues.metadt2.x = pos;
	glMes[glMesC].id = 3;
	glMesC++;
}

void sliderMetadt2y(u8 pos){
	if(settings & 0x40){
		metadt5Sel.g = pos;
		metadt5[editBlockSel].g = metadt5Sel.g;
	}
	else{
		metadt2Sel.g = pos;
		metadt2[editBlockSel].g = metadt2Sel.g;
	}
	sliderValues.metadt2.y = pos;
	glMes[glMesC].id = 3;
	glMesC++;
}

void sliderMetadt2z(u8 pos){
	if(settings & 0x40){
		metadt5Sel.a = pos;
		metadt5[editBlockSel].id= metadt5Sel.a;
	}
	else{
		metadt2Sel.a = pos;
		metadt2[editBlockSel].id= metadt2Sel.a;
	}
	sliderValues.metadt2.z = pos;
	glMes[glMesC].id = 3;
	glMesC++;
}

void sliderMetadt3x(u8 pos){
	if(settings & 0x40){
		metadt6Sel.r = pos;
		metadt6[editBlockSel].r = metadt6Sel.r;
	}
	else{
		metadt3Sel.r = pos;
		metadt3[editBlockSel].r = metadt3Sel.r;
	}
	sliderValues.metadt3.x = pos;
	glMes[glMesC].id = 3;
	glMesC++;
}

void sliderMetadt3y(u8 pos){
	if(settings & 0x40){
		metadt6Sel.g = pos;
		metadt6[editBlockSel].g = metadt6Sel.g;
	}
	else{
		metadt3Sel.g = pos;
		metadt3[editBlockSel].g = metadt3Sel.g;
	}
	sliderValues.metadt3.y = pos;
	glMes[glMesC].id = 3;
	glMesC++;
}

void sliderMetadt3z(u8 pos){
	if(settings & 0x40){
		metadt6Sel.a = pos;
		metadt6[editBlockSel].id= metadt6Sel.a;
	}
	else{
		metadt3Sel.a = pos;
		metadt3[editBlockSel].id= metadt3Sel.a;
	}
	sliderValues.metadt3.z = pos;
	glMes[glMesC].id = 3;
	glMesC++;
}

void sliderFOV(u8 pos){
	player->fov.y = (f32)pos/127.5f;
	player->fov.x = (f32)pos/127.5f*16.0f/9.0f;
	sliderValues.fov = pos;
	glMes[glMesC].id = 10;
	glMesC++;
}

void sliderSensitivity(u8 pos){
	properties->sensitivity = (f32)pos/255.0f;
	sliderValues.sensitivity = pos;
	glMes[glMesC].id = 11;
	glMesC++;
}

void sliderIP1(u8 pos){
	sliderValues.serverIP.p1 = pos;
}

void sliderIP2(u8 pos){
	sliderValues.serverIP.p2 = pos;
}

void sliderIP3(u8 pos){
	sliderValues.serverIP.p3 = pos;
}

void sliderIP4(u8 pos){
	sliderValues.serverIP.p4 = pos;
}

void (*sliders[32])(u8 pos) = {sliderRed,sliderGreen,sliderBlue,sliderMetadt1x,sliderMetadt1y,sliderMetadt1z,
sliderMetadt2x,sliderMetadt2y,sliderMetadt2z,sliderMetadt3x,sliderMetadt3y,sliderMetadt3z,sliderFOV,sliderSensitivity,
sliderIP1,sliderIP2,sliderIP3,sliderIP4};