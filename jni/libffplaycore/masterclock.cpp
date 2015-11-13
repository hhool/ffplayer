#include "masterclock.h"
#include "playcore.h"

CMasterClock::CMasterClock (ClockType type, double pts, double pts_time) : mOperationLock("CMasterClock")
{
    mbStop 		= true;
    mfStopTime  = INVALID_TIME_VALUE;
	
    Init ();
    switch (type)
    {
    case CLOCK_EXTERN:
        UseExternClock (pts, pts_time);
        break;
    case CLOCK_VIDEO:
        UseVideoClock (pts, pts_time);
        break;
    case CLOCK_AUDIO:
        UseAudioClock (pts, pts_time);
        break;
    default:
        ERROR ("you should not reach here!");
        break;
    }
	INFO("0x%x",this);	
}
CMasterClock::~CMasterClock ()
{
	INFO("0x%x",this);
}

void CMasterClock::Reset ()
{
    mOperationLock.Lock ();
    mbStop 		= true;
    mfStopTime	= INVALID_TIME_VALUE;
    Init ();
    mOperationLock.Unlock ();
}

void CMasterClock::SetClockType(ClockType type)
{
 	mOperationLock.Lock ();
	meType = type;
	mOperationLock.Unlock ();
}
void CMasterClock::SetOriginClock (double pts, double pts_time, ClockType type)
{
    DEBUG ("pts=%f, pts_time=%f, type=%d", pts, pts_time, (int)type);
	
    mOperationLock.Lock ();
    if (type == CLOCK_NOT_CHANGE) type = meType;
    switch (type)
    {
    case CLOCK_EXTERN:
        UseExternClock (pts, pts_time);
        break;
    case CLOCK_VIDEO:
        UseVideoClock (pts, pts_time);
        break;
    case CLOCK_AUDIO:
        UseAudioClock (pts, pts_time);
        break;
    default:
        ERROR ("you should not reach here!");
        break;
    }
    mOperationLock.Unlock (); 
}

void CMasterClock::Stop ()
{
	double fHasPlayTime = GetCurrentClock();
    mOperationLock.Lock ();
	if(!mbStop)
    {
    	mbStop = true;
	    mfStopTime = SystemTime ();
		DEBUG("CMasterClock::Stop:mbStop %d,mfStopTime %f fHasPlayTime %f",mbStop,mfStopTime, fHasPlayTime);
	}
    mOperationLock.Unlock (); 
}
bool CMasterClock::IsRun()
{
	return !mbStop;
}
void CMasterClock::Start ()
{
	double fHasPlayTime = GetCurrentClock ();

    mOperationLock.Lock ();

    mbStop = false;

	switch (meType)
	{
	case CLOCK_EXTERN:
		if(mfExternOriginTime == INVALID_TIME_VALUE)
			mfExternOriginTime = SystemTime();
		break;
	case CLOCK_VIDEO:
		if(mfVideoOriginTime == INVALID_TIME_VALUE)
			mfVideoOriginTime = SystemTime();
		break;
	case CLOCK_AUDIO:
		if(mfAudioOriginTime == INVALID_TIME_VALUE)		
			mfAudioOriginTime = SystemTime();
		break;
	default:
		ERROR ("you should not reach here!");
		break;
	}
	DEBUG("mfAudioOriginTime %f mfAudioOriginPts %f, mfVideoOriginTime %f mfVideoOriginPts %f,fHasPlayTime %f\
		",mfAudioOriginTime,mfAudioOriginPts,mfVideoOriginTime,mfVideoOriginPts,fHasPlayTime);
		
	if (mfStopTime > 0.0)
    {
        double diff = SystemTime () - mfStopTime;
        switch (meType)
        {
        case CLOCK_EXTERN:
            mfExternOriginTime += diff;
            break;
        case CLOCK_VIDEO:
            mfVideoOriginTime += diff;
            break;
        case CLOCK_AUDIO:
            mfAudioOriginTime += diff;
            break;
        default:
            ERROR ("you should not reach here!");
            break;
        }
		mfStopTime = INVALID_TIME_VALUE;

		
	DEBUG("mfAudioOriginTime %f mfAudioOriginPts %f, mfVideoOriginTime %f mfVideoOriginPts %f,fHasPlayTime %f,diff %f\
		",mfAudioOriginTime,mfAudioOriginPts,mfVideoOriginTime,mfVideoOriginPts,fHasPlayTime,diff);
    }
    		
    mOperationLock.Unlock (); 
	DEBUG("GetCurrentClock () %f",GetCurrentClock ());	
}

double CMasterClock::GetCurrentClock ()
{
    double ret = INVALID_TIME_VALUE;
		
    mOperationLock.Lock ();
	DEBUG("CMasterClockType %d",meType);
    switch (meType)
    {
    case CLOCK_EXTERN:
        ret = GetExternClock ();
        break;
    case CLOCK_VIDEO:
        ret = GetVideoClock ();
        break;
    case CLOCK_AUDIO:
        ret = GetAudioClock ();
        break;
    default:
        ERROR ("should not reach here!");
        ret = INVALID_TIME_VALUE;
        break;
    }
	DEBUG("mbStop %d,mfStopTime %f ret %f ",mbStop,mfStopTime,ret);
    if (mbStop)
    {
    	if(ret > 0)
        {
        	if(mfStopTime > 0.0f)
        	{
        		ret -= (SystemTime () - mfStopTime);
        	}
    	}
		
    }
    mOperationLock.Unlock ();
    DEBUG("GetCurrentClock %f",ret);
    return ret;
}
CMasterClock::ClockType CMasterClock::GetClockType()
{
	return meType;
}
void CMasterClock::Init ()
{
    //meType = CLOCK_EXTERN;
    mfExternOriginPts 	= INVALID_TIME_VALUE; // from CVideoDecoderImp::Decode () or CAudioDecoderImp::Decode () returned pts.
    mfExternOriginTime 	= INVALID_TIME_VALUE; // av_gettime() / 1000000.0

    mfVideoOriginPts 	= INVALID_TIME_VALUE; // from CVideoDecoderImp::Decode () returned pts
    mfVideoOriginTime 	= INVALID_TIME_VALUE; // av_gettime() / 1000000.0
    
    mfAudioOriginPts 	= INVALID_TIME_VALUE; // from CAudioDecoderImp::Decode () returned pts
    mfAudioOriginTime 	= INVALID_TIME_VALUE; // av_gettime() / 1000000.0
}

void CMasterClock::UseExternClock (double pts, double pts_time)
{
    Init ();
    meType = CLOCK_EXTERN;
    mfExternOriginPts 	= pts;
	mfExternOriginTime  = pts_time;

    DEBUG("mfExternOriginPts %f,mfExternOriginTime %f",mfExternOriginPts,mfExternOriginTime);
}
void CMasterClock::UseVideoClock (double pts, double pts_time)
{
    Init ();
    meType = CLOCK_VIDEO;
    mfVideoOriginPts 	= pts;
	mfVideoOriginTime   = pts_time;

    DEBUG("mfVideoOriginPts %f,mfVideoOriginTime %f",mfVideoOriginPts,mfVideoOriginTime);
}
void CMasterClock::UseAudioClock (double pts, double pts_time)
{
    Init ();
    meType = CLOCK_AUDIO;
    mfAudioOriginPts 	= pts;
	mfAudioOriginTime   = pts_time;
	
    DEBUG("mfAudioOriginPts %f,mfAudioOriginTime %f",mfAudioOriginPts,mfAudioOriginTime);
}

double CMasterClock::GetExternClock ()
{
    DEBUG("mfExternOriginPts %f,mfExternOriginTime %f",mfVideoOriginPts,mfExternOriginTime);

    if (mfExternOriginPts < 0 || mfExternOriginTime < 0)
    {
        return INVALID_TIME_VALUE;
    }
    else if(mfExternOriginTime>=0.0f)
    {
        return SystemTime() - mfExternOriginTime + mfExternOriginPts;
    }
	else
		return mfExternOriginPts;
}

double CMasterClock::GetVideoClock ()
{
    DEBUG("mfVideoOriginPts %f,mfVideoOriginTime %f",mfVideoOriginPts,mfVideoOriginTime);

    if (mfVideoOriginPts < 0 || mfVideoOriginTime < 0)
    {
        return INVALID_TIME_VALUE;
    }
    else if(mfVideoOriginTime>=0.0f)
    {
        return SystemTime() - mfVideoOriginTime + mfVideoOriginPts;
    }
	else
		return mfVideoOriginPts;
}

double CMasterClock::GetAudioClock ()
{
    DEBUG("mfAudioOriginPts %f,mfAudioOriginTime %f",mfAudioOriginPts,mfAudioOriginTime);

    if (mfAudioOriginPts < 0 || mfAudioOriginTime < 0)
    {
        return INVALID_TIME_VALUE;
    }
    else if(mfAudioOriginPts>=0.0f)
    {
        return SystemTime() - mfAudioOriginTime + mfAudioOriginPts;
    }
	else
		return mfAudioOriginTime;
}

double CMasterClock::SystemTime ()
{
	double Time = PlayCore::GetInstance()->av_gettime() / 1000000.0;
	DEBUG("SystemTime %f",Time);
    return Time;
}
