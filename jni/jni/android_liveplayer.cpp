#include <common.h>
#if PLATFORM < 9
#include "android_audiotrack_render.h"
#else
#include "android_audioopensles_render.h"
#endif
#if PLATFORM <=17
#include "android_videosurface_render.h"
#else
#include "android_nativewindow_render.h"
#endif
#include "android_liveplayer.h"
#include "android_stub_render.h"
#include "util_log.h"

CAndroidLivePlayer::CAndroidLivePlayer (string strPackageName):CBasePlayer(strPackageName)
{  
    // init android environment
    IVideoRender* VideoRender = new CAndroidVideoSurfaceRender ();

    if (VideoRender == NULL) 
	{
        ERROR ("warning! create CAndroidVideoSurfaceRender fail! no video output!");
    }
#if PLATFORM < 9
	IAudioRender* AudioRender = new CAndroidAudioTrackRender (mpClock);
    if (AudioRender == NULL) 
	{
        ERROR ("warning! create CAndroidAudioTrackRender fail! no sound output!");
    }
#else
	IAudioRender* AudioRender = new CAndroidAudioOpenSLESRender (mpClock);
    if (AudioRender == NULL) 
	{
        ERROR ("warning! create CAndroidAudioTrackRender fail! no sound output!");
    }
#endif

    // create video outer & audio outer to play sound & show picture.
    CAndroidAudioStubRender* AudioStubRender = new CAndroidAudioStubRender (AudioRender);
    if (AudioStubRender == NULL) 
	{
        ERROR ("warning! create CAndroidAudioStubRender fail! no sound output!");
    }
    CAndroidVideoStubRender* VideoStubRender = new CAndroidVideoStubRender (VideoRender);
    if (VideoStubRender == NULL) 
	{
        ERROR ("warning! create CAndroidVideoStubRender fail! no sound output!");
    }
    SetStubRender(VideoStubRender,AudioStubRender);

    mpVideoRender = VideoRender;
    mpAudioRender = AudioRender;
    mpAudioStubRender = AudioStubRender;
    mpVideoStubRender = VideoStubRender;

    miVideoWidth = -1;
    miVideoHeight = -1;
	INFO("0x%x",this);
}

CAndroidLivePlayer::~CAndroidLivePlayer ()
{
	INFO("0x%x",this);
	Release ();
}

int CAndroidLivePlayer::SetLogLevel(int level)
{
	if(!(level>=CLL_EMERG && level<=CLL_NOTSET))
		return -1;

	_LogSetLevel((eUtilLogLevel)level,1);

	CBasePlayer::SetCoreLogLevel(level);
	
	return ERR_NONE;
}

void CAndroidLivePlayer::SetListener(CBasePlayerListener* Listener)
{
	CBasePlayer::SetListener(Listener);
}

#if PLATFORM <= 17

int CAndroidLivePlayer::SetVideoSurface (const android::sp<android::Surface>& surface)
{
	int Ret = ERR_INVALID_DATA;
	
    if (!!mpVideoRender)
	{
        Ret = ((CAndroidVideoSurfaceRender*)mpVideoRender)->SetSurface (surface);

		CBasePlayer::ReSendVideo();
    }
	else
	{
		ERROR ("mpVideoRender == NULL!");
        Ret =  ERR_INVALID_DATA;
	}
	
	return Ret;
}
#else

int  CAndroidLivePlayer::SetVideoSurface (void* surface)
{
	int Ret = ERR_INVALID_DATA;
	
    if (!!mpVideoRender)
	{
        Ret = ((CAndroidVideoSurfaceRender*)mpVideoRender)->SetSurface (surface);

		CBasePlayer::ReSendVideo();
    }
	else
	{
		ERROR ("mpVideoRender == NULL!");
        Ret =  ERR_INVALID_DATA;
	}
	
	return Ret;
}

#endif
int CAndroidLivePlayer::SetDisplayType(int DType)
{
	int Ret = ERR_INVALID_DATA;
	
	if (!!mpVideoRender)
	{
        mpVideoRender->SetDisplayType ((DisplayType)DType);
		CBasePlayer::ReSendVideo();
    } 
	else 
	{
        ERROR ("mpVideoRender == NULL!");
        Ret =  ERR_INVALID_DATA;
    }
	
	return Ret;
}

int CAndroidLivePlayer::SetDataSource (const char* filepath,int type)
{
    miVideoWidth = -1;
    miVideoHeight = -1;

	if(filepath == NULL || strlen(filepath) <= 0)
		return -1;
	
    DEBUG ("SetDataSource filepath 0x%x,= %s,type %d",filepath,filepath,type);

    return CBasePlayer::SetDataSource (filepath,type);
}

int CAndroidLivePlayer::Start ()
{
    DEBUG ("start player!");
    return CBasePlayer::Start ();
}

int CAndroidLivePlayer::Stop ()
{
    DEBUG ("stop player!");
    return CBasePlayer::Stop ();
}

int CAndroidLivePlayer::Pause ()
{
    DEBUG ("pause player!");
    return CBasePlayer::Pause (true);
}

int CAndroidLivePlayer::Seek (double sec)
{
    DEBUG ("seek player!");
    return CBasePlayer::Seek (sec);
}

int CAndroidLivePlayer::Release ()
{
	if (mpAudioStubRender)
		delete mpAudioStubRender;
	mpAudioStubRender = NULL;

	if (mpVideoStubRender)
		delete mpVideoStubRender;
	mpVideoStubRender = NULL;

	if (mpVideoRender)
	{
		mpVideoRender->Reset();
		delete (CAndroidVideoSurfaceRender*)mpVideoRender;
	}
	mpVideoRender = NULL;

	if (mpAudioRender)
	{
		if(mpAudioRender->IsOpened())
		{
			mpAudioRender->Close();
		}
#if PLATFORM < 9
		delete (CAndroidAudioTrackRender*)mpAudioRender;
#else
		delete (CAndroidAudioOpenSLESRender*)mpAudioRender;
#endif
	}
	mpAudioRender = NULL;

    return ERR_NONE;
}

int CAndroidLivePlayer::Reset ()
{
	DEBUG("_reset");

   	CBasePlayer::Reset();
   
    INFO("mpVideoRender 0x%x,mpVideoRender->Reset ()",mpVideoRender);
    if (mpVideoRender)
    {
        mpVideoRender->Reset ();
    }
    INFO("mpAudioRender 0x%x,mpAudioRender->Reset ()",mpAudioRender);
    if (mpAudioRender)
    {
        mpAudioRender->Reset ();
    }
    DEBUG("_reset finish");
    return ERR_NONE;
}

bool CAndroidLivePlayer::SetupVideoOutPut()
{
	// set up Video output!
    AVStream* VStream = GetVideoStream ();

	if(!VStream || !mpVideoRender)
	{
		ERROR ("error! VStream == 0x%x,mpVideoRender == 0x%x",VStream,mpVideoRender);
		return false;
	}
		
	if(!mpVideoRender->IsInit())
	{

		AVCodecContext *codecCtx = VStream->codec;

		if (codecCtx != NULL)
		{
			miVideoWidth = codecCtx->width;
			miVideoHeight = codecCtx->height;

			if (mpVideoRender->Init(VStream) != ERR_NONE)
			{
				ERROR ("Init fail!");
				return false;
			}
		}
		else
		{
			ERROR ("error! codecCtx == NULL!");
			return false;
		}
	}
	
    return true; 
}

bool CAndroidLivePlayer::SetupAudioOutPut()
{
	//set up Audio output
    AVStream* AStream = GetAudioStream ();

	if(!AStream || !mpAudioRender)
	{
		ERROR ("error! AStream == 0x%x,mpAudioRender == 0x%x",AStream,mpAudioRender);
		return false;
	}

	if(!mpAudioRender->IsOpened())
	{
		DEBUG("sample_rate %d,chnnels %d sample_fmt %d",AStream->codec->sample_rate,
				AStream->codec->channels,AStream->codec->sample_fmt);
		//音频需要根据设备支持的输出参数转换 av_audio_convert_alloc 等ffmpeg函数实现
		if(mpAudioRender->Open(mOutAudioFormat)!=ERR_NONE)
		{
			ERROR ("open fail!");
			return false;
		}	
	}	
    return true;
}

int CAndroidLivePlayer::SetVolume (float left, float right)
{
    return ERR_NONE;
}

int CAndroidLivePlayer::GetVideoWidth ()
{
    return miVideoWidth;
}

int CAndroidLivePlayer::GetVideoHeight ()
{
    return miVideoHeight;
}

bool CAndroidLivePlayer::IsPlaying ()
{
    return CBasePlayer::IsPlaying ();
}

double CAndroidLivePlayer::GetCurrentPosition ()
{
    return CBasePlayer::GetPlayingTime ();
}

double CAndroidLivePlayer::GetDuration ()
{
   return CBasePlayer::GetDuration ();
}

bool CAndroidLivePlayer::IsSeekable()
{	
	return CBasePlayer::IsSeekable ();
}

bool CAndroidLivePlayer::IsCanPause()
{	
	return CBasePlayer::IsSeekable ();
}


