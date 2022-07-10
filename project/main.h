#pragma once

#define PI_05  1.57079632679f
#define PI_2   6.28318530718f
#define PI     3.14159265358f
#define PI_025 0.78539816339f


#include <windows.h>
#include "vec3.h"

typedef struct{
	unsigned char id;
	unsigned char r;
	unsigned char g;
	unsigned char b;
}MAP;

typedef struct{
	unsigned int p1;
	unsigned int p2;
	unsigned int p3;
	unsigned int p4;
	unsigned int p5;
	unsigned int p6;
}LPMAP;

typedef struct{ 
	unsigned char x;
	unsigned char y;
	unsigned char z;
}CVEC3;

typedef struct {
	VEC3 Right;
	VEC3 Up;
	VEC3 Front;
} Mat3;

typedef struct{
	float x;
	float y;
}VEC2;

typedef struct{
	float xangle;
	float yangle;

	float xpos;
	float ypos;
	float zpos;

	float xfov;
	float yfov;

	float xvel;
	float yvel;
	float zvel;

	float xydir;
	float xdir;
	float ydir;
	float zdir;

	float xspawn;
	float yspawn;
	float zspawn;
}PLAYERDATA;

typedef struct{
	int xres;
	int yres;
	int lvlSz;
	int renderDistance;
	float fog;
	int lmapSz;
	int lmapSz2;
	int lmapSz3;
}PROPERTIES;

typedef struct{
	float x;
	float y;
	float z;

	float vx;
	float vy;
	float vz;

	float sz;

	float cd1;
	float cd2;
	float cd3;
	int id;
}ENTITY;

typedef struct{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
}RGBA;

typedef struct{
	unsigned char r;
	unsigned char g;
	unsigned char b;
}RGB;

typedef struct{
	short id;
	short data1;
	short data2;
	short data3;
	short data4;
	short data5;
	short data6;
	float fdata1;
	float fdata2;
	float fdata3;
	float fdata4;
	float fdata5;
	float fdata6;
}OPENGLMESSAGE;

typedef struct{
	float x;
	float y;
	float z;
	float vx;
	float vy;
	float vz;
	float deltax;
	float deltay;
	float deltaz;
	float sidex;
	float sidey;
	float sidez;
	int stepx;
	int stepy;
	int stepz;
	int side;
	int ix;
	int iy;
	int iz;
}RAY;

typedef struct{
	VEC2 pos;
	unsigned char id;
}BUTTON;

typedef struct{
	unsigned int strC;
	char **str;
}STRINGS;

extern unsigned char buttonC;
extern BUTTON *button;

extern int glMesC;

extern unsigned char blockSel;
extern unsigned char toolSel;

extern PLAYERDATA     *player;
extern PROPERTIES     *properties;
extern ENTITY         *entity;
extern OPENGLMESSAGE  *glMes;
extern MAP            *map;
extern LPMAP          *lpmap;
extern RGB            *lmap;

extern HDC dc;
extern HWND window;
extern float brightness;
extern int entityC;
extern char sprite;
extern char buttonId;
extern int tick;
extern int staticentityC;
extern int settings;
extern RGBA colorSel;
extern CVEC3 selarea;
extern Mat3 cameraMatrix;
extern void (*buttons[6])();
extern unsigned char menuSel;
extern STRINGS fileNames;
extern unsigned char tempVar[2];
extern float quad[8192];
extern unsigned int totalCar;
extern unsigned int lmapC;
extern long long fps;

extern unsigned char *inputStr;

inline void buttonCreate(VEC2 pos,unsigned char id);
inline void buttonDestroy(unsigned char id);

void openGL();
void openCLmain();
void levelgen();
void commands();
void entities();
void rayItterate(RAY *ray);
void sound(int type);
void initSound();
void tools();
void ittmap();
void spawnEntity(float x,float y,float z,float vx,float vy,float vz,float sz,int id);
void updateCamera();
void updateLight2();
void levelSave();
void levelLoad();
void drawUI();
void drawSprite(float x,float y,float z,float id,float xsize,float ysize);
void updateLight(int pos,float r,float g,float b);
void updateLightSingle(unsigned int block);

RAY rayCreate(VEC3 pos,VEC3 dir);

unsigned int crds2map(int x,int y,int z);
unsigned int crds2lmap(int x,int y,int z);
CVEC3 map2crds(int 	map);

char *loadFile(char *name);

inline int max3(int val1,int val2,int val3);







