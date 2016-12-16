#ifndef SOCKETTOOLS_H
#define SOCKETTOOLS_H
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cv.h>
#include <highgui.h>

class SocketTools
{
public:
    //从socket中读取图片
    static IplImage* read_image_from_socket(int fd);

private:
    SocketTools();
    //从socket中读取图片的信息，包括尺寸、图像深度、图像通道
    static void read_image_head_information(int fd, int &width, int &height, int &depth, int &channels);
};

#endif // SOCKETTOOLS_H
