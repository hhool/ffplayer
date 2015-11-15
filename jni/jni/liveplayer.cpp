#include <common.h>
#include <util_log.h>
#include <nativehelper/jni.h>
#include <nativehelper/JNIHelp.h>
#include <android_runtime/AndroidRuntime.h>
#include "android_liveplayerlistener.h"
#include "android_liveplayer.h"

using namespace android;

// ----------------------------------------------------------------------------
struct fields_t
{
    jfieldID    context;
    jfieldID    surface;
    /* actually in android.view.Surface XXX */
    jfieldID    surface_native;
	jfieldID	package_name;
	jfieldID	timeshiftInfo;
	jfieldID	allow_timeshift;
	jfieldID	filename;
	jfieldID    filesize;
	
};

JavaVM * 				g_jvm = NULL;
fields_t 				g_fields;
int		 				g_nSDKVersion = 0;
string					g_strPackageName;
static TimeShiftInfo 	g_TimeShiftInfo;
static Mutex 		 	gs_Lock;
static eUtilLogLevel 	gs_CurrentLogLevel=CLL_WARN;

// ----------------------------------------------------------------------------

static Surface* GetSurface(JNIEnv* env, jobject clazz)
{
    return (Surface*)env->GetIntField(clazz, g_fields.surface_native);
}

static TimeShiftInfo* GetTimeShiftInfo(JNIEnv* env, jobject clazz,TimeShiftInfo* TShiftInfo)
{
	const char * c_filename;
	
	if(TShiftInfo == NULL)
		return NULL;

	memset((void*)TShiftInfo,0,sizeof(TimeShiftInfo));
	
	TShiftInfo->mbAllowTimeShift = env->GetBooleanField(clazz, g_fields.allow_timeshift);

	jstring filename = (jstring)env->GetObjectField(clazz, g_fields.filename);

	c_filename = env->GetStringUTFChars(filename, NULL);
	
	strcpy(TShiftInfo->mFileName,c_filename);
	
	env->ReleaseStringUTFChars(filename, c_filename);
	
	TShiftInfo->mFileSize = env->GetLongField(clazz, g_fields.filesize);	
	
	return TShiftInfo;
}

static CAndroidLivePlayer* GetLivePlayer(JNIEnv* env, jobject thiz)
{
    Mutex::Autolock l(gs_Lock);
    CAndroidLivePlayer* const p = (CAndroidLivePlayer*)env->GetIntField(thiz, g_fields.context);
    return p;
}

static CAndroidLivePlayer* SetLivePlayer(JNIEnv* env, jobject thiz, CAndroidLivePlayer* player)
{
    Mutex::Autolock l(gs_Lock);
    CAndroidLivePlayer* old = (CAndroidLivePlayer*)env->GetIntField(thiz, g_fields.context);
    env->SetIntField(thiz, g_fields.context, (int)player);
    return old;
}


static int GetSDKVersion(JNIEnv *env)
{
    if (env->ExceptionCheck())
        return false; // already got an exception pending

    bool success = true;

    // VERSION is a nested class within android.os.Build (hence "$" rather than "/")
    jclass versionClass = env->FindClass("android/os/Build$VERSION");
    if (NULL == versionClass)
        success = false;

    jfieldID sdkIntFieldID = NULL;
    if (success)
        success = (NULL != (sdkIntFieldID = env->GetStaticFieldID(versionClass, "SDK_INT", "I")));

    jint sdkInt = 0;
    if (success)
    {
        sdkInt = env->GetStaticIntField(versionClass, sdkIntFieldID);
        DEBUG("sdkInt = %d", sdkInt);
    }

    // cleanup
    env->DeleteLocalRef(versionClass);

    return sdkInt;
}
// ----------------------------------------------------------------------------
static int liveplayer_release(JNIEnv *env, jobject thiz);
// ----------------------------------------------------------------------------

// This function gets some field IDs, which in turn causes class initialization.
// It is called from a static block in CBasePlayer, which won't run until the
// first time an instance of this class is used.
static void liveplayer_native_init(JNIEnv *env)
{
    jclass clazz;

    clazz = env->FindClass(PLAYER_CLASS_DIR);
    if (clazz == NULL)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Can't find com/pbi/player/LivePlayer");
        return;
    }

    g_fields.context = env->GetFieldID(clazz, "mNativeContext", "I");
    if (g_fields.context == NULL)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Can't find LivePlayer.mNativeContext");
        return;
    }

    g_fields.surface = env->GetFieldID(clazz, "mSurface", "Landroid/view/Surface;");
    if (g_fields.surface == NULL)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Can't find LivePlayer.mSurface");
        return;
    }

    jclass surface = env->FindClass("android/view/Surface");
    if (surface == NULL)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Can't find android/view/Surface");
        return;
    }

	g_nSDKVersion = GetSDKVersion(env);

    if(g_nSDKVersion<=8)
    {
    	g_fields.surface_native = env->GetFieldID(surface, "mSurface", "I");
    }
    else if(g_nSDKVersion > 8 && g_nSDKVersion < 19)
    {
    	g_fields.surface_native = env->GetFieldID(surface, "mNativeSurface", "I");
    }
    else if(g_nSDKVersion == 19)
    {
    	g_fields.surface_native = env->GetFieldID(surface, "mNativeObject", "I");
    }
    else if(g_nSDKVersion >= 20)
    {
        g_fields.surface_native = env->GetFieldID(surface, "mNativeObject", "J");
    }

    if (g_fields.surface_native == NULL)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Can't find Surface.mSurface");
        return;
    }


    CAndroidLivePlayerListener::mspost_event = env->GetStaticMethodID(clazz, "postEventFromNative","(Ljava/lang/Object;IIILjava/lang/Object;)V");
	if (CAndroidLivePlayerListener::mspost_event == NULL)
	{
		jniThrowException(env, "java/lang/RuntimeException", "Can't find postEventFromNative");
		return;
	}

	g_fields.package_name  = env->GetStaticFieldID(clazz, "mPackageName", "Ljava/lang/String;");
	if(g_fields.package_name == NULL)
	{
	   jniThrowException(env, "java/lang/RuntimeException", "Can't find LivePlayer.mPackageName");
	   return;
	}	

	const char * c_PackageName;
	
	jstring jstrPackageName = (jstring)env->GetStaticObjectField(clazz, g_fields.package_name);

	c_PackageName = env->GetStringUTFChars(jstrPackageName, NULL);
	
	g_strPackageName = c_PackageName;
	
	env->ReleaseStringUTFChars(jstrPackageName, c_PackageName);

	
	g_fields.timeshiftInfo = env->GetFieldID(clazz, "mTimeShiftInfo", "L"PLAYER_CLASS_DIR"$TimeShiftInfo;");
	if (g_fields.timeshiftInfo == NULL)
	{
	   jniThrowException(env, "java/lang/RuntimeException", "Can't find LivePlayer.mTimeShiftInfo");
	   return;
	}

	clazz = env->FindClass(PLAYER_CLASS_DIR"$TimeShiftInfo");
	if (clazz == NULL)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Can't find com/pbi/player/LivePlayer$TimeShiftInfo");
        return;
    }

	g_fields.allow_timeshift = env->GetFieldID(clazz, "mbAllowTimeShift", "Z");
	if (g_fields.allow_timeshift == NULL)
	{
	   jniThrowException(env, "java/lang/RuntimeException", "Can't find TimeShiftInfo.mbAllowTimeShift");
	   return;
	}

	g_fields.filename = env->GetFieldID(clazz, "mFileName", "Ljava/lang/String;");
	if (g_fields.filename == NULL)
	{
	   jniThrowException(env, "java/lang/RuntimeException", "Can't find TimeShiftInfo.mFileName");
	   return;
	}

	g_fields.filesize = env->GetFieldID(clazz, "mFileSize", "J");
	if (g_fields.filesize == NULL)
	{
	   jniThrowException(env, "java/lang/RuntimeException", "Can't find TimeShiftInfo.mFileSize");
	   return;
	}
}

static void liveplayer_native_setup(JNIEnv *env, jobject thiz, jobject weak_this)
{	
    CAndroidLivePlayer* mp = new CAndroidLivePlayer(g_strPackageName);

    if (mp == NULL)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return;
    }

	mp->SetLogLevel(gs_CurrentLogLevel);
		
    // create new listener and give it to MediaPlayer
    CAndroidLivePlayerListener* listener = new CAndroidLivePlayerListener(env, thiz, weak_this);
    mp->SetListener(listener);

    // Stow our new C++ LivePlayer in an opaque field in the Java object.
    SetLivePlayer(env, thiz, mp);
}

static int liveplayer_setLogLevel(JNIEnv *env, jobject thiz,int level)
{
	if(!(level>=CLL_EMERG && level<=CLL_NOTSET))
		return -1;
	
	gs_CurrentLogLevel = (eUtilLogLevel)level;

	_LogSetLevel((eUtilLogLevel)level,1);
	
	CAndroidLivePlayer* mp = GetLivePlayer(env, thiz);
	
    if (mp == NULL )
    {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return -1;
    }

	return mp->SetLogLevel(level);
}

static void liveplayer_native_finalize(JNIEnv *env, jobject thiz)
{
    INFO("native_finalize");
    liveplayer_release(env, thiz);
}

static int liveplayer_setVideoSurface(JNIEnv *env, jobject thiz)
{
    CAndroidLivePlayer* mp = GetLivePlayer(env, thiz);
    if (mp == NULL )
    {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return -1;
    }

	jobject surface = env->GetObjectField(thiz, g_fields.surface);

	if (surface != NULL)
    {
#if PLATFORM <= 17
        Surface* native_surface = GetSurface(env, surface);
        return mp->SetVideoSurface(native_surface);
#else
		return mp->SetVideoSurface((void*)surface);
#endif
    }

    return mp->SetVideoSurface(NULL);
}

static int liveplayer_setDisplayType(JNIEnv *env, jobject thiz, int DType)
{
    CAndroidLivePlayer* mp = GetLivePlayer(env, thiz);
    if (mp == NULL )
    {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return -1;
    }
	
    return mp->SetDisplayType(DType);
}

static int liveplayer_updateTShiftInfo(JNIEnv *env, jobject thiz)
{
	CAndroidLivePlayer* mp = GetLivePlayer(env, thiz);
    if (mp == NULL )
    {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return -1;
    }

	jobject TShiftInfo = env->GetObjectField(thiz, g_fields.timeshiftInfo);
	
	if (TShiftInfo != NULL)
	{
		GetTimeShiftInfo(env,TShiftInfo,&g_TimeShiftInfo);
		return mp->SetTimeShiftInfo(g_TimeShiftInfo);
	}
	
	return -1;
}

static bool liveplayer_isTShiftRun(JNIEnv *env, jobject thiz)
{
	CAndroidLivePlayer* mp = GetLivePlayer(env, thiz);
    if (mp == NULL )
    {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return -1;
    }

	return mp->IsTShiftRun();
}

static int liveplayer_getPlayMode(JNIEnv *env, jobject thiz)
{
	CAndroidLivePlayer* mp = GetLivePlayer(env, thiz);
    if (mp == NULL )
    {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return -1;
    }

	return mp->GetPlayMode();
}

static int liveplayer_setDataSource(JNIEnv *env, jobject thiz, jstring path,int type)
{
    CAndroidLivePlayer* mp = GetLivePlayer(env, thiz);
    if (mp == NULL )
    {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return -1;
    }

    if (path == NULL)
    {
        jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return -1;
    }

	jobject TShiftInfo = env->GetObjectField(thiz, g_fields.timeshiftInfo);
	
	if (TShiftInfo != NULL)
	{
		GetTimeShiftInfo(env,TShiftInfo,&g_TimeShiftInfo);
		mp->SetTimeShiftInfo(g_TimeShiftInfo);
	}
	
    jboolean iscopy;
    const char *pathStr = env->GetStringUTFChars(path, &iscopy);
    if (pathStr == NULL)
    {
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return -1;
    }
	
    INFO("setDataSource: path 0x%x,%s", pathStr,pathStr);
    int opStatus = mp->SetDataSource(pathStr,type);

    env->ReleaseStringUTFChars(path, pathStr);

	return opStatus;
}

static int liveplayer_start(JNIEnv *env, jobject thiz)
{
    CAndroidLivePlayer* mp = GetLivePlayer(env, thiz);
    if (mp == NULL )
    {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return -1;
    }
    return mp->Start();
}

static int liveplayer_stop(JNIEnv *env, jobject thiz)
{
    CAndroidLivePlayer* mp = GetLivePlayer(env, thiz);
    if (mp == NULL )
    {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return -1;
    }
    return mp->Stop();
}

static int liveplayer_pause(JNIEnv *env, jobject thiz)
{
    CAndroidLivePlayer* mp = GetLivePlayer(env, thiz);
    if (mp == NULL )
    {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return -1;
    }
    return mp->Pause();
}

static int liveplayer_seek(JNIEnv *env, jobject thiz, int msec)
{
    CAndroidLivePlayer* mp = GetLivePlayer(env, thiz);
    if (mp == NULL )
    {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return -1;
    }
    double sec = msec / 1000.0;
    INFO("seekTo: %d(msec), %f(sec)", msec, sec);
	return mp->Seek(sec);
}

static int liveplayer_release(JNIEnv *env, jobject thiz)
{
    DEBUG("release");
    CAndroidLivePlayer* mp = SetLivePlayer(env, thiz, 0);
    if (mp != NULL)
    {
        // this prevents native callbacks after the object is released
        mp->SetListener(NULL);
        mp->Release ();
		delete mp;
		return 0;
    }
	return -1;
}

static int liveplayer_reset(JNIEnv *env, jobject thiz)
{
    CAndroidLivePlayer* mp = GetLivePlayer(env, thiz);
    if (mp == NULL )
    {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return -1;
    }
    return mp->Reset();
}

static int liveplayer_setVolume(JNIEnv *env, jobject thiz, float leftVolume, float rightVolume)
{
    CAndroidLivePlayer* mp = GetLivePlayer(env, thiz);
    if (mp == NULL )
    {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return -1;
    }
    return mp->SetVolume(leftVolume, rightVolume);
}

static int liveplayer_getVideoWidth(JNIEnv *env, jobject thiz)
{
    CAndroidLivePlayer* mp = GetLivePlayer(env, thiz);
    if (mp == NULL )
    {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return -1;
    }
    return mp->GetVideoWidth();
}

static int liveplayer_getVideoHeight(JNIEnv *env, jobject thiz)
{
    CAndroidLivePlayer* mp = GetLivePlayer(env, thiz);
    if (mp == NULL )
    {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return -1;
    }
    return mp->GetVideoHeight();
}

static jboolean liveplayer_isPlaying(JNIEnv *env, jobject thiz)
{
    CAndroidLivePlayer* mp = GetLivePlayer(env, thiz);
    if (mp == NULL )
    {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return false;
    }
    const jboolean is_playing = mp->IsPlaying();

    return is_playing;
}

static int liveplayer_getCurrentPosition(JNIEnv *env, jobject thiz)
{
    CAndroidLivePlayer* mp = GetLivePlayer(env, thiz);
    if (mp == NULL )
    {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return -1;
    }
    double sec = mp->GetCurrentPosition();

    return sec * 1000;
}

static int liveplayer_getDuration(JNIEnv *env, jobject thiz)
{
    CAndroidLivePlayer* mp = GetLivePlayer(env, thiz);
    if (mp == NULL )
    {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return -1;
    }
    double sec = mp->GetDuration();

    return sec * 1000;
}

static bool liveplayer_isSeekable(JNIEnv *env, jobject thiz)
{
    CAndroidLivePlayer* mp = GetLivePlayer(env, thiz);
    if (mp == NULL )
    {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return -1;
    }
	
    return mp->IsSeekable();
}
static bool liveplayer_isCanPause(JNIEnv *env, jobject thiz)
{
	CAndroidLivePlayer* mp = GetLivePlayer(env, thiz);
    if (mp == NULL )
    {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return -1;
    }
	
    return mp->IsCanPause();
}


static JNINativeMethod gMethods[] = {
    {"native_init",         "()V",                              (void *)liveplayer_native_init},
    {"native_setup",        "(Ljava/lang/Object;)V",            (void *)liveplayer_native_setup},
    {"native_finalize",     "()V",                              (void *)liveplayer_native_finalize},
    {"_setLogLevel",        "(I)I",                             (void *)liveplayer_setLogLevel},
    {"_setDisplayType",     "(I)I",                             (void *)liveplayer_setDisplayType},
    {"_setVideoSurface",    "()I",                              (void *)liveplayer_setVideoSurface},
    {"_setDataSource",      "(Ljava/lang/String;I)I",            (void *)liveplayer_setDataSource},
    {"_start",              "()I",                              (void *)liveplayer_start},
    {"_stop",               "()I",                              (void *)liveplayer_stop},
    {"_pause",              "()I",                              (void *)liveplayer_pause},
    {"_seek",               "(I)I",                             (void *)liveplayer_seek},
    {"_release",            "()I",                              (void *)liveplayer_release},
    {"_reset",              "()I",                              (void *)liveplayer_reset},
    {"_setVolume",          "(FF)I",                            (void *)liveplayer_setVolume},
    {"getVideoWidth",       "()I",                              (void *)liveplayer_getVideoWidth},
    {"getVideoHeight",      "()I",                              (void *)liveplayer_getVideoHeight},
    {"isPlaying",           "()Z",                              (void *)liveplayer_isPlaying},
    {"getCurrentPosition",  "()I",                              (void *)liveplayer_getCurrentPosition},
    {"_getDuration",        "()I",                              (void *)liveplayer_getDuration},
    {"isSeekable",          "()Z",                              (void *)liveplayer_isSeekable},
    {"isCanPause",          "()Z",                              (void *)liveplayer_isCanPause},
    {"updateTShiftInfo",    "()I",                              (void *)liveplayer_updateTShiftInfo},
    {"isTShiftRun",    		"()Z",                              (void *)liveplayer_isTShiftRun},    
    {"getPlayMode",    		"()I",                              (void *)liveplayer_getPlayMode},    
};

__attribute__ ((visibility("default"))) jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv ((void**) &env, JNI_VERSION_1_4) != JNI_OK)
    {
        ERROR ("ERROR: GetEnv failed\n");
        goto bail;
    }
    assert(env != NULL);

    if (AndroidRuntime::registerNativeMethods (env, PLAYER_CLASS_DIR, gMethods, NELEM(gMethods)) < 0)
    {
    	ERROR("ERROR: LivePlayer native registration failed\n");
        goto bail;
    }

	if (env->GetJavaVM(&g_jvm) < 0) 
	{ 
		ERROR("Could not get handle to the VM");    
	}
	
    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

bail:
    return result;
}

JNIEnv* getJNIEnv()
{
    JNIEnv* env = NULL;
    if (g_jvm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK)
    {
    	ERROR ("ERROR: GetEnv failed\n");

    	return NULL;
    }
    return env;
}
