#include <windows.h>
#include <GL/gl.h>
#include "main.h"
#include <stdio.h>
#include <math.h>
#include <intrin.h>

#include "tmgl.h"

#pragma comment(lib,"opengl32.lib")

#define WGL_DRAW_TO_WINDOW_ARB            0x2001
#define WGL_SUPPORT_OPENGL_ARB            0x2010
#define WGL_COLOR_BITS_ARB                0x2014
#define WGL_RED_BITS_ARB                  0x2015
#define WGL_GREEN_BITS_ARB                0x2017
#define WGL_BLUE_BITS_ARB                 0x2019
#define WGL_ALPHA_BITS_ARB                0x201B
#define WGL_SAMPLES_ARB                   0x2042
#define WGL_DOUBLE_BUFFER_ARB             0x2011
#define WGL_STENCIL_BITS_ARB              0x2023

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
#define GL_FRAMEBUFFER 0x8D40
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_RENDERBUFFER 0x8D41
#define GL_DEPTH24_STENCIL8 0x88F0
#define GL_DEPTH_STENCIL_ATTACHMENT 0x821A
#define GL_MAX_3D_TEXTURE_SIZE 0x8073

i8 *VERTsource;
i8 *FRAGsource;
i8 *VERTsourceFont;
i8 *FRAGsourceFont;
i8 *FRAGsourceEditor;
i8 *fontImage;
i8 *VERTsourcePost;
i8 *FRAGsourcePost;
i8 *FRAGsourceSmooth;
i8 *epicTexture;
i8 *epicTexture2;
i8 *slope;
i8 *spikes;
i8 *chessPieces;
i8 *models8;

int glMesC;
int tick;

OPENGLMESSAGE *glMes;

u32 shaderProgram;
u32 shaderProgramFont;
u32 shaderProgramEditor;
u32 shaderProgramPost;
u32 shaderProgramSmooth;
u32 VBO;

u32 vertexShader;
u32 fragmentShader;
u32 vertexShaderFont;
u32 vertexShaderPost;
u32 fragmentShaderFont;
u32 fragmentShaderEditor;
u32 fragmentShaderPost;
u32 fragmentShaderSmooth;
u32 mapText;
u32 mapTextFont;
u32 *blockTextures[30];
u32 chessText;
u32 models8Text;
u32 VAO;
u32 lpmapText;
u32 lmapText;
u32 lmapText2;
u32 lmapText3;
u32 lmapText4;
u32 metadtText;
u32 metadt2Text;
u32 metadt3Text;
u32 metadt4Text;
u32 metadt5Text;
u32 metadt6Text;
u32 entityTextText;
u32 skyboxText;

u32 FBO;
u32 fboText;

u32 RBO;

u32 godraysText;
u32 entityText;

u32 quadVBO;

RGBA *postText;

unsigned char *inputStr;

long long fps = 1;

u8 openglINIT = 0;

f32 quadVertices[] = {
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
    1.0f, -1.0f,  1.0f, 0.0f,
	1.0f,  1.0f,  1.0f, 1.0f
};

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

void GPUtextureUpload(){
	if(lmapC > properties->tex3DSzLimit*3){
		glActiveTexture(GL_TEXTURE9);
		glBindTexture(GL_TEXTURE_3D, lmapText);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB16, properties->lmapSz,properties->lmapSz,properties->tex3DSzLimit,0, GL_RGB, GL_UNSIGNED_SHORT, lmap);
		glActiveTexture(GL_TEXTURE17);
		glBindTexture(GL_TEXTURE_3D, lmapText2);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB16, properties->lmapSz,properties->lmapSz,properties->tex3DSzLimit, 0, GL_RGB, GL_UNSIGNED_SHORT, lmap + properties->tex3DSzLimit * properties->lmapSz * properties->lmapSz);
		glActiveTexture(GL_TEXTURE18);
		glBindTexture(GL_TEXTURE_3D, lmapText3);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB16, properties->lmapSz,properties->lmapSz,properties->tex3DSzLimit, 0, GL_RGB, GL_UNSIGNED_SHORT, lmap + properties->tex3DSzLimit * 2 * properties->lmapSz * properties->lmapSz);
		glActiveTexture(GL_TEXTURE19);
		glBindTexture(GL_TEXTURE_3D, lmapText4);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB16, properties->lmapSz,properties->lmapSz,lmapC - properties->tex3DSzLimit * 3, 0, GL_RGB, GL_UNSIGNED_SHORT, lmap + properties->tex3DSzLimit*3*properties->lmapSz * properties->lmapSz);
	}
	else if(lmapC > properties->tex3DSzLimit * 2){
		glActiveTexture(GL_TEXTURE9);
		glBindTexture(GL_TEXTURE_3D, lmapText);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB16, properties->lmapSz,properties->lmapSz,properties->tex3DSzLimit,0,GL_RGB, GL_UNSIGNED_SHORT,lmap);
		glActiveTexture(GL_TEXTURE17);
		glBindTexture(GL_TEXTURE_3D, lmapText2);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB16, properties->lmapSz,properties->lmapSz,properties->tex3DSzLimit,0,GL_RGB, GL_UNSIGNED_SHORT,lmap + properties->tex3DSzLimit * properties->lmapSz * properties->lmapSz);
		glActiveTexture(GL_TEXTURE18);
		glBindTexture(GL_TEXTURE_3D, lmapText3);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB16, properties->lmapSz,properties->lmapSz,lmapC - properties->tex3DSzLimit*2,0,GL_RGB, GL_UNSIGNED_SHORT,lmap+properties->tex3DSzLimit*2*properties->lmapSz*properties->lmapSz);
	}
	else if(lmapC > properties->tex3DSzLimit){
		glActiveTexture(GL_TEXTURE9);
		glBindTexture(GL_TEXTURE_3D, lmapText);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB16, properties->lmapSz,properties->lmapSz,properties->tex3DSzLimit,0,GL_RGB, GL_UNSIGNED_SHORT, lmap);
		glActiveTexture(GL_TEXTURE17);
		glBindTexture(GL_TEXTURE_3D, lmapText2);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB16, properties->lmapSz,properties->lmapSz,lmapC - properties->tex3DSzLimit,0,GL_RGB, GL_UNSIGNED_SHORT, lmap + properties->tex3DSzLimit * properties->lmapSz * properties->lmapSz);
	}
	else{
		glActiveTexture(GL_TEXTURE9);
		glBindTexture(GL_TEXTURE_3D, lmapText);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB16, properties->lmapSz,properties->lmapSz,lmapC,0,GL_RGB,GL_UNSIGNED_SHORT,lmap);
	}

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_3D,lpmapText);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage3D(GL_TEXTURE_3D,0,GL_R32UI,properties->lvlSz*12,properties->lvlSz,properties->lvlSz,0,GL_RED_INTEGER,GL_UNSIGNED_INT,lpmap);

	glActiveTexture(GL_TEXTURE17);
	glBindTexture(GL_TEXTURE_3D,lmapText2);
	glTexImage3D(GL_TEXTURE_3D,0,GL_RGB16,lmapC,properties->lmapSz,properties->lmapSz,0,GL_RGB,GL_UNSIGNED_SHORT,lmap);
	glGenerateMipmap(GL_TEXTURE_3D);

	glActiveTexture(GL_TEXTURE18);
	glBindTexture(GL_TEXTURE_3D,lmapText3);
	glTexImage3D(GL_TEXTURE_3D,0,GL_RGB16,lmapC,properties->lmapSz,properties->lmapSz,0,GL_RGB,GL_UNSIGNED_SHORT,lmap);
	glGenerateMipmap(GL_TEXTURE_3D);

	glActiveTexture(GL_TEXTURE19);
	glBindTexture(GL_TEXTURE_3D,lmapText4);
	glTexImage3D(GL_TEXTURE_3D,0,GL_RGB16,lmapC,properties->lmapSz,properties->lmapSz,0,GL_RGB,GL_UNSIGNED_SHORT,lmap);
	glGenerateMipmap(GL_TEXTURE_3D);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D,mapText);
	glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,properties->lvlSz,properties->lvlSz,properties->lvlSz,0,GL_RGBA,GL_UNSIGNED_BYTE,map);
	glGenerateMipmap(GL_TEXTURE_3D);

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_3D,metadtText);
	glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,properties->lvlSz,properties->lvlSz,properties->lvlSz,0,GL_RGBA,GL_UNSIGNED_BYTE,metadt);
	glGenerateMipmap(GL_TEXTURE_3D);

	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_3D,metadt2Text);
	glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,properties->lvlSz,properties->lvlSz,properties->lvlSz,0,GL_RGBA,GL_UNSIGNED_BYTE,metadt2);
	glGenerateMipmap(GL_TEXTURE_3D);

	glActiveTexture(GL_TEXTURE12);
	glBindTexture(GL_TEXTURE_3D,metadt3Text);
	glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,properties->lvlSz,properties->lvlSz,properties->lvlSz,0,GL_RGBA,GL_UNSIGNED_BYTE,metadt3);
	glGenerateMipmap(GL_TEXTURE_3D);

	glActiveTexture(GL_TEXTURE13);
	glBindTexture(GL_TEXTURE_3D,metadt4Text);
	glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,properties->lvlSz,properties->lvlSz,properties->lvlSz,0,GL_RGBA,GL_UNSIGNED_BYTE,metadt4);
	glGenerateMipmap(GL_TEXTURE_3D);

	glActiveTexture(GL_TEXTURE14);
	glBindTexture(GL_TEXTURE_3D,metadt5Text);
	glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,properties->lvlSz,properties->lvlSz,properties->lvlSz,0,GL_RGBA,GL_UNSIGNED_BYTE,metadt5);
	glGenerateMipmap(GL_TEXTURE_3D);

	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_3D,metadt6Text);
	glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,properties->lvlSz,properties->lvlSz,properties->lvlSz,0,GL_RGBA,GL_UNSIGNED_BYTE,metadt6);
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

}

void updateShaderVariables(u32 program){
	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program,"wounded"),player->wounded);
	glUniform2f(glGetUniformLocation(program,"angle"),player->xangle,player->yangle);
	glUniform4f(glGetUniformLocation(program,"dir"),player->xdir,player->ydir, player->zdir,player->xydir);
	glUniform1i(glGetUniformLocation(program,"tick"),tick);
	glUniform1i(glGetUniformLocation(program,"state"),settings);
	glUniform1i(glGetUniformLocation(program,"entityC"),entityC);
	glUniform1f(glGetUniformLocation(program,"brightness"),brightness);
	glUniform3f(glGetUniformLocation(program,"Pos"),player->pos.x,player->pos.y,player->pos.z);
}

void openGL(){
	glMes = HeapAlloc(GetProcessHeap(),8,sizeof(OPENGLMESSAGE) * 1024);

	VERTsource       = loadFile("shaders/vertex.vert");
	FRAGsource       = loadFile("shaders/fragment.frag");
	VERTsourceFont   = loadFile("shaders/fontvert.vert");
	FRAGsourceFont   = loadFile("shaders/fontfrag.frag");
	FRAGsourceEditor = loadFile("shaders/editor.frag");
	VERTsourcePost   = loadFile("shaders/post.vert");
	FRAGsourcePost   = loadFile("shaders/post.frag");
	FRAGsourceSmooth = loadFile("shaders/smooth.frag");

	fontImage    = loadTexture("textures/font.bmp");

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
	glUniform3i               = wglGetProcAddress("glUniform3i");
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
	glGenFramebuffers         = wglGetProcAddress("glGenFramebuffers");
	glBindFramebuffer         = wglGetProcAddress("glBindFramebuffer");
	glFramebufferTexture      = wglGetProcAddress("glFramebufferTexture2D");
	glGenRenderbuffers        = wglGetProcAddress("glGenRenderbuffers");
	glBindRenderbuffer        = wglGetProcAddress("glBindRenderbuffer");
	glRenderbufferStorage     = wglGetProcAddress("glRenderbufferStorage");
	glFramebufferRenderbuffer = wglGetProcAddress("glFramebufferRenderbuffer");
	glCheckFramebufferStatus  = wglGetProcAddress("glCheckFramebufferStatus");

	wglSwapIntervalEXT        = wglGetProcAddress("wglSwapIntervalEXT");

	wglSwapIntervalEXT(settings&0x200);

	glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE,&properties->tex3DSzLimit);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shaderProgram       = glCreateProgram();
	shaderProgramFont   = glCreateProgram();
	shaderProgramEditor = glCreateProgram();
	shaderProgramPost   = glCreateProgram();
	shaderProgramSmooth = glCreateProgram();

	vertexShader         = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader       = glCreateShader(GL_FRAGMENT_SHADER);
	vertexShaderFont     = glCreateShader(GL_VERTEX_SHADER);
	fragmentShaderFont   = glCreateShader(GL_FRAGMENT_SHADER);
	fragmentShaderEditor = glCreateShader(GL_FRAGMENT_SHADER);
	fragmentShaderPost   = glCreateShader(GL_FRAGMENT_SHADER);
	vertexShaderPost     = glCreateShader(GL_VERTEX_SHADER);
	fragmentShaderSmooth = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader,1,(const char**)&VERTsource,0);
	glShaderSource(fragmentShader,1,(const char**)&FRAGsource,0);
	glShaderSource(vertexShaderFont,1,(const char**)&VERTsourceFont,0);
	glShaderSource(fragmentShaderFont,1,(const char**)&FRAGsourceFont,0);
	glShaderSource(fragmentShaderEditor,1,(const char**)&FRAGsourceEditor,0);
	glShaderSource(vertexShaderPost,1,(const char**)&VERTsourcePost,0);
	glShaderSource(fragmentShaderPost,1,(const char**)&FRAGsourcePost,0);
	glShaderSource(fragmentShaderSmooth,1,(const char**)&FRAGsourceSmooth,0);

	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);
	glCompileShader(vertexShaderFont);
	glCompileShader(fragmentShaderFont);
	glCompileShader(fragmentShaderEditor);
	glCompileShader(fragmentShaderPost);
	glCompileShader(vertexShaderPost);
	glCompileShader(fragmentShaderSmooth);

	char *bericht = HeapAlloc(GetProcessHeap(),8,1000);
	glGetShaderInfoLog(fragmentShader,1000,0,bericht);
	printf("%s\n",bericht);
	glGetShaderInfoLog(vertexShader,1000,0,bericht);
	printf("%s\n",bericht);
	HeapFree(GetProcessHeap(),0,bericht);

	glAttachShader(shaderProgramEditor,vertexShader);
	glAttachShader(shaderProgramEditor,fragmentShaderEditor);
	glAttachShader(shaderProgram,vertexShader);
	glAttachShader(shaderProgram,fragmentShader);
	glAttachShader(shaderProgramFont,vertexShaderFont);
	glAttachShader(shaderProgramFont,fragmentShaderFont);
	glAttachShader(shaderProgramPost,fragmentShaderPost);
	glAttachShader(shaderProgramPost,vertexShaderPost);
	glAttachShader(shaderProgramSmooth,vertexShader);
	glAttachShader(shaderProgramSmooth,fragmentShaderSmooth);

	glLinkProgram(shaderProgramEditor);
	glLinkProgram(shaderProgram);
	glLinkProgram(shaderProgramFont);
	glLinkProgram(shaderProgramPost);
	glLinkProgram(shaderProgramSmooth);

	glCreateBuffers(1,&quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER,quadVBO);
	glBufferData(GL_ARRAY_BUFFER,36 * sizeof(float),quad,GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, 0, 4 * sizeof(float), (void*)0);

	glUseProgram(shaderProgramPost);
	glUniform1i(glGetUniformLocation(shaderProgramPost,"screenTexture"),18);

	glGenTextures(1,&fboText);
	glGenTextures(1,&mapText);
	glGenTextures(1,&mapTextFont);
	glGenTextures(1,&lpmapText);
	glGenTextures(1,&lmapText);
	glGenTextures(1,&lmapText2);
	glGenTextures(1,&lmapText3);
	glGenTextures(1,&lmapText4);
	glGenTextures(30,(void*)blockTextures);
	glGenTextures(1,&chessText);
	glGenTextures(1,&models8Text);
	glGenTextures(1,&metadtText);
	glGenTextures(1,&metadt2Text);
	glGenTextures(1,&metadt3Text);
	glGenTextures(1,&metadt4Text);
	glGenTextures(1,&metadt5Text);
	glGenTextures(1,&metadt6Text);
	glGenTextures(1,&entityText);
	glGenTextures(1,&godraysText);
	glGenTextures(1,&entityTextText);
	glGenTextures(1,&skyboxText);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D,godraysText);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,godraySz,godraySz,0,GL_RGBA,GL_UNSIGNED_BYTE,godraymap);
	glGenerateMipmap(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_3D,entityTextText);
	glTexImage3D(GL_TEXTURE_3D,0,GL_RGB,ENTITYTEXTSZ,ENTITYTEXTSZ,entityC,0,GL_RGB,GL_UNSIGNED_BYTE,entityTexture);
	glGenerateMipmap(GL_TEXTURE_3D);

	glBindTexture(GL_TEXTURE_1D,entityText);
	GPUtextureUpload();

	glUseProgram(shaderProgramEditor);

	GPUtextureUpload();

	glUseProgram(shaderProgramSmooth);

	GPUtextureUpload();

	glUseProgram(shaderProgram);

	glUniform1i(glGetUniformLocation(shaderProgram,"map"),1);
	glUniform1i(glGetUniformLocation(shaderProgram,"godraymap"),2);
	glUniform1i(glGetUniformLocation(shaderProgram,"entityTextures"),3);
	glUniform1i(glGetUniformLocation(shaderProgram,"skybox"),4);
	glUniform1i(glGetUniformLocation(shaderProgram,"lpmap"),6);
	glUniform1i(glGetUniformLocation(shaderProgram,"lmap"),9);
	glUniform1i(glGetUniformLocation(shaderProgram,"metadt"),10);
	glUniform1i(glGetUniformLocation(shaderProgram,"metadt2"),11);
	glUniform1i(glGetUniformLocation(shaderProgram,"metadt3"),12);
	glUniform1i(glGetUniformLocation(shaderProgram,"metadt4"),13);
	glUniform1i(glGetUniformLocation(shaderProgram,"metadt5"),14);
	glUniform1i(glGetUniformLocation(shaderProgram,"metadt6"),15);
	glUniform1i(glGetUniformLocation(shaderProgram,"entity"),16);
	glUniform1i(glGetUniformLocation(shaderProgram,"lmap2"),17);
	glUniform1i(glGetUniformLocation(shaderProgram,"lmap3"),18);
	glUniform1i(glGetUniformLocation(shaderProgram,"lmap4"),19);

	glUseProgram(shaderProgramSmooth);

	glUniform1i(glGetUniformLocation(shaderProgram,"map"),1);
	glUniform1i(glGetUniformLocation(shaderProgram,"godraymap"),2);
	glUniform1i(glGetUniformLocation(shaderProgram,"entityTextures"),3);
	glUniform1i(glGetUniformLocation(shaderProgram,"skybox"),4);
	glUniform1i(glGetUniformLocation(shaderProgram,"lpmap"),6);
	glUniform1i(glGetUniformLocation(shaderProgram,"lmap"),9);
	glUniform1i(glGetUniformLocation(shaderProgram,"metadt"),10);
	glUniform1i(glGetUniformLocation(shaderProgram,"metadt2"),11);
	glUniform1i(glGetUniformLocation(shaderProgram,"metadt3"),12);
	glUniform1i(glGetUniformLocation(shaderProgram,"metadt4"),13);
	glUniform1i(glGetUniformLocation(shaderProgram,"metadt5"),14);
	glUniform1i(glGetUniformLocation(shaderProgram,"metadt6"),15);
	glUniform1i(glGetUniformLocation(shaderProgram,"entity"),16);
	glUniform1i(glGetUniformLocation(shaderProgram,"lmap2"),17);
	glUniform1i(glGetUniformLocation(shaderProgram,"lmap3"),18);
	glUniform1i(glGetUniformLocation(shaderProgram,"lmap4"),19);

	glUseProgram(shaderProgramEditor);

	glUniform1i(glGetUniformLocation(shaderProgramEditor,"map"),1);
	glUniform1i(glGetUniformLocation(shaderProgramEditor,"skybox"),4);
	glUniform1i(glGetUniformLocation(shaderProgramEditor,"entities"),5);
	glUniform1i(glGetUniformLocation(shaderProgramEditor,"metadt"),10);
	glUniform1i(glGetUniformLocation(shaderProgramEditor,"metadt2"),11);
	glUniform1i(glGetUniformLocation(shaderProgramEditor,"metadt3"),12);
	glUniform1i(glGetUniformLocation(shaderProgramEditor,"metadt4"),13);
	glUniform1i(glGetUniformLocation(shaderProgramEditor,"metadt5"),14);
	glUniform1i(glGetUniformLocation(shaderProgramEditor,"metadt6"),15);

	glCreateBuffers(1,&VBO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER,36 * sizeof(float),quad,GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,0,6 * sizeof(float),(void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,2,GL_FLOAT,0,6 * sizeof(float),(void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,1,GL_FLOAT,0,6 * sizeof(float),(void*)(5 * sizeof(float)));

	glUseProgram(shaderProgram);

	glUniform2i(glGetUniformLocation(shaderProgram,"reso"),properties->xres,properties->yres);
	glUniform2f(glGetUniformLocation(shaderProgram,"fov"),player->fov.x,player->fov.y);
	glUniform1i(glGetUniformLocation(shaderProgram,"mapSz"),properties->lvlSz);
	glUniform1f(glGetUniformLocation(shaderProgram,"lmapsz"),properties->lmapSz);
	glUniform1i(glGetUniformLocation(shaderProgram,"tex3DszLimit"),properties->tex3DSzLimit);

	glUseProgram(shaderProgramSmooth);

	glUniform2i(glGetUniformLocation(shaderProgramSmooth,"reso"),properties->xres,properties->yres);
	glUniform2f(glGetUniformLocation(shaderProgramSmooth,"fov"),player->fov.x,player->fov.y);
	glUniform1i(glGetUniformLocation(shaderProgramSmooth,"mapSz"),properties->lvlSz);
	glUniform1f(glGetUniformLocation(shaderProgramSmooth,"lmapsz"),properties->lmapSz);
	glUniform1i(glGetUniformLocation(shaderProgramSmooth,"tex3DszLimit"),properties->tex3DSzLimit);

	glUseProgram(shaderProgramEditor);

	glUniform2i(glGetUniformLocation(shaderProgramEditor,"reso"),properties->xres,properties->yres);
	glUniform2f(glGetUniformLocation(shaderProgramEditor,"fov"),player->fov.x,player->fov.y);
	glUniform1i(glGetUniformLocation(shaderProgramEditor,"mapSz"),properties->lvlSz);
	glUniform1f(glGetUniformLocation(shaderProgramEditor,"lmapsz"),properties->lmapSz);

	glUseProgram(shaderProgramFont);
	glUniform1i(glGetUniformLocation(shaderProgramFont,"font"),0);
	glUniform2i(glGetUniformLocation(shaderProgramFont,"reso"),properties->xres,properties->yres);
	glUniform2f(glGetUniformLocation(shaderProgramFont,"fov"),player->fov.x*0.5f,player->fov.y*0.5f);
	glUniform1f(glGetUniformLocation(shaderProgramFont,"sensitivity"),properties->sensitivity);
	glUseProgram(shaderProgram);

	openglINIT = 1;

	for(;;){
		u64 timeB;
		QueryPerformanceCounter(&timeB);
		if(settings & 0x80){
			SwapBuffers(dc);
			Sleep(100);
		}
		else{
			if(settings & 0x20){
				drawUI();
			}
			glUseProgram(shaderProgramFont);
			glUniform3i(glGetUniformLocation(shaderProgramFont,"colorSel"),colorSel.r,colorSel.g,colorSel.b);
			glUniform3i(glGetUniformLocation(shaderProgramFont,"metadt1"),metadtSel.r,metadtSel.g,metadtSel.a);
			glUniform3i(glGetUniformLocation(shaderProgramFont,"metadt2"),metadt2Sel.r,metadt2Sel.g,metadt2Sel.a);
			glUniform3i(glGetUniformLocation(shaderProgramFont,"metadt3"),metadt3Sel.r,metadt3Sel.g,metadt3Sel.a);
			glUniform3i(glGetUniformLocation(shaderProgramFont,"metadt4"),metadt4Sel.r,metadt4Sel.g,metadt4Sel.a);
			glUniform3i(glGetUniformLocation(shaderProgramFont,"metadt5"),metadt5Sel.r,metadt5Sel.g,metadt5Sel.a);
			glUniform3i(glGetUniformLocation(shaderProgramFont,"metadt6"),metadt6Sel.r,metadt6Sel.g,metadt6Sel.a);
			glUniform1i(glGetUniformLocation(shaderProgramFont,"setting"),settings);
			glUniform1f(glGetUniformLocation(shaderProgramFont,"playerFlightSpeed"),player->flightSpeed);
			glUseProgram(shaderProgram);
			glUniform1i(glGetUniformLocation(shaderProgram,"entityC"),entityC);
			while(glMesC > 0){	
				glMesC--;
				switch(glMes[glMesC].id){
				case 0:
					glViewport(0,0,properties->xres,properties->yres);
					glUseProgram(shaderProgram);
					glUniform2i(glGetUniformLocation(shaderProgram,"reso"),properties->xres,properties->yres);
					glUniform2f(glGetUniformLocation(shaderProgram,"fov"),player->fov.x,player->fov.y);
					glUseProgram(shaderProgramFont);
					glUniform2i(glGetUniformLocation(shaderProgramFont,"reso"),properties->xres,properties->yres);
					glUseProgram(shaderProgramEditor);
					glUniform2i(glGetUniformLocation(shaderProgramEditor,"reso"),properties->xres,properties->yres);
					glUniform2f(glGetUniformLocation(shaderProgramEditor,"fov"),player->fov.x,player->fov.y);
					glUseProgram(shaderProgram);
					break;
				case 1:
					glActiveTexture(GL_TEXTURE1);
					glTexSubImage3D(GL_TEXTURE_3D,0,glMes[glMesC].data1,glMes[glMesC].data2,glMes[glMesC].data3,1,1,1,GL_RGBA,GL_UNSIGNED_BYTE,map+(glMes[glMesC].data1 + glMes[glMesC].data2 * properties->lvlSz + glMes[glMesC].data3 * properties->lvlSz * properties->lvlSz));
					break;
				case 2:
					glTexSubImage3D(GL_TEXTURE_3D,0,glMes[glMesC].data1,glMes[glMesC].data2,glMes[glMesC].data3,glMes[glMesC].data4,glMes[glMesC].data5,glMes[glMesC].data6,GL_RED,GL_UNSIGNED_BYTE,map+glMes[glMesC].data1 + glMes[glMesC].data2 * properties->lvlSz + glMes[glMesC].data3 * properties->lvlSz * properties->lvlSz);
					break;
				case 3:
					glActiveTexture(GL_TEXTURE1);
					glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,properties->lvlSz,properties->lvlSz,properties->lvlSz,0,GL_RGBA,GL_UNSIGNED_BYTE,map);
					glActiveTexture(GL_TEXTURE10);
					glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,properties->lvlSz,properties->lvlSz,properties->lvlSz,0,GL_RGBA,GL_UNSIGNED_BYTE,metadt);
					glActiveTexture(GL_TEXTURE11);
					glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,properties->lvlSz,properties->lvlSz,properties->lvlSz,0,GL_RGBA,GL_UNSIGNED_BYTE,metadt2);
					glActiveTexture(GL_TEXTURE12);
					glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,properties->lvlSz,properties->lvlSz,properties->lvlSz,0,GL_RGBA,GL_UNSIGNED_BYTE,metadt3);
					glActiveTexture(GL_TEXTURE13);
					glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,properties->lvlSz,properties->lvlSz,properties->lvlSz,0,GL_RGBA,GL_UNSIGNED_BYTE,metadt4);
					glActiveTexture(GL_TEXTURE14);
					glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,properties->lvlSz,properties->lvlSz,properties->lvlSz,0,GL_RGBA,GL_UNSIGNED_BYTE,metadt5);
					glActiveTexture(GL_TEXTURE15);
					glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,properties->lvlSz,properties->lvlSz,properties->lvlSz,0,GL_RGBA,GL_UNSIGNED_BYTE,metadt6);
					break;
				case 4:
					for(u32 i = 0;i < 20;i++){
						drawSprite((VEC3){glMes[glMesC].fdata1,glMes[glMesC].fdata2,glMes[glMesC].fdata5},(VEC2){glMes[glMesC].fdata3,glMes[glMesC].fdata4},glMes[glMesC].fdata6);
					}
					break;
				case 5:
					glActiveTexture(GL_TEXTURE6);
					break;
				case 6:
					glActiveTexture(GL_TEXTURE6);
					glTexImage3D(GL_TEXTURE_3D,0,GL_R32UI,properties->lvlSz*12,properties->lvlSz,properties->lvlSz,0,GL_RED_INTEGER,GL_UNSIGNED_INT,lpmap);
				    if(lmapC>properties->tex3DSzLimit*3){
						glActiveTexture(GL_TEXTURE9);
						glTexImage3D(GL_TEXTURE_3D,0,GL_RGB16,properties->lmapSz,properties->lmapSz,properties->tex3DSzLimit,0,GL_RGB,GL_UNSIGNED_SHORT,lmap);
						glActiveTexture(GL_TEXTURE17);
						glTexImage3D(GL_TEXTURE_3D,0,GL_RGB16,properties->lmapSz,properties->lmapSz,properties->tex3DSzLimit,0,GL_RGB,GL_UNSIGNED_SHORT,lmap+properties->tex3DSzLimit*properties->lmapSz*properties->lmapSz);
						glActiveTexture(GL_TEXTURE18);
						glTexImage3D(GL_TEXTURE_3D,0,GL_RGB16,properties->lmapSz,properties->lmapSz,properties->tex3DSzLimit,0,GL_RGB,GL_UNSIGNED_SHORT,lmap+properties->tex3DSzLimit*2*properties->lmapSz*properties->lmapSz);
						glActiveTexture(GL_TEXTURE19);
						glTexImage3D(GL_TEXTURE_3D,0,GL_RGB16,properties->lmapSz,properties->lmapSz,lmapC-properties->tex3DSzLimit*3,0,GL_RGB,GL_UNSIGNED_SHORT,lmap+properties->tex3DSzLimit*3*properties->lmapSz*properties->lmapSz);
					}
					else if(lmapC>properties->tex3DSzLimit*2){
						glActiveTexture(GL_TEXTURE9);
						glTexImage3D(GL_TEXTURE_3D,0,GL_RGB16,properties->lmapSz,properties->lmapSz,properties->tex3DSzLimit,0,GL_RGB,GL_UNSIGNED_SHORT,lmap);
						glActiveTexture(GL_TEXTURE17);
						glTexImage3D(GL_TEXTURE_3D,0,GL_RGB16,properties->lmapSz,properties->lmapSz,properties->tex3DSzLimit,0,GL_RGB,GL_UNSIGNED_SHORT,lmap+properties->tex3DSzLimit*properties->lmapSz*properties->lmapSz);
						glActiveTexture(GL_TEXTURE18);
						glTexImage3D(GL_TEXTURE_3D,0,GL_RGB16,properties->lmapSz,properties->lmapSz,lmapC-properties->tex3DSzLimit*2,0,GL_RGB,GL_UNSIGNED_SHORT,lmap+properties->tex3DSzLimit*2*properties->lmapSz*properties->lmapSz);
					}
					else if(lmapC>properties->tex3DSzLimit){
						glActiveTexture(GL_TEXTURE9);
						glTexImage3D(GL_TEXTURE_3D,0,GL_RGB16,properties->lmapSz,properties->lmapSz,properties->tex3DSzLimit,0,GL_RGB,GL_UNSIGNED_SHORT,lmap);
						glActiveTexture(GL_TEXTURE17);
						glTexImage3D(GL_TEXTURE_3D,0,GL_RGB16,properties->lmapSz,properties->lmapSz,lmapC-properties->tex3DSzLimit,0,GL_RGB,GL_UNSIGNED_SHORT,lmap+properties->tex3DSzLimit*properties->lmapSz*properties->lmapSz);
					}
					else{
						glActiveTexture(GL_TEXTURE9);
						glTexImage3D(GL_TEXTURE_3D,0,GL_RGB16,properties->lmapSz,properties->lmapSz,lmapC,0,GL_RGB,GL_UNSIGNED_SHORT,lmap);
					}
					glGenerateMipmap(GL_TEXTURE_3D);
					glUniform1f(glGetUniformLocation(shaderProgram,"lmapsz"),properties->lmapSz);
					generateSkyBox();
					break;
				case 7:
					glActiveTexture(GL_TEXTURE9);
					glTexSubImage3D(GL_TEXTURE_3D,0,0,0,glMes[glMesC].data1/properties->lmapSz/properties->lmapSz,properties->lmapSz,properties->lmapSz,1,GL_RGB,GL_UNSIGNED_SHORT,lmap+glMes[glMesC].data1/properties->lmapSz/properties->lmapSz*properties->lmapSz*properties->lmapSz);
					break;
				case 8:
					generateSkyBox();
					break;
				case 9:
					wglSwapIntervalEXT(settings&0x200);
					break;
				case 10:
					glUseProgram(shaderProgram);
					glUniform2f(glGetUniformLocation(shaderProgram,"fov"),player->fov.x,player->fov.y);
					glUseProgram(shaderProgramEditor);
					glUniform2f(glGetUniformLocation(shaderProgramEditor,"fov"),player->fov.x,player->fov.y);
					glUseProgram(shaderProgramFont);
					glUniform2f(glGetUniformLocation(shaderProgramFont,"fov"),player->fov.x*0.5f,player->fov.y*0.5f);
					break;
				case 11:
					glUseProgram(shaderProgramFont);
					glUniform1f(glGetUniformLocation(shaderProgramFont,"sensitivity"),properties->sensitivity);
					break;
				case 12:
					glUseProgram(shaderProgram);
					glActiveTexture(GL_TEXTURE3);
					glBindTexture(GL_TEXTURE_3D,entityTextText);
					glTexImage3D(GL_TEXTURE_3D,0,GL_RGB,ENTITYTEXTSZ,ENTITYTEXTSZ,entityC,0,GL_RGB,GL_UNSIGNED_BYTE,entityTexture);
					glGenerateMipmap(GL_TEXTURE_3D);
					break;
				}
			}
			sprite = 0;
			glUseProgram(shaderProgramFont);
			glUniform3f(glGetUniformLocation(shaderProgramFont,"color"),(float)colorSel.r/255.0f,(float)colorSel.g/255.0f,(float)colorSel.b/255.0f);
			if(settings & 0x04){
				glUseProgram(shaderProgramEditor);
				glUniform2f(glGetUniformLocation(shaderProgramEditor,"angle"),player->xangle,player->yangle);
				glUniform4f(glGetUniformLocation(shaderProgramEditor,"dir"),player->xdir,player->ydir, player->zdir,player->xydir);
				glUniform3f(glGetUniformLocation(shaderProgramEditor,"Pos"),player->pos.x,player->pos.y,player->pos.z);
				glUniform1i(glGetUniformLocation(shaderProgramEditor,"tick"),tick);
				glUniform1i(glGetUniformLocation(shaderProgramEditor,"state"),settings);
				glUniform1i(glGetUniformLocation(shaderProgramEditor,"entityC"),entityC);
				glUniform1f(glGetUniformLocation(shaderProgramEditor,"brightness"),brightness);
				glDrawArrays(GL_TRIANGLES,0,6);
			}
			else{
				if(settings & 0x400){
					updateShaderVariables(shaderProgramSmooth);
				}
				else{
					updateShaderVariables(shaderProgram);
				}
				for(u32 i = 0;i < entityC;i++){
					switch(entity.cpu[i].id){
					case 4:
						entity.gpu[i].pos.x = player->pos.x-player->xdir*player->xydir/16.0f;
						entity.gpu[i].pos.y = player->pos.y-player->ydir*player->xydir/16.0f;
						entity.gpu[i].pos.z = player->pos.z-player->zdir/16.0f-0.1f;
						entity.gpu[i].pos2.x = cosf(player->xangle)*cosf(player->yangle)/4.0f;
						entity.gpu[i].pos2.y = sinf(player->xangle)*cosf(player->yangle)/4.0f;
						entity.gpu[i].pos2.z = sinf(player->yangle)/4.0f;
						if(entity.cpu[i].aniTime){
							entity.cpu[i].aniTime--;
							switch(entity.cpu[i].aniType){
							case 1:
								entity.gpu[i].pos2.z -= (f32)entity.cpu[i].aniTime/60.0f;
								break;
							case 2:
								VEC3div(&entity.gpu[i].color,1.2f);
								if(entity.cpu[i].aniTime==1){
									entity.gpu[i].color = entity.cpu[i].baseColor;
								}
								break;
							}
						}
						break;
					}
				}
				glActiveTexture(GL_TEXTURE16);
				glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA32F,entityC*20,0,GL_RGBA,GL_FLOAT,entity.gpu);
				glGenerateMipmap(GL_TEXTURE_1D);
				glDrawArrays(GL_TRIANGLES,0,6);
				glUseProgram(shaderProgram);
			}
			if(settings & 0x20){
				glUseProgram(shaderProgramFont);
				drawSprite((VEC3){0.0,0.0,0.0},(VEC2){0.0,0.0},0.0);
				glDrawArrays(GL_TRIANGLES,6,totalCar*6+6);
			}
			SwapBuffers(dc);
			totalCar = 0;
		}
		QueryPerformanceCounter(&fps);
		fps -= timeB;
	}
}
