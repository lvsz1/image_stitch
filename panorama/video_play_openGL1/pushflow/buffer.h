#ifndef BUFFER_H
#define BUFFER_H
#include <stdio.h>
#include <string.h>

struct vpu_h264_out_buffer{
    unsigned char *out_buf;  //vpu编码后的数据
    int out_size;            //vpu编码后数据的大小
    int index;               //rtmp读指针

    int start;               //数据起始位置
    int end;                 //数据结束位置 [start, end)
    int total;               //缓冲池大小
};

/**
 * @brief vpu_buffer_add h264缓冲操作，如果缓冲区剩余的空间能够容纳n个数据，则进行拷贝；否则，不拷贝
 * @param vpu_out_buffer 缓冲的结构体
 * @param buf            添加数据的起始位置
 * @param n              数据的大小
 * @return               成功返回n，失败返回0
 */
int vpu_buffer_add(struct vpu_h264_out_buffer &vpu_out_buffer, char *buf, int n);

/**
 * @brief vpu_buffer_remove  h264缓冲操作，如果缓冲区存在n个数据，则进行拷贝；否则，不拷贝
 * @param vpu_out_buffer
 * @param buf
 * @param n
 * @return
 */
int vpu_buffer_remove(struct vpu_h264_out_buffer &vpu_out_buffer,char *buf, int n);

#endif // BUFFER_H
