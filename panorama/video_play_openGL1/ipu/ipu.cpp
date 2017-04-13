#include"ipu_csc.h"
#include <fcntl.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include<sys/mman.h>
#include<string.h>
#include<QDebug>
#include"capture/parameters.h"

//全景图像的宽度、高度,在main.cpp中有定义
extern int panorama_width, panorama_height;

char *IMG;
char *destImage;

struct ipu_task ipu_task_single_toRGB;
struct ipu_task ipu_task_muti;

void *IPU_inbuf_m,*IPU_outbuf_m;
void *IPU_inbuf_S_RGB,*IPU_outbuf_S_RGB;

int IPU_inbuf_size_S,IPU_outbuf_size_S;
int IPU_inbuf_size_S_RGB,IPU_outbuf_size_S_RGB;
int IPU_inbuf_size_m,IPU_outbuf_size_m;


int fd_ipu;//ipu句柄

void ipu_close(){
    qDebug("ipu xi gou diaoyongle");
    free(IMG);
    free(destImage);
    ::close(fd_ipu);
}

int ipu_init()
{
    int ret=0;
    memset(&ipu_task_muti, 0, sizeof(ipu_task_muti));
    memset(&ipu_task_single_toRGB, 0, sizeof(ipu_task_single_toRGB));

    ipu_task_single_toRGB.input.width=V4L2_WIDTH;
    ipu_task_single_toRGB.input.height=V4L2_HEIGHT;
    ipu_task_single_toRGB.input.format=v4l2_fourcc('Y', 'U', 'Y', 'V');

    ipu_task_single_toRGB.output.width=320;
    ipu_task_single_toRGB.output.height=240;
    ipu_task_single_toRGB.output.format=v4l2_fourcc('R', 'G', 'B', '3');


    ipu_task_muti.input.width    = panorama_width;
    ipu_task_muti.input.height   = panorama_height;
    ipu_task_muti.input.format   = v4l2_fourcc('Y', 'U', 'Y', 'V');
    ipu_task_muti.output.width   = panorama_width;
    ipu_task_muti.output.height  = panorama_height;
//    ipu_task_muti.output.format  = v4l2_fourcc('R', 'G', 'B', '3');
    ipu_task_muti.output.format  = v4l2_fourcc('N', 'V', '1', '2');

    // 打开ipu设备
    fd_ipu = open("/dev/mxc_ipu", O_RDWR, 0);

    if (fd_ipu < 0) {
        printf("IPU init error.\n");
        //qDebug("open ipu dev fail\n");
        ret = -1;
    }

    IPU_inbuf_size_m =  ipu_task_muti.input.paddr   = ipu_task_muti.input.width   *  ipu_task_muti.input.height   * fmt_to_bpp(ipu_task_muti.input.format)/8;
    IPU_inbuf_size_S_RGB=ipu_task_single_toRGB.input.paddr=ipu_task_single_toRGB.input.width*ipu_task_single_toRGB.input.height*fmt_to_bpp(ipu_task_single_toRGB.input.format)/8;

    // Allocate contingous physical memory for input image

    ret=ioctl(fd_ipu,IPU_ALLOC,&ipu_task_single_toRGB.input.paddr);
    ret = ioctl(fd_ipu, IPU_ALLOC, &ipu_task_muti.input.paddr);


    // Create memory map and obtain the allocated memory virtual address

    IPU_inbuf_m = mmap(0, IPU_inbuf_size_m, PROT_READ | PROT_WRITE,MAP_SHARED, fd_ipu, ipu_task_muti.input.paddr);
    if (!IPU_inbuf_m) {
        qDebug("IPU inbuf mmap error.\n");
        ret = -1;
    }
    IPU_inbuf_S_RGB = mmap(0, IPU_inbuf_size_S_RGB, PROT_READ | PROT_WRITE,MAP_SHARED, fd_ipu, ipu_task_single_toRGB.input.paddr);
    if (!IPU_inbuf_S_RGB) {
        qDebug("IPU inbuf mmap error.\n");
        ret = -1;
    }

    IPU_outbuf_size_S_RGB=ipu_task_single_toRGB.output.paddr=ipu_task_single_toRGB.output.width*ipu_task_single_toRGB.output.height*fmt_to_bpp(ipu_task_single_toRGB.output.format)/8;
    IPU_outbuf_size_m = ipu_task_muti.output.paddr = ipu_task_muti.output.width*  ipu_task_muti.output.height *fmt_to_bpp(ipu_task_muti.output.format)/8;

    // Create memory map for output imagIPU_translatee
    ret = ioctl(fd_ipu, IPU_ALLOC, &ipu_task_single_toRGB.output.paddr);
    if (ret < 0){
        qDebug("IPU outbuf allocate error.\n");
    }
    ret = ioctl(fd_ipu, IPU_ALLOC, &ipu_task_muti.output.paddr);
    if (ret < 0){
        qDebug("IPU outbuf allocate error.\n");
    }

    IPU_outbuf_S_RGB=mmap(0,IPU_outbuf_size_S_RGB,PROT_READ | PROT_WRITE, MAP_SHARED, fd_ipu, ipu_task_single_toRGB.output.paddr);
    IPU_outbuf_m = mmap(0, IPU_outbuf_size_m, PROT_READ | PROT_WRITE, MAP_SHARED, fd_ipu, ipu_task_muti.output.paddr);

    return 0;
}


int ipu_csc_muti(uchar *sour_dat,uchar *dest_dat)
{
    int ret;

    memcpy(IPU_inbuf_m, sour_dat, panorama_width*panorama_height*2);

    ret = ioctl(fd_ipu, IPU_QUEUE_TASK, &ipu_task_muti);
    if (ret < 0) {
        qDebug("ioct IPU_QUEUE_TASK fail %x\n", ret);
    }

//    memcpy(dest_dat, IPU_outbuf_m, panorama_width*panorama_height*3);
    memcpy(dest_dat, IPU_outbuf_m, panorama_width*panorama_height*3/2);

    return 0;
}
int ipu_csc_single_toRGB(unsigned char *sour_dat, unsigned char *dest_dat)
{
    int ret;

    memcpy(IPU_inbuf_S_RGB, sour_dat, V4L2_WIDTH*V4L2_HEIGHT*2);

    ret = ioctl(fd_ipu, IPU_QUEUE_TASK, &ipu_task_single_toRGB);
    if (ret < 0) {
        qDebug("ioct IPU_QUEUE_TASK fail %x\n", ret);
    }
    memcpy(dest_dat, IPU_outbuf_S_RGB, 320*240*3);

    return 0;
}
unsigned int fmt_to_bpp(unsigned  int pixelformat){

    unsigned int bpp;

    switch (pixelformat)
    {
    case IPU_PIX_FMT_RGB565:
        /*interleaved 422*/
    case IPU_PIX_FMT_YUYV:
    case IPU_PIX_FMT_UYVY:
        /*non-interleaved 422*/
    case IPU_PIX_FMT_YUV422P:
    case IPU_PIX_FMT_YVU422P:
        bpp = 16;
        break;
    case IPU_PIX_FMT_BGR24:
    case IPU_PIX_FMT_RGB24:
    case IPU_PIX_FMT_YUV444:
    case IPU_PIX_FMT_YUV444P:
        bpp = 24;
        break;
    case IPU_PIX_FMT_BGR32:
    case IPU_PIX_FMT_BGRA32:
    case IPU_PIX_FMT_RGB32:
    case IPU_PIX_FMT_RGBA32:
    case IPU_PIX_FMT_ABGR32:
        bpp = 32;
        break;
        /*non-interleaved 420*/
    case IPU_PIX_FMT_YUV420P:
    case IPU_PIX_FMT_YVU420P:
    case IPU_PIX_FMT_YUV420P2:
    case IPU_PIX_FMT_NV12:
    case IPU_PIX_FMT_TILED_NV12:
        bpp = 12;
        break;
    default:
        bpp = 8;
        break;
    }
    return bpp;

}

