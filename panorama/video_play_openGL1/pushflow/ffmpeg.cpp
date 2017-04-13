#ifndef FFMPEG_C
#define FFMPEG_C
#include "vpu/vpu_test.h"

void ffmpeg_init()
{
    av_register_all();
    //Network
    avformat_network_init();
}

void ffmpeg_parameters_set(struct cmd_line *cmd, char *mv_type)
{
    cmd->av_ctx = (struct av_context *)malloc(sizeof(struct av_context));
    struct av_context *av_ctx = cmd->av_ctx;
 #if 1
    //format init and set
    avformat_alloc_output_context2(&av_ctx->pFormatCtx , NULL, mv_type, NULL);

    if(NULL == cmd->av_ctx->pFormatCtx){
        printf("error: avformat_alloc_output_context2\n");
        exit(-1);
    }

    av_ctx->pOutputFmt = av_ctx->pFormatCtx->oformat;

    //codec init and set
    av_ctx->pCodec = avcodec_find_encoder(AV_CODEC_ID_H264);

    //create  AVStream
    AVStream *out_stream = avformat_new_stream(av_ctx->pFormatCtx, av_ctx->pCodec);
    if(NULL == out_stream){
        printf("error: avformat_new_stream\n");
        exit(-1);
    }
    printf("nb_streams: %d\n", av_ctx->pFormatCtx->nb_streams);
    out_stream->id = av_ctx->pFormatCtx->nb_streams - 1;



    av_ctx->pCodecCtx = avcodec_alloc_context3(av_ctx->pCodec);
    if(NULL == av_ctx->pCodecCtx){
        printf("error: avcodec_alloc_context3\n");
        exit(-1);
    }

    av_ctx->frame_index = 0;
    av_ctx->key_frame = -1;



    av_ctx->pCodecCtx->extradata = (uint8_t *)malloc(32);//给extradata成员参数分配内存
     av_ctx->pCodecCtx->extradata_size = 32;//extradata成员参数分配内存大小

     av_ctx->pFormatCtx->streams[0]->codec->extradata = av_ctx->pCodecCtx->extradata;
     av_ctx->pFormatCtx->streams[0]->codec->extradata_size = 32;

    //给extradata成员参数设置值
     //00 00 00 01
     av_ctx->pCodecCtx->extradata[0] = 0x00;
     av_ctx->pCodecCtx->extradata[1] = 0x00;
     av_ctx->pCodecCtx->extradata[2] = 0x00;
     av_ctx->pCodecCtx->extradata[3] = 0x01;
     //67 42 80 1e
     av_ctx->pCodecCtx->extradata[4] = 0x67;
     av_ctx->pCodecCtx->extradata[5] = 0x42;
     av_ctx->pCodecCtx->extradata[6] = 0x80;
     av_ctx->pCodecCtx->extradata[7] = 0x1e;
     //88 8b 40 50
     av_ctx->pCodecCtx->extradata[8] = 0x88;
     av_ctx->pCodecCtx->extradata[9] = 0x8b;
     av_ctx->pCodecCtx->extradata[10] = 0x40;
     av_ctx->pCodecCtx->extradata[11] = 0x50;
     //1e d0 80 00
     av_ctx->pCodecCtx->extradata[12] = 0x1e;
     av_ctx->pCodecCtx->extradata[13] = 0xd0;
     av_ctx->pCodecCtx->extradata[14] = 0x80;
     av_ctx->pCodecCtx->extradata[15] = 0x00;
     //03 84 00 00
     av_ctx->pCodecCtx->extradata[16] = 0x03;
     av_ctx->pCodecCtx->extradata[17] = 0x84;
     av_ctx->pCodecCtx->extradata[18] = 0x00;
     av_ctx->pCodecCtx->extradata[19] = 0x00;
     //af c8 02 00
     av_ctx->pCodecCtx->extradata[20] = 0xaf;
     av_ctx->pCodecCtx->extradata[21] = 0xc8;
     av_ctx->pCodecCtx->extradata[22] = 0x02;
     av_ctx->pCodecCtx->extradata[23] = 0x00;
     //00 00 00 01
     av_ctx->pCodecCtx->extradata[24] = 0x00;
     av_ctx->pCodecCtx->extradata[25] = 0x00;
     av_ctx->pCodecCtx->extradata[26] = 0x00;
     av_ctx->pCodecCtx->extradata[27] = 0x01;
     //68 ce 38 80
     av_ctx->pCodecCtx->extradata[28] = 0x68;
     av_ctx->pCodecCtx->extradata[29] = 0xce;
     av_ctx->pCodecCtx->extradata[30] = 0x38;
     av_ctx->pCodecCtx->extradata[31] = 0x80;


    av_ctx->pCodecCtx->codec_id = AV_CODEC_ID_H264;
    av_ctx->pCodecCtx->bit_rate = cmd->bitrate * 1000;
    av_ctx->pCodecCtx->width = cmd->width;
    av_ctx->pCodecCtx->height = cmd->height;
    av_ctx->pCodecCtx->time_base = (AVRational){1, cmd->fps};
    out_stream->time_base = av_ctx->pCodecCtx->time_base;
    av_ctx->pCodecCtx->gop_size = cmd->gop;
    av_ctx->pCodecCtx->max_b_frames = 0;
    av_ctx->pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    if(av_ctx->pFormatCtx->oformat->flags & AVFMT_GLOBALHEADER)
        out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

    if(avcodec_open2(av_ctx->pCodecCtx, av_ctx->pCodec, NULL) != 0){
        printf("error: avcodec_open2\n");
        exit(-1);
    }
    /* copy the stream parameters to the muxer */
    if(avcodec_parameters_from_context(out_stream->codecpar, av_ctx->pCodecCtx) < 0){
        fprintf(stderr, "Could not copy the stream parameters\n");
        exit(1);
    }
    av_dump_format(av_ctx->pFormatCtx, 0, cmd->output, 1);

    //open output file
    if(!(av_ctx->pOutputFmt->flags & AVFMT_NOFILE)){
        if((avio_open(&av_ctx->pFormatCtx->pb, cmd->output, AVIO_FLAG_WRITE)) < 0){
            printf("error: avio_open\n");
            exit(-1);
        }
    }
    printf("output: %s\n", cmd->output);
    //write file header
    if(avformat_write_header(av_ctx->pFormatCtx, NULL) < 0){
        printf("error: avformat_write_header\n");
        exit(-1);
    }
#else
//    AVFormatContext *ifmt_ctx = NULL;
    AVOutputFormat *ofmt = NULL;
    av_ctx->pFormatCtxInput = NULL;
    int ret;
    if ((ret = avformat_open_input(&av_ctx->pFormatCtxInput, "/mnt/nfs/blue.h264", 0, 0)) < 0) {
        printf( "Could not open input file.\n");
        exit(-1);
    }
    if ((ret = avformat_find_stream_info(av_ctx->pFormatCtxInput, 0)) < 0) {
        printf( "Failed to retrieve input stream information");
        exit(-1);
    }


    int i;
    int videoindex=-1;
    for(i=0; i<av_ctx->pFormatCtxInput->nb_streams; i++)
        if(av_ctx->pFormatCtxInput->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
            videoindex=i;
            break;
        }

    printf("videoindex: %d\n", videoindex);
    av_dump_format(av_ctx->pFormatCtxInput, 0, "/mnt/nfs/blue.h264", 0);

    //输出（Output）

    avformat_alloc_output_context2(&av_ctx->pFormatCtx, NULL, "flv", cmd->output); //RTMP
    //avformat_alloc_output_context2(&ofmt_ctx, NULL, "mpegts", out_filename);//UDP

    if (!av_ctx->pFormatCtx) {
        printf( "Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        exit(-1);
    }
    ofmt = av_ctx->pFormatCtx->oformat;



    for (i = 0; i < av_ctx->pFormatCtxInput->nb_streams; i++) {
        //根据输入流创建输出流（Create output AVStream according to input AVStream）
        AVStream *in_stream = av_ctx->pFormatCtxInput->streams[i];
        AVStream *out_stream = avformat_new_stream(av_ctx->pFormatCtx, in_stream->codec->codec);
        if (!out_stream) {
            printf( "Failed allocating output stream\n");
            ret = AVERROR_UNKNOWN;
            exit(-1);
        }
        //复制AVCodecContext的设置（Copy the settings of AVCodecContext）
        ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
        if (ret < 0) {
            printf( "Failed to copy context from input to output stream codec context\n");
            exit(-1);
        }
//        out_stream->codec->codec_tag = 0;
        if (av_ctx->pFormatCtx->oformat->flags & AVFMT_GLOBALHEADER)
            out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }
    //Dump Format------------------
    av_dump_format(av_ctx->pFormatCtx, 0, cmd->output, 1);
    //打开输出URL（Open output URL）
    if (!(ofmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&av_ctx->pFormatCtx->pb, cmd->output, AVIO_FLAG_WRITE);
        if (ret < 0) {
            printf( "Could not open output URL '%s'", cmd->output);
            exit(-1);
        }
    }

    //写文件头（Write file header）
    ret = avformat_write_header(av_ctx->pFormatCtx, NULL);
    if (ret < 0) {
        printf( "Error occurred when opening output URL\n");
        exit(-1);
    }
    printf("here\n");
 #endif
}


// < 0 = error
// 0 = I-Frame
// 1 = P-Frame
// 2 = B-Frame
// 3 = S-Frame
int getVopType( const void *p, int len )
{
        if ( !p || 6 >= len )
                return -1;

        unsigned char *b = (unsigned char*)p;

        // Verify NAL marker
        if ( b[ 0 ] || b[ 1 ] || 0x01 != b[ 2 ] )
        {   b++;
        if ( b[ 0 ] || b[ 1 ] || 0x01 != b[ 2 ] )
                return -1;
        } // end if

        b += 3;

        // Verify VOP id
        if ( 0xb6 == *b )
        {   b++;
        return ( *b & 0xc0 ) >> 6;
        } // end if

        switch( *b )
        {   case 0x65 : return 0;
        case 0x61 : return 1;
        case 0x01 : return 2;
        } // end switch

        return -1;
}


void ffmpeg_clean(struct cmd_line *cmd)
{
    avcodec_free_context(&cmd->av_ctx->pCodecCtx);
    avformat_free_context(cmd->av_ctx->pFormatCtx);
    free(cmd->av_ctx);
}


#endif // FFMPEG_C
