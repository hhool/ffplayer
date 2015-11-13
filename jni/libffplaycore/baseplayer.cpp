#include "baseplayer.h"
#include "playcore.h"
#include "util_log.h"
#include "util_uri.h"

CBasePlayer::CBasePlayer (string strPackageName):mOperationLock("CBasePlayer"),mTimeShiftManage(this)
{
	PlayCore::mstrPackageName = strPackageName;
	PlayCore::GetInstance()->av_register_all();
	PlayCore::GetInstance()->av_log_set_callback(CBasePlayer::AVLogCallBack);

    mpDemux 	= new CDemux (this);
    mpClock 	= new CMasterClock (CMasterClock::CLOCK_AUDIO);
    mpVDecoder 	= new CVideoDecoder ();
    mpADecoder 	= new CAudioDecoder ();
    mpVideoStubRender 	= NULL;
    mpAudioStubRender 	= NULL;

    mfDuration 	= INVALID_TIME_VALUE;
    mfStartTime = 0.0f;
	mfTimePos   = 0.0f;
    mListener 	= NULL;
	mThreadID	= pthread_self();
	mbStop		= false;
	mbNotifyMsg = true;

	mOutAudioFormat.SampleRate		= 44100;
	mOutAudioFormat.Channels		= 2;
	mOutAudioFormat.Bits			= 16;

	mPlayMode						= MEDIA_PLAY_MODE_NOP;
	INFO("0x%x",this);
}

CBasePlayer::~CBasePlayer ()
{
	INFO("0x%x",this);

	if (mpADecoder)
	{
		delete mpADecoder;
		mpADecoder = NULL;
	}
	if (mpVDecoder)
	{
		delete mpVDecoder;
		mpVDecoder = NULL;
	}
	if (mpClock)
	{
		delete mpClock;
		mpClock = NULL;
	}
	if (mpDemux)
	{
		delete mpDemux;
		mpDemux = NULL;
	}
	PlayCore::DestroyInstace();
}

static void log_format_line(void *ptr, int level, const char *fmt, va_list vl,
                        char *line, int line_size, int *print_prefix)
{
    AVClass* avc = ptr ? *(AVClass **) ptr : NULL;
    line[0] = 0;
    if (*print_prefix && avc) 
	{
        if (avc->parent_log_context_offset) 
		{
            AVClass** parent = *(AVClass ***) (((uint8_t *) ptr) +
                                   avc->parent_log_context_offset);
            if (parent && *parent) 
			{
                snprintf(line, line_size, "[%s @ %p] ",
                         (*parent)->item_name(parent), parent);
            }
        }
        snprintf(line + strlen(line), line_size - strlen(line), "[%s @ %p] ",
                 avc->item_name(ptr), ptr);
    }

    vsnprintf(line + strlen(line), line_size - strlen(line), fmt, vl);

    *print_prefix = strlen(line) && line[strlen(line) - 1] == '\n';
}


static void sanitize(uint8_t *line)
{
    while(*line)
	{
        if(*line < 0x08 || (*line > 0x0D && *line < 0x20))
            *line='?';
        line++;
    }
}

void CBasePlayer::AVLogCallBack(void* ptr, int level, const char* fmt, va_list vl)
{
	static int print_prefix = 1;
    static int count;
    static char prev[1024];
    char line[1024];

    log_format_line(ptr, level, fmt, vl, line, sizeof(line), &print_prefix);

    if (print_prefix && !strcmp(line, prev))
	{
        count++;
        return;
    }
    if (count > 0) 
	{
        //INFO("Last message repeated %d times\n", count);
        count = 0;
    }
    strcpy(prev, line);
    sanitize((uint8_t*)line);
   // INFO("%s",line);
}

int CBasePlayer::SetCoreLogLevel(int level)
{
	if(!(level>=CLL_EMERG && level<=CLL_NOTSET))
		return ERR_INVALID_PARAM;

	_LogSetLevel((eUtilLogLevel)level,1);

	return ERR_NONE;
}

int CBasePlayer::SetListener(CBasePlayerListener* listener)
{
    DEBUG("setListener 0x%x",listener);

	if(mListener)
	{
		delete mListener;
		mListener = NULL;
	}

	mListener = listener;

	return ERR_NONE;
}
int CBasePlayer::AttachThread(UtilThread* pThread)
{
	if (mListener == NULL)
		return ERR_INVALID_DATA;


	int TID = pthread_self();
	
	bool bFind = false;
	
	for (size_t i = 0; i < mThreadObjectList.size(); ++ i)
	{
		if(mThreadObjectList[i].mThreadID == TID)
		{
			bFind = true;
			break;
		}
	}

	if(!bFind)
	{
		CThreadObject ThreadObject;

		ThreadObject.mThreadID  = TID;
		ThreadObject.mpThread	= pThread;
		mThreadObjectList.push_back(ThreadObject);
	}

	return mListener->AttachThread();
}
void   CBasePlayer::DetachThread()
{
	if (mListener == NULL)
		return ;

	int TID = pthread_self();

	for (size_t i = 0; i < mThreadObjectList.size(); ++ i)
	{
		if(mThreadObjectList[i].mThreadID == TID)
		{
			mThreadObjectList.erase(mThreadObjectList.begin()+i);
			break;
		}
	}

	mListener->DetachThread();
}

UtilThread* CBasePlayer::GetThreadById(int ThreadId)
{
	if(ThreadId == 0)
	{
		ThreadId = pthread_self();
	}
	
	for (size_t i = 0; i < mThreadObjectList.size(); ++ i)
	{
		if(mThreadObjectList[i].mThreadID == ThreadId)
		{
			return (UtilThread*)mThreadObjectList[i].mpThread;
		}
	}

	
	return NULL;
}

void CBasePlayer::Notify(int msg, int ext1, int ext2)
{			
    if (mListener != NULL && mbNotifyMsg)
	{	   
       	mListener->Notify(msg, ext1, ext2);	   
	}
}

bool CBasePlayer::IsNeedQuit()
{
	if(mbStop)
	{
		INFO("this 0x%x , mbQuit %d",this,mbStop);
	}
	return mbStop;
}

void CBasePlayer::ThreadEntry ()
{
    DEBUG ("ThreadEntry Enter");

	AttachThread(this);

	Notify(MEDIA_OPENING,0,0);
		
	int ret = _SetDataSource(mURL.c_str());
		
	if(ret < 0)
	{
		Notify(MEDIA_ERROR,MEDIA_OPENING,0);
	}
	else
	{	
		SetupVideoOutPut();
		SetupAudioOutPut();

		Notify(MEDIA_OPENED,0,0);
	}

    DEBUG ("ThreadEntry DetachThread Leave");

	DetachThread();
	
    DEBUG ("ThreadEntry Leave");
}
int CBasePlayer::_SetDataSource (const char* filename)
{
	DEBUG ("Demux->SetDataSource()");
	CDemux* Demux = mpDemux;
	
	Demux->SetDataSource (filename);
	AVFormatContext* formatCtx = Demux->GetFormatContext ();
	if (formatCtx == NULL)
	{
		ERROR ("formatCtx == NULL");
		return ERR_NOT_SUPPORTED;
	}
	
	DEBUG ("reset clock!");
	mpClock->Reset ();

	// get duration & start_time
	double duration = formatCtx->duration / AV_TIME_BASE;
	double start_time = formatCtx->start_time / AV_TIME_BASE;
	if(start_time < 0)
		start_time = 0.0;
	DEBUG ("duration=%f, start_time=%f", duration, start_time);
	mfDuration = duration;
	if(mfDuration<0.0f)
		mfDuration = INVALID_TIME_VALUE;
	
	mfStartTime = start_time;
	int vid = Demux->GetVideoStreamIndex ();
	int aid = Demux->GetAudioStreamIndex ();

	// create video play thread to play video.
	if (vid >= 0)
	{
		mpVDecoder->SetDemux (Demux);
		AVStream* stream = NULL;
		if (formatCtx != NULL && vid >= 0)
		{
			stream = formatCtx->streams[vid];
			DEBUG ("video: time_base:den=%d, num=%d", stream->time_base.den, stream->time_base.num);
		}
		mpVDecoder->SetStream (stream);
		mpVDecoder->SetMasterClock (mpClock);
		mpClock->SetClockType(CMasterClock::CLOCK_VIDEO);
		Demux->SetVideoOutPlay(mpVDecoder);
	}

	// create audio play thread to play audio.
	if (aid >= 0)
	{
		mpADecoder->SetDemux (Demux);
		AVStream* stream = NULL;
		if (formatCtx != NULL && aid >= 0)
		{
			stream = formatCtx->streams[aid];
			DEBUG ("audio: time_base:den=%d, num=%d", stream->time_base.den, stream->time_base.num);
		}
		mpADecoder->SetStream (stream);
		mpADecoder->SetMasterClock (mpClock);
		mpADecoder->SetOutputFormat(mOutAudioFormat.SampleRate,mOutAudioFormat.Channels,mOutAudioFormat.Bits);
		mpClock->SetClockType(CMasterClock::CLOCK_AUDIO);
		Demux->SetAudioOutPlay(mpADecoder);
	}

	Demux->SetMasterClock(mpClock);
	
	return ERR_NONE;
	
}
#define _tolower(c) ((c)+'a'-'A')  //定义成宏  
  
static char *mystrlwr(char *s)  
{  
    char *str;  
    str = s;  
    while(*str != '/0' && *str != 0)  
    {  
        if(*str > 'A' && *str < 'Z')
		{  
           _tolower(*str);  //使用宏实现  
        }  
        str++;  
    }  
    return s;  
}  

int CBasePlayer::SetDataSource (const char* filename,int type)
{
	if (mpDemux == NULL)
	{
		ERROR ("mpDemux == NULL!");
		return ERR_INVALID_DATA;
	}
	if (filename == NULL)
	{
		ERROR ("filename == NULL!");
		return ERR_INVALID_PARAM;
	}
	
	mbNotifyMsg = true;
	mURL = filename;
	mPlayMode = (media_play_mode_type)type;
	INFO("mURL %s mPlayMode %d",filename,mPlayMode);
	
	if(mPlayMode == MEDIA_PLAY_MODE_LIVE)
	{
		mTimeShiftManage.SetDataSource(mURL.c_str());
	}

	ThreadExec ();
		
	return ERR_NONE;
}

void CBasePlayer::ReSendVideo()
{
	if(mpVDecoder)
		mpVDecoder->ReDrawPictrue();
}

AVStream* CBasePlayer::GetVideoStream ()
{
	if (mpDemux == NULL)
	{
		ERROR ("mpDemux == NULL");
		return NULL;
	}

	AVFormatContext* formatCtx = mpDemux->GetFormatContext ();
	if (formatCtx == NULL)
	{
		ERROR ("formatCtx == NULL");
		return NULL;
	}

	int vid = mpDemux->GetVideoStreamIndex ();
	if (vid >= 0)
	{
		return formatCtx->streams[vid];
	}
	return NULL;
}

AVStream* CBasePlayer::GetAudioStream ()
{
	if (mpDemux == NULL)
	{
		ERROR ("mpDemux == NULL");
		return NULL;
	}

	AVFormatContext* formatCtx = mpDemux->GetFormatContext ();
	if (formatCtx == NULL)
	{
		ERROR ("formatCtx == NULL");
		return NULL;
	}

	int aid = mpDemux->GetAudioStreamIndex ();
	if (aid >= 0)
	{
		return formatCtx->streams[aid];
	}
	return NULL;
}

int CBasePlayer::SetStubRender(CVideoStubRender* VideoStubRender, CAudioStubRender* AudioStubRender)
{
	mpVideoStubRender = VideoStubRender;
	mpAudioStubRender = AudioStubRender;

	if (mpVDecoder) 
	{
		mpVDecoder->SetStubRender (mpVideoStubRender);
	}

	if (mpADecoder) 
	{
		mpADecoder->SetStubRender (mpAudioStubRender);
	}
	
	return ERR_NONE;
}

int CBasePlayer::Start ()
{
	//Pause to Play
	int Ret = ERR_NONE;
	
	if(mpDemux->ThreadIsRunning() && !mpClock->IsRun())
	{
		Ret = Pause(false);
	}
	else//Stop or new file opened state to play
	{
		DEBUG ("reset clock");
		if (mpClock) 
		{
			mpClock->Reset ();
		}
		DEBUG ("start Demux");
		if (mpDemux) 
		{
			mpDemux->Start ();
		}
		DEBUG ("start vplay");
		if (mpVDecoder) 
		{
			mpVDecoder->Start ();
		}
		DEBUG ("start aplay");
		if (mpADecoder)
		{	
			mpADecoder->Start ();
		}
		DEBUG ("after start all!");
	}
	return Ret;
}

int CBasePlayer::Stop ()
{	

	mbNotifyMsg = false;

	// stop decorder, will unblocking video & audio PacketList.
	
	mbStop = true;

	if (mpDemux) 
	{
		mpDemux->Interrupt();
	}
	
	INFO ("stop vplay");
	if (mpVDecoder) 
	{
		mpVDecoder->Stop ();
	}
	
	INFO ("stop aplay");
	if (mpADecoder) 
	{
		mpADecoder->Stop ();
	}


	DEBUG ("begin wait thread!");
	ThreadWait (); //wait for Opened Thread finish if it working now
    DEBUG ("end wait thread!");
	
	INFO ("stop Demux");
	if (mpDemux) 
	{
		mpDemux->Stop ();
	}
	
	DEBUG ("stoped all!");
	return ERR_NONE;
}
int CBasePlayer::Reset()
{
	INFO("");

	if(mTimeShiftManage.IsRun())
	{
		mTimeShiftManage.Stop();
	}
	
	Stop();

	if (mpDemux) 
	{
		mpDemux->Reset();
	}

	return ERR_NONE;
}

bool CBasePlayer::IsTShiftRun()
{
	return mTimeShiftManage.IsRun();
}

media_play_mode_type  CBasePlayer::GetPlayMode()
{
	return mPlayMode;
}

int CBasePlayer::Pause (bool bPause)
{	
	if (!mpDemux->ThreadIsRunning())
	{
		INFO ("!mpDemux->ThreadIsRunning()");
		return ERR_INVALID_DATA;
	}
	
	if (bPause && IsPlaying())
	{		
		INFO ("CBasePlayer::Pause Pause");
		mpDemux->Pause(true);
		
		DEBUG ("stop clock");
		mpClock->Stop ();

		if(mPlayMode == MEDIA_PLAY_MODE_LIVE && mTimeShiftManage.IsAlowTimeShift()&&!mTimeShiftManage.IsRun())
		{			
			if(mTimeShiftManage.Start() == ERR_NONE)
			{
				INFO("Record start");
				return ERR_NONE;
			}
			else
			{
				return ERR_NOT_SUPPORTED;
			}
		}

		return ERR_NONE;
	}
	else if(!bPause && !IsPlaying())
	{
		INFO ("CBasePlayer::Pause Resume ");
		
		if(mPlayMode == MEDIA_PLAY_MODE_LIVE && mTimeShiftManage.IsAlowTimeShift() && mTimeShiftManage.IsDataAvailable())
		{
			Stop();

			mpDemux->Reset();

			INFO("PLAY TIMESHIFT %s",mTimeShiftManage.GetTimeShiftURL());

			string TimeShiftURL = "dyna:";
			TimeShiftURL.append(mTimeShiftManage.GetTimeShiftURL());
			
			if(SetDataSource(TimeShiftURL,MEDIA_PLAY_MODE_TIMESHIFT)!= ERR_NONE)
			{
				ERROR("PLAY TIMESHIFT ERROR %s",mTimeShiftManage.GetTimeShiftURL());

				Notify(MEDIA_TIMESHIFT,TSHIFT_PLAYER_OPEN_DST_ERR,0);
					
				return ERR_DEVICE_ERROR;
			}
			
			return ERR_NONE;
		}	
		else
		{
			if(mPlayMode == MEDIA_PLAY_MODE_TIMESHIFT)
			{
				INFO("timeshift play mode");
			}
			else
			{
				INFO("normal Play");
				if(mTimeShiftManage.IsRun())
					mTimeShiftManage.Stop();

				mTimeShiftManage.Reset();
			}
			
			mpDemux->Pause(false);
			
			mpClock->Start ();

			return ERR_NONE;
		}
	}
	
	return ERR_NOT_SUPPORTED;
}

int  CBasePlayer::SetTimeShiftInfo(TimeShiftInfo TShiftInfo)
{
	mTimeShiftManage.SetTimeShiftInfo(TShiftInfo);
	return ERR_NONE;
}

int CBasePlayer::Seek (double sec)
{
	int ret = 0;
	
	mOperationLock.Lock();

	DEBUG("mfDuration %f,sec %f",mfDuration,sec);
	if(mfDuration<0.0f)
	{
		mOperationLock.Unlock();
		return ERR_INVALID_PARAM;
	}
	if(sec >= mfDuration)
	{	
		sec = mfDuration-0.5f;
	}
	if(sec < 0.0f)
	{
		sec = 0.0f;
	}
	DEBUG("mfDuration modify %f,sec %f",mfDuration,sec);
	
	DEBUG ("stop vplay");
	if (mpVDecoder)
	{
		mpVDecoder->Stop ();
	}

	DEBUG ("stop aplay");
	if (mpADecoder)
	{
		mpADecoder->Stop ();
	}

	if (mpClock)
	{
		mpClock->Reset ();
	}

	if (mpDemux)
		ret = mpDemux->Seek (sec);

	if(ret >= 0)
	{
		DEBUG ("start vplay");
		if (mpVDecoder)
			mpVDecoder->Start ();

		DEBUG ("start aplay");
		if (mpADecoder)
			mpADecoder->Start ();
	}

	mfTimePos = sec;

	DEBUG("SEEK mfTimePos %f",mfTimePos);
	
	mOperationLock.Unlock();
	
	return ERR_NONE;
}

double CBasePlayer::GetDuration ()
{
	AVFormatContext* FormatContext = mpDemux->GetFormatContext ();
	if (FormatContext == NULL)
	{
		ERROR ("FormatContext == NULL");
		return ERR_NOT_SUPPORTED;
	}
	
	double duration = FormatContext->duration / AV_TIME_BASE;
	double start_time = FormatContext->start_time / AV_TIME_BASE;
	
	if(start_time < 0)
		start_time = 0.0;
	DEBUG ("duration=%f, start_time=%f", duration, start_time);
	mfDuration = duration;
	if(mfDuration<0.0f)
		mfDuration = INVALID_TIME_VALUE;

	return mfDuration;
}

double CBasePlayer::GetPlayingTime ()
{
	double curr = 0.0;
	double time = 0.0;

	mOperationLock.Lock();

	if (mpClock&&mpDemux&&mpDemux->IsSynced())
	{
		curr = mpClock->GetCurrentClock ();
		if((curr - mfStartTime) >= 0.0)
		{
			mfTimePos = curr - mfStartTime;
			DEBUG("curr %f time %f mfStartTime %f",curr,mfTimePos,mfStartTime);
		}
	}
	else
	{
		DEBUG("NOT SYNC mfTimePos %f",mfTimePos);
	}
	
	mOperationLock.Unlock();

	return mfTimePos;
}
bool CBasePlayer::IsPlaying ()
{
	if(mpDemux)
	{
		return !(mpDemux->IsPaused());
	}

	return false;
}

bool CBasePlayer::IsSeekable()
{
	if(mpDemux)
	{
		return mpDemux->IsSeekable();
	}

	return false;
}

bool CBasePlayer::IsCanPause()
{
	if(mpDemux)
	{
		return mpDemux->IsCanPause();
	}

	return false;
}


