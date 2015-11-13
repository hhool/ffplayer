#ifndef __ANDROID_LIVEPLAYER_LISTENER_H__
#define __ANDROID_LIVEPLAYER_LISTENER_H__

#include "baseplayer.h"

class ThreadInfo
{
public:
	JNIEnv* mEnv;
	int     mThreadID;
};
// ----------------------------------------------------------------------------
// ref-counted object for callbacks
class CAndroidLivePlayerListener: public CBasePlayerListener
{
public:
	CAndroidLivePlayerListener(JNIEnv* env, jobject thiz, jobject weak_thiz);
    ~CAndroidLivePlayerListener();
	int  AttachThread();
	void DetachThread();
    void Notify(int msg, int ext1, int ext2);
private:
    CAndroidLivePlayerListener();
    jclass      mClass;     // Reference to MediaPlayer class
    jobject     mObject;    // Weak ref to MediaPlayer Java object to call on
    UtilSingleLock  mOperationLock;

	list<ThreadInfo> mEnvList;
public:
    static jmethodID   mspost_event;
};

#endif
