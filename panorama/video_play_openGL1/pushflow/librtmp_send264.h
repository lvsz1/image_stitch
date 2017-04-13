#ifndef LIBRTMP_SENDH264_H
#define LIBRTMP_SENDH264_H
//extern "C"
//{
//#ifdef __cplusplus
//#define __STDC_CONSTANT_MACROS
#include "rtmp.h"
#include "rtmp_sys.h"
#include "amf.h"
//#endif
//}
#include "sps_decode.h"
#include "pushflow/buffer.h"

//rtmp
int RTMP264_Connect(const char* url);
int RTMP264_Send(int (*read_buffer)(unsigned char *buf, int buf_size));
void RTMP264_Close();
int read_buffer1(unsigned char *buf, int buf_size );
int read_h264_raw_data(unsigned char *buf, int buf_size);



#endif // LIBRTMP_SENDH264_H
