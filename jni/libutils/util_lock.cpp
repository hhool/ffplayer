/**
 * @file   util_lock.cpp
 * @author windsome <windsome@windsome-desktop>
 * @date   Tue Jan 12 18:13:45 2010
 * 
 * @brief  
 * 
 * 
 */
#include "util_log.h"
#include "util_lock.h"
#include <unistd.h>

void UtilCondLock::Lock(int res) {
	if (res>max)
		res=max;

    DEBUG ("Lock!");
	pthread_mutex_lock(&mutex);
	while ((counter+res)>max)
		pthread_cond_wait(&cond, &mutex);

	counter+=res;
	pthread_mutex_unlock(&mutex);
}

bool UtilCondLock::TryLock(int res) {
	if (res>max)
		res=max;

    DEBUG ("TryLock!");
	pthread_mutex_lock(&mutex);

    /// Resources locked by others, return immediately without lock
    if ((counter+res)>max) {
        pthread_mutex_unlock(&mutex);
        return false;
    }

    /// Resources avaliable, lock and return
    counter+=res;
	pthread_mutex_unlock(&mutex);
    return true;
}

void UtilCondLock::UnLock(int res) {
	if (res>max)
		res=max;

    DEBUG ("UnLock!");
	pthread_mutex_lock(&mutex);
	counter-=res;
	pthread_mutex_unlock(&mutex);
	pthread_cond_signal(&cond);
}

UtilCondLock::UtilCondLock(int max): max(max) {
	pthread_mutex_init(&mutex, 0);
	pthread_cond_init(&cond, 0);
	counter=0;
	pid=-1;
}

UtilCondLock::~UtilCondLock() {
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
}

UtilLocker::UtilLocker(UtilCondLock &lock, int res): lock(lock), res(res) {
	lock.Lock(res);
}

UtilLocker::~UtilLocker() {
	lock.UnLock(res);
}

UtilSemaphore::UtilSemaphore(int initValue) {
	v = initValue;
	pthread_mutex_init (&mutex, 0);
	pthread_cond_init (&cond, 0);
}

UtilSemaphore::~UtilSemaphore()
{
	pthread_mutex_destroy (&mutex);
	pthread_cond_destroy (&cond);
}

int UtilSemaphore::Down() {
	int value_after_op;
	pthread_mutex_lock (&mutex);
	while (v <= 0)
		pthread_cond_wait (&cond, &mutex);
	v--;
	value_after_op = v;
	pthread_mutex_unlock (&mutex);
	return value_after_op;
}

int UtilSemaphore::Decrement() {
	int value_after_op;
	pthread_mutex_lock (&mutex);
	v--;
	value_after_op = v;
	pthread_mutex_unlock (&mutex);
	pthread_cond_signal (&cond);
	return value_after_op;
}

int UtilSemaphore::Up() {
	int value_after_op;
	pthread_mutex_lock (&mutex);
	v++;
	value_after_op = v;
	pthread_mutex_unlock (&mutex);
	pthread_cond_signal (&cond);
	return value_after_op;
}

int UtilSemaphore::Value() {
	int value_after_op;
	pthread_mutex_lock (&mutex);
	value_after_op = v;
	pthread_mutex_unlock (&mutex);
	return value_after_op;
}
