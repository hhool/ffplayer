#ifndef __PLAYCORE__H__
#define __PLAYCORE__H__

#include "ffmpeg.h"
#include <dlfcn.h>
#include "config/stl_config.h"

#define OPTIMIZE_UNKNOWN -1
#define OPTIMIZE_ARMV5 0
#define OPTIMIZE_ARMV6_VFP 1
#define OPTIMIZE_ARMV7_NEON 2

class PlayCore {
public:
  PlayCore();
  ~PlayCore();
public:
  static string 	 mstrPackageName;
  static PlayCore* GetInstance();
  static void		 DestroyInstace();
public:
  AV_LOG_SET_CALLBACK av_log_set_callback;
  AV_REGISTER_ALL	av_register_all;
  AV_MALLOC 		av_malloc;
  AV_FREEP  		av_freep;
  AV_FREEP  		av_free;
  AV_GETTIME 		av_gettime;
  AV_FREE_PACKET 	av_free_packet;
  AV_DUP_PACKET 	av_dup_packet;
  AV_RESCALE_Q 	av_rescale_q;
  AV_READ_FRAME	av_read_frame;
  AV_SEEK_FRAME 	av_seek_frame;

  AV_OPEN_INPUT_FILE  	av_open_input_file;
  AV_REGISTER_PROTOCOL2 	av_register_protocol2;
  AV_CLOSE_INPUT_FILE 	av_close_input_file;
  AV_FIND_STREAM_INFO 	av_find_stream_info;
  AV_UPDATE_ESTIMATE_TIMINGS	av_update_timings;

  URL_FOPEN				url_fopen;
  URL_FCLOSE				url_fclose;
  URL_FSEEK				url_fseek;
  URL_FSKIP				url_fskip;
  URL_FTELL				url_ftell;
  URL_FSIZE				url_fsize;
  URL_SET_INTERRUPT_CB	url_set_interrupt_cb;
  URL_FEOF				url_feof;
  URL_FERROR				url_ferror;

  AVCODEC_FIND_DECODER 			avcodec_find_decoder;
  AVCODEC_OPEN 					avcodec_open;
  AVCODEC_DEFAULT_GET_BUFFER 		avcodec_default_get_buffer;
  AVCODEC_DEFAULT_RELEASE_BUFFER 	avcodec_default_release_buffer;
  AVCODEC_DECODE_VIDEO 			avcodec_decode_video;
  AVCODEC_DECODE_AUDIO2 			avcodec_decode_audio2;
  AVCODEC_ALLOC_FRAME 			avcodec_alloc_frame;
  AVCODEC_FLUSH_BUFFERS			avcodec_flush_buffers;
  AVCODEC_CLOSE					avcodec_close;

  AV_AUDIO_CONVERT_ALLOC			av_audio_convert_alloc;
  AV_GET_SAMPLE_FMT_NAME			av_get_sample_fmt_name;
  AV_GET_BITS_PERSAMPLE_FMT		av_get_bits_per_sample_fmt;
  AV_GET_BYTES_PER_SAMPLE			av_get_bytes_per_sample;
  AV_AUDIO_CONVERT				av_audio_convert;
  AV_AUDIO_CONVERT_FREE			av_audio_convert_free;

  AV_AUDIO_RESAMPLE_INIT			av_audio_resample_init;
  AUDIO_RESAMPLE_INIT				audio_resample_init;
  AUDIO_RESAMPLE					audio_resample;
  AUDIO_RESAMPLE_CLOSE			audio_resample_close;

  AVPICTURE_GET_SIZE				avpicture_get_size;
  AVPICTURE_FILL					avpicture_fill;
private:
  bool LoadPlugin(const char* filename);
  void UnLoadPlugin();
  void Reset();
  static void* mModule;
  static PlayCore* mPlayCore;
};
#endif
