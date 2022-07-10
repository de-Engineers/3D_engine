#include <windows.h>
#include <GL/gl.h>
#include "main.h"
#include <stdio.h>
#include <math.h>
#include <intrin.h>

#include "tmgl.h"

#pragma comment(lib,"opengl32.lib")

#define GL_RED_INTEGER 0x8D94
#define GL_R32UI 0x8236
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_ARRAY_BUFFER 0x8892
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE1 0x84C1
#define GL_TEXTURE2 0x84C2
#define GL_TEXTURE3 0x84C3
#define GL_TEXTURE4 0x84C4
#define GL_TEXTURE5 0x84C5
#define GL_TEXTURE6 0x84C6
#define GL_TEXTURE7 0x84C7
#define GL_TEXTURE8 0x84C8
#define GL_TEXTURE9 0x84C9
#define GL_TEXTURE10 0x84CA
#define GL_TEXTURE11 0x84CB
#define GL_TEXTURE12 0x84CC
#define GL_TEXTURE13 0x84CD
#define GL_TEXTURE14 0x84CE
#define GL_TEXTURE15 0x84CF
#define GL_TEXTURE16 0x84D0
#define GL_TEXTURE17 0x84D1
#define GL_TEXTURE18 0x84D2
#define GL_TEXTURE19 0x84D3
#define GL_TEXTURE20 0x84D4
#define GL_TEXTURE21 0x84D5
#define GL_TEXTURE22 0x84D6
#define GL_TEXTURE23 0x84D7
#define GL_TEXTURE24 0x84D8
#define GL_TEXTURE25 0x84D9
#define GL_TEXTURE26 0x84DA
#define GL_TEXTURE27 0x84DB
#define GL_TEXTURE28 0x84DC
#define GL_TEXTURE29 0x84DD
#define GL_TEXTURE30 0x84DE
#define GL_TEXTURE31 0x84DF
#define GL_TEXTURE_3D 0x806F
#define GL_RGBA32F 0x8814

char *VERTsource;
char *FRAGsource;
char *VERTsourceFont;
char *FRAGsourceFont;
char *fontImage;
char *epicTexture;
char *epicTexture2;
char *slope;
char *spikes;
char *chessPieces;
char *models8;

int glMesC;
int tick;

OPENGLMESSAGE *glMes;

unsigned int shaderProgram;
unsigned int shaderProgramFont;
unsigned int VBO;

unsigned int vertexShader;
unsigned int fragmentShader;
unsigned int vertexShaderFont;
unsigned int fragmentShaderFont;
unsigned int mapText;
unsigned int mapTextFont;
unsigned int *blockTextures[30];
unsigned int entityTexture;
unsigned int chessText;
unsigned int models8Text;
unsigned int VAO;
unsigned int lpmapText;
unsigned int lmapText;

unsigned char *inputStr;

long long fps = 1;

PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR), 1,
PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,PFD_TYPE_RGBA,
24,0, 0, 0, 0, 0, 0,0,0,0,
0,0,0,0,32,0,0,PFD_MAIN_PLANE,
0,0,0,0	};

float quad[8192]  = {1.0,1.0 ,0.999,1.0,0.0,1.0,
					-1.0,1.0 ,0.999,1.0,0.0,1.0,
					1.0,-1.0 ,0.999,1.0,0.0,1.0,
					-1.0,-1.0,0.999,1.0,0.0,1.0,
					-1.0,1.0 ,0.999,1.0,0.0,1.0,
					1.0,-1.0 ,0.999,1.0,0.0,1.0};

char *loadTexture(char *name){
	HANDLE h = CreateFileA(name,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	int fsize = GetFileSize(h,0);
	char *text = HeapAlloc(GetProcessHeap(),8,fsize+1);
	SetFilePointer(h,138,0,0);
	ReadFile(h,text,fsize - 138,0,0);
	CloseHandle(h);
	return text;
}

char *loadFile(char *name){
	HANDLE h = CreateFileA(name,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	int fsize = GetFileSize(h,0);
	char *file = HeapAlloc(GetProcessHeap(),8,fsize+1);
	ReadFile(h,file,fsize+1,0,0);
	CloseHandle(h);
	return file;
}

void openGL(){
	glMes = HeapAlloc(GetProcessHeap(),8,sizeof(OPENGLMESSAGE) * 1024);

	VERTsource     = loadFile("shaders/vertex.vert");
	FRAGsource     = loadFile("shaders/fragment.frag");
	VERTsourceFont = loadFile("shaders/fontvert.vert");
	FRAGsourceFont = loadFile("shaders/fontfrag.frag");

	chessPieces = loadFile("3D models/model16.mdl");
	models8     = loadFile("3D models/swastika.mdl");

	fontImage    = loadTexture("textures/font.bmp");
	epicTexture  = loadTexture("textures/epic_texture.bmp");
	slope        = loadTexture("textures/slope.bmp");
	spikes       = loadTexture("textures/spikes.bmp");

	SetPixelFormat(dc, ChoosePixelFormat(dc, &pfd), &pfd);
	wglMakeCurrent(dc, wglCreateContext(dc));

	glCreateProgram			  = wglGetProcAddress("glCreateProgram");
	glCreateShader			  = wglGetProcAddress("glCreateShader");
	glShaderSource			  = wglGetProcAddress("glShaderSource");
	glCompileShader			  = wglGetProcAddress("glCompileShader");
	glAttachShader			  = wglGetProcAddress("glAttachShader");
	glLinkProgram			  = wglGetProcAddress("glLinkProgram");
	glUseProgram			  = wglGetProcAddress("glUseProgram");
	glEnableVertexAttribArray = wglGetProcAddress("glEnableVertexAttribArray");
	glVertexAttribPointer     = wglGetProcAddress("glVertexAttribPointer");
	glBufferData              = wglGetProcAddress("glBufferData");
	glCreateBuffers           = wglGetProcAddress("glCreateBuffers");
	glBindBuffer              = wglGetProcAddress("glBindBuffer");
	glGetShaderInfoLog        = wglGetProcAddress("glGetShaderInfoLog");
	glTexSubImage3D			  = wglGetProcAddress("glTexSubImage3D");
	glUniform1i				  = wglGetProcAddress("glUniform1i");
	glUniform4f				  = wglGetProcAddress("glUniform4f");
	glActiveTexture			  = wglGetProcAddress("glActiveTexture");
	glGenerateMipmap		  = wglGetProcAddress("glGenerateMipmap");
	glUniform3f				  = wglGetProcAddress("glUniform3f");
	glUniform2i				  = wglGetProcAddress("glUniform2i");
	glTexImage3D			  = wglGetProcAddress("glTexImage3D");
	glUniformMatrix3fv		  = wglGetProcAddress("glUniformMatrix3fv");
	glGetUniformLocation      = wglGetProcAddress("glGetUniformLocation");
	glUniform1f				  = wglGetProcAddress("glUniform1f");
	glUniform2f               = wglGetProcAddress("glUniform2f");
	wglSwapIntervalEXT        = wglGetProcAddress("wglSwapIntervalEXT");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);  
	shaderProgram = glCreateProgram();
	shaderProgramFont = glCreateProgram();
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	vertexShaderFont = glCreateShader(GL_VERTEX_SHADER);
	fragmentShaderFont = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertexShader,1,(const char**)&VERTsource,0);
	glShaderSource(fragmentShader,1,(const char**)&FRAGsource,0);
	glShaderSource(vertexShaderFont,1,(const char**)&VERTsourceFont,0);
	glShaderSource(fragmentShaderFont,1,(const char**)&FRAGsourceFont,0);
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);
	glCompileShader(vertexShaderFont);
	glCompileShader(fragmentShaderFont);
	char *bericht = HeapAlloc(GetProcessHeap(),8,1000);
	glGetShaderInfoLog(fragmentShader,1000,0,bericht);
	printf("%s\n",bericht);
	glGetShaderInfoLog(vertexShader,1000,0,bericht);
	printf("%s\n",bericht);
	glAttachShader(shaderProgram,vertexShader);
	glAttachShader(shaderProgram,fragmentShader);
	glAttachShader(shaderProgramFont,vertexShaderFont);
	glAttachShader(shaderProgramFont,fragmentShaderFont);
	glLinkProgram(shaderProgram);
	glLinkProgram(shaderProgramFont);
	glUseProgram(shaderProgram);

	glGenTextures(1,&mapText);
	glGenTextures(1,&mapTextFont);
	glGenTextures(1,&lpmapText);
	glGenTextures(1,&lmapText);
	glGenTextures(30,(void*)blockTextures);
	glGenTextures(1,&chessText);
	glGenTextures(1,&models8Text);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_3D,lpmapText);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage3D(GL_TEXTURE_3D,0,GL_R32UI,properties->lvlSz*6,properties->lvlSz,properties->lvlSz,0,GL_RED_INTEGER,GL_UNSIGNED_INT,lpmap);

	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_3D,lmapText);
	glTexImage3D(GL_TEXTURE_3D,0,GL_RGB8,lmapC,properties->lmapSz,properties->lmapSz,0,GL_RGB,GL_UNSIGNED_BYTE,lmap);
	glGenerateMipmap(GL_TEXTURE_3D);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D,mapText);
	glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,properties->lvlSz,properties->lvlSz,properties->lvlSz,0,GL_RGBA,GL_UNSIGNED_BYTE,map);
	glGenerateMipmap(GL_TEXTURE_3D);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D,(unsigned int)blockTextures[0]);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,2068,1024,0,GL_RGBA,GL_UNSIGNED_BYTE,epicTexture);
	glGenerateMipmap(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,mapTextFont);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,320,144,0,GL_RGBA,GL_UNSIGNED_BYTE,fontImage);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D,(unsigned int)blockTextures[1]);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,32,32,0,GL_RGBA,GL_UNSIGNED_BYTE,spikes);
	glGenerateMipmap(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D,(unsigned int)blockTextures[2]);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,1024,1024,0,GL_RGBA,GL_UNSIGNED_BYTE,slope);
	glGenerateMipmap(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_1D,entityTexture);
	glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA32F,256,0,GL_RGBA,GL_FLOAT,entity);
	glGenerateMipmap(GL_TEXTURE_1D);

	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_3D,chessText);
	glTexImage3D(GL_TEXTURE_3D,0,GL_RED,16,16,96,0,GL_RED,GL_UNSIGNED_BYTE,chessPieces);
	glGenerateMipmap(GL_TEXTURE_3D);
	
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_3D,models8Text);
	glTexImage3D(GL_TEXTURE_3D,0,GL_RED,8,8,8,0,GL_RED,GL_UNSIGNED_BYTE,models8);
	glGenerateMipmap(GL_TEXTURE_3D);

	glUniform1i(glGetUniformLocation(shaderProgram,"map"),1);
	glUniform1i(glGetUniformLocation(shaderProgram,"epicTexture"),2);
	glUniform1i(glGetUniformLocation(shaderProgram,"spikes"),3);
	glUniform1i(glGetUniformLocation(shaderProgram,"slope"),4);
	glUniform1i(glGetUniformLocation(shaderProgram,"entities"),5);
	glUniform1i(glGetUniformLocation(shaderProgram,"lpmap"),6);
	glUniform1i(glGetUniformLocation(shaderProgram,"chessModels"),7);
	glUniform1i(glGetUniformLocation(shaderProgram,"models8"),8);
	glUniform1i(glGetUniformLocation(shaderProgram,"lmap"),9);

	glCreateBuffers(1,&VBO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER,36 * sizeof(float),quad,GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,0,6 * sizeof(float),(void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,2,GL_FLOAT,0,6 * sizeof(float),(void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,1,GL_FLOAT,0,6 * sizeof(float),(void*)(5 * sizeof(float)));

	glUniform2i(glGetUniformLocation(shaderProgram,"reso"),properties->xres,properties->yres);
	glUniform2f(glGetUniformLocation(shaderProgram,"fov"),player->xfov,player->yfov);
	glUniform1i(glGetUniformLocation(shaderProgram,"mapSz"),properties->lvlSz);
	glUniform1f(glGetUniformLocation(shaderProgram,"lmapsz"),properties->lmapSz);

	glUseProgram(shaderProgramFont);
	glUniform2i(glGetUniformLocation(shaderProgramFont,"reso"),properties->xres,properties->yres);
	glUseProgram(shaderProgram);
	for(;;){
		long long timeB;
		QueryPerformanceCounter(&timeB);
		if(settings & 0x80){
			SwapBuffers(dc);
			Sleep(100);
		}
		else{
			if(settings & 0x20){
				drawUI();
			}
			glUseProgram(shaderProgram);

			while(glMesC > 0){
				glMesC--;
				switch(glMes[glMesC].id){
				case 0:	
					glViewport(0,0,properties->xres,properties->yres);
					glUniform2i(glGetUniformLocation(shaderProgram,"reso"),properties->xres,properties->yres);
					glUseProgram(shaderProgramFont);
					glUniform2i(glGetUniformLocation(shaderProgramFont,"reso"),properties->xres,properties->yres);
					glUseProgram(shaderProgram);
					break;
				case 1:
					glActiveTexture(GL_TEXTURE1);
					glTexSubImage3D(GL_TEXTURE_3D,0,glMes[glMesC].data1,glMes[glMesC].data2,glMes[glMesC].data3,1,1,1,GL_RGBA,GL_UNSIGNED_BYTE,map+(glMes[glMesC].data1 + glMes[glMesC].data2 * properties->lvlSz + glMes[glMesC].data3 * properties->lvlSz * properties->lvlSz));
					lmapC = 0;
					break;
				case 2:
					glTexSubImage3D(GL_TEXTURE_3D,0,glMes[glMesC].data1,glMes[glMesC].data2,glMes[glMesC].data3,glMes[glMesC].data4,glMes[glMesC].data5,glMes[glMesC].data6,GL_RED,GL_UNSIGNED_BYTE,map+glMes[glMesC].data1 + glMes[glMesC].data2 * properties->lvlSz + glMes[glMesC].data3 * properties->lvlSz * properties->lvlSz);
					break;
				case 3:
					glActiveTexture(GL_TEXTURE1);
					glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,properties->lvlSz,properties->lvlSz,properties->lvlSz,0,GL_RGBA,GL_UNSIGNED_BYTE,map);
					lmapC = 0;
					break;
				case 4:
					drawSprite(glMes[glMesC].fdata1,glMes[glMesC].fdata2,glMes[glMesC].fdata5,glMes[glMesC].fdata6,glMes[glMesC].fdata3,glMes[glMesC].fdata4);
					break;
				case 5:
					glActiveTexture(GL_TEXTURE6);
					break;
				case 6:{
					glActiveTexture(GL_TEXTURE6);
					glTexImage3D(GL_TEXTURE_3D,0,GL_R32UI,properties->lvlSz*6,properties->lvlSz,properties->lvlSz,0,GL_RED_INTEGER,GL_UNSIGNED_INT,lpmap);
					glActiveTexture(GL_TEXTURE9);
					glTexImage3D(GL_TEXTURE_3D,0,GL_RGB,properties->lmapSz,properties->lmapSz,lmapC,0,GL_RGB,GL_UNSIGNED_BYTE,lmap);
					glGenerateMipmap(GL_TEXTURE_3D);
					glUniform1f(glGetUniformLocation(shaderProgram,"lmapsz"),properties->lmapSz);
					break;
					}
				}
			}
			drawSprite(0.0,0.0,0.0,0.0,0.0,0.0);
			sprite = 0;
			glActiveTexture(GL_TEXTURE6);
			glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA32F,256,0,GL_RGBA,GL_FLOAT,entity); 
			glUniform2f(glGetUniformLocation(shaderProgram,"angle"),player->xangle,player->yangle);
			glUniform4f(glGetUniformLocation(shaderProgram,"dir"),player->xdir,player->ydir, player->zdir,player->xydir);
			glUniform3f(glGetUniformLocation(shaderProgram,"Pos"),player->xpos,player->ypos,player->zpos);
			glUniform1i(glGetUniformLocation(shaderProgram,"tick"),tick);
			glUniform1i(glGetUniformLocation(shaderProgram,"state"),settings);
			glUniform1i(glGetUniformLocation(shaderProgram,"entityC"),entityC);
			glUniform1i(glGetUniformLocation(shaderProgram,"renderDistance"),properties->renderDistance);
			glUniformMatrix3fv(glGetUniformLocation(shaderProgram, "cameraMatrix"), 1, GL_FALSE, (void*)&cameraMatrix);
			glUseProgram(shaderProgramFont);
			glUniform3f(glGetUniformLocation(shaderProgramFont,"color"),(float)colorSel.r/255.0f,(float)colorSel.g/255.0f,(float)colorSel.b/255.0f);
			glUseProgram(shaderProgram);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDrawArrays(GL_TRIANGLES,0,6);
			glUseProgram(shaderProgramFont);
			glDrawArrays(GL_TRIANGLES,6,totalCar*6+6);
			SwapBuffers(dc);
			totalCar = 0;
		}
		QueryPerformanceCounter(&fps);
		fps -= timeB;
	}
}
