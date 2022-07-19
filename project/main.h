#pragma once

#define resx 512
#define resy 512

#define RENDERDISTANCE 32

#define MAPSZ 64

#define MAPRAM MAPSZ*MAPSZ*MAPSZ*8
#define BLOCKCOUNT MAPSZ*MAPSZ*MAPSZ

#define VRAM resx*resy*4

#define LMAPSZ 4
#define LMAPSZT LMAPSZ*LMAPSZ*LMAPSZ

#define PI_05  1.57079632679f
#define PI_2   6.28318530718f
#define PI     3.14159265358f
#define PI_025 0.78539816339f


#include <windows.h>
#include "vec3.h"
#include "vec2.h"

typedef char               i8;
typedef short              i16;
typedef int                i32;
typedef long long          i64;

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

typedef float              f32;
typedef double             f64;

typedef struct{
	u8 id;
	u8 r;
	u8 g;
	u8 b;
}MAP;

typedef struct{
	u32 p1;
	u32 p2;
	u32 p3;
	u32 p4;
	u32 p5;
	u32 p6;
}LPMAP;

typedef struct{ 
	u8 x;
	u8 y;
	u8 z;
}CVEC3;

typedef struct {
	VEC3 Right;
	VEC3 Up;
	VEC3 Front;
} Mat3;

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
	f32 fog;
	u32 lmapSzb;
	u32 lmapSz;
	u32 lmapSz2;
	u32 lmapSz3;
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
	u8 r;
	u8 g;
	u8 b;
	u8 a;
}RGBA;

typedef struct{
	u8 r;
	u8 g;
	u8 b;
}RGB;

typedef struct{
	u16 r;
	u16 g;
	u16 b;
}EXRGB;

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
	VEC3 pos;
	VEC3 dir;
	VEC3 delta;
	VEC3 side;
	int stepx;
	int stepy;
	int stepz;
	int sid;
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
extern MAP            *metadt;
extern MAP            *metadt2;
extern MAP            *metadt3;
extern LPMAP          *lpmap;
extern EXRGB          *lmap;

extern HDC dc;
extern HWND window;
extern int entityC;
extern char sprite;
extern char buttonId;
extern int tick;
extern int staticentityC;
extern int settings;
extern RGBA colorSel;
extern RGBA metadtSel;
extern RGBA metadt2Sel;
extern RGBA metadt3Sel;
extern CVEC3 selarea;
extern Mat3 cameraMatrix;
extern void (*buttons[6])();
extern unsigned char menuSel;
extern STRINGS fileNames;
extern unsigned char tempVar[2];
extern float quad[8192];
extern unsigned int totalCar;
extern unsigned int totalCar;
extern unsigned int lmapC;
extern long long fps;
extern f32 brightness;

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
void initOpenCL();
void HDR();
void updateBlock(int pos,int val);

RAY rayCreate(VEC3 pos,VEC3 dir);

unsigned int crds2map(int x,int y,int z);
unsigned int crds2lmap(int x,int y,int z);
inline i32 irnd();
inline f32 rnd();
CVEC3 map2crds(u32 	map);

char *loadFile(char *name);

inline int max3(int val1,int val2,int val3);
inline float fract(float p);








