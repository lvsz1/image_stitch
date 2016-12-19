#ifndef SOCKETSELECT_H
#define SOCKETSELECT_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>
#include <arpa/inet.h>
#include <error.h>
#include "io_limit.h"

//select机制对tcp的连接处理
class SocketSelect
{
public:
    SocketSelect(int listen_fd, void *(*connect_process)(void *));

    //进入select等待状态
    void connect_by_select();

    //回调函数 当tcp建立连接时，进行任务处理
    void *(*connect_process)(void *);

    void set_connect_process(void *(*connect_process)(void *));

private:
    int listen_fd, connt_fd;
    int connt_fds[CONNECT_MAX];
    fd_set all_set, r_set;
    struct sockaddr_in cli_addr;
    int nready, maxfd, max_index;
    int connt_count;
};

#endif // SOCKETSELECT_H
