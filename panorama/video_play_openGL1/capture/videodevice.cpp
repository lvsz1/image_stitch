#include"v4l2.h"
#include"parameters.h"
int fd[6];
//int index_num;//缓存索引号
unsigned int n_buffers;
buffer *buffers[6];

int open_device(char *dev_name,int index)
{

    fd[index]=open(dev_name,O_RDWR,0);
    if(fd[index]==-1)
    {
        perror("open videodevice error");
        return -1;
    }
    return 0;
}

int close_device(int index)
{
    stop_capturing(index);
    if(-1==close(fd[index]))
    {
        perror("close  error");
        return -1;
    }
    uninit_device(index);
    return 0;
}
/*
 *视频采集帧格式设置
 */
int init_device(int index)
{
    v4l2_crop crop;
    v4l2_format fmt;

    CLEAR(crop);
    crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    crop.c.height = 200; //采集视频的高度
    crop.c.width = 200; //宽度
    crop.c.left = 0; //左偏移
    crop.c.top = 0;

    if (-1 == ioctl (fd[index], VIDIOC_S_CROP, &crop))
    {
        perror("video set error");
        return -1;
    }

    //设置视频帧格式
    CLEAR(fmt);

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width =V4L2_WIDTH;
    fmt.fmt.pix.height = V4L2_HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    if(-1 == ioctl(fd[index], VIDIOC_S_FMT, &fmt))
    {
        perror("video set fmt error");
        return -1;
    }

    if(-1==init_mmap(index))
        {
        perror("mmap error");
        return -1;
    }

    return 0;

}
/*
 *v4l2 申请缓存4个用于存放视频帧
 */
int init_mmap(int index)
{
    v4l2_requestbuffers req;
    CLEAR(req);
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if(-1 == ioctl(fd[index], VIDIOC_REQBUFS, &req))//申请 4 个缓存
    {
        if(EINVAL == errno)
        {
            perror("does not support memory mapping");
            return -1;
        }
        else
        {
            perror("VIDIOC_REQBUFS error");
            return -1;
        }
    }
    else
    {
        perror("framebuffer request success");
    }
    if(req.count < 2)
    {
        perror("insufficient buffer memory !");
        return -1;
    }

    buffers[index] = (buffer*)calloc(req.count, sizeof(buffer));

    if(!buffers[index])
      {
      perror("out of memory");
      return -1;
    }

    //申请的4个缓存映射到用户空间
    for(n_buffers = 0; n_buffers < req.count; ++n_buffers)
    {
        v4l2_buffer buf;
        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;

        if(-1 == ioctl(fd[index], VIDIOC_QUERYBUF, &buf))//VIDIOC_QUERYBUF 获取缓存的地址
        {
            perror("VIDIOC_QUERYBUF error");
            return -1;
        }

        buffers[index][n_buffers].length = buf.length;
        buffers[index][n_buffers].offset=buf.m.offset;
        buffers[index][n_buffers].start =
                mmap(NULL, // start anywhere
                     buf.length,
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED,
                     fd[index], buffers[index][n_buffers].offset);

        if(MAP_FAILED == buffers[index][n_buffers].start)
        {
            perror("map failed");
            return -1;
        }
    }
    return 0;
}
/*
 *1.开始采集将空缓存放入缓冲队列2.开始采集
 */
int start_capturing(int index)
{
    unsigned int i;
    for(i = 0; i < n_buffers; ++i)//n_buffers为4
    {
        v4l2_buffer buf;
        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory =V4L2_MEMORY_MMAP;
        buf.index = i;

        if(-1 == ioctl(fd[index], VIDIOC_QBUF, &buf))//VIDIOC_QBUF将空缓冲区移入待处理队列，准备接收图像数据
        {
            perror("start VIDIOC_QBUF error");
            return -1;
        }
    }

    v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if(-1 == ioctl(fd[index], VIDIOC_STREAMON, &type))  //VIDIOC_STREAMON 命令开始采集
    {
       perror("VIDIOC_STREAMON error");
        return -1;
    }
    return 0;

}
int stop_capturing(int index)
{
    v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if(-1 == ioctl(fd[index], VIDIOC_STREAMOFF, &type))
    {
        perror("VIDIOC_STREAMOFF error");
        return -1;
    }
    return 0;
}
/*
 *取消缓存的映射
 */
int uninit_device(int index)
{
    unsigned int i;
    for(i = 0; i < n_buffers; ++i)
    {
        if(-1 == munmap(buffers[index][i].start, buffers[index][i].length))
        {
            perror("munmap error");
            return -1;
        }

    }
    free(buffers);
    return 0;
}
/*
 *读取输出缓存一帧图像 VIDIOC_DQBUF
 */
int get_frame(void **frame_buf,size_t *len,int index,size_t *offset)
{

    v4l2_buffer queue_buf;
    CLEAR(queue_buf);

    queue_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    queue_buf.memory = V4L2_MEMORY_MMAP;



    if(-1 == ioctl(fd[index], VIDIOC_DQBUF, &queue_buf))//读取缓存
    {
        switch(errno)
        {
        case EAGAIN:

            return -1;
        case EIO:
            return -1 ;
        default:
            perror("VIDIOC_DQBUF error");
            return -1;
        }
    }


    *frame_buf = buffers[index][queue_buf.index].start;
//    qDebug("queue_buf.index_num de zhi %d",queue_buf.index);
    //*offset=buffers[index][queue_buf.index].offset;
    *len = buffers[index][queue_buf.index].length;
//    index_num = queue_buf.index;

    return queue_buf.index;

}
/*
 *将读取后的缓存放回
 */
int unget_frame(int index,int index_num)
{
    if(index_num != -1)
    {
        v4l2_buffer queue_buf;
        CLEAR(queue_buf);

        queue_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        queue_buf.memory = V4L2_MEMORY_MMAP;
        queue_buf.index = index_num;//在get_frame中赋值

        if(-1 == ioctl(fd[index], VIDIOC_QBUF, &queue_buf))//把缓存放入缓存队列
        {
            printf("%d VIDIOC_QBUF error \n",index);
            return -1;
        }
        return 0;
    }
    return -1;
}
