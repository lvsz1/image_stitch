#ifndef SOCKETLIBEVENT_H
#define SOCKETLIBEVENT_H
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <errno.h>
#include <cv.h>
#include <highgui.h>
#include "schedule/threadpool.h"
#include "sift/siftmatch.h"


class SocketLibevent
{
public:
    SocketLibevent(int listen_fd);

    void connect_by_libevent();

    //回调函数 当tcp建立连接时，进行任务处理
    void *(*connect_process)(void *);

    void set_connect_process(void *(*connect_process)(void *));


    static ThreadPool pool;

private:
    int listen_fd;
    struct event_base *base;
    struct evconnlistener *listener;
};

#endif // SOCKETLIBEVENT_H
