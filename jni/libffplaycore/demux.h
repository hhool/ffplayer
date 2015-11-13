#ifndef __DEMUXE_H__
#define __DEMUXE_H__

#include "ffmpeg.h"
#include "decoder_impl.h"
#include <stdio.h>
#include <math.h>
#include "util_lock.h"
#include "util_thread.h"
#include "packetList.h"
#include "config/stl_config.h"

class CBasePlayer;
class CBaseDecoder;
class CMasterClock;

class CDemux : public UtilThread
{
public:
    CDemux (CBasePlayer* Player);
    ~CDemux ();

    int 	SetDataSource(const char *url);
    int 	Reset ();
    int 	Start ();
	int 	Pause (bool bPause);
	bool 	IsPaused();
    int 	Stop ();
	void	Interrupt();
    int 	Seek (double pos);
	bool 	IsSeekable();
	bool 	IsCanPause();
    void 	ThreadEntry ();

    AVPacket GetVideoPacket ();
    AVPacket GetAudioPacket ();

    // get information, must call after OpenInputFile ().
    AVFormatContext* GetFormatContext ();
    int 	GetVideoStreamIndex ();
    int 	GetAudioStreamIndex ();
    void   	SetSyncPTS(double SyncPTS);
	double 	GetSyncPTS();
	void   	SetSynced(bool bSynced = true);
	bool   	IsSynced();
	void	SetAudioReadForPlay(bool bReady = true);
	bool	IsAudioReadyForPlay();
	void   	SetAudioOutPlay(CBaseDecoder* AOP);
	void   	SetVideoOutPlay(CBaseDecoder* VOP);
	void   	SetMasterClock (CMasterClock* clock);
	bool	IsEmptyAudioQueue();
	bool	IsEmptyVideoQueue();
protected:
	int     CheckQueueData(CDemux* This,bool bCheckLow = true);
	void	UpdateDuration(bool bForce = false);
private:
	bool 	mIsPaused;
	bool 	mbLow;
	bool 	mbReachEnd;
	bool 	mbReadOk;
	int		mPercent;
	bool	mFirstCache;
	
private:
	typedef enum _Demux_Cache_State
	{
		DCS_NONE				= 0x0,
		DCS_BUFFING_BEFORE_SYNC = 0x1,
		DCS_SYNC				= 0x2,
		DCS_BUFFING_AFTER_SYNC  = 0x3,
		DCS_CHECK_LOW			= 0x4,
	}Demux_Cache_State;
	
    int OpenInputFile (string url);
    int AvSeekFrame (double pos);
    static int DecodeInterruptCB(void* pThis);
private:
    string          msUrl;
    bool     		mbQuit;
	static bool		mbInterrupt;
    UtilSingleLock  mOperationLock;

    AVFormatContext *mpFormatCtx;
    int             miVideoStream;
    int             miAudioStream;

    CPacketQueue    mAudioQueue;
    CPacketQueue    mVideoQueue;
    CBasePlayer*	mPlayer;
    double			mSyncPTS;
	bool			mbSynced;
	bool			mbAudioReadyForPlay;
	bool			mbPaused;
	CBaseDecoder*	mAOutPlay;
	CBaseDecoder*	mVOutPlay;
    CMasterClock* 	mpClock;
	bool			mbSeek;
	bool			mSeekable;
	bool			mCanPause;
	int				mMaxCache;
	Demux_Cache_State	mDCS;
	bool			mbOpened;
};

#endif
