#ifndef __ANDROID_LIVE_PLAYER_H__
#define __ANDROID_LIVE_PLAYER_H__

#if PLATFORM < 8
#include <ui/Surface.h>
#elif PLATFORM>=16
#include <gui/Surface.h>
#else
#include <surfaceflinger/Surface.h>
#endif
#include "baseplayer.h"

class CBasePlayerListener;
class IVideoRender;
class IAudioRender;
class CAndroidAudioStubRender;
class CAndroidVideoStubRender;
class CAndroidLivePlayer : public CBasePlayer
{
public:
    CAndroidLivePlayer (string strPackageName);
    ~CAndroidLivePlayer ();
    void SetListener(CBasePlayerListener* Listener);
#if PLATFORM <= 17
    int  SetVideoSurface (const android::sp<android::Surface>& surface);
#else
	int  SetVideoSurface (void* surface);
#endif
	int  SetDisplayType(int DType);
    int  SetDataSource (const char* filepath,int type);
    int  Start ();
    int  Stop ();
    int  Pause ();
    int  Seek (double sec);
    int  Release ();
    int  Reset ();
    int  SetVolume (float left, float right);
    int  GetVideoWidth ();
    int  GetVideoHeight ();
    bool 	IsPlaying ();
    double 	GetCurrentPosition ();
    double 	GetDuration ();
	bool 	IsSeekable();
	bool	IsCanPause();
	int     SetLogLevel(int level);
protected:
	bool	SetupVideoOutPut();
	void	ReSendVideo();
	bool	SetupAudioOutPut();
private:
    IVideoRender* 				mpVideoRender;
    IAudioRender* 				mpAudioRender;
    CAndroidAudioStubRender* 	mpAudioStubRender;
    CAndroidVideoStubRender* 	mpVideoStubRender;
    int miVideoWidth;
    int miVideoHeight;
};

#endif
