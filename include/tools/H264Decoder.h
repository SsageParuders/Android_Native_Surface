//
// Created by fgsqme on 2022/10/3.
//

#ifndef H264DECODER_H
#define H264DECODER_H


#include <cstdio>
#include <unistd.h>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
};

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55, 28, 1)
#define av_frame_alloc  avcodec_alloc_frame
#endif

class H264Decoder {

public :

    H264Decoder();

    void init();

    void decode(uint8_t *inputbuff, size_t size);

    uint8_t *getDecBuffer();

    int getWidth() const;

    void setWidth(int width);

    int getHeight() const;

    void setHeight(int height);

private:

    const AVCodec *codec;
    AVCodecContext *c = nullptr;
    int frame_count;
    AVFrame *frame;
    AVPacket avpkt;
    AVFrame *pFrameRGB;

    int RGBsize;
    int width;
    int height;
    uint8_t *out_buffer = nullptr;
    struct SwsContext *img_convert_ctx;
    bool ready;
};

#endif
