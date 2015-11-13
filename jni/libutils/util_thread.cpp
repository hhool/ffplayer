/**
 * @file   util_thread.cpp
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:12:24 2010
 * 
 * @brief  
 * 
 * 
 */
#include "util_thread.h"
#include "util_log.h"

#include <stdio.h>
#include <unistd.h>

void* UtilThread::Wrapper(void *ptr) {
    UtilThread *p = (UtilThread*)ptr;
    pthread_cleanup_push( ThreadCompleted, (void*)p );
    DEBUG ("");

    p->ThreadEntry();
    DEBUG ("");

    pthread_exit(0);
    pthread_cleanup_pop(1);
    DEBUG ("");

    return 0;
}

void UtilThread::ThreadCompleted(void *ptr) {
    UtilThread *p = (UtilThread*) ptr;

    p->mb_Alive = false;
    p->ThreadFinished();
}
    
void UtilThread::ThreadExec(bool bwait, int prio, int policy) {
    if (mb_Alive) {
        DEBUG ("thread already running !!\n");
        return;
    }

    DEBUG ("");
	
    pthread_attr_t attr;
    pthread_attr_init(&attr);
#if !defined (PLATFORM_ANDROID)
    if (prio||policy) {
        struct sched_param p;
        p.__sched_priority=prio;
        pthread_attr_setschedpolicy(&attr, policy );
        pthread_attr_setschedparam(&attr, &p);
    }
#endif
    DEBUG ("");
	
    if ( pthread_create(&mt_Thread, &attr, Wrapper, this) ) {
        DEBUG("couldn't create new thread\n");
        return;
    }
            
    mb_Alive = true;
    if (bwait)
        pthread_join (mt_Thread, NULL);
}
pthread_t UtilThread::ThreadID()
{
	return pthread_self();
}
bool UtilThread::IsNeedQuit()
{
	return false;
}
void UtilThread::ThreadWait() {
    if (!mt_Thread)
        return;
	
    if ( mb_Alive) {
        DEBUG("Wait for thread:%d\n", mt_Thread);
        pthread_join (mt_Thread, NULL);
    }
}

void UtilThread::ThreadKill(bool sendcancel) {
    if (!mt_Thread)
        return;
	
    if ( mb_Alive && sendcancel ) {
        DEBUG("Send cancel to thread:%d\n", mt_Thread);
        mb_Alive = false;
#if !defined (PLATFORM_ANDROID)
        pthread_cancel(mt_Thread);
#endif
    }
	
    pthread_join (mt_Thread, NULL);
    DEBUG ("Thread %d been killed!\n", mt_Thread);
    mt_Thread=0;
}
