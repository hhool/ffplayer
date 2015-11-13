/**
 * @file   util_lock.h
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:13:41 2010
 * 
 * @brief  
 * 
 * 
 */
#ifndef __UtilLOCK_H__
#define __UtilLOCK_H__

#include <pthread.h>
#include "util_log.h"
#include "config/stl_config.h"

class UtilSingleLock {
private:
    pthread_mutex_t &lock;
    pthread_mutex_t mutex;
    string ms_lockerName;

public:
    UtilSingleLock( pthread_mutex_t &m ) : lock(m) {
    }
        
    UtilSingleLock (const char* name = "Default Locker") : lock(mutex) {
        ms_lockerName = name;
        pthread_mutex_init(&mutex, 0);
    }

    inline void Lock (const char* who = __FUNCTION__) {
        //DEBUG ("%s Lock 1 by %s", ms_lockerName.c_str(), who);
        pthread_mutex_lock(&mutex);
        //DEBUG ("%s Lock 2 by %s", ms_lockerName.c_str(), who);
    }

    inline void Unlock (const char* who = __FUNCTION__) {
        //DEBUG ("%s Unlock 1 by %s", ms_lockerName.c_str(), who);
        pthread_mutex_unlock(&mutex);
        //DEBUG ("%s Unlock 2 by %s", ms_lockerName.c_str(), who);
    }
        
    ~UtilSingleLock() {
        pthread_mutex_destroy(&mutex);
    }

    pthread_mutex_t* GetMutex () {
        return &mutex;
    }
};

class UtilSimpleLock {
public:
    UtilSimpleLock () {
        pthread_mutex_init (&mutex, 0);
        pthread_mutex_lock (&mutex);
    }

    ~UtilSimpleLock () {
        pthread_mutex_unlock (&mutex);
        pthread_mutex_destroy (&mutex);
    }

    pthread_mutex_t* GetMutex () {
        return &mutex;
    }

private:
    pthread_mutex_t mutex;
};

class UtilCondLock {
    pthread_mutex_t mutex;
    pthread_cond_t cond;

    int pid;
    int counter, max;

public:
    int getDiff() { return max - counter; }
    void Lock(int res=100);
    void UnLock(int res=100);
    bool TryLock(int res=100);

    UtilCondLock(int max=100);
    ~UtilCondLock();
};

class UtilLocker {
    UtilCondLock &lock;
    int res;

public:
    UtilLocker(UtilCondLock &lock, int res=100);
    ~UtilLocker();
};

class UtilSemaphore {
    int v;
    pthread_mutex_t mutex;
    pthread_cond_t cond;

public:
    UtilSemaphore(int initValue = 0);
    ~UtilSemaphore();
	
    int Down();
    int Decrement();
    int Up();
    int Value();
};

class UtilCond {
private:
    pthread_cond_t cond;

public:
    UtilCond () {
        pthread_cond_init (&cond, 0);
    }
    ~UtilCond () {
        pthread_cond_destroy (&cond);
    }
    void Signal () {
        pthread_cond_signal (&cond);
    }
    void Wait (pthread_mutex_t* mutex) {
		pthread_cond_wait (&cond, mutex);
    }
};

#endif
