#ifndef __ANDROID_VIDEO_NATIVEWINDOW_RENDER_H__
#define __ANDROID_VIDEO_NATIVEWINDOW_RENDER_H__

#include <stdint.h>
#include <sys/types.h>
// SurfaceFlinger
#include <gui/Surface.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>

#include <SkRect.h>
#include "ivideorender.h"
#include "playcore.h"

class UtilSingleLock;
class CAndroidVideoSurfaceRender:public IVideoRender
{

public:
    CAndroidVideoSurfaceRender ();
    ~CAndroidVideoSurfaceRender ();

	int  SetSurface (void* surface);
    int	 Init (AVStream *vstream);
	bool IsInit();
    bool GetVideoSize (int* w, int* h);
    int  ShowPicture (AVFrame *pFrame);
	void Flush();
	void SetDisplayType(DisplayType DType);
    int  Reset (); //mean uninit or clean the object;

protected:
	void*	GetSurfacePtr(const ANativeWindow_Buffer &Info);
	int  	GetSurfacePitch(const ANativeWindow_Buffer &Info);
	SkRect  MakeDstRect(const ANativeWindow_Buffer &Info,const DisplayType DType);
	int  	MakeAVFrame(const ANativeWindow_Buffer &Info,const SkRect Rect);
	int  	SetVideoSize (int width, int height);
private:
    bool             mInitialized;
    jobject			 mSurface;
	ANativeWindow *	 mWindow;

    // frame buffer support
	SkRect  					   mRectDst;
	ANativeWindow_Buffer		   mWindowBuffer;


	AVFrame*				mScaleFrame;
	void*					mPicBuf;
	AVStream*			    mAVStream;
    // frame and surface size.
    int 					miVideoWidth;
    int 					miVideoHeight;
	enum PixelFormat		miVideoPixfmt;
	int						mDither;

	DisplayType				mDisplayType;
	UtilSingleLock			mLock;
};

#endif
