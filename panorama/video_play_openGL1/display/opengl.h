#ifndef OPENGL_H
#define OPENGL_H

#define USE_GL20
#define LINUX
#define EGL_API_FB
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <math.h>
#include <signal.h>

#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"

#include "EGL/egl.h"
#include "FSL/fsl_egl.h"


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <math.h>
#include"mxcfb.h"

#include <fcntl.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include<sys/mman.h>
#include<string.h>

extern char *buf;
typedef unsigned char uchar;
int gl_Init(uchar *texture_image,uchar *sub_texture_image);
void gl_deinit(void);
void gl_render(uchar *video);
void gl_render_singl(uchar *video);
bool initfb(const char *dev, int width, int height, int bits);
#endif // OPENGL_H
