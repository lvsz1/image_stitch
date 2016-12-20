#include "socketepoll.h"
#include "threadpool.h"

SocketEpoll::SocketEpoll(int listen_fd, void *(*connect_process)(void *))
    : listen_fd(listen_fd), connect_process(connect_process)
{

}

void SocketEpoll::set_connect_process(void *(*connect_process)(void *))
{
    this->connect_process = connect_process;
}

void SocketEpoll::connect_by_epoll()
{
    struct epoll_event ev, events[EVENTS_MAX];
    int epoll_fd;
    if((epoll_fd = epoll_create(EPOLL_SIZE)) == -1)
    {
        perror("epoll_create");
    }
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev) == -1)
    {
        perror("epoll_ctl");
    }

    ThreadPool thread_pool;

    for(;;)
    {
        if((nready = epoll_wait(epoll_fd, events, EVENTS_MAX, -1)) < 0)
        {
            perror("epoll_wait");
        }
        for(int i = 0; i < nready; i ++)
        {
            if(events[i].data.fd == listen_fd)
            {
                socklen_t len = sizeof(struct sockaddr_in);
                if((connt_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &len)) < 0)
                {
                    perror("accept");
                }
                connt_set.insert(connt_fd);
                ev.events = EPOLLIN;
                ev.data.fd = connt_fd;
                if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connt_fd, &ev) == -1)
                {
                    perror("epoll_ctl");
                }
            }else{
                connt_set.erase(connt_set.find(events[i].data.fd));
                if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL) < 0)
                {
                    perror("epoll_ctl");
                }

                thread_pool.add_job(connect_process, (void *)(events[i].data.fd));
            }
        }
    }
}
