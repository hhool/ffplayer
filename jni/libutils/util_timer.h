#ifndef __UtilTIMER_H__
#define __UtilTIMER_H__

//#define USE_SIGALARM_MODE

#include <stdio.h>
#include <stdlib.h>
#include "config/stl_config.h"

#define ONE_SECOND 2 //一秒几个tick
typedef int (*TimeOutFunction)(void* cookie);

class UtilTimer {
public:
    UtilTimer (int interval, TimeOutFunction fun = NULL, void* cookie = NULL, bool toStart = true);
    ~UtilTimer ();
    void SetTimeOutFunction (TimeOutFunction fun, void* cookie);
    void Start ();
    void Stop ();
    int TimeTick ();
private:
    int miInterval;
    int miCurrentValue;
    bool mbRunning;

    TimeOutFunction mpFunction;
    void* mpCookie;
};

#ifdef USE_SIGALARM_MODE
class UtilTimerManager {
public:
    UtilTimerManager ();
    ~UtilTimerManager ();
    static UtilTimerManager* getInstance ();
    static void destroyInstance ();

    static void TimerSigHandlerStub (int signo);
    void TimerSigHandler (int signo);

    UtilTimer* NewTimer (int interval, TimeOutFunction fun = NULL, void* cookie = NULL, bool toStart = true);
    int ReleaseTimer (UtilTimer* timer);

private:
    static UtilTimerManager* mpInstance;
    list<UtilTimer*> mTimerList;
    int miMinTick; //usually by ONE_SECOND. 10*ONE_SECOND, ONE_SECOND/2
};

#else
#include "util_eventloop.h"
class UtilTimerManager : public UtilEventLoop {
public:
    UtilTimerManager ();
    ~UtilTimerManager ();
    static UtilTimerManager* getInstance ();
    static void destroyInstance ();

    virtual void EventLoopProcess ();

    UtilTimer* NewTimer (int interval, TimeOutFunction fun = NULL, void* cookie = NULL, bool toStart = true);
    int ReleaseTimer (UtilTimer* timer);

private:
    int TimeTick ();

private:
    static UtilTimerManager* mpInstance;
    list<UtilTimer*> mTimerList;
    int miMinTick; //usually by ONE_SECOND. 10*ONE_SECOND, ONE_SECOND/2
};
#endif

#endif
