#ifndef __MASTER_CLOCK_H__
#define __MASTER_CLOCK_H__

#include "ffmpeg.h"
#include "util_lock.h"

#define INVALID_TIME_VALUE (-1.0f)

class CMasterClock
{
public:
	typedef enum _ClockType
	{
		CLOCK_NOT_CHANGE = -1,
		CLOCK_EXTERN = 0,
		CLOCK_VIDEO,
		CLOCK_AUDIO
	} ClockType;

    static const double AVThresholdSync = 0.01;
    static const double AVThresholdNoSync = 0.2;

public:
    CMasterClock (ClockType type = CLOCK_EXTERN, double pts = INVALID_TIME_VALUE, double pts_time = INVALID_TIME_VALUE);
    ~CMasterClock ();

    void Reset ();
    void SetOriginClock (double pts, double pts_time = INVALID_TIME_VALUE, ClockType type = CLOCK_NOT_CHANGE);
    void Stop ();
	bool IsRun();
    void Start ();
    double GetCurrentClock (); //base on pts.
    CMasterClock::ClockType GetClockType();
    void SetClockType(ClockType type);

private:
    void Init ();

    void UseExternClock (double pts, double pts_time);
    void UseVideoClock (double pts, double pts_time);
    void UseAudioClock (double pts, double pts_time);
    
    double GetExternClock ();
    double GetVideoClock ();
    double GetAudioClock ();
    double SystemTime ();

private:
    ClockType meType;
    UtilSingleLock  mOperationLock;

    bool mbStop;
    double mfStopTime;

    double mfExternOriginPts;
    double mfExternOriginTime;

    double mfVideoOriginPts;
    double mfVideoOriginTime;
    
    double mfAudioOriginPts;
    double mfAudioOriginTime;
};

#endif
