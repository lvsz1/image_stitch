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

//    printf("width: %d height: %d\n", width, height);

    char *ptr = (char *)image->imageData;
    int image_size = width * height * channels;

    while(image_size > 0){
        int next_count = image_size < 1024 ? image_size : 1024;
        n = read(fd, ptr, next_count);
//        printf("recv: %d\n", n);
        ptr += n;
        image_size -= n;
    }

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

void SocketTools::send_image_by_socket(int fd, IplImage *image)
{
    int width, height, depth, channels;
    width = image->width;
    height = image->height;
    depth = image->depth;
    channels = image->nChannels;

    send_image_head_information(fd, width, height, depth, channels);

    int image_size = width * height * channels;
    char *ptr = image->imageData;
    while(image_size > 0){
        int n = image_size < 1024 ? image_size: 1024;
        write(fd, ptr, n);
        image_size -= n;
        ptr += n;
    }
}

void SocketTools::send_image_head_information(int fd, int width, int height, int depth, int channels)
{
    int tmp;
    tmp = htonl(width);
    write(fd, &tmp, 4);

    tmp = htonl(height);
    write(fd, &tmp, 4);

    tmp = htonl(depth);
    write(fd, &tmp, 4);

    tmp = htonl(channels);
    write(fd, &tmp, 4);
}

void SocketTools::send_image_by_libevent(bufferevent *bev, IplImage *image)
{
    int width, height, depth, channels;
    width = image->width;
    height = image->height;
    depth = image->depth;
    channels = image->nChannels;
    int image_size = width * height * channels;

    bufferevent_lock(bev);
    send_image_head_information(bev, width, height, depth, channels);
    bufferevent_write(bev, image->imageData, image_size);
    bufferevent_unlock(bev);
}

void SocketTools::send_image_head_information(bufferevent *bev, int width, int height, int depth, int channels)
{
    int tmp;
    tmp = htonl(width);
    bufferevent_write(bev, &tmp, 4);

    tmp = htonl(height);
    bufferevent_write(bev, &tmp, 4);

    tmp = htonl(depth);
    bufferevent_write(bev, &tmp, 4);

    tmp = htonl(channels);
    bufferevent_write(bev, &tmp, 4);
}
