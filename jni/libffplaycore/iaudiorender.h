#ifndef __IAUDIORENDER__
#define __IAUDIORENDER__

#include "audio.h"

typedef enum
{
	AR_NONE		= 0,
	AR_OPENED 	= 1,
	AR_RUNNING	= 2,
	AR_PAUSE	= 3,
	AR_STOP		= 4,
	AR_CLOSE	= 5,
}ARState;
	

class IAudioRender
{
public:
	typedef void 	(*AudioCallback)(IAudioRender *audioSink, const AudioSample& audioSample, void *cookie);
	
	virtual int 	Open (AudioFormat audioFormat) = 0;
	virtual bool	IsOpened()	= 0;
	virtual void    Start () 	= 0;
	virtual ARState	GetState()	= 0;
    virtual int     Write (const AudioSample& SrcAudioSample) = 0;
    virtual void    Stop ()  	= 0;
    virtual void    Flush ()	= 0;
    virtual void    Pause ()	= 0;
    virtual void    Close ()	= 0;
	virtual int 	Reset ()	= 0;
	virtual void	IsRenderFinish(int* IsFinish) = 0;
	virtual void	SetVolume (float left, float right) = 0;
};

#endif
