#ifndef __ANDROID_VIDEO_SURFACE_RENDER_H__
#define __ANDROID_VIDEO_SURFACE_RENDER_H__

#include <stdint.h>
#include <sys/types.h>

// SurfaceFlinger
#if PLATFORM < 8
#include <ui/Surface.h>
#elif PLATFORM >= 8 && PLATFORM < 16
#include <surfaceflinger/Surface.h>
#elif PLATFORM>=16
#include <gui/Surface.h>
#else
#error "?"
#endif

#include <SkRect.h>
#include "ivideorender.h"
#include "playcore.h"

class UtilSingleLock;
class CAndroidVideoSurfaceRender:public IVideoRender
{

public:
    CAndroidVideoSurfaceRender ();
    ~CAndroidVideoSurfaceRender ();

    int  SetSurface (const android::sp<android::Surface>& surface);
    int	 Init (AVStream *vstream);
	bool IsInit();
    bool GetVideoSize (int* w, int* h);
    int  ShowPicture (AVFrame *pFrame);
	void Flush();
	void SetDisplayType(DisplayType DType);
    int  Reset (); //mean uninit or clean the object;

protected:
	void*	GetSurfacePtr(const android::Surface::SurfaceInfo &Info);
	int  	GetSurfacePitch(const android::Surface::SurfaceInfo &Info);
	SkRect  MakeDstRect(const android::Surface::SurfaceInfo &Info,const DisplayType DType);
	int  	MakeAVFrame(const android::Surface::SurfaceInfo &Info,const SkRect Rect);
	int  	SetVideoSize (int width, int height);
private:
    bool                           mInitialized;
    android::sp<android::Surface>  mSurface;

    // frame buffer support
	SkRect  					   mRectDst;
	android::Surface::SurfaceInfo  mSurfaceInfo;


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
