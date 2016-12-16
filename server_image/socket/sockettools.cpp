#include "sockettools.h"

SocketTools::SocketTools()
{
}


IplImage* SocketTools::read_image_from_socket(int fd)
{
    int width = 0, height = 0, depth, channels;
    int n;

    read_image_head_information(fd, width, height, depth, channels);
    IplImage *image = cvCreateImage(cvSize(width, height), depth, channels);

    printf("width: %d height: %d\n", width, height);

    char *ptr = (char *)image->imageData;
    int image_size = width * height * channels;

    while(image_size > 0){
        n = read(fd, ptr, 1024);
        printf("recv: %d\n", n);
        ptr += n;
        image_size -= n;
    }

    cvSaveImage("t1.jpg", image);
    cvNamedWindow("result");
    cvShowImage("result", image);
    cvWaitKey();

    return image;
}


void SocketTools::read_image_head_information(int fd, int &width, int &height, int &depth, int &channels)
{
    int tmp;
    read(fd, &tmp, 4);
    width = ntohl(tmp);

    read(fd, &tmp, 4);
    height = ntohl(tmp);

    read(fd, &tmp, 4);
    depth = ntohl(tmp);

    read(fd, &tmp, 4);
    channels = ntohl(tmp);
}
