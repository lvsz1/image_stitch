#ifndef SOCKETEPOLL_H
#define SOCKETEPOLL_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <error.h>
#include <sys/epoll.h>
#include <set>
#include <iostream>
#include "io_limit.h"
using namespace std;

class SocketEpoll
{
public:
    SocketEpoll(int listen_fd, void *(*connect_process)(void *));

    //进入select等待状态
    void connect_by_epoll();

    //回调函数 当tcp建立连接时，进行任务处理
    void *(*connect_process)(void *);

    void set_connect_process(void *(*connect_process)(void *));

private:
    int listen_fd, connt_fd;
    set<int> connt_set;
    struct sockaddr_in cli_addr;
    int nready, maxfd;
    int connt_count;
};

#endif // SOCKETEPOLL_H
