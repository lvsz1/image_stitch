#include "buffer.h"


/**
 * @brief vpu_buffer_add h264缓冲操作，如果缓冲区剩余的空间能够容纳n个数据，则进行拷贝；否则，不拷贝
 * @param vpu_out_buffer 缓冲的结构体
 * @param buf            添加数据的起始位置
 * @param n              数据的大小
 * @return               成功返回n，失败返回0
 */
int vpu_buffer_add(struct vpu_h264_out_buffer &vpu_out_buffer, char *buf, int n)
{
    if(vpu_out_buffer.end >= vpu_out_buffer.start){
        /*    out_buffer
         *     --------------------
         *    |     |  data  |     |
         *     --------------------
         *      start      end
         */
        int right_remain = vpu_out_buffer.total - vpu_out_buffer.end;
        if(right_remain >= n){
            memcpy(vpu_out_buffer.out_buf + vpu_out_buffer.end, buf, n);
            vpu_out_buffer.end = (vpu_out_buffer.end + n) % vpu_out_buffer.total;
            return n;
        }
        int total_remain = right_remain + vpu_out_buffer.start;
        if(total_remain >= n){
            memcpy(vpu_out_buffer.out_buf + vpu_out_buffer.end, buf, right_remain);
            vpu_out_buffer.end = 0;
            memcpy(vpu_out_buffer.out_buf + vpu_out_buffer.end, buf + right_remain, n - right_remain);
            vpu_out_buffer.end += (n - right_remain);
            return n;
        }
    }else{
        /*    out_buffer
         *     --------------------
         *    | data |      | data |
         *     --------------------
         *         end     start
         */
        int remain = vpu_out_buffer.start - vpu_out_buffer.end;
        if(remain >= n){
            memcpy(vpu_out_buffer.out_buf + vpu_out_buffer.end, buf, n);
            vpu_out_buffer.end += n;
            return n;
        }
    }
    return 0;
}

/**
 * @brief vpu_buffer_remove  h264缓冲操作，如果缓冲区存在n个数据，则进行拷贝；否则，不拷贝
 * @param vpu_out_buffer     缓冲的结构体
 * @param buf                输出缓冲区的起始位置
 * @param n                  拷贝数据长度
 * @return                   成功返回n，失败返回0
 */
int vpu_buffer_remove(struct vpu_h264_out_buffer &vpu_out_buffer,char *buf, int n)
{
    if(vpu_out_buffer.end >= vpu_out_buffer.start){
        /*    out_buffer
         *     --------------------
         *    |     |  data  |     |
         *     --------------------
         *      start      end
         */
        int have = vpu_out_buffer.end - vpu_out_buffer.start;
        if(have >= n){
            memcpy(buf, vpu_out_buffer.out_buf + vpu_out_buffer.start, n);
            vpu_out_buffer.start += n;
            return n;
        }
    }else{
        /*    out_buffer
         *     --------------------
         *    | data |      | data |
         *     --------------------
         *         end     start
         */
        int right_have = vpu_out_buffer.total - vpu_out_buffer.start;
        if(right_have >= n){
            memcpy(buf, vpu_out_buffer.out_buf + vpu_out_buffer.start, n);
            vpu_out_buffer.start += n;
            return n;
        }
        int total_have = right_have + vpu_out_buffer.end;
        if(total_have >= n){
            memcpy(buf, vpu_out_buffer.out_buf + vpu_out_buffer.start, right_have);
            vpu_out_buffer.start = 0;
            memcpy(buf + right_have, vpu_out_buffer.out_buf + vpu_out_buffer.start, n - right_have);
            vpu_out_buffer.start += (n - right_have);
            return n;
        }
    }
    return 0;
}
