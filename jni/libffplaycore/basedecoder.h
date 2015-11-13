#ifndef __BASEDEODER_H__
#define __BASEDEODER_H__

#include "ffmpeg.h"
#include "demux.h"
#include "util_lock.h"
#include "util_thread.h"
#include "packetList.h"
#include "masterclock.h"

/** 
 * this is an empty base class, just a interface. 
 * 
 * 
 */
class CStubRender
{
};

class CBaseDecoder : public UtilThread
{
public:
	typedef enum _ExitReason
	{
		ER_AUTO = 0x1,
		ER_QUIT = 0x2
	}ExitReason;
	
    CBaseDecoder ();
    ~CBaseDecoder ();

    void SetDemux	 	(CDemux* Demux);
    void SetStream 		(AVStream* Stream);
    void SetStubRender  (CStubRender* StubRender);
    void SetMasterClock (CMasterClock* Clock);

    virtual int Start ();
    virtual int Stop ();
	int Wait ();
	CBaseDecoder::ExitReason ThreadExitReason();
protected:
    virtual void ThreadEntry ();
protected:
    CDemux* 		mpDemux;
    AVStream* 		mpStream;
    CStubRender* 	mpStubRender;
    CMasterClock* 	mpClock;

    bool mbQuit;
    UtilSingleLock  mOperationLock;
};

#endif
