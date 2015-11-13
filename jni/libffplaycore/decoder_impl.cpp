#include "decoder_impl.h"
#include "playcore.h"
#include "util_log.h"

uint8_t CAudioDecoderImp::mBuffer[CAudioDecoderImp::miBufferSize] = {0};
uint8_t CAudioDecoderImp::mBufferResample[CAudioDecoderImp::miBufferSizeResample] = {0};

AVRational CAudioDecoderImp::mTimebase = {1, 90000};
int64_t CAudioDecoderImp::mNextPts = 0;


void CAudioDecoderImp::SetTimebase (AVRational timebase)
{
    mTimebase = timebase;
}

int CAudioDecoderImp::Clear ()
{
    global_pkt_pts = 0;
    mNextPts = 0;
    return 0;
}

int CAudioDecoderImp::Decode (AVCodecContext *avctx, int16_t *samples, int *frame_size_ptr, AVPacket *avpkt, int64_t* pts, int ignore_size)
{
    if (avctx == NULL || avpkt == NULL || pts == NULL || avpkt->data == NULL || avpkt->size == 0 || frame_size_ptr == NULL)
    {
        ERROR ("parameter error!");
        return -1;
    }
    *pts = 0;

    int len1 = PlayCore::GetInstance()->avcodec_decode_audio2 (avctx, samples, frame_size_ptr, avpkt->data+ignore_size, avpkt->size-ignore_size);
    if (len1 < 0)
    {
        ERROR ("decode audio error!");
        return len1;
    }
    if (*frame_size_ptr <= 0)
    {
        VERBOSE ("no data yet! need next frame!");
        return len1;
    }
    int64_t currPts = mNextPts;
    int data_size = *frame_size_ptr;
    if (ignore_size == 0)
        currPts = avpkt->pts;
    mNextPts = currPts + data_size / (2 * avctx->channels * avctx->sample_rate) * (mTimebase.den/mTimebase.num);
    
    DEBUG ("mNextPts=%lld, currPts=%lld, data_size=%d, channels=%d, sample_rate=%d, den=%d, num=%d", mNextPts, currPts, data_size, avctx->channels, avctx->sample_rate, mTimebase.den, mTimebase.num);
    *pts = currPts;
    return len1;
}

uint64_t CAudioDecoderImp::global_pkt_pts = AV_NOPTS_VALUE;

int CAudioDecoderImp::our_get_buffer(struct AVCodecContext *c, AVFrame *pic)
{
    int ret = PlayCore::GetInstance()->avcodec_default_get_buffer(c, pic);
    uint64_t *pts = (uint64_t*)PlayCore::GetInstance()->av_malloc(sizeof(uint64_t));
    *pts = global_pkt_pts;
    //DEBUG ("debug audio pts=%ld", global_pkt_pts);
    pic->opaque = pts;
    return ret;	
}
void CAudioDecoderImp::our_release_buffer(struct AVCodecContext *c, AVFrame *pic)
{
    if(pic) PlayCore::GetInstance()->av_freep(&pic->opaque);
    PlayCore::GetInstance()->avcodec_default_release_buffer(c, pic);
}

/**************************************************** 
 * video decoder
 ****************************************************/

int CVideoDecoderImp::Clear ()
{
    global_pkt_pts = 0;
    return 0;
}

int CVideoDecoderImp::Decode (AVCodecContext *avctx, AVFrame *picture, int *got_picture_ptr, AVPacket *avpkt, int64_t* pts, int ignore_size) 
{
    if (avctx == NULL || avpkt == NULL || pts == NULL) 
	{
        ERROR ("parameter error!");
        return -1;
    }
    *pts = 0;
	*got_picture_ptr = 0;

    int64_t gotPts = 0;
    global_pkt_pts = avpkt->pts;
  
    int len1 = PlayCore::GetInstance()->avcodec_decode_video (avctx, picture, got_picture_ptr, avpkt->data, avpkt->size);

    if(avpkt->dts == AV_NOPTS_VALUE && picture->opaque != NULL && *(uint64_t*)picture->opaque != AV_NOPTS_VALUE)
	{
        gotPts = *(uint64_t *)picture->opaque;
    } 
	else if(avpkt->dts != AV_NOPTS_VALUE) 
	{
        gotPts = avpkt->dts;
    } 
	else 
	{
        gotPts = 0;
    }
    *pts = gotPts;
    return len1;
}

uint64_t CVideoDecoderImp::global_pkt_pts = AV_NOPTS_VALUE;

int CVideoDecoderImp::our_get_buffer(struct AVCodecContext *c, AVFrame *pic)
{
    int ret = PlayCore::GetInstance()->avcodec_default_get_buffer(c, pic);
    uint64_t *pts = (uint64_t*)PlayCore::GetInstance()->av_malloc(sizeof(uint64_t));
    *pts = global_pkt_pts;
    pic->opaque = pts;
    return ret;
}
void CVideoDecoderImp::our_release_buffer(struct AVCodecContext *c, AVFrame *pic)
{
    if(pic) PlayCore::GetInstance()->av_freep(&pic->opaque);
    PlayCore::GetInstance()->avcodec_default_release_buffer(c, pic);
}
