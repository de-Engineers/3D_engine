#include <windows.h>
#include <glew.h>
#include <main.h>
#include <stdio.h>
#include <math.h>
#include <intrin.h>

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

char *words[]  = {"air","rgb","water","mirror","sphere","tower","tegeltjes","white",
	"light","mist","customslope","glass","bounce","mirror","spikes","normal",
	"donut","shape1","greekpillar",
	
	};
char *words2[] = {"normal","cube","lighting","entities","colorselect"};

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
unsigned int mapdataText;
unsigned int chessText;
unsigned int models8Text;
unsigned int VAO;

unsigned int totalCar;

long long fps = 1;

VEC2 mousePos;

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
	int size;
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
		drawChar(str[i] - 87,x+i*0.0235,y,-0.1,id,0.04,0.04);
	}
}

char *loadTexture(char *name){
	HANDLE h = CreateFile(name,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	int fsize = GetFileSize(h,0);
	char *text = HeapAlloc(GetProcessHeap(),8,fsize+1);
	SetFilePointer(h,138,0,0);
	ReadFile(h,text,fsize - 138,0,0);
	CloseHandle(h);
	return text;
}

char *loadFile(char *name){
	HANDLE h = CreateFile(name,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
	int fsize = GetFileSize(h,0);
	char *file = HeapAlloc(GetProcessHeap(),8,fsize+1);
	ReadFile(h,file,fsize+1,0,0);
	CloseHandle(h);
	return file;
}

float distance(VEC2 p1,VEC2 p2){
	float r1 = p1.x-p2.x;
	float r2 = p1.y-p2.y;
	return sqrtf(r1*r1+r2*r2);
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

	glewInit();
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
	glGenTextures(30,(void*)blockTextures);
	glGenTextures(1,&mapdataText);
	glGenTextures(1,&chessText);
	glGenTextures(1,&models8Text);

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

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_3D,mapdataText);
	glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,properties->lvlSz*properties->lmapSz2,properties->lvlSz*properties->lmapSz2,properties->lvlSz*properties->lmapSz2,0,GL_RGBA,GL_UNSIGNED_BYTE,mapdata);
	glGenerateMipmap(GL_TEXTURE_3D);

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
	glUniform1i(glGetUniformLocation(shaderProgram,"mapdata"),6);
	glUniform1i(glGetUniformLocation(shaderProgram,"chessModels"),7);
	glUniform1i(glGetUniformLocation(shaderProgram,"models8"),8);

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
	glUniform1f(glGetUniformLocation(shaderProgram,"lmapsz"),properties->lmapSz2);

	glUseProgram(shaderProgramFont);
	glUniform2i(glGetUniformLocation(shaderProgramFont,"reso"),properties->xres,properties->yres);
	glUseProgram(shaderProgram);

	for(;;){
		if(settings & 0x80){
			SwapBuffers(dc);
			Sleep(100);
		}
		else{
			glUseProgram(shaderProgram);
			long long fpstime = _rdtsc();
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

			drawVar(-0.9,0.7,3400000000 / fps);

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
				}
				else{
					drawSprite(button[i].pos.x,button[i].pos.y,-0.05f,3,0.03f,0.05f);
				}
			}
			drawSprite(0.8,-0.65,-0.2,7,0.1,0.1);
			if(settings & 0x10){
				POINT p;
				GetCursorPos(&p);
				ScreenToClient(window,&p);
				mousePos.x = (float)p.x/properties->xres*2.0-1.0;
				mousePos.y = -((float)p.y/properties->yres*2.0-1.0);
				drawSprite(mousePos.x-0.0075f,mousePos.y-0.0125f,-0.2,6,0.015,0.025);
				drawWord("settings",-0.11,0.42,0.0);
				drawWord("create new world",-0.45,-0.35,0.0);
				drawWord("quit",-0.45,-0.42,0.0);
				drawSprite(-0.5,-0.5,0.0,2,1.0,1.0);
			}
			else{
				drawChar(36,-0.01,-0.02,-0.99,0,0.04,0.04);
			}
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
					glTexSubImage3D(GL_TEXTURE_3D,0,glMes[glMesC].data1,glMes[glMesC].data2,glMes[glMesC].data3,1,1,1,GL_RGBA,GL_UNSIGNED_BYTE,map+(glMes[glMesC].data1 + glMes[glMesC].data2 * properties->lvlSz + glMes[glMesC].data3 * properties->lvlSz * properties->lvlSz) * 4);
					break;
				case 2:
					glTexSubImage3D(GL_TEXTURE_3D,0,glMes[glMesC].data1,glMes[glMesC].data2,glMes[glMesC].data3,glMes[glMesC].data4,glMes[glMesC].data5,glMes[glMesC].data6,GL_RED,GL_UNSIGNED_BYTE,map+glMes[glMesC].data1 + glMes[glMesC].data2 * properties->lvlSz + glMes[glMesC].data3 * properties->lvlSz * properties->lvlSz);
					break;
				case 3:
					glActiveTexture(GL_TEXTURE1);
					glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,properties->lvlSz,properties->lvlSz,properties->lvlSz,0,GL_RGBA,GL_UNSIGNED_BYTE,map);
					break;
				case 4:
					drawSprite(glMes[glMesC].fdata1,glMes[glMesC].fdata2,glMes[glMesC].fdata5,glMes[glMesC].fdata6,glMes[glMesC].fdata3,glMes[glMesC].fdata4);
					break;
				case 5:
					glActiveTexture(GL_TEXTURE6);
					glTexSubImage3D(GL_TEXTURE_3D,0,glMes[glMesC].data1,glMes[glMesC].data2,glMes[glMesC].data3,1,1,1,GL_RGBA,GL_UNSIGNED_BYTE,mapdata+(glMes[glMesC].data1 + glMes[glMesC].data2 * properties->lvlSz + glMes[glMesC].data3 * properties->lvlSz * properties->lvlSz) * 4);
					break;
				case 6:
					glActiveTexture(GL_TEXTURE6);
					glTexImage3D(GL_TEXTURE_3D,0,GL_RGBA,properties->lvlSz*properties->lmapSz2,properties->lvlSz*properties->lmapSz2,properties->lvlSz*properties->lmapSz2,0,GL_RGBA,GL_UNSIGNED_BYTE,mapdata);
					break;
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
			fps = _rdtsc() - fpstime;
			if(3400000000 / fps < 50){
				properties->renderDistance-=4;
			}
			else if(3400000000 / fps > 100 && properties->renderDistance < 255){
				properties->renderDistance++;
			}
		}
	}
}
