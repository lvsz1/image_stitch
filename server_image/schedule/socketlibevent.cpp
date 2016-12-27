#include "socketlibevent.h"
#include "socket/sockettools.h"

//多线程处理函数的传参
struct match_images
{
   IplImage *image1;
   IplImage *image2;
   struct bufferevent *bev;
};

//多线程处理函数
static void *image_match(void *arg)
{
    printf("thread poll image_match\n");

    struct match_images *images = (struct match_images *)arg;
    SiftMatch matcher(images->image1, images->image2);
    IplImage *result = matcher.match();

    SocketTools::send_image_by_libevent(images->bev, result);

    cvReleaseImage(&images->image1);
    cvReleaseImage(&images->image2);
    delete images;
}


//socket read回调函数，读取图片
static void connected_read_cb(struct bufferevent *bev, void *ctx)
{
    //因为该函数重入，所以把一些变量设置为static，以保持上次的数值
    static int image_size = 0x7fffffff;
    static IplImage *image1, *image2;
    static int image_index = 1;
    static int width, height, depth, channels;
    struct evbuffer *input = bufferevent_get_input(bev);
    int len = evbuffer_get_length(input);
    //接收的数据是否为image head信息
    static bool isImageHead = true;
    if(isImageHead && len >= 16){
        int tmp;
        evbuffer_remove(input, &tmp, 4);
        width = ntohl(tmp);
        evbuffer_remove(input, &tmp, 4);
        height = ntohl(tmp);
        evbuffer_remove(input, &tmp, 4);
        depth = ntohl(tmp);
        evbuffer_remove(input, &tmp, 4);
        channels = ntohl(tmp);
        image_size = width * height * channels;
        isImageHead = false;
    }

    if(len >= image_size)
    {
        printf("recv len: %d\n", len);
        char *ptr;
        if(1 == image_index){
            image1 = cvCreateImage(cvSize(width, height), depth, channels);
            ptr = (char *)image1->imageData;
            evbuffer_remove(input, ptr, image_size);
            image_index = 2;
        }
        else if(2 == image_index)
        {
            image2 = cvCreateImage(cvSize(width, height), depth, channels);
            ptr = (char *)image2->imageData;
            evbuffer_remove(input, ptr, image_size);

            struct match_images *images = new match_images();
            images->image1 = image1;
            images->image2 = image2;
            images->bev = bev;
            SocketLibevent::pool.add_job(image_match, images);
            image_index = 1;
        }

        isImageHead = true;
        image_size = 0x7fffffff;
    }
}

//连接的socket事件函数
static void connected_event_cb(struct bufferevent *bev, short events, void *ctx)
{
    if (events & BEV_EVENT_ERROR)
        perror("Error from bufferevent");
    if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR))
    {
        printf("close\n");
        bufferevent_free(bev);
    }
}

//连接请求的回调函数，注册事件驱动函数
static void accept_cb(struct evconnlistener *listener, evutil_socket_t sock,
               struct sockaddr *addr, int len, void *ptr)
{
    struct event_base *base = evconnlistener_get_base(listener);
    struct bufferevent *bev = bufferevent_socket_new(base, sock, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, connected_read_cb, NULL, connected_event_cb, NULL);
    bufferevent_enable(bev, EV_READ|EV_WRITE);
}

//声明线程池
ThreadPool SocketLibevent::pool;

SocketLibevent::SocketLibevent(int listen_fd)
    :listen_fd(listen_fd)
{
    base = NULL;
    listener = NULL;
}

void SocketLibevent::connect_by_libevent()
{
    if((base = event_base_new()) == NULL)
    {
        perror("event_base_new");
    }
    //设置为非阻塞状态
    evutil_make_socket_nonblocking(listen_fd);
    listener = evconnlistener_new(base, accept_cb, NULL, LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE,
                                  10, listen_fd);
    if(NULL == listener)
    {
        perror("evconnlistener");
    }
    event_base_dispatch(base);
}
