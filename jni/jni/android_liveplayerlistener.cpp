#include <nativehelper/jni.h>
#include <nativehelper/JNIHelp.h>
#include <android_runtime/AndroidRuntime.h>
#include <util_log.h>
#include "android_liveplayerlistener.h"

using namespace android;

extern JNIEnv* getJNIEnv();
extern JavaVM *g_jvm;

jmethodID CAndroidLivePlayerListener::mspost_event = 0;

CAndroidLivePlayerListener::CAndroidLivePlayerListener(JNIEnv* env, jobject thiz, jobject weak_thiz):mOperationLock("CAndroidLivePlayerListener")
{
    // Hold onto the MediaPlayer class for use in calling the static method
    // that posts events to the application thread.
    jclass clazz = env->GetObjectClass(thiz);
    if (clazz == NULL)
    {
        jniThrowException(env, "java/lang/Exception", "Listener");
        return;
    }
    mClass = (jclass)env->NewGlobalRef(clazz);

    // We use a weak reference so the MediaPlayer object can be garbage collected.
    // The reference is only used as a proxy for callbacks.
    mObject  = env->NewGlobalRef(weak_thiz);

	INFO("0x%x",this);
}

CAndroidLivePlayerListener::~CAndroidLivePlayerListener()
{
    // remove global references
   	INFO("0x%x",this);
    JNIEnv *env = getJNIEnv();
    env->DeleteGlobalRef(mObject);
    env->DeleteGlobalRef(mClass);

	mEnvList.clear();
}

int  CAndroidLivePlayerListener::AttachThread()
{
	if (g_jvm == NULL)
		return -1;

	JNIEnv *Env  = NULL;
	int 	TID  = pthread_self();
	
	if (g_jvm->AttachCurrentThread(&Env, NULL) != JNI_OK) 
	{       
		ERROR("AttachCurrentThread() failed");   
		return -1;    
	}

	ThreadInfo TInfo;
	TInfo.mThreadID  	= TID;
	TInfo.mEnv			= Env;

	DEBUG("Attach:TID %d,Env 0x%x",TID,Env);
	
	mEnvList.push_back(TInfo);

	return 0;
}

void CAndroidLivePlayerListener::DetachThread()
{
	if (g_jvm == NULL)
		return ;
	
	int TID  = pthread_self();

	DEBUG("Detach:TID %d",TID);
#if 0
	DEBUG("Detach:Before size %d",mEnvList.size());
		
	for (size_t i = 0; i < mEnvList.size(); ++ i)
	{
		DEBUG("Detach:TID %d,Env 0x%x",TID,mEnvList[i].mEnv);
	}
#endif

	for (size_t i = 0; i < mEnvList.size(); ++ i)
	{
		if(mEnvList[i].mThreadID == TID)
		{
			g_jvm->DetachCurrentThread();
			mEnvList.erase(mEnvList.begin()+i);
			DEBUG("Detach:TID %d,Env 0x%x",TID,mEnvList[i].mEnv);
			break;
		}
	}
#if 0
	DEBUG("Detach:After size %d",mEnvList.size());
	
	for (size_t i = 0; i < mEnvList.size(); ++ i)
	{
		
		DEBUG("Detach:TID %d,Env 0x%x",TID,mEnvList[i].mEnv);
	}
#endif
}

void CAndroidLivePlayerListener::Notify(int msg, int ext1, int ext2)
{
    JNIEnv *env = NULL;
	DEBUG("Notify mspost_event 0x%x",mspost_event);
	mOperationLock.Lock();
	DEBUG("Notify mspost_event Enter 0x%x,msg %d,ext1 %d,ext2 %d",mspost_event,msg, ext1,ext2);

	int nIndex = -1;
	int TID  = pthread_self();
	
	for (size_t i = 0; i < mEnvList.size(); ++ i)
	{
		if(mEnvList[i].mThreadID == TID)
		{
			nIndex = i;
			break;
		}
	}

	DEBUG("nIndex %d",nIndex);
	
	if(nIndex >= 0)
	{
		env = mEnvList[nIndex].mEnv;
		
	    if(mspost_event != NULL)
	    {
	    	env->CallStaticVoidMethod(mClass, mspost_event, mObject, msg, ext1, ext2, 0);
	    }
	}
	DEBUG("Notify mspost_event after Leave 0x%x",mspost_event);
	mOperationLock.Unlock();
	DEBUG("Notify mspost_event after 0x%x",mspost_event);
}
