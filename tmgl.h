#pragma once

unsigned int (*glCreateProgram)();
unsigned int (*glCreateShader)(unsigned int shader);
unsigned int (*wglSwapIntervalEXT)(unsigned int satus);
unsigned int (*wglChoosePixelFormatARB)(HDC hdc,const int *piAttribIList,const float *pfAttribFList,unsigned int nMaxFormats,int *piFormats,unsigned int *nNumFormats);

int (*glGetUniformLocation)(unsigned int program,const char *name);
int (*glCheckFramebufferStatus)(unsigned int target);

void (*glShaderSource)(unsigned int shader,int count,const char **string,int *length);
void (*glCompileShader)(unsigned int shader);
void (*glAttachShader)(unsigned int program,unsigned int shader);
void (*glLinkProgram)(unsigned int program);
void (*glUseProgram)(unsigned int program);
void (*glEnableVertexAttribArray)(unsigned int index);
void (*glVertexAttribPointer)(unsigned int index,int size,unsigned int type,unsigned char normalized,unsigned int stride,const void *pointer);
void (*glBufferData)(unsigned int target,unsigned int size,const void *data,unsigned int usage);
void (*glCreateBuffers)(unsigned int n,unsigned int *buffers);
void (*glBindBuffer)(unsigned int target,unsigned int buffer);
void (*glGetShaderInfoLog)(unsigned int shader,unsigned int maxlength,unsigned int *length,unsigned char *infolog);
void (*glTexSubImage3D)(unsigned int target,int level,int xoff,int yoff,int zoff,int width,int height,int depth,unsigned int format,unsigned int type,const void *pixels);
void (*glActiveTexture)(unsigned int texture);
void (*glGenerateMipmap)(unsigned int target);
void (*glTexImage3D)(unsigned int target,int level,int intlevel,int width,int height,int depth,int border,unsigned int format,unsigned int type,const void *data);
void (*glUniform3i)(int loc,int v1,int v2,int v3);
void (*glUniform2i)(int loc,int v1,int v2);
void (*glUniform1i)(int loc,int v1);
void (*glUniform4f)(int loc,float v1,float v2,float v3,float v4);
void (*glUniform3f)(int loc,float v1,float v2,float v3);
void (*glUniform2f)(int loc,float v1,float v2);
void (*glUniform1f)(int loc,float v1);
void (*glUniformMatrix3fv)(int loc,int count,unsigned char transpose,const float *value);
void (*glGenFramebuffers)(int n,unsigned int *ids);
void (*glBindFramebuffer)(unsigned int target,unsigned int framebuffer);
void (*glFramebufferTexture)(unsigned int target,unsigned int attachment,unsigned int texture,int level);
void (*glGenRenderbuffers)(int n,unsigned int *ids);
void (*glBindRenderbuffer)(unsigned int target,unsigned int framebuffer);
void (*glRenderbufferStorage)(unsigned int target,unsigned int internalformat,unsigned int width,unsigned int height);
void (*glFramebufferRenderbuffer)(unsigned int target,unsigned int attachment,unsigned int renderbuffertarget,unsigned int renderbuffer);