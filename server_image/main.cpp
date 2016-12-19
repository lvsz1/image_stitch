#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <error.h>

#include "./socket/sockettools.h"
#include "sift/siftmatch.h"
#include "schedule/socketselect.h"


void *connect_process(void* arg)
{
    int fd = (int)arg;
    IplImage *image1 = SocketTools::read_image_from_socket(fd);
    IplImage *image2 = SocketTools::read_image_from_socket(fd);

    SiftMatch matcher(image1, image2);
    IplImage *result = matcher.match();

    SocketTools::send_image_by_socket(fd, result);

    close(fd);
//    cvSaveImage("result.jpg", result);

//    cvNamedWindow("result");
//    cvShowImage("result", result);
//    cvWaitKey(2000);
}

int main()
{
    int listen_fd;
    struct sockaddr_in serv_addr;

    if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket");
    }

    bzero(&serv_addr, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5556);
    if(inet_pton(AF_INET, "192.168.10.128", &serv_addr.sin_addr) < 0){
        perror("inet_pton");
    }

    int optval = 1;
    if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0){
        perror("setsockopt");
    }

    if(bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)) < 0){
        perror("bind");
    }
    if(listen(listen_fd, 10) < 0){
        perror("listen");
    }

    SocketSelect socket_select(listen_fd, connect_process);
    socket_select.connect_by_select();

    close(listen_fd);
    return 0;
}

