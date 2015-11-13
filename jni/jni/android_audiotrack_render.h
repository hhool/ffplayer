#ifndef __ANDROID_AUDIOTRACK_RENDER_H__
#define __ANDROID_AUDIOTRACK_RENDER_H__

#include <stdint.h>
#include <sys/types.h>
#include <media/AudioTrack.h>
#include "util_lock.h"
#include "baseaudiorender.h"
#include "iaudiorender.h"


class CMasterClock;
class CAndroidAudioTrackRender : public IAudioRender,
								 public CBaseAudioRender
{

public:
    CAndroidAudioTrackRender (CMasterClock* Clock);
    ~CAndroidAudioTrackRender ();

    int      Open (AudioFormat audioFormat);
	bool	 IsOpened();
	ARState	 GetState();
    void     Start ();
    void     Stop ();
    void     Flush ();
    void     Pause ();
    void     Close ();
    int      Write (const AudioSample& SrcAudioSample);
	int 	 Reset ();
	void	 IsRenderFinish(int* IsFinish);
	void	 SetVolume (float left, float right);
	
private:
    static void CallbackWrapper(int event, void *me, void *info);

    android::AudioTrack*   mTrack;
    int           mStreamType;
    float         mLeftVolume;
    float         mRightVolume;
    float         mMsecsPerFrame;
    uint32_t      mLatency;
	ARState		  mState;

	int			  mMinFrameCount;	
    uint32_t      mNumFramesWritten;
};

#endif
