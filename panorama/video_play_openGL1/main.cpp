#include"capture/v4l2.h"
#include"pthread.h"
#include"capture/thread_cap.h"
#include<semaphore.h>
#include<cv.h>
#include<highgui.h>
#include "capture/parameters.h"
#include <signal.h>
#include <semaphore.h>
#include"ipu/ipu_csc.h"
#include"display/framebuffer.h"
#include"display/opengl.h"
#include"match/match.h"

#include "vpu/vpu_test.h"
#include "pushflow/librtmp_send264.h"

#define DISON_FB0	"/dev/fb1"

//开发板处理的全景图像的宽度、高度，大致上 panorama_width=table_width*WIDTH_INTERVAL
int panorama_width, panorama_height;
//原始表格的宽度、高度
int table_width, table_height;

volatile sig_atomic_t quit = 0;

extern uchar *dest_vide0,*dest_vide1,*dest_vide2,*dest_vide3,*dest_vide4,*dest_vide5;

//退出时执行
void sighandler(int signal)
{
    printf("Caught signal %d, setting flaq to quit.\n", signal);
    quit = 1;
    sleep(1);
    exit(0);
}

//vpu
void* encode_process(void *arg);

// vpu 数据传递时同步
sem_t sem_full; // 缓存中存在拼接好的图像
sem_t sem_empty;//缓存中不存在拼接好的图像

int main(int argc,char *argv[])
{
    int ret;
    pthread_t id00,id0,id1,id2,id3,id4,id5, id_vpu;
    void *retVal0,*retVal1,*retVal2,*retVal3,*retVal4,*retVal5;

    signal(SIGINT,sighandler);
    signal(SIGTERM,sighandler);

    //信号量初始化，用于推流同步
    sem_init(&sem_full, 0, 0);
    sem_init(&sem_empty, 0, 1);

    //加载尺寸参数
    readWidhtHeight(table_width, table_height, panorama_width, panorama_height);
    //加载表格
    loadTable(table_width, table_height);

    //fb 初始化
    initfb("/dev/fb1", 1024, 600, 16);
    initfb("/dev/fb1", 1024, 600, 16);

    ipu_init();//ipu 初始化
    framebuffer_init(DISON_FB0);

    //v4l2初始化
    init_v4l2(0);
    usleep(20);
    init_v4l2(1);
    usleep(20);
    init_v4l2(2);
    usleep(20);
    init_v4l2(3);
    usleep(20);
    init_v4l2(4);
    usleep(20);
    init_v4l2(5);
    usleep(20);

    dest_vide0=(uchar *)malloc(V4L2_WIDTH*V4L2_HEIGHT*3);
    dest_vide1=(uchar *)malloc(V4L2_WIDTH*V4L2_HEIGHT*3);
    dest_vide2=(uchar *)malloc(V4L2_WIDTH*V4L2_HEIGHT*3);
    dest_vide3=(uchar *)malloc(V4L2_WIDTH*V4L2_HEIGHT*3);
    dest_vide4=(uchar *)malloc(V4L2_WIDTH*V4L2_HEIGHT*3);
    dest_vide5=(uchar *)malloc(V4L2_WIDTH*V4L2_HEIGHT*3);

    //六路视频采集
    ret=pthread_create(&id0,NULL,threadCap0,NULL);
    ret=pthread_create(&id1,NULL,threadCap1,NULL);
    ret=pthread_create(&id2,NULL,threadCap2,NULL);
    ret=pthread_create(&id3,NULL,threadCap3,NULL);
    ret=pthread_create(&id4,NULL,threadCap4,NULL);
    ret=pthread_create(&id5,NULL,threadCap5,NULL);

    //拼接显示处理  
    ret=pthread_create(&id00,NULL,threadCap00,NULL);

    //vpu压缩传输处理的线程
    usleep(200000);
    ret = pthread_create(&id_vpu, NULL, encode_process, NULL);

    pthread_join(id0,&retVal0);
    pthread_join(id1,&retVal1);
    pthread_join(id2,&retVal2);
    pthread_join(id3,&retVal3);
    pthread_join(id4,&retVal4);
    pthread_join(id5,&retVal5);
    pthread_join(id00,&retVal0);
    pthread_join(id_vpu, NULL);

    framebuffer_close();
    sleep(100);
    return 0;
}
