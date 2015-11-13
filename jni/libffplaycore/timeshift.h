#ifndef __DTIMESHIFTCACHE_H__
#define __DTIMESHIFTCACHE_H__

#include "ffmpeg.h"
#include "decoder_impl.h"
#include <stdio.h>
#include <math.h>
#include "util_lock.h"
#include "util_thread.h"
#include "packetList.h"
#include "config/stl_config.h"
#include "timeshiftbase.h"

class CBasePlayer;

#define VAILD_DATASIZE (1*1024*1024)

class CTimeShiftManage : public UtilThread
{
public:
    CTimeShiftManage (CBasePlayer* Player);
    ~CTimeShiftManage ();

	int		SetDataSource(const char *url);
    void 	ThreadEntry ();
	int		Reset ();
	void	SetTimeShiftInfo(TimeShiftInfo TShiftInfo);
	bool	IsDataAvailable();
	bool	IsAlowTimeShift();
	char*	GetTimeShiftURL();
	int		Start();
	int		Stop();
	bool	IsRun();
	bool 	IsNeedQuit();
protected:
	FILE*			mTimeShiftFile;
	int				mState;
	int64_t			mCurSize;
	TimeShiftInfo 	mTShiftInfo;
    CBasePlayer*	mPlayer;
	bool			mbQuit;
	AVIOContext* 	mpIO;
	string			mStrDataSource;
	
};

#endif
