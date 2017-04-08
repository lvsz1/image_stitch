#ifndef V4L2_H
#define V4L2_H
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>


#include <sys/ioctl.h>
#include <sys/mman.h>

#include <asm/types.h>
#include <linux/videodev2.h>
#include"stdlib.h"
#include <unistd.h>
#include <stdio.h>
#include<string.h>
#include<QDebug>
#define CLEAR(x) memset(&x,0,sizeof(x))


int open_device(char *dev_name,int index);
int init_device(int index);
int init_mmap(int index);
int uninit_device(int index);
int close_device(int index);
int start_capturing(int index);
int stop_capturing(int index);
//int get_frame(void **frame_buf,size_t *len,int index,size_t *offset);
int get_frame(void **frame_buf,size_t *len,int index,size_t *offset);
int unget_frame(int index,int index_num);



struct buffer{
    void *start;
    size_t offset;
    size_t length;
};





#endif // V4L2_H
