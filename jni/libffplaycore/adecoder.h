#ifndef __AUDIO_OUT_H__
#define __AUDIO_OUT_H__

#include "common.h"
#include "basedecoder.h"

class CAudioStubRender : public CStubRender
{
public:
    virtual ~CAudioStubRender () {}
	
public:
	virtual int  Start() 	= 0;
	virtual int  Flush() 	= 0;
	virtual int  Stop() 	= 0;
	virtual int	 Pause()	= 0;
	virtual bool IsStart() 	= 0;
    virtual int  Write (const AudioSample& SrcAudioSample) = 0;
	virtual bool IsRenderFinish() = 0;
};

class CAudioResample;
class CAudioDecoder : public CBaseDecoder {
public:
	CAudioDecoder();
	~CAudioDecoder();
public:
	void SetOutputFormat(int SampleRate,int nChannel,int BitPersample);
protected:
    void ThreadEntry ();
private:
	CAudioResample* 	mAudioResample;
	AudioFormat			mInAudioFormat;
	AudioFormat			mOutAudioFormat;
};

#endif
