package com.pbi.player;

import android.content.Context;
import android.net.Uri;
import android.os.Build;
import android.os.PowerManager;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;

public class LivePlayer {
    static {
        Init();
    }

    private static final void Init() {
        int version = Integer.parseInt(Build.VERSION.SDK);

        mPackageName = "com.pbi.live";

        System.loadLibrary("ffpcore");

        if (version < 8) {
            System.loadLibrary("ffplay-4-jni");
        } else if (version == 8) {
            System.loadLibrary("ffplay-8-jni");
        } else if (version >= 9 && version < 14) {
            System.loadLibrary("ffplay-9-jni");
        } else if (version >= 14 && version < 16) {
            System.loadLibrary("ffplay-14-jni");
        } else if (version == 16 || version == 17) {
            System.loadLibrary("ffplay-16-jni");
        } else if (version >= 18 && version <= 20) {
            System.loadLibrary("ffplay-18-jni");
        } else {
            System.loadLibrary("ffplay-18-jni");
        }

        native_init();
    }

    private static final int LOG_LEVEL_EMERG = 0;
    private static final int LOG_LEVEL_FATAL = 1;
    private static final int LOG_LEVEL_ALERT = 2;
    private static final int LOG_LEVEL_CRIT = 3;
    private static final int LOG_LEVEL_ERROR = 4;
    private static final int LOG_LEVEL_INFO = 5;
    private static final int LOG_LEVEL_WARN = 6;
    private static final int LOG_LEVEL_NOTICE = 7;
    private static final int LOG_LEVEL_DEBUG = 8;
    private static final int LOG_LEVEL_VERBOSE = 9;

    public static final int DType_Auto_Scale = 0;
    public static final int DType_4_3_Scale = 1;
    public static final int DType_16_9_Scale = 2;

    public static final int MEDIA_PLAY_NOP = 0;
    public static final int MEDIA_PLAY_LOCAL = 1;
    public static final int MEDIA_PLAY_LIVE = 2;
    public static final int MEDIA_PLAY_TSHIFT = 3;
    public static final int MEDIA_PLAY_ONDEMAND = 4;

    static public class TimeShiftInfo {
        public boolean mbAllowTimeShift;
        public String mFileName;
        public long mFileSize;
    }

    private static native final void native_init();

    private native final void native_setup(Object mediaplayer_this);

    private native final void native_finalize();

    private native int _setLogLevel(int level);

    private native int _setDisplayType(int DType);

    private native int _setVideoSurface();

    private native int _setDataSource(String path, int type);

    private native int _start();

    private native int _stop();

    private native int _pause();

    private native int _seek(int msec);

    private native int _release();

    private native int _reset();

    private native int _setVolume(float leftVolume, float rightVolume);

    private native int _getDuration();

    public native int getVideoWidth();

    public native int getVideoHeight();

    public native boolean isPlaying();

    public native int getCurrentPosition();

    public native boolean isSeekable();

    public native boolean isCanPause();

    public native int updateTShiftInfo();

    public native boolean isTShiftRun();

    public native int getPlayMode();

    private final static String TAG = "LivePlayer";
    private int mNativeContext; // accessed by native methods
    private int mListenerContext; // accessed by native methods
    private Surface mSurface; // accessed by native methods
    private TimeShiftInfo mTimeShiftInfo; //accessed by native methods
    private static String mPackageName;
    private SurfaceHolder mSurfaceHolder;
    private PowerManager.WakeLock mWakeLock = null;
    private boolean mScreenOnWhilePlaying;
    private boolean mStayAwake;

    public LivePlayer() {

        /* Native setup requires a weak reference to our object.
         * It's easier to create it here than in C++.
         */
        native_setup(this);
        _setLogLevel(LOG_LEVEL_VERBOSE);
    }

    public int setDisplayType(int DType) {
        if (DType >= DType_Auto_Scale && DType <= DType_16_9_Scale) {
            return _setDisplayType(DType);
        }
        return -1;
    }

    public void setDisplay(SurfaceHolder sh) {
        mSurfaceHolder = sh;
        if (sh != null) {
            mSurface = sh.getSurface();
        } else {
            mSurface = null;
        }
        int ret = _setVideoSurface();
        if (ret < 0) {
            Log.d(TAG, "setVideoSurface fail!");
        }
        updateSurfaceScreenOn();
    }

    /**
     * Sets the data source as a content Uri.
     *
     * @param context the Context to use when resolving the Uri
     * @param uri     the Content URI of the data you want to play
     * @throws IllegalStateException if it is called in an invalid state
     */
    public int setDataSource(Context context, Uri uri) {
        String scheme = uri.getScheme();
        if (scheme == null || scheme.equals("file")) {
            int ret = 0;

            if (uri.getPath().endsWith("mpg"))
                ret = _setDataSource("http://127.0.0.1:9906/p2p-live/124.219.23.162:3001/null/e6d903419621022afd29ff5c3c700366",
                        MEDIA_PLAY_LIVE);
            else
                ret = _setDataSource(uri.toString(), MEDIA_PLAY_LOCAL);

            if (ret < 0) {
                Log.d(TAG, "setDataSource fail!");
            }
            return ret;
        } else {
            Log.d(TAG, "unrecognized file" + uri.toString());
            return -1;
        }
    }

    public int SetTimeShiftInfo(TimeShiftInfo TShiftInfo) {
        //if(!isTShiftRun())
        {
            mTimeShiftInfo = TShiftInfo;
            return updateTShiftInfo();
        }
        //return -1;
    }

    public static final int MEDIA_OPENING = 1;
    public static final int MEDIA_OPENED = 2;
    public static final int MEDIA_SYNCING = 3;
    public static final int MEDIA_SYNC = 4;
    public static final int MEDIA_BUFFERING = 5;
    public static final int MEDIA_PLAYBACK_COMPLETE = 6;
    public static final int MEDIA_SEEK_COMPLETE = 7;
    public static final int MEDIA_PLAYING = 8;
    public static final int MEDIA_PAUSED = 9;
    public static final int MEDIA_TIMESHIFT = 10;
    public static final int MEDIA_ERROR = 100;
    public static final int MEDIA_INFO = 200;

    public static final int TSHIFT_NOP = 0;
    public static final int TSHIFT_BEGIN_RECORD = 1;
    public static final int TSHIFT_PROGRESS = 2;
    public static final int TSHIFT_END_RECORD = 3;
    public static final int TSHIFT_OPEN_SRC_ERR = 4;
    public static final int TSHIFT_OPEN_DST_ERR = 5;
    public static final int TSHIFT_READ_SRC_ERR = 6;
    public static final int TSHIFT_WRITE_DST_ERR = 7;
    public static final int TSHIFT_PLAYER_OPEN_DST_ERR = 8;

    private static void postEventFromNative(Object mediaplayer_ref,
                                            int what, int arg1, int arg2, Object obj) {
        LivePlayer Player = (LivePlayer) mediaplayer_ref;

        if (what == MEDIA_OPENING) {
            Log.d(TAG, "postEventFromNative: " + " MEDIA_OPENING " + what + " arg1 " + arg1 + " arg2 " + arg2);
        } else if (what == MEDIA_OPENED) {
            Log.d(TAG, "postEventFromNative: " + " MEDIA_OPENED " + what + " arg1 " + arg1 + " arg2 " + arg2);
            Player.start();
        } else if (what == MEDIA_SYNCING) {
            Log.d(TAG, "postEventFromNative: " + " MEDIA_SYNCING " + what + " arg1 " + arg1 + " arg2 " + arg2);
        } else if (what == MEDIA_SYNC) {
            Log.d(TAG, "postEventFromNative: " + " MEDIA_SYNC " + what + " arg1 " + arg1 + " arg2 " + arg2);
        } else if (what == MEDIA_BUFFERING) {
            Log.d(TAG, "postEventFromNative: " + " MEDIA_BUFFERING " + what + " arg1 " + arg1 + " arg2 " + arg2);
        } else if (what == MEDIA_PLAYBACK_COMPLETE) {
            Log.d(TAG, "postEventFromNative: " + " MEDIA_PLAYBACK_COMPLETE " + what + " arg1 " + arg1 + " arg2 " + arg2);
            if (Player.getPlayMode() == MEDIA_PLAY_TSHIFT) {
                Log.d(TAG, "TimeShift Play has finished,turn to live mode or other view");
            }
        } else if (what == MEDIA_PLAYING) {
            Log.d(TAG, "postEventFromNative: " + " MEDIA_PLAYING " + what + " arg1 " + arg1 + " arg2 " + arg2);
        } else if (what == MEDIA_PAUSED) {
            Log.d(TAG, "postEventFromNative: " + " MEDIA_PAUSED " + what + " arg1 " + arg1 + " arg2 " + arg2);
        } else if (what == MEDIA_SEEK_COMPLETE) {
            Log.d(TAG, "postEventFromNative: " + " MEDIA_SEEK_COMPLETE " + what + " arg1 " + arg1 + " arg2 " + arg2);
        } else if (what == MEDIA_TIMESHIFT) {
            if (arg1 == TSHIFT_BEGIN_RECORD) {
                Log.d(TAG, "time shift begin record");
            } else if (arg1 == TSHIFT_PROGRESS) {
                //Log.d(TAG, "time shift begin record,arg2 is size "+arg2+" has record");
            } else if (arg1 == TSHIFT_END_RECORD) {
                Log.d(TAG, "time shift finish record");
            }
        } else if (what == MEDIA_ERROR) {
            Log.d(TAG, "postEventFromNative: " + " MEDIA_ERROR IsPlaying " + Player.isPlaying() + " " + what + " arg1 " + arg1 + " arg2 " + arg2);
            if (arg1 == MEDIA_TIMESHIFT) {
                if (arg2 >= TSHIFT_OPEN_SRC_ERR && arg2 <= TSHIFT_WRITE_DST_ERR) {
                    Log.d(TAG, "time shift record has error");
                } else if (arg2 == TSHIFT_PLAYER_OPEN_DST_ERR) {
                    Log.d(TAG, "time shift can't play");
                }
            }

        }
    }

    /**
     * Starts or resumes playback. If playback had previously been paused,
     * playback will continue from where it was paused. If playback had
     * been stopped, or never started before, playback will start at the
     * beginning.
     *
     * @throws IllegalStateException if it is called in an invalid state
     */
    public void start() throws IllegalStateException {
        stayAwake(true);
        _start();
    }

    /**
     * Stops playback after playback has been stopped or paused.
     *
     * @throws IllegalStateException if the internal player engine has not been
     *                               initialized.
     */
    public void stop() throws IllegalStateException {
        stayAwake(false);
        _stop();
    }

    /**
     * Pauses playback. Call start() to resume.
     *
     * @throws IllegalStateException if the internal player engine has not been
     *                               initialized.
     */
    public void pause() throws IllegalStateException {
        stayAwake(false);
        _pause();
    }

    /**
     * Seek playback.
     *
     * @throws IllegalStateException if the internal player engine has not been
     *                               initialized.
     */
    public void seekTo(int msec) throws IllegalStateException {
        stayAwake(false);
        if (isSeekable()) {
            _seek(msec);
        }
    }

    public int getDuration() {
        int duration = _getDuration();
        if (duration < 0)
            return -1;
        return duration;
    }

    /**
     * Releases resources associated with this LivePlayer object.
     * It is considered good practice to call this method when you're
     * done using the LivePlayer.
     */
    public void release() {
        stayAwake(false);
        updateSurfaceScreenOn();
        _release();
    }

    /**
     * Resets the LivePlayer to its uninitialized state. After calling
     * this method, you will have to initialize it again by setting the
     * data source and calling prepare().
     */
    public void reset() {
        stayAwake(false);
        _reset();
    }

    public int setVolume(float leftVolume, float rightVolume) {
        return _setVolume(leftVolume, rightVolume);
    }

    /**
     * Set the low-level power management behavior for this LivePlayer.  This
     * can be used when the LivePlayer is not playing through a SurfaceHolder
     * set with {@link #setDisplay(SurfaceHolder)} and thus can use the
     * high-level {@link #setScreenOnWhilePlaying(boolean)} feature.
     * <p/>
     * <p>This function has the LivePlayer access the low-level power manager
     * service to control the device's power usage while playing is occurring.
     * The parameter is a combination of {@link android.os.PowerManager} wake flags.
     * Use of this method requires {@link android.Manifest.permission#WAKE_LOCK}
     * permission.
     * By default, no attempt is made to keep the device awake during playback.
     *
     * @param context the Context to use
     * @param mode    the power/wake mode to set
     * @see android.os.PowerManager
     */
    public void setWakeMode(Context context, int mode) {
        boolean washeld = false;
        if (mWakeLock != null) {
            if (mWakeLock.isHeld()) {
                washeld = true;
                mWakeLock.release();
            }
            mWakeLock = null;
        }

        PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
        mWakeLock = pm.newWakeLock(mode | PowerManager.ON_AFTER_RELEASE, LivePlayer.class.getName());
        mWakeLock.setReferenceCounted(false);
        if (washeld) {
            mWakeLock.acquire();
        }
    }

    /**
     * Control whether we should use the attached SurfaceHolder to keep the
     * screen on while video playback is occurring.  This is the preferred
     * method over {@link #setWakeMode} where possible, since it doesn't
     * require that the application have permission for low-level wake lock
     * access.
     *
     * @param screenOn Supply true to keep the screen on, false to allow it
     *                 to turn off.
     */
    public void setScreenOnWhilePlaying(boolean screenOn) {
        if (mScreenOnWhilePlaying != screenOn) {
            mScreenOnWhilePlaying = screenOn;
            updateSurfaceScreenOn();
        }
    }

    private void stayAwake(boolean awake) {
        if (mWakeLock != null) {
            if (awake && !mWakeLock.isHeld()) {
                mWakeLock.acquire();
            } else if (!awake && mWakeLock.isHeld()) {
                mWakeLock.release();
            }
        }
        mStayAwake = awake;
        updateSurfaceScreenOn();
    }

    private void updateSurfaceScreenOn() {
        if (mSurfaceHolder != null) {
            mSurfaceHolder.setKeepScreenOn(mScreenOnWhilePlaying && mStayAwake);
        }
    }
}
