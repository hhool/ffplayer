#include <common.h>
#include "android_stub_render.h"
#include "util_log.h"

CAndroidAudioStubRender::CAndroidAudioStubRender (IAudioRender* stub) : mpStub(stub)
{
   	INFO("0x%x",this);
}

CAndroidAudioStubRender::~CAndroidAudioStubRender ()
{
   	INFO("0x%x",this);
}

int	CAndroidAudioStubRender::Start()
{
	if (mpStub)
    {
        mpStub->Start();
        return ERR_NONE;
    }
	return ERR_STUB_ERROR;
}

int CAndroidAudioStubRender::Flush()
{
	if (mpStub)
    {
        mpStub->Flush ();
        return ERR_NONE;
    }
	return ERR_STUB_ERROR;
}

int CAndroidAudioStubRender::Stop()
{
	if (mpStub)
    {
        mpStub->Stop();
        return ERR_NONE;
    }
	return ERR_STUB_ERROR;
}

int CAndroidAudioStubRender::Pause()
{
	if (mpStub)
    {
        mpStub->Pause();
        return ERR_NONE;
    }
	return ERR_STUB_ERROR;
}
bool CAndroidAudioStubRender::IsStart()
{
	if (mpStub)
    {
    	ARState arState = mpStub->GetState();
		DEBUG("IsStart arState %d arState != AR_RUNNING %d",arState,arState != AR_RUNNING);
        return (arState == AR_RUNNING);
    }
	return false;
}

bool CAndroidAudioStubRender::IsRenderFinish()
{
	if (mpStub)
    {
    	int Finish = 0;
    	mpStub->IsRenderFinish(&Finish);
		DEBUG("IsRenderFinish %d",Finish);
        return !!Finish;
    }
	return false;
}
int CAndroidAudioStubRender::Write (const AudioSample& SrcAudioSample)
{
    if (mpStub)
    {
        return mpStub->Write(SrcAudioSample);
    }
    return ERR_STUB_ERROR;
}


/********************************** 
 * video outer.
 **********************************/
CAndroidVideoStubRender::CAndroidVideoStubRender (IVideoRender* stub) : mpStub(stub)
{
   	INFO("0x%x",this);
}

CAndroidVideoStubRender::~CAndroidVideoStubRender ()
{
   	INFO("0x%x",this);
}

int CAndroidVideoStubRender::ShowPicture (AVFrame *pFrame)
{
    if (mpStub)
        return mpStub->ShowPicture (pFrame); 

    return ERR_STUB_ERROR;
}

int CAndroidVideoStubRender::Flush()
{
	if (mpStub)
    {
      	mpStub->Flush ();
		return ERR_NONE;
	}

	return ERR_STUB_ERROR;
}