/**
 * @file   util_thread.h
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:12:18 2010
 * 
 * @brief  
 * 
 * 
 */
#ifndef   	UtilTHREAD_H_
#define   	UtilTHREAD_H_

#include <pthread.h>

class UtilThread {
    pthread_t mt_Thread;
    bool mb_Alive;

    static void *Wrapper(void *ptr);
    static void ThreadCompleted(void *ptr);
            
public:
    bool ThreadIsRunning() { 
        return mb_Alive; 
    }

UtilThread()
    :mt_Thread(0), mb_Alive(false)
    {
    }

    virtual ~UtilThread() {
        ThreadKill ();
    }
	pthread_t ThreadID();
    void ThreadExec(bool bwait = false, int prio=0, int policy=0);
    void ThreadKill(bool sendcancel=false);
    void ThreadWait();

	virtual bool IsNeedQuit() ;
    virtual void ThreadEntry() = 0;
    virtual void ThreadFinished() { }
};

#endif
