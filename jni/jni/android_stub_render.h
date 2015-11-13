#ifndef __ANDROID_STUB_RENDER_H__
#define __ANDROID_STUB_RENDER_H__

#include "adecoder.h"
#include "vdecoder.h"
#include "iaudiorender.h"
#include "ivideorender.h"

class CAndroidAudioStubRender : public CAudioStubRender
{
public:
    CAndroidAudioStubRender (IAudioRender* stub);
    ~CAndroidAudioStubRender ();
public:
	virtual int 	Start();
	virtual	int 	Flush();
	virtual int  	Stop();
	virtual int		Pause();
	virtual bool 	IsStart();
    virtual int 	Write (const AudioSample& SrcAudioSample);
	virtual bool	IsRenderFinish();

private:
    IAudioRender* mpStub;
};

class CAndroidVideoStubRender : public CVideoStubRender {
public:
    CAndroidVideoStubRender (IVideoRender* stub);
    ~CAndroidVideoStubRender ();
    int ShowPicture (AVFrame *pFrame);
	virtual int Flush();

private:
    IVideoRender* mpStub;
};


#endif
