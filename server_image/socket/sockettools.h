#ifndef SOCKETTOOLS_H
#define SOCKETTOOLS_H
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cv.h>
#include <highgui.h>
#include <event2/event.h>
#include <event2/bufferevent.h>

class SocketTools
{
public:
    //从socket中读取图片
    static IplImage* read_image_from_socket(int fd);
    //采用socket传输图片
    static void send_image_by_socket(int fd, IplImage *image);
    //采用libevent传输图片
    static void send_image_by_libevent(bufferevent *bev, IplImage *image);

private:
    SocketTools();
    //从socket中读取图片的信息，包括尺寸、图像深度、图像通道
    static void read_image_head_information(int fd, int &width, int &height, int &depth, int &channels);
    //采用socket发送图片的信息，包括尺寸、图像深度、图像通道
    static void send_image_head_information(int fd, int width, int height, int depth, int channels);

    static void send_image_head_information(bufferevent *bev, int width, int height, int depth, int channels);
};

#endif // SOCKETTOOLS_H
