#include "common.h"
#include "basedecoder.h"

CBaseDecoder::CBaseDecoder () : mpDemux(NULL), mpStream(NULL), mpStubRender(NULL), mpClock (NULL),
                            mbQuit(false), mOperationLock("CBaseDecoder")
{
	INFO("0x%x",this);
}

CBaseDecoder::~CBaseDecoder ()
{
	INFO("0x%x",this);
}

void CBaseDecoder::SetDemux (CDemux* Demux)
{
    mpDemux = Demux;
}

void CBaseDecoder::SetStream (AVStream* Stream)
{
    mpStream = Stream;
}

void CBaseDecoder::SetStubRender (CStubRender* StubRender)
{
    mpStubRender = StubRender;
}

void CBaseDecoder::SetMasterClock (CMasterClock* Clock)
{
    mpClock  = Clock;
}

int CBaseDecoder::Start ()
{
    mbQuit = false;

	if (mpDemux == NULL)
    {
        ERROR ("no demux set!"); 
        return ERR_INVALID_DATA;
    }
    ThreadExec ();
	
    return ERR_NONE;
}

int CBaseDecoder::Stop ()
{
    mbQuit = true;
    Wait ();
    return ERR_NONE;
}

int CBaseDecoder::Wait ()
{
	ThreadWait ();
	
	return ERR_NONE;
}
CBaseDecoder::ExitReason CBaseDecoder::ThreadExitReason()
{
	if(mbQuit)
	{
		return ER_QUIT;
	}
	return ER_AUTO;
}
void CBaseDecoder::ThreadEntry ()
{
    DEBUG ("empty ThreadEntry! you must do it!");
}
