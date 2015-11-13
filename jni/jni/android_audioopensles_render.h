#ifndef __ANDROID_AUDIOOPENSLES_RENDER_H__
#define __ANDROID_AUDIOOPENSLES_RENDER_H__

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "util_lock.h"
#include "baseaudiorender.h"
#include "iaudiorender.h"


class CMasterClock;
class CAndroidAudioOpenSLESRender : public IAudioRender,
									public CBaseAudioRender
{

public:
    CAndroidAudioOpenSLESRender (CMasterClock* Clock);
    ~CAndroidAudioOpenSLESRender ();

    int      Open (AudioFormat audioFormat);
	bool	 IsOpened();
	ARState	 GetState();
    void     Start ();
    void     Stop ();
    void     Flush ();
    void     Pause ();
    void     Close ();
    int      Write (const AudioSample& audioSample);
	int 	 Reset ();
	void	 IsRenderFinish(int* IsFinish);
	void	 SetVolume (float left, float right);
protected:
	static void CallbackWrapper(SLAndroidSimpleBufferQueueItf bq, void *User);
private:
	SLObjectItf 					mEngineObject;
	SLEngineItf 					mEngineEngine;
	
	// output mix interfaces
	SLObjectItf 					mOutputMixObject;
	SLEnvironmentalReverbItf 		mOutputMixERb;
	
	// buffer queue player interfaces
	SLObjectItf 					mPlayerObject;
	SLPlayItf 						mPlayerPlay;
	SLAndroidSimpleBufferQueueItf 	mPlayerBufferQueue;
	SLMuteSoloItf 					mPlayerMuteSolo;
	SLVolumeItf 					mPlayerVolume;

	ARState		  					mState;
	bool							mbFlushed;
	unsigned char					mSliceBuf[BUFFER_SIZE];
};



#endif

