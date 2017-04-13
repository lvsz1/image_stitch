#ifndef SPS_DECODE_H
#define SPS_DECODE_H
/**
 * Simplest Librtmp Send 264
 *
 * 雷霄骅，张晖
 * leixiaohua1020@126.com
 * zhanghuicuc@gmail.com
 * 中国传媒大学/数字电视技术
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 * 本程序用于将内存中的H.264数据推送至RTMP流媒体服务器。
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>


typedef  unsigned int UINT;
typedef  unsigned char BYTE;
typedef  unsigned long DWORD;

UINT Ue(BYTE *pBuff, UINT nLen, UINT *nStartBit);

int Se(BYTE *pBuff, UINT nLen, UINT *nStartBit);

DWORD u(UINT BitCount,BYTE * buf,UINT *nStartBit);

/**
 * H264的NAL起始码防竞争机制
 *
 * @param buf SPS数据内容
 *
 * @无返回值
 */
void de_emulation_prevention(BYTE* buf,unsigned int* buf_size);
/**
 * 解码SPS,获取视频图像宽、高信息
 *
 * @param buf SPS数据内容
 * @param nLen SPS数据的长度
 * @param width 图像宽度
 * @param height 图像高度

 * @成功则返回1 , 失败则返回0
 */
int h264_decode_sps(BYTE * buf,unsigned int nLen,int *width,int *height,int *fps);

#endif // SPS_DECODE_H
