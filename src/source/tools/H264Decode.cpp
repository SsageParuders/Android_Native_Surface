//
// Created by fgsqme on 2022/10/3.
//

#include "H264Decoder.h"


void H264Decoder::init() {
    ready = false;
    avcodec_register_all();
    av_init_packet(&avpkt);
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }
    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate video codec context\n");
        exit(1);
    }
    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    frame_count = 0;
    //存储解码后转换的RGB数据
    pFrameRGB = av_frame_alloc();
}

int ScaleYUVImgToRGB(int nSrcW, int nSrcH, uint8_t *src_data, int *linesize, int nDstW, int nDstH) {
    int i;
    int ret;
    FILE *nRGB_file;

    AVFrame *nDst_picture;
    struct SwsContext *m_pSwsContext;

    nDst_picture = av_frame_alloc();
    if (!nDst_picture) {
        printf("nDst_picture avcodec_alloc_frame failed\n");
        exit(1);
    }
    if (avpicture_alloc((AVPicture *) nDst_picture, AVPixelFormat::AV_PIX_FMT_RGB24, nDstW, nDstH) < 0) {
        printf("dst_picture avpicture_alloc failed\n");
        exit(1);
    }
    m_pSwsContext = sws_getContext(nSrcW, nSrcH, AVPixelFormat::AV_PIX_FMT_YUV420P,
                                   nDstW, nDstH, AVPixelFormat::AV_PIX_FMT_RGB24,
                                   SWS_BICUBIC,
                                   NULL, NULL, NULL);

    if (NULL == m_pSwsContext) {
        printf("ffmpeg get context error!\n");
        exit(-1);
    }

    sws_scale(m_pSwsContext, (const uint8_t *const *) src_data, linesize, 0, nSrcH, nDst_picture->data,
              nDst_picture->linesize);

    nRGB_file = fopen("..\\YUV_STREAM\\RGBFile.rgb", "ab+");
    fwrite(nDst_picture->data[0], nDstW * nDstH * 3, 1, nRGB_file);
    fclose(nRGB_file);

    sws_freeContext(m_pSwsContext);
    avpicture_free((AVPicture *) nDst_picture);

    return 0;
}

void H264Decoder::decode(uint8_t *inputbuf, size_t size) {

    avpkt.size = size;
    if (avpkt.size == 0)
        return;
    avpkt.data = inputbuf;
    int len, got_frame;
    len = avcodec_decode_video2(c, frame, &got_frame, &avpkt);

    if (len < 0) {
        ready = false;
        fprintf(stderr, "Error while decoding frame %d\n", frame_count);
        frame_count++;
        return;
    }
    if (out_buffer == nullptr) {
        RGBsize = avpicture_get_size(AV_PIX_FMT_RGB24, c->width,
                                     c->height);
        out_buffer = (uint8_t *) av_malloc(RGBsize);
        avpicture_fill((AVPicture *) pFrameRGB, out_buffer, AV_PIX_FMT_RGB24,
                       c->width, c->height);
        img_convert_ctx =
                sws_getContext(c->width, c->height, c->pix_fmt,
                               c->width, c->height,
                               AV_PIX_FMT_RGB24,
                               SWS_BICUBIC,
                               NULL, NULL, NULL);
        width = c->width;
        height = c->height;
        printf("decode width: %d height: %d\n", width, height);
    }

    if (got_frame) {
        ready = true;
        sws_scale(img_convert_ctx, (const uint8_t *const *) frame->data,
                  frame->linesize, 0, c->height, pFrameRGB->data, pFrameRGB->linesize);
        frame_count++;
    } else {
        ready = false;
    }
    if (avpkt.data) {
        avpkt.size -= len;
        avpkt.data += len;
    }
}


H264Decoder::H264Decoder() {
    init();
}

uint8_t *H264Decoder::getDecBuffer() {
    if (ready) {
        return out_buffer;
    } else {
        return nullptr;
    }
}

int H264Decoder::getWidth() const {
    return width;
}

void H264Decoder::setWidth(int width) {
    H264Decoder::width = width;
}

int H264Decoder::getHeight() const {
    return height;
}

void H264Decoder::setHeight(int height) {
    H264Decoder::height = height;
}
