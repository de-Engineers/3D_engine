#pragma once

#include <windows.h>
#include "ivec3.h"
#include "vec3.h"
#include "vec2.h"

#define resx 512
#define resy 512

#define skyboxSz 1024

#define ENTITYTEXTSZ 32

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

#define BLOCK_VOID         0
#define BLOCK_AIR          1
#define BLOCK_AMBIENTLIGHT 2
#define BLOCK_LIGHT1       3
#define BLOCK_LIGHT2       4
#define BLOCK_LIGHT3       5
#define BLOCK_LIGHT4       6
#define BLOCK_LIGHT5       7
#define BLOCK_LIGHT6       8
#define BLOCK_SPHERE       9
#define BLOCK_CUBE         12
#define BLOCK_GLASS        15
#define BLOCK_REFLECTIVE2  18
#define BLOCK_SPAWN        25
#define BLOCK_CLIP         26
#define BLOCK_REFLECTIVE   27
#define BLOCK_SOLID        28

#define SETTINGS_MOVEMENT   0x0001
#define SETTINGS_FULLSCREEN 0x0002
#define SETTINGS_LIGHTING   0x0004
#define SETTINGS_FOG        0x0008
#define SETTINGS_UI         0x0020
#define SETTINGS_SUBBLOCK   0x0040
#define SETTINGS_PAUZE      0x0080
#define SETTINGS_GAMEPLAY   0x0100
#define SETTINGS_VSYNC      0x0200
#define SETTINGS_SMOOTH     0x0400

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
	union{
		u32 x;
		u32 r;
	};
	union{
		u32 y;
		u32 g;
	};
	union{
		u32 z;
		u32 b;
	};
	union{
		u32 w;
		u32 a;
	};
}IVEC4;

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
	u32 p7;
	u32 p8;
	u32 p9;
	u32 p10;
	u32 p11;
	u32 p12;
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
	f32 xangle;
	f32 yangle;

	VEC2 recoil;

	VEC3 pos;

	VEC2 fov;

	VEC3 vel;

	f32 xydir;
	f32 xdir;
	f32 ydir;
	f32 zdir;

	f32 stamina;

	f32 hitboxHeight;
	f32 hitboxWantedHeight;
	f32 hitboxWantedHeightQueued;

	f32 flightSpeed;
	f32 movementSpeed;
	f32 jumpHeight;

	u16 shotCooldown;
	u8  weaponEquiped;

	u8  health;
	u8  aniType;
	u16 aniTime;
}PLAYERDATA;


typedef struct{
	u32 p1;
	u32 p2;
	u32 p3;
	u32 p4;
}IPADDRESS;

typedef struct{
	u8  rayAcceleration;

	u16  godrayRes;
	u16  reflectRes;
	u16 xres;
	u16 yres;
	u16 windowOffsetX;
	u16 windowOffsetY;
	u16 lvlSz;

	u32 lmapSzb;
	u32 lmapSz;
	u32 tex3DSzLimit;

	f32 sensitivity;
	f32 gravity;
	f32 airFrictionVert;
	f32 airFrictionHor;
	f32 groundFriction;
	f32 godrayAmm;
}PROPERTIES;

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
	u16 r;
	u16 g;
	u16 b;
	u16 a;
}EXRGBA;

typedef struct{
	VEC3 pos;
	f32  id;
	VEC3 color;
	f32  rad;
	union{
		VEC3 pos2;
		VEC3 rot;
	};
	f32  tId;
	VEC3 scale;
	f32  res2;
	VEC3 srot;
	f32  res3;
}GPUDATA;

typedef struct{
	VEC3 vel;
	u8   id;
	f32  playerDist;
	union{
		u8 health;
		u8 playerid;
	};
	VEC3 pos;
	VEC3 baseColor;
	u8   aniType;
	u16  aniTime; 
}CPUDATA;

typedef struct{
	GPUDATA *gpu;
	CPUDATA *cpu;
}ENTITY;

typedef struct{
	VEC3 pos;
	u16 cooldown;
	u16 id;
	u8 power;
	union{
		u8 totalCooldown;
		u8 pickupId;
	};
}TURRET;

typedef struct{
	short id;
	u32 data1;
	u32 data2;
	u32 data3;
	u32 data4;
	u32 data5;
	u32 data6;
	f32 fdata1;
	f32 fdata2;
	f32 fdata3;
	f32 fdata4;
	f32 fdata5;
	f32 fdata6;
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
	IVEC3 col;
	IVEC4 metadt;
	IVEC4 metadt2;
	IVEC4 metadt3;
	u32 fov;
	u32 sensitivity;
	IPADDRESS serverIP;
}SLIDERVALUE;

typedef struct{
	VEC2 pos;
	u32 id;
}BUTTON;

typedef struct{
	unsigned int strC;
	char **str;
}STRINGS;

typedef struct{
	VEC3 pos;
	RGB col;
	VEC2 skyPos;
	RGB skyCol;
}STAR;

extern u8 starC;

extern unsigned char buttonC;
extern u8 sliderC;


extern BUTTON *button;
extern BUTTON *slider;

extern int glMesC;

extern unsigned char blockSel;
extern unsigned char toolSel;

extern PLAYERDATA     *player;
extern PROPERTIES     *properties;
extern OPENGLMESSAGE  *glMes;
extern MAP            *map;
extern MAP            *metadt;
extern MAP            *metadt2;
extern MAP            *metadt3;
extern MAP            *metadt4;
extern MAP            *metadt5;
extern MAP            *metadt6;
extern LPMAP          *lpmap;
extern EXRGB          *lmap;
extern EXRGB          *bmap;
extern RGB            *entityTexture;
extern RGB            *skyboxTexture;
extern STAR           *star;

extern VEC3           *lmapb;

extern ENTITY         entity;

extern TURRET         *turret;

extern u32 turretC;

extern u32 editBlockSel;
extern HDC dc;
extern HWND window;
extern u32 entityC;
extern char sprite;
extern i8 buttonId;
extern i8 sliderId;
extern u8 sliderPos;
extern int tick;
extern u32 settings;
extern RGBA colorSel;
extern RGBA metadtSel;
extern RGBA metadt2Sel;
extern RGBA metadt3Sel;
extern RGBA metadt4Sel;
extern RGBA metadt5Sel;
extern RGBA metadt6Sel;
extern CVEC3 selarea;
extern void (*sliders[32])(u8 pos);
extern void (*buttons[32])();
extern u8 menuSel;
extern STRINGS fileNames;
extern unsigned char tempVar[2];
extern f32 quad[8192];
extern unsigned int totalCar;
extern unsigned int lmapC;
extern long long fps;
extern f32 brightness;
extern u32 skyboxText;
extern u8 touchedSpace;
extern VEC2 mousePos;
extern u8 networkSettings;
extern u8 openglINIT;
extern SLIDERVALUE sliderValues;
extern HANDLE networkThread;
extern u8 playerspawnC;
extern VEC3 playerspawn[16];

extern unsigned char *inputStr;

void buttonCreate(VEC2 pos,unsigned char id);
void sliderCreate(VEC2 pos,u8 id);


void openGL();
void levelgen();
void entities();
void rayItterate(RAY *ray);
void initSound();
void tools();
void ittmap();
void updateLight2();
void levelLoad(char *name);
void drawUI();
void drawSprite(VEC3 pos,VEC2 size,f32 id);
void initOpenCL();
void HDR();
void spawnEntity(VEC3 pos,VEC3 vel,u8 id);
void spawnEntityEx(VEC3 pos,VEC3 pos2,VEC3 vel,u8 id,VEC3 color);
void playerWorldCollision();
void generateSkyBox();
void playerDeath();
void networking();
void spawnPlayer(u8 id);
void cpuGenLight(VEC3 pos, VEC3 color,u64 itt,f32 rndOffset);
void cpuGenLightAmbientX(VEC3 dir,VEC3 color,u64 itt);
void cpuGenLightAmbientY(VEC3 dir,VEC3 color,u64 itt);
void cpuGenLightAmbientZ(VEC3 dir,VEC3 color,u64 itt);

void closeEngine();

void setBlock(u32 block);

RAY rayCreate(VEC3 pos,VEC3 dir);

unsigned int crds2map(int x,int y,int z);
i32 irnd();
f32 rnd();
CVEC3 map2crds(u32 	map);
VEC3 getCoords(RAY ray);
VEC2 rotVEC2(VEC2 p,f32 rot);
VEC3 getSubCoords(RAY ray);
void entityDeath(int id);

char *loadFile(char *name);

i32 getLmapLocation(RAY *ray);
f32 fract(f32 p);

HANDLE physicsThread;
HANDLE entityThread;








