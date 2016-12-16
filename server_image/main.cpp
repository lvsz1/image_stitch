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

#define CONNECT_MAX 100
#define BUFFER_SIZE 100

int main()
{
    IplImage *image1 = cvLoadImage("1.jpg");
    IplImage *image2 = cvLoadImage("2.jpg");

    SiftMatch matcher(image1, image2);
    IplImage *result = matcher.match();
    cvNamedWindow("result");
    cvShowImage("result", result);
    cvWaitKey();

//    int listen_fd, connt_fd;
//    int connt_fds[CONNECT_MAX];
//    fd_set all_set, r_set;
//    struct sockaddr_in serv_addr, cli_addr;
//    int nready, maxfd, max_index;
//    int connt_count = 0;
//    char buff[BUFFER_SIZE];

//    if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
//        perror("socket");
//    }

//    bzero(&serv_addr, sizeof(struct sockaddr_in));
//    serv_addr.sin_family = AF_INET;
//    serv_addr.sin_port = htons(5556);
//    if(inet_pton(AF_INET, "192.168.10.128", &serv_addr.sin_addr) < 0){
//        perror("inet_pton");
//    }

//    int optval = 1;
//    if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0){
//        perror("setsockopt");
//    }

//    if(bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)) < 0){
//        perror("bind");
//    }
//    if(listen(listen_fd, 10) < 0){
//        perror("listen");
//    }

//    FD_ZERO(&all_set);
//    FD_ZERO(&r_set);
//    FD_SET(listen_fd, &all_set);

//    for(int i = 0; i < CONNECT_MAX; i ++){
//        connt_fds[i] = -1;
//    }
//    max_index = -1;
//    maxfd = listen_fd + 1;

//    while(1){
//        printf("connect count: %d\n", connt_count);

//        r_set = all_set;
//        if((nready = select(maxfd, &r_set, NULL, NULL, NULL)) < 0){
//            perror("select");
//        }

//        if(FD_ISSET(listen_fd, &r_set)){
//            socklen_t len = sizeof(struct sockaddr_in);
//            if((connt_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &len)) < 0){
//                perror("accept");
//            }
//            int i = 0;
//            for(; i < CONNECT_MAX; i ++){
//                if(connt_fds[i] == -1){
//                    connt_fds[i] = connt_fd;
//                    FD_SET(connt_fd, &all_set);
//                    if((connt_fd + 1) > maxfd)
//                        maxfd = connt_fd + 1;
//                    connt_count ++;
//                    break;
//                }
//            }
//            if(i == CONNECT_MAX){
//                printf("it is out the max connect num\n");
//                close(listen_fd);
//            }
//            if(i > max_index)
//                max_index = i;
//            if(--nready <= 0)
//                continue;
//        }

//        for(int i = 0; i <= max_index; i ++){
//            if(connt_fds[i] != -1 && FD_ISSET(connt_fds[i], &r_set)){
//                int n;
////                if((n = read(connt_fds[i], buff, BUFFER_SIZE)) < 0){
////                    perror("read");
////                }else if(0 == n){
////                    close(connt_fds[i]);
////                    FD_CLR(connt_fds[i], &all_set);
////                    connt_count --;
////                }else{
////                    if(write(connt_fds[i], buff, n) < 0){
////                        perror("write");
////                    }
////                }
////                read_image_from_socket(connt_fds[i]);
//                IplImage *image = SocketTools::read_image_from_socket(connt_fds[i]);
//                if(-- nready <= 0)
//                    break;
//            }
//        }
//    }

//    close(listen_fd);
    return 0;
}

