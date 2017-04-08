#include"thread_cap.h"
#include"v4l2.h"
#include"display/opengl.h"
#include"ipu/ipu_csc.h"
#include"display/framebuffer.h"

#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<stdio.h>
#include<semaphore.h>
#include"parameters.h"
#include"match/match.h"



extern EGLDisplay egldisplay;
extern EGLSurface eglsurface;

extern char *IMG;
extern char *destImage;

//全景图像的宽度、高度,在main.cpp中有定义
extern int panorama_width, panorama_height;

uchar *video0;
uchar *video1;
uchar *video2;
uchar *video3;
uchar *video4;
uchar *video5;

size_t *offset0;
size_t *offset1;
size_t *offset2;
size_t *offset3;
size_t *offset4;
size_t *offset5;

uchar *dest_vide0;
uchar *dest_vide1;
uchar *dest_vide2;
uchar *dest_vide3;
uchar *dest_vide4;
uchar *dest_vide5;


uchar *dest_spliceVid;
uchar *sigl_dis_video;

size_t length0,length1,length2,length3,length4,length5;


extern sig_atomic_t quit;
//extern int fd_ipu;


uchar * SRC_IMG;
uchar * DST_MATCH_RGB;
uchar *SrcImag[6];

unsigned char *p=NULL;

unsigned char *ffm_vid_SRC;
extern int startSave;


extern void *IPU_outbuf_S0,*IPU_outbuf_S1,*IPU_outbuf_S2,*IPU_outbuf_S3,*IPU_outbuf_S4,*IPU_outbuf_S5;

extern void *IPU_outbuf_m;

void init_v4l2(int id)
{
    int ret;
    char temp[20];

    printf("******************\n");

    sprintf(temp,"/dev/video%d",id);

    // printf("%s\n",temp);


    ret=open_device(temp,id);
    printf("open_device successfully\n");
    if(0==ret)
    {

        printf("video%d open success ! \n",id);
    }
    else{
        printf("video%d device open fault !\n",id);
    }
    ret=init_device(id);//v4l2初始化

    printf("init_device successfully\n");
    if(0==ret)
    {

        printf("init video%d device success ! \n",id);
    }
    ret=start_capturing(id);
    printf("start_capturing successfully\n");
    if(0==ret)
    {

        printf("video%d start capture success ! \n",id);
    }

}
int process_init()
{

    SRC_IMG = (uchar *)malloc(panorama_width*panorama_height* 2* sizeof(uchar));

    DST_MATCH_RGB = (uchar *)malloc(panorama_width*panorama_height* 3* sizeof(uchar));

    SrcImag[0]=dest_vide0;
    SrcImag[1]=dest_vide1;
    SrcImag[2]=dest_vide2;
    SrcImag[3]=dest_vide3;
    SrcImag[4]=dest_vide4;
    SrcImag[5]=dest_vide5;

    return 0;
}


void *threadCap00(void *)
{

    int ret;

    printf("thread have been called\n!");

    sigl_dis_video=(uchar *)malloc(320*240*3);

    dest_spliceVid=(uchar *)malloc(panorama_width*panorama_height*3);

    ffm_vid_SRC=(uchar *)malloc(panorama_width*panorama_height*2);

//    ret=process_init();
    printf("panorama_width: %d  panorama_height: %d\n", panorama_width, panorama_height);
    SRC_IMG = (uchar *)malloc(panorama_width*panorama_height* 2* sizeof(uchar));
    if(NULL == SRC_IMG){
        printf("SRC_IMG malloc error\n");
    }

    DST_MATCH_RGB = (uchar *)malloc(panorama_width*panorama_height* 3* sizeof(uchar));

    SrcImag[0]=dest_vide0;
    SrcImag[1]=dest_vide1;
    SrcImag[2]=dest_vide2;
    SrcImag[3]=dest_vide3;
    SrcImag[4]=dest_vide4;
    SrcImag[5]=dest_vide5;

//    printf("before gl_Init\n!");

//    gl_Init(DST_MATCH_RGB,sigl_dis_video);//openGl 初始化
//    printf("\ngl_Init\n!");


    while (!quit)
    {

//        gettimeofday(&begin, NULL);

//        imageReflect(SrcImag[0], SrcImag[0], SrcImag[0],
//                SrcImag[0], SrcImag[0], SrcImag[0], SRC_IMG);
        //拼接处理
        imageReflect(dest_vide0, dest_vide0, dest_vide0,
                dest_vide0, dest_vide0, dest_vide0, SRC_IMG);

//        ipu_csc_muti(SRC_IMG, DST_MATCH_RGB);
//        memcpy(SRC_IMG, dest_vide0, panorama_width*panorama_height* 2);
        //转码
        ipu_csc_muti(SRC_IMG, DST_MATCH_RGB);

//        switch(0)
//        {

//        case 0:
//            ipu_csc_single_toRGB(dest_vide0,sigl_dis_video);
//            break;
//        case 1:
//            ipu_csc_single_toRGB(dest_vide1,sigl_dis_video);
//            break;
//        case 2:
//            ipu_csc_single_toRGB(dest_vide2,sigl_dis_video);
//            break;
//        case 3:
//            ipu_csc_single_toRGB(dest_vide3,sigl_dis_video);
//            break;
//        case 4:
//            ipu_csc_single_toRGB(dest_vide4,sigl_dis_video);
//            break;
//        case 5:
//            ipu_csc_single_toRGB(dest_vide5,sigl_dis_video);
//            break;
//        }

//        framebuffer_display(sigl_dis_video,320,240,400,10);
//        memset(DST_MATCH_RGB, 255, panorama_width*panorama_height*3);

        //显示
        framebuffer_display(DST_MATCH_RGB,panorama_width,panorama_height,0,0);

        //        gl_render(DST_MATCH_RGB);
//        gl_render_singl(sigl_dis_video);
//        printf("there");
//        eglSwapBuffers(egldisplay, eglsurface);

//        frameCount ++;
//        if(frameCount % 10 == 0)
//        {
//            frameCount = 0;
//            gettimeofday(&end, NULL);
//            sec = end.tv_sec - begin.tv_sec;
//            usec = end.tv_usec - begin.tv_usec;
//            if (usec < 0) {
//                sec--;
//                usec = usec + 1000000;
//            }
//            run_time = ((sec * 1000000) + usec)/1000;
//            printf("process time: %d ms\n", run_time);
//        }
//        if(frameCount % 10 == 0)
//            gettimeofday(&begin, NULL);
//        gettimeofday(&end, NULL);

//        sec = end.tv_sec - begin.tv_sec;
//        usec = end.tv_usec - begin.tv_usec;
//        if (usec < 0) {
//            sec--;
//            usec = usec + 1000000;
//        }
//        run_time = ((sec * 1000000) + usec)/1000;

        // sem_post(&sem1);
       // printf("process time: %d ms\n", run_time);
    }

//    usleep(600000);
//    gl_deinit();
    ipu_close();

//    close_device(0);
//    close_device(1);
//    close_device(2);
//    close_device(3);
//    close_device(4);
//    close_device(5);
//    free(dest_spliceVid);
//    free(SRC_IMG);
//    free(sigl_dis_video);

}

void *threadCap0(void *)
{
    while (!quit)
    {
        int temp_num=get_frame((void **)&video0,&length0,0,offset0);
        if(temp_num != -1){
            memcpy(dest_vide0, video0, V4L2_WIDTH*V4L2_HEIGHT*2);
        //        ipu_csc_single0(video0,dest_vide0);
        //        framebuffer_display(video0,320,240,0,0);
            unget_frame(0,temp_num);
        }
    }
}

void *threadCap1(void *)
{
    while (!quit)
    {

        int temp_num=get_frame((void **)&video1,&length1,1,offset1);
        if(temp_num != -1){
    //        ipu_csc_single1(video1,dest_vide1);
            memcpy(dest_vide1, video1, V4L2_WIDTH*V4L2_HEIGHT*2);
    //        framebuffer_display(dest_vide1,640,480,320,240);
            unget_frame(1,temp_num);
            usleep(100000);
        }
    }
}


void *threadCap2(void *)
{
    while (!quit)
    {
        int temp_num=get_frame((void **)&video2,&length2,2,offset2);
        if(temp_num != -1){
         //   ipu_csc_single2(video2,dest_vide2);
            memcpy(dest_vide2, video2, V4L2_WIDTH*V4L2_HEIGHT*2);
            unget_frame(2,temp_num);
            usleep(100000);
        }
    }
}


void *threadCap3(void *)
{
    while (!quit)
    {
        int temp_num=get_frame((void **)&video3,&length3,3,offset3);
        if(temp_num != -1){
    //        ipu_csc_single3(video3,dest_vide3);
            memcpy(dest_vide3, video3, V4L2_WIDTH*V4L2_HEIGHT*2);
            unget_frame(3,temp_num);
            usleep(100000);
        }
    }
}

void *threadCap4(void *)
{
    while (!quit)
    {
        int temp_num=get_frame((void **)&video4,&length4,4,offset4);
        if(temp_num != -1){
    //        ipu_csc_single4(video4,dest_vide4);
            memcpy(dest_vide4, video4, V4L2_WIDTH*V4L2_HEIGHT*2);
            unget_frame(4,temp_num);
            usleep(100000);
        }
    }
}

void *threadCap5(void *)
{
    while (!quit)
    {
        int temp_num=get_frame((void **)&video5,&length5,5,offset5);
        if(temp_num != -1){
    //        ipu_csc_single5(video5,dest_vide5);
            memcpy(dest_vide5, video5, V4L2_WIDTH*V4L2_HEIGHT*2);
            unget_frame(5,temp_num);
            usleep(100000);
        }
    }
}


