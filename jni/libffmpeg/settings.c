#include <libavcodec/avcodec.h>
#include <libavcodec/audioconvert.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include "settings.h"

Decode_flags decode_flags;


__attribute__((visibility("default"))) void set_loop_filter(int loop_filter_flag) {
	decode_flags.loop_filter_flag = loop_filter_flag;
}

__attribute__((visibility("default"))) void dummy() {
	av_audio_convert_alloc(0,0,0,0,0,0);
	audio_resample_init(0,0,0,0);

	avcodec_register_all();//avcodec
	av_register_all(); //avformat
}