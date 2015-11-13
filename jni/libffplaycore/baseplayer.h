#ifndef __BASE_PLAYER_H__
#define __BASE_PLAYER_H__

#include "common.h"
#include "vdecoder.h"
#include "adecoder.h"
#include "demux.h"
#include "timeshift.h"


enum media_event_type {
    MEDIA_NOP               = 0, // interface test message
    MEDIA_OPENING			= 1,
    MEDIA_OPENED			= 2,
    MEDIA_SYNCING			= 3,
    MEDIA_SYNC				= 4,
    MEDIA_BUFFERING			= 5,
    MEDIA_PLAYBACK_COMPLETE = 6,
    MEDIA_SEEK_COMPLETE     = 7,
    MEDIA_PLAYING			= 8,
    MEDIA_PAUSED			= 9,
    MEDIA_TIMESHIFT			= 10,
    MEDIA_ERROR             = 100,
    MEDIA_INFO              = 200,
};

typedef enum media_play_mode_type
{
	MEDIA_PLAY_MODE_NOP			= 0x0,
	MEDIA_PLAY_MODE_LOCAL 		= 0x1,
	MEDIA_PLAY_MODE_LIVE		= 0x2,			
	MEDIA_PLAY_MODE_TIMESHIFT	= 0x3,
	MEDIA_PLAY_MODE_ONDEMAND	= 0x4
}media_play_mode_type;
	

// ----------------------------------------------------------------------------
// ref-counted object for callbacks
class CBasePlayerListener
{
public:
	virtual ~CBasePlayerListener (){INFO("0x%x",this);};
	virtual int  AttachThread() = 0;
	virtual void DetachThread() = 0;
    virtual void Notify(int msg, int ext1, int ext2) = 0;
};

class CThreadObject
{
public:
	void* 	mpThread;
	int     mThreadID;
};

class CBasePlayer : public UtilThread
{
protected:
	list<CThreadObject> mThreadObjectList;
	
    CDemux* 		mpDemux;
    CMasterClock* 	mpClock;
    CVideoDecoder* 	mpVDecoder;
    CAudioDecoder*	mpADecoder;
    CVideoStubRender* 	mpVideoStubRender;
    CAudioStubRender* 	mpAudioStubRender;
    double 			mfDuration;
    double 			mfStartTime;
	double 			mfTimePos;
    UtilSingleLock  mOperationLock;
	int 			mThreadID;
	string			mURL;
	media_play_mode_type	mPlayMode;
	bool			mbStop;
	bool			mbNotifyMsg;

    CBasePlayerListener* mListener;
public:
    CBasePlayer (string strPackageName);
    virtual ~CBasePlayer ();

    int SetListener(CBasePlayerListener* listener);
    int SetDataSource (const char* filename,int type);
	int Reset();
    AVStream* GetVideoStream ();
    AVStream* GetAudioStream ();

    int SetStubRender (CVideoStubRender* VideoStubRender, CAudioStubRender* AudioStubRender);

    int Start ();
    int Stop ();
    int Pause (bool bPause);
    int Seek (double sec);

    double GetDuration ();
    double GetPlayingTime ();
    bool   IsPlaying ();
	bool   IsSeekable();
	bool   IsCanPause();
	int    AttachThread(UtilThread* pThread);
	void   DetachThread();
	UtilThread* GetThreadById(int ThreadId=0);
	bool   IsNeedQuit();
    void   Notify(int msg, int ext1, int ext2);
    void   ThreadEntry ();
	int    SetCoreLogLevel(int level);

	int   					SetTimeShiftInfo(TimeShiftInfo TShiftInfo);
	bool   					IsTShiftRun();
	media_play_mode_type	GetPlayMode();
protected:
	int 	_SetDataSource(const char* filename);
	void	ReSendVideo();
	virtual bool	SetupVideoOutPut()=0;
	virtual bool	SetupAudioOutPut()=0;
	static  void    AVLogCallBack(void* ptr, int level, const char* fmt, va_list vl);
protected:
	AudioFormat 		mOutAudioFormat;
protected:
	CTimeShiftManage	mTimeShiftManage;
};

#endif
