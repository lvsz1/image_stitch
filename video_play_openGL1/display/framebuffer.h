#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/ioctl.h>


void framebuffer_init(const char * device  = "/dev/fb0");
void framebuffer_display(const unsigned char *data, int width, int height, int startx = 0, int starty = 0);
void framebuffer_close();

#endif // FRAMEBUFFER_H
