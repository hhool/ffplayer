#include "playcore.h"
#include "cpu.h"
#include "util_log.h"

PlayCore* 	PlayCore::mPlayCore			= NULL;
void* 		PlayCore::mModule			= NULL;
string 		PlayCore::mstrPackageName	= "";




PlayCore* PlayCore::GetInstance()
{
	if(!mPlayCore)
	{
		int Flag = GetCpuFeatures();
		char szDir[1024]={"/data/data/"};
		if(Flag == OPTIMIZE_UNKNOWN)
			return NULL;

		strcat(szDir,mstrPackageName.c_str());
		strcat(szDir,"/lib/");

		if(Flag == OPTIMIZE_ARMV5)
		{
			strcat(szDir,"libffmpeg_armv5.so");
		}
		else if(Flag == OPTIMIZE_ARMV6_VFP)
		{
			strcat(szDir,"libffmpeg_armv6_vfp.so");
		}
		else if(Flag == OPTIMIZE_ARMV7_NEON)
		{
			strcat(szDir,"libffmpeg_armv7_neon.so");
		}

		mPlayCore = new PlayCore();

		INFO("CorePath %s",szDir);

		if(!mPlayCore->LoadPlugin(szDir))
		{
			delete mPlayCore;
			mPlayCore = NULL;
		}
		return mPlayCore;
	}
	return mPlayCore;
}

void PlayCore::DestroyInstace()
{
	if(mPlayCore != NULL)
	{
		delete mPlayCore;
		mPlayCore = NULL;
	}
}

PlayCore::PlayCore()
{

}

PlayCore::~PlayCore()
{
	UnLoadPlugin();
	Reset();
}

bool PlayCore::LoadPlugin(const char* filename)
{
	bool bOk = false;

	DEBUG("LoadPlugin mModule %s",filename);

	if(!(mModule = dlopen(filename,RTLD_LAZY)))
	{
		ERROR("LoadPlugin mModule 0x%x ERROR",mModule);
		return false;
	}

	if(!(av_log_set_callback	= (AV_LOG_SET_CALLBACK)dlsym(mModule,"av_log_set_callback")))
	{
		ERROR("dlsym av_log_set_callback error");
		goto LOAD_END;
	}
	DEBUG("dlsym av_log_set_callback 0x%x",av_log_set_callback);
	
	if(!(av_register_all	= (AV_REGISTER_ALL)dlsym(mModule,"av_register_all")))
	{
		ERROR("dlsym av_register_all error");
		goto LOAD_END;
	}
	DEBUG("dlsym av_register_all 0x%x",av_register_all);

	if(!(av_malloc		= (AV_MALLOC)dlsym(mModule,"av_malloc")))
	{
		ERROR("dlsym av_malloc error");
		goto LOAD_END;
	}
	DEBUG("dlsym av_malloc 0x%x",av_malloc);

	if(!(av_freep		= (AV_FREEP)dlsym(mModule,"av_freep")))
	{
		ERROR("dlsym av_freep error");
		goto LOAD_END;
	}
	DEBUG("dlsym av_freep 0x%x",av_freep);

	if(!(av_free			= (AV_FREE)dlsym(mModule,"av_free")))
	{
		ERROR("dlsym av_free error");
		goto LOAD_END;
	}
	DEBUG("dlsym av_free 0x%x",av_free);

	if(!(av_gettime		= (AV_GETTIME)dlsym(mModule,"av_gettime")))
	{
		ERROR("dlsym av_gettime error");
		goto LOAD_END;
	}
	DEBUG("dlsym av_gettime 0x%x",av_gettime);

	if(!(av_free_packet	= (AV_FREE_PACKET)dlsym(mModule,"av_free_packet")))
	{
		ERROR("dlsym av_free_packet error");
		goto LOAD_END;
	}
	DEBUG("dlsym av_free_packet 0x%x",av_free_packet);

	if(!(av_dup_packet	= (AV_DUP_PACKET)dlsym(mModule,"av_dup_packet")))
	{
		ERROR("dlsym av_dup_packet error");
		goto LOAD_END;
	}
	DEBUG("dlsym av_dup_packet 0x%x",av_dup_packet);

	if(!(av_rescale_q	= (AV_RESCALE_Q)dlsym(mModule,"av_rescale_q")))
	{
		ERROR("dlsym av_rescale_q error");
		goto LOAD_END;
	}
	DEBUG("dlsym av_rescale_q 0x%x",av_rescale_q);

	if(!(av_read_frame	= (AV_READ_FRAME)dlsym(mModule,"av_read_frame")))
	{
		ERROR("dlsym av_read_frame error");
		goto LOAD_END;
	}
	DEBUG("dlsym av_read_frame 0x%x",av_read_frame);

	if(!(av_seek_frame	= (AV_SEEK_FRAME)dlsym(mModule,"av_seek_frame")))
	{
		ERROR("dlsym av_seek_frame error");
		goto LOAD_END;
	}
	DEBUG("dlsym av_seek_frame 0x%x",av_seek_frame);

	if(!(av_open_input_file	= (AV_OPEN_INPUT_FILE)dlsym(mModule,"av_open_input_file")))
	{
		ERROR("dlsym av_open_input_file error");
		goto LOAD_END;
	}
	DEBUG("dlsym av_open_input_file 0x%x",av_open_input_file);
		
		
	if(!(av_register_protocol2	= (AV_REGISTER_PROTOCOL2)dlsym(mModule,"av_register_protocol2")))
	{
		ERROR("dlsym av_register_protocol2 error");
		goto LOAD_END;
	}
	DEBUG("dlsym av_register_protocol2 0x%x",av_register_protocol2);

	if(!(av_close_input_file = (AV_CLOSE_INPUT_FILE)dlsym(mModule,"av_close_input_file")))
	{
		ERROR("dlsym av_close_input_file error");
		goto LOAD_END;
	}
	DEBUG("dlsym av_close_input_file 0x%x",av_close_input_file);

	if(!(av_find_stream_info = (AV_FIND_STREAM_INFO)dlsym(mModule,"av_find_stream_info")))
	{
		ERROR("dlsym av_find_stream_info error");
		goto LOAD_END;
	}
	DEBUG("dlsym av_find_stream_info 0x%x",av_find_stream_info);

	
	if(!(av_update_timings = (AV_UPDATE_ESTIMATE_TIMINGS)dlsym(mModule,"av_update_estimate_timings_from_pts")))
	{
		ERROR("dlsym av_update_timings error");
		goto LOAD_END;
	}
	DEBUG("dlsym av_update_timings 0x%x",av_update_timings);

	if(!(url_fopen = (URL_FOPEN)dlsym(mModule,"url_fopen")))
	{
		ERROR("dlsym url_fopen error");
		goto LOAD_END;
	}
	DEBUG("dlsym url_fopen 0x%x",url_fopen);
	
	
	if(!(url_fclose = (URL_FCLOSE)dlsym(mModule,"url_fclose")))
	{
		ERROR("dlsym url_fclose error");
		goto LOAD_END;
	}
	DEBUG("dlsym url_fclose 0x%x",url_fclose);
	
	
	if(!(url_fseek = (URL_FSEEK)dlsym(mModule,"url_fseek")))
	{
		ERROR("dlsym url_fseek error");
		goto LOAD_END;
	}
	DEBUG("dlsym url_fseek 0x%x",url_fseek);
	
	
	if(!(url_fskip = (URL_FSKIP)dlsym(mModule,"url_fskip")))
	{
		ERROR("dlsym url_fskip error");
		goto LOAD_END;
	}
	DEBUG("dlsym url_fskip 0x%x",url_fskip);
	
	
	if(!(url_ftell = (URL_FTELL)dlsym(mModule,"url_ftell")))
	{
		ERROR("dlsym url_ftell error");
		goto LOAD_END;
	}
	DEBUG("dlsym url_ftell 0x%x",url_ftell);

	
	if(!(url_fsize = (URL_FSIZE)dlsym(mModule,"url_fsize")))
	{
		ERROR("dlsym url_fsize error");
		goto LOAD_END;
	}
	DEBUG("dlsym url_fsize 0x%x",url_fsize);
	
	if(!(url_set_interrupt_cb = (URL_SET_INTERRUPT_CB)dlsym(mModule,"url_set_interrupt_cb")))
	{
		ERROR("dlsym url_set_interrupt_cb error");
		goto LOAD_END;
	}
	DEBUG("dlsym url_set_interrupt_cb 0x%x",url_set_interrupt_cb);

	if(!(url_feof = (URL_FEOF)dlsym(mModule,"url_feof")))
	{
		ERROR("dlsym url_feof error");
		goto LOAD_END;
	}
	DEBUG("dlsym url_feof 0x%x",url_feof);
	

	if(!(avcodec_find_decoder= (AVCODEC_FIND_DECODER)dlsym(mModule,"avcodec_find_decoder")))
	{
		ERROR("dlsym avcodec_find_decoder error");
		goto LOAD_END;
	}
	DEBUG("dlsym avcodec_find_decoder 0x%x",avcodec_find_decoder);

	if(!(avcodec_open		= (AVCODEC_OPEN)dlsym(mModule,"avcodec_open")))
	{
		ERROR("dlsym avcodec_open error");
		goto LOAD_END;
	}
	DEBUG("dlsym avcodec_open 0x%x",avcodec_open);

	if(!(avcodec_default_get_buffer		= (AVCODEC_DEFAULT_GET_BUFFER)dlsym(mModule,"avcodec_default_get_buffer")))
	{
		ERROR("dlsym avcodec_default_get_buffer error");
		goto LOAD_END;
	}
	DEBUG("dlsym avcodec_default_get_buffer 0x%x",avcodec_default_get_buffer);

	if(!(avcodec_default_release_buffer	= (AVCODEC_DEFAULT_RELEASE_BUFFER)dlsym(mModule,"avcodec_default_release_buffer")))
	{
		ERROR("dlsym avcodec_default_release_buffer error");
		goto LOAD_END;
	}
	DEBUG("dlsym avcodec_default_release_buffer 0x%x",avcodec_default_release_buffer);

	if(!(avcodec_decode_video	= (AVCODEC_DECODE_VIDEO)dlsym(mModule,"avcodec_decode_video")))
	{
		ERROR("dlsym avcodec_decode_video error");
		goto LOAD_END;
	}
	DEBUG("dlsym avcodec_decode_video 0x%x",avcodec_decode_video);

	if(!(avcodec_decode_audio2	= (AVCODEC_DECODE_AUDIO2)dlsym(mModule,"avcodec_decode_audio2")))
	{
		ERROR("dlsym avcodec_decode_audio2 error");
		goto LOAD_END;
	}
	DEBUG("dlsym avcodec_decode_audio2 0x%x",avcodec_decode_audio2);

	if(!(avcodec_alloc_frame		= (AVCODEC_ALLOC_FRAME)dlsym(mModule,"avcodec_alloc_frame")))
	{
		ERROR("dlsym avcodec_alloc_frame error");
		goto LOAD_END;
	}
	DEBUG("dlsym avcodec_alloc_frame 0x%x",avcodec_alloc_frame);

	if(!(avcodec_flush_buffers		= (AVCODEC_FLUSH_BUFFERS)dlsym(mModule,"avcodec_flush_buffers")))
	{
		ERROR("dlsym avcodec_flush_buffers error");
		goto LOAD_END;
	}

	DEBUG("dlsym avcodec_flush_buffers 0x%x",avcodec_flush_buffers);
	
	if(!(avcodec_close		= (AVCODEC_CLOSE)dlsym(mModule,"avcodec_close")))
	{
		ERROR("dlsym avcodec_close error");
		goto LOAD_END;
	}

	DEBUG("dlsym avcodec_close 0x%x",avcodec_close);


	if(!(av_audio_convert_alloc		= (AV_AUDIO_CONVERT_ALLOC)dlsym(mModule,"av_audio_convert_alloc")))
	{
		ERROR("dlsym av_audio_convert_alloc error");
		goto LOAD_END;
	}

	DEBUG("dlsym av_audio_convert_alloc 0x%x",av_audio_convert_alloc);

	if(!(av_get_sample_fmt_name		= (AV_GET_SAMPLE_FMT_NAME)dlsym(mModule,"av_get_sample_fmt_name")))
	{
		ERROR("dlsym av_get_sample_fmt_name error");
		goto LOAD_END;
	}

	DEBUG("dlsym av_get_sample_fmt_name 0x%x",av_get_sample_fmt_name);

	
	if(!(av_get_bits_per_sample_fmt		= (AV_GET_BITS_PERSAMPLE_FMT)dlsym(mModule,"av_get_bits_per_sample_fmt")))
	{
		ERROR("dlsym av_get_bits_per_sample_fmt error");
		goto LOAD_END;
	}

	DEBUG("dlsym av_get_bits_per_sample_fmt 0x%x",av_get_bits_per_sample_fmt);

	if(!(av_get_bytes_per_sample		= (AV_GET_BYTES_PER_SAMPLE)dlsym(mModule,"av_get_bytes_per_sample")))
	{
		ERROR("dlsym av_get_bytes_per_sample error");
		goto LOAD_END;
	}

	DEBUG("dlsym av_get_bytes_per_sample 0x%x",av_get_bits_per_sample_fmt);
	

	if(!(av_audio_convert		= (AV_AUDIO_CONVERT)dlsym(mModule,"av_audio_convert")))
	{
		ERROR("dlsym av_audio_convert error");
		goto LOAD_END;
	}

	DEBUG("dlsym av_audio_convert 0x%x",av_audio_convert);


	if(!(av_audio_convert_free		= (AV_AUDIO_CONVERT_FREE)dlsym(mModule,"av_audio_convert_free")))
	{
		ERROR("dlsym av_audio_convert_free error");
		goto LOAD_END;
	}

	DEBUG("dlsym av_audio_convert_free 0x%x",av_audio_convert_free);

	if(!(av_audio_resample_init		= (AV_AUDIO_RESAMPLE_INIT)dlsym(mModule,"av_audio_resample_init")))
	{
		ERROR("dlsym av_audio_resample_init error");
		goto LOAD_END;
	}

	DEBUG("dlsym av_audio_resample_init 0x%x",av_audio_resample_init);

	if(!(audio_resample_init		= (AUDIO_RESAMPLE_INIT)dlsym(mModule,"audio_resample_init")))
	{
		ERROR("dlsym audio_resample_init error");
		goto LOAD_END;
	}

	DEBUG("dlsym audio_resample_init 0x%x",audio_resample_init);

	if(!(audio_resample		= (AUDIO_RESAMPLE)dlsym(mModule,"audio_resample")))
	{
		ERROR("dlsym audio_resample error");
		goto LOAD_END;
	}

	DEBUG("dlsym audio_resample 0x%x",audio_resample);

	if(!(audio_resample_close		= (AUDIO_RESAMPLE_CLOSE)dlsym(mModule,"audio_resample_close")))
	{
		ERROR("dlsym audio_resample_close error");
		goto LOAD_END;
	}

	DEBUG("dlsym audio_resample_close 0x%x",audio_resample_close);

	if(!(avpicture_get_size		= (AVPICTURE_GET_SIZE)dlsym(mModule,"avpicture_get_size")))
	{
		ERROR("dlsym avpicture_get_size error");
		goto LOAD_END;
	}

	DEBUG("dlsym avpicture_get_size 0x%x",avpicture_get_size);

	if(!(avpicture_fill		= (AVPICTURE_FILL)dlsym(mModule,"avpicture_fill")))
	{
		ERROR("dlsym avpicture_fill error");
		goto LOAD_END;
	}

	DEBUG("dlsym avpicture_fill 0x%x",avpicture_fill);

	bOk = true;
LOAD_END:
	if(!bOk)
	{
		dlclose(mModule);
		mModule = NULL;
		return false;
	}
	return true;
}

void PlayCore::Reset()
{	
	av_log_set_callback = NULL;
	av_register_all		= NULL;
	av_malloc			= NULL;
	av_freep			= NULL;
	av_free				= NULL;
	av_gettime			= NULL;
	av_free_packet		= NULL;
	av_dup_packet		= NULL;
	av_rescale_q		= NULL;
	av_read_frame		= NULL;
	av_seek_frame		= NULL;

	av_open_input_file		= NULL ;
	av_register_protocol2 	= NULL;
	av_close_input_file		= NULL;
	av_find_stream_info		= NULL;
	url_set_interrupt_cb	= NULL;
	url_feof				= NULL;

	avcodec_find_decoder			= NULL;
	avcodec_open					= NULL;
	avcodec_default_get_buffer		= NULL;
	avcodec_default_release_buffer	= NULL;
	avcodec_decode_video			= NULL;
	avcodec_decode_audio2			= NULL;
	avcodec_alloc_frame				= NULL;
	avcodec_flush_buffers			= NULL;
	avcodec_close					= NULL;

	av_audio_convert_alloc		= NULL;
	av_get_sample_fmt_name		= NULL;
	av_get_bits_per_sample_fmt	= NULL;
	av_get_bytes_per_sample		= NULL;
	av_audio_convert			= NULL;
	av_audio_convert_free		= NULL;

	av_audio_resample_init	= NULL;
	audio_resample_init		= NULL;
	audio_resample			= NULL;
	audio_resample_close	= NULL;

	avpicture_get_size	= NULL;
	avpicture_fill		= NULL;
}

void  PlayCore::UnLoadPlugin()
{
	if(mModule)
	{
		INFO("UnLoadPlugin 0x%x",mModule);
		dlclose(mModule);
		mModule = NULL;
	}
}
