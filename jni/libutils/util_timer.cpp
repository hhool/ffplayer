#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "util_timer.h"
#include "util_time.h"
#include "util_log.h"

//不精确的定时，取决于信号处理程序的执行时间。所以，不能阻塞。

UtilTimer::UtilTimer (int interval, TimeOutFunction fun, void* cookie, bool toStart) {
    miInterval = interval;
    miCurrentValue = interval;
    mbRunning = toStart;
    mpFunction = fun;
    mpCookie = cookie;
    DEBUG ("new timer: interval=%d", interval);
}
UtilTimer::~UtilTimer () {

}
void UtilTimer::SetTimeOutFunction (TimeOutFunction fun, void* cookie) {
    mpFunction = fun;
    mpCookie = cookie;
}
void UtilTimer::Start () {
    mbRunning = true;
}
void UtilTimer::Stop () {
    mbRunning = false;
}
int UtilTimer::TimeTick () {
    if (!mbRunning)
        return -1;
    miCurrentValue--;
    if (miCurrentValue <= 0) {
        //DEBUG ("timer timeout");
        miCurrentValue = miInterval;
        if (mpFunction)
            return mpFunction (mpCookie);
    }
    return 0;
}

#ifdef USE_SIGALARM_MODE
UtilTimerManager* UtilTimerManager::mpInstance = NULL;

UtilTimerManager::UtilTimerManager () {
    DEBUG ("construct UtilTimerManager!");
    miMinTick = 1;
    mTimerList.clear ();

    struct sigaction tact;
    /*信号到了要执行的任务处理函数*/
    tact.sa_handler = TimerSigHandlerStub;
    tact.sa_flags = 0;
    /*初始化信号集*/
    sigemptyset(&tact.sa_mask);
    /*建立信号处理机制*/
    sigaction(SIGALRM, &tact, NULL);

    int sec = miMinTick / ONE_SECOND;
    int usec = (1000000/ONE_SECOND) * (miMinTick - sec * ONE_SECOND);
    DEBUG ("sec=%d, usec=%d", sec, usec);
    struct itimerval value;
    /*设定执行任务的时间间隔*/
    value.it_value.tv_sec = sec;
    value.it_value.tv_usec = usec;
    /*设定初始时间计数*/
    value.it_interval = value.it_value;
    /*设置计时器ITIMER_REAL*/
    setitimer(ITIMER_REAL, &value, NULL);
}

UtilTimerManager::~UtilTimerManager () {
    DEBUG ("release UtilTimerManager!");
    list<UtilTimer*>::iterator it = mTimerList.begin ();
    for (; it != mTimerList.end (); it++) {
        UtilTimer* timer = *it;
        delete timer;
    }
    mTimerList.clear();
}
UtilTimerManager* UtilTimerManager::getInstance () {
    if(mpInstance == NULL) {
         mpInstance = new UtilTimerManager ();
    }
    return mpInstance;
}
void UtilTimerManager::destroyInstance () {
    if (mpInstance) {
        delete mpInstance;
        mpInstance = NULL;
    }
}
void UtilTimerManager::TimerSigHandlerStub (int signo) {
    if (mpInstance)
        mpInstance->TimerSigHandler (signo);
}
void UtilTimerManager::TimerSigHandler (int signo) {
    //string tt = UtilTime::CurrentTimeString ();
    //DEBUG ("timer tick! %s", tt.c_str ());
    list<UtilTimer*>::iterator it = mTimerList.begin ();
    for (; it != mTimerList.end (); it++) {
        UtilTimer* timer = *it;
        timer->TimeTick ();
    }
}

UtilTimer* UtilTimerManager::NewTimer (int interval, TimeOutFunction fun, void* cookie, bool toStart) {
    UtilTimer* timer = new UtilTimer (interval, fun, cookie, toStart);
    if (timer) {
        mTimerList.push_back (timer);
        DEBUG ("create UtilTimer:%p!", timer);
    } else {
        ERROR ("create UtilTimer fail!");
    }
}

int UtilTimerManager::ReleaseTimer (UtilTimer* timer) {
    UtilTimer* timer_found = NULL;
    list<UtilTimer*>::iterator it = mTimerList.begin ();
    for (; it != mTimerList.end (); it++) {
        if (timer == *it) {
            timer_found = timer;
        }
    }
    if (timer_found) {
        DEBUG ("remove UtilTimer:%p!", timer_found);
        mTimerList.remove (timer_found);
        delete timer_found;
        DEBUG ("delete UtilTimer success!");
        return 0;
    } else {
        ERROR ("not find timer!");
        return -1;
    }
}

#else
UtilTimerManager* UtilTimerManager::mpInstance = NULL;

UtilTimerManager::UtilTimerManager () : UtilEventLoop ("TimerThread") {
    DEBUG ("construct UtilTimerManager!");
    miMinTick = 1;
    mTimerList.clear ();
    ThreadExec ();
}

UtilTimerManager::~UtilTimerManager () {
    DEBUG ("release UtilTimerManager!");
    NotifyExit ();
    list<UtilTimer*>::iterator it = mTimerList.begin ();
    for (; it != mTimerList.end (); it++) {
        UtilTimer* timer = *it;
        delete timer;
    }
    mTimerList.clear();
}
UtilTimerManager* UtilTimerManager::getInstance () {
    if(mpInstance == NULL) {
         mpInstance = new UtilTimerManager ();
    }
    return mpInstance;
}
void UtilTimerManager::destroyInstance () {
    if (mpInstance) {
        delete mpInstance;
        mpInstance = NULL;
    }
}

void UtilTimerManager::EventLoopProcess () {

    int sec = miMinTick / ONE_SECOND;
    int usec = (1000000/ONE_SECOND) * (miMinTick - sec * ONE_SECOND);
    DEBUG ("Start Timer Thread! sec=%d, usec=%d", sec, usec);

    struct timespec interval = { 0 };
    interval.tv_sec = sec;
    interval.tv_nsec = usec * 1000;
    
    // begin download loop.
    while (!NeedExit()) {
        // sleep 
        struct timespec usleep_req = interval;
        nanosleep(&usleep_req, 0);

        // emit timeout.
        TimeTick ();
    }
    DEBUG ("Normal Exit Timer thread!");
}
int UtilTimerManager::TimeTick () {
    //string tt = UtilTime::CurrentTimeString ();
    //DEBUG ("timer tick! %s", tt.c_str ());
    list<UtilTimer*>::iterator it = mTimerList.begin ();
    for (; it != mTimerList.end (); it++) {
        UtilTimer* timer = *it;
        timer->TimeTick ();
    }
    return 0;
}

UtilTimer* UtilTimerManager::NewTimer (int interval, TimeOutFunction fun, void* cookie, bool toStart) {
    UtilTimer* timer = new UtilTimer (interval, fun, cookie, toStart);
    if (timer) {
        mTimerList.push_back (timer);
        DEBUG ("create UtilTimer:%p!", timer);
        return timer;
    } else {
        ERROR ("create UtilTimer fail!");
        return NULL;
    }
}

int UtilTimerManager::ReleaseTimer (UtilTimer* timer) {
    UtilTimer* timer_found = NULL;
    list<UtilTimer*>::iterator it = mTimerList.begin ();
    for (; it != mTimerList.end (); it++) {
        if (timer == *it) {
            timer_found = timer;
        }
    }
    if (timer_found) {
        DEBUG ("remove UtilTimer:%p!", timer_found);
        mTimerList.remove (timer_found);
        delete timer_found;
        DEBUG ("delete UtilTimer success!");
        return 0;
    } else {
        ERROR ("not find timer!");
        return -1;
    }
}

#endif
