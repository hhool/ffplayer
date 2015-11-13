#ifndef __DECODER_IMPL_H__
#define __DECODER_IMPL_H__

#include "ffmpeg.h"
#include <stdio.h>
#include <math.h>
#include "util_lock.h"
#include "packetList.h"
#include "config/stl_config.h"

class CAudioDecoderImp
{
public:
    static void SetTimebase (AVRational timebase);
    /** 
     * decode audio
     * 
     * @param avctx 
     * @param samples 
     * @param frame_size_ptr 
     * @param avpkt 
     * @param pos 
     * 
     * @return 
     */
    static int Decode (AVCodecContext *avctx, int16_t *samples, int *frame_size_ptr, AVPacket *avpkt, int64_t* pts, int ignore_size = 0);

    static int Clear ();

    const static int miBufferSize = (AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2;
    static uint8_t mBuffer[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];
	const static int miBufferSizeResample = (AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2;
    static uint8_t mBufferResample[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];
    static int64_t mNextPts;
    static AVRational mTimebase;

    static int our_get_buffer(AVCodecContext *c, AVFrame *pic);
    static void our_release_buffer(AVCodecContext *c, AVFrame *pic);
private:
    static uint64_t global_pkt_pts;
};

class CVideoDecoderImp
{
public:
    static int Decode (AVCodecContext *avctx, AVFrame *picture, int *got_picture_ptr, AVPacket *avpkt, int64_t* pts, int ignore_size = 0);

    static int Clear ();

    static int our_get_buffer(AVCodecContext *c, AVFrame *pic);
    static void our_release_buffer(AVCodecContext *c, AVFrame *pic);
private:
    static uint64_t global_pkt_pts;

};

#endif
