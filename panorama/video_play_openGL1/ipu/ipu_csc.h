#ifndef IPU_CSC_H
#define IPU_CSC_H
//#include"opengl.h"
#include"ipu.h"
#include"stdio.h"



void ipu_close();
int ipu_init();

unsigned int fmt_to_bpp(unsigned  int pixelformat);
int ipu_csc_single_toRGB(unsigned char *sour_dat, unsigned char *dest_dat);
int ipu_csc_muti(unsigned char *sour_dat, unsigned char *dest_dat);


#endif // IPU_CSC_H
