#include "socketselect.h"

SocketSelect::SocketSelect(int listen_fd, void(*connect_process)(int))
    :listen_fd(listen_fd), connect_process(connect_process)
{
    connt_count = 0;
}

void SocketSelect::set_connect_process(void (*connect_process)(int))
{
    this->connect_process = connect_process;
}

void SocketSelect::connect_by_select()
{
    FD_ZERO(&all_set);
    FD_ZERO(&r_set);
    FD_SET(listen_fd, &all_set);

    for(int i = 0; i < CONNECT_MAX; i ++){
        connt_fds[i] = -1;
    }
    max_index = -1;
    maxfd = listen_fd + 1;

    while(1){
        printf("connect count: %d\n", connt_count);

        r_set = all_set;
        if((nready = select(maxfd, &r_set, NULL, NULL, NULL)) < 0){
            perror("select");
        }

        if(FD_ISSET(listen_fd, &r_set)){
            socklen_t len = sizeof(struct sockaddr_in);
            if((connt_fd = accept(listen_fd, (struct sockaddr *)&cli_addr, &len)) < 0){
                perror("accept");
            }
            int i = 0;
            for(; i < CONNECT_MAX; i ++){
                if(connt_fds[i] == -1){
                    connt_fds[i] = connt_fd;
                    FD_SET(connt_fd, &all_set);
                    if((connt_fd + 1) > maxfd)
                        maxfd = connt_fd + 1;
                    connt_count ++;
                    break;
                }
            }
            if(i == CONNECT_MAX){
                printf("it is out the max connect num\n");
                close(listen_fd);
            }
            if(i > max_index)
                max_index = i;
            if(--nready <= 0)
                continue;
        }

        for(int i = 0; i <= max_index; i ++){
            if(connt_fds[i] != -1 && FD_ISSET(connt_fds[i], &r_set)){
                int n;
//                if((n = read(connt_fds[i], buff, BUFFER_SIZE)) < 0){
//                    perror("read");
//                }else if(0 == n){
//                    close(connt_fds[i]);
//                    FD_CLR(connt_fds[i], &all_set);
//                    connt_count --;
//                }else{
//                    if(write(connt_fds[i], buff, n) < 0){
//                        perror("write");
//                    }
//                }
//                read_image_from_socket(connt_fds[i]);
//                IplImage *image = SocketTools::read_image_from_socket(connt_fds[i]);

                connect_process(connt_fds[i]);
                printf("over\n");

                if(-- nready <= 0)
                    break;
            }
        }
    }
}
