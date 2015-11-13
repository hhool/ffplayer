#ifndef __FFMPEG_H__
#define __FFMPEG_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#include "libavcodec/audioconvert.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"

typedef void 	(*AV_LOG_SET_CALLBACK)	(void (*)(void*, int, const char*, va_list));
typedef	void 	(*AV_REGISTER_ALL)		(void);
typedef void*	(*AV_MALLOC)			(FF_INTERNAL_MEM_TYPE size);
typedef void 	(*AV_FREEP)				(void *arg);
typedef void 	(*AV_FREE)				(void *ptr);
typedef	int64_t (*AV_GETTIME)			(void);
typedef void    (*AV_FREE_PACKET)		(AVPacket *pkt);
typedef int     (*AV_DUP_PACKET)		(AVPacket *pkt);
typedef int64_t (*AV_RESCALE_Q)			(int64_t a, AVRational bq, AVRational cq);
typedef int		(*AV_READ_FRAME)		(AVFormatContext *s, AVPacket *pkt);
typedef int     (*AV_SEEK_FRAME)		(AVFormatContext *s, int stream_index, int64_t timestamp, int flags);
typedef int 	(*AV_OPEN_INPUT_FILE) 	(AVFormatContext **ic_ptr, const char *filename,
                       	   	   	   	   	 AVInputFormat *fmt,int buf_size,AVFormatParameters *ap);
typedef int 	(*AV_REGISTER_PROTOCOL2)(URLProtocol *protocol, int size);
typedef void 	(*AV_CLOSE_INPUT_FILE) 	(AVFormatContext *s);
typedef int 	(*AV_FIND_STREAM_INFO) 	(AVFormatContext *ic);
typedef void 	(*AV_UPDATE_ESTIMATE_TIMINGS)(struct AVFormatContext *ctx);


typedef AVCodec*(*AVCODEC_FIND_DECODER)				(enum CodecID id);
typedef int 	(*AVCODEC_OPEN)						(AVCodecContext *avctx, AVCodec *codec);
typedef int 	(*AVCODEC_DEFAULT_GET_BUFFER)		(AVCodecContext *s, AVFrame *pic);
typedef void 	(*AVCODEC_DEFAULT_RELEASE_BUFFER)	(AVCodecContext *s, AVFrame *pic);
typedef int 	(*AVCODEC_DECODE_VIDEO)				(AVCodecContext *avctx, AVFrame *picture,
                         	 	 	 				 int *got_picture_ptr,
                         	 	 	 				 const uint8_t *buf, int buf_size);
typedef int	 	(*AVCODEC_DECODE_AUDIO2)			(AVCodecContext *avctx, int16_t *samples,
                         	 	 	 				 int *frame_size_ptr,
                         	 	 	 				 const uint8_t *buf, int buf_size);
typedef AVFrame*(*AVCODEC_ALLOC_FRAME)				(void);
typedef void 	(*AVCODEC_FLUSH_BUFFERS)			(AVCodecContext *avctx);
typedef int 	(*AVCODEC_CLOSE)					(AVCodecContext *avctx);


typedef int 	(*URL_FOPEN)						(AVIOContext **s, const char *url, int flags);
typedef int 	(*URL_FCLOSE)						(AVIOContext *s);
typedef int64_t	(*URL_FSEEK)						(AVIOContext *s, int64_t offset, int whence);
typedef int 	(*URL_FSKIP)						(AVIOContext *s, int64_t offset);
typedef int64_t (*URL_FTELL)						(AVIOContext *s);
typedef int64_t (*URL_FSIZE)						(AVIOContext *s);
typedef void 	(*URL_SET_INTERRUPT_CB)				(int (*interrupt_cb)(void*),void* This);
typedef	int 	(*URL_FEOF)							(AVIOContext *s);
typedef int 	(*URL_FERROR)						(AVIOContext *s);


typedef AVAudioConvert *(*AV_AUDIO_CONVERT_ALLOC)	(enum AVSampleFormat out_fmt, int out_channels,enum AVSampleFormat in_fmt, int in_channels,const float *matrix, int flags);
typedef const char *	(*AV_GET_SAMPLE_FMT_NAME)	(enum AVSampleFormat sample_fmt);
typedef int 			(*AV_GET_BITS_PERSAMPLE_FMT)(enum AVSampleFormat sample_fmt);
typedef int 			(*AV_GET_BYTES_PER_SAMPLE)	(enum AVSampleFormat sample_fmt);
typedef int 			(*AV_AUDIO_CONVERT)			(AVAudioConvert *ctx,void * const out[6], const int out_stride[6],const void * const  in[6], const int  in_stride[6], int len);
typedef void 			(*AV_AUDIO_CONVERT_FREE)	(AVAudioConvert *ctx);

typedef ReSampleContext*(*AV_AUDIO_RESAMPLE_INIT)	(int output_channels, int input_channels,
													 int output_rate, int input_rate,
													 enum AVSampleFormat sample_fmt_out,
													 enum AVSampleFormat sample_fmt_in,
													 int filter_length, int log2_phase_count,
													 int linear, double cutoff);
typedef ReSampleContext*(*AUDIO_RESAMPLE_INIT)		(int output_channels, int input_channels,int output_rate, int input_rate);
typedef	int (*AUDIO_RESAMPLE)						(ReSampleContext *s, short *output, short *input, int nb_samples);
typedef void(*AUDIO_RESAMPLE_CLOSE)					(ReSampleContext *s);

typedef int (*AVPICTURE_GET_SIZE)					(enum PixelFormat pix_fmt, int width, int height);
typedef int (*AVPICTURE_FILL)						(AVPicture *picture, uint8_t *ptr, enum PixelFormat pix_fmt, int width, int height);
#ifdef __cplusplus
}
#endif

#endif
