#pragma once

#include <windows.h>

typedef struct{ 
	unsigned char x;
	unsigned char y;
	unsigned char z;
}CVEC3;

typedef struct{
	float x;
	float y;
	float z;
}VEC3;

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

extern unsigned char buttonC;
extern BUTTON *button;

extern int glMesC;

extern unsigned char blockSel;
extern unsigned char toolSel;

extern PLAYERDATA     *player;
extern PROPERTIES     *properties;
extern ENTITY         *entity;
extern OPENGLMESSAGE  *glMes;
extern unsigned char  *map;
extern unsigned char  *mapdata;

extern HDC dc;
extern HWND window;
extern float brightness;
extern int entityC;
extern char sprite;
extern char buttonId;
extern int tick;
extern int staticentityC;
extern int settings;
extern RGB colorSel;
extern CVEC3 selarea;
extern Mat3 cameraMatrix;
extern void (*buttons[2])();

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

RAY rayCreate(float x,float y,float z,float rx,float ry,float rz);

unsigned int crds2map(int x,int y,int z);
unsigned int crds2lmap(int x,int y,int z);
CVEC3 map2crds(int 	map);

char *loadFile(char *name);

inline int max3(int val1,int val2,int val3);







