#include <common.h>
#include <libyuv.h>
#include <yuv2rgb.h>
#include <util_log.h>
#include <util_lock.h>
#include "android_nativewindow_render.h"
#include <jni.h>

using  namespace libyuv;

extern JNIEnv*   getJNIEnv();
extern int		 g_nSDKVersion;


CAndroidVideoSurfaceRender::CAndroidVideoSurfaceRender ():mLock("SurfaceRender")
{
    mInitialized 		= false;
    mSurface 			= NULL;
	mWindow				= NULL;

    miVideoWidth 		= -1;
    miVideoHeight 		= -1;
	miVideoPixfmt	 	= PIX_FMT_NONE;
	mDither				= 0;
	mScaleFrame			= NULL;
	mPicBuf				= NULL;

	mDisplayType 		= DT_4_3_Type;
   	DEBUG("0x%x",this);
}

CAndroidVideoSurfaceRender::~CAndroidVideoSurfaceRender () 
{
	DEBUG("0x%x",this);
    Reset ();
	if(!!mWindow)
		ANativeWindow_release(mWindow);
}

int CAndroidVideoSurfaceRender::SetSurface (void* surface)
{
	mLock.Lock();
	
    if (surface != NULL)
	{
        mSurface = (jobject)surface;
		JNIEnv* env = getJNIEnv();
		mWindow= ANativeWindow_fromSurface(env, mSurface);
	} 
	else
	{
   		DEBUG("mSurface IS Set NULL");
        mSurface = NULL;
    }

	mLock.Unlock();
	
    return ERR_NONE;
}

int CAndroidVideoSurfaceRender::Init (AVStream *vstream) 
{
    // release resources if previously initialized
	mInitialized = false;

    if (vstream == NULL) 
	{
        ERROR ("vstream == NULL!");
        return ERR_INVALID_PARAM;
    }

	mAVStream = vstream;
	
    AVCodecContext *codecCtx = vstream->codec;
    if (codecCtx == NULL) 
	{
        ERROR ("error! codecCtx == NULL!");
        return ERR_INVALID_PARAM;
    }
	
    SetVideoSize (codecCtx->width, codecCtx->height);

	miVideoPixfmt = codecCtx->pix_fmt;
	
	if(miVideoPixfmt != PIX_FMT_YUV420P && miVideoPixfmt != PIX_FMT_YUVJ420P )
	{
		ERROR ("error! Video is not YUV420P! %d",miVideoPixfmt);
		return ERR_INVALID_PARAM;
	}

	mScaleFrame = PlayCore::GetInstance()->avcodec_alloc_frame();
	if (mScaleFrame == NULL)
	{
		ERROR("avcodec_alloc_frame Failed");
		return ERR_OUT_OF_MEMORY;
	}

	DEBUG ("video = %d x %d", codecCtx->width, codecCtx->height);
   
	mRectDst.setEmpty();

	memset(&mWindowBuffer,0,sizeof(ANativeWindow_Buffer));
    // register frame buffers with SurfaceFlinger
    mInitialized = true;

    return ERR_NONE;
}

bool CAndroidVideoSurfaceRender::GetVideoSize (int* w, int* h) 
{
    *w = miVideoWidth;
    *h = miVideoHeight;
    return miVideoWidth > 0 && miVideoHeight > 0;
}

int  CAndroidVideoSurfaceRender::SetVideoSize (int width, int height)
{
    miVideoWidth = width;
    miVideoHeight = height;
    return ERR_NONE;
}

int  CAndroidVideoSurfaceRender::GetSurfacePitch(const ANativeWindow_Buffer &Info)
{
	return Info.stride*2;
}

SkRect CAndroidVideoSurfaceRender::MakeDstRect(const ANativeWindow_Buffer &Info,const DisplayType DType)
{
	SkRect  DstRect;
	int DstX		= 0;
	int	DstY		= 0;
	int DstWidth 	= Info.width;
	int DstHeight	= Info.height;
	
	DEBUG("Scale Before DstWidth %d DstHeight %d",DstWidth,DstHeight);

	DstRect.set(DstX, DstY, DstX+DstWidth, DstY+DstHeight);

	if(DType == DT_4_3_Type)
	{
		if(DstHeight*4 > DstWidth*3)
		{
			int Height = (DstWidth*3)/4;
			DstY	   = (DstHeight-Height)/2;
			DstHeight  = Height;
		}
		else
		{
			int Width	= (DstHeight*4)/3;
			DstX		= (DstWidth-Width)/2;
			DstWidth	=  Width;
		}
	}
	else if(DType == DT_16_9_Type)
	{
		if(DstHeight*16 > DstWidth*9)
		{
			int Height = (DstWidth*9)/16;
			DstY	   = (DstHeight-Height)/2;
			DstHeight  = Height;
		}
		else
		{
			int Width	= (DstHeight*16)/9;
			DstX		= (DstWidth-Width)/2;
			DstWidth	=  Width;
		}
	}
	
	DEBUG("Scale after DstX %d DstY %d DstWidth %d DstHeight %d",DstX,DstY,DstWidth,DstHeight);

	DstRect.set(DstX, DstY, DstX+DstWidth, DstY+DstHeight);

	return DstRect;
}

int CAndroidVideoSurfaceRender::MakeAVFrame(const ANativeWindow_Buffer &Info,const SkRect Rect)
{
	int DstX		= 0;
	int	DstY		= 0;
	int DstWidth 	= Info.width;
	int DstHeight	= Info.height;
	
	DEBUG("Scale Before DstWidth %d DstHeight %d",DstWidth,DstHeight);

	if(miVideoPixfmt != PIX_FMT_YUV420P && miVideoPixfmt != PIX_FMT_YUVJ420P )
	{
		ERROR ("error! Video is not YUV420P! %d",miVideoPixfmt);
		return ERR_INVALID_PARAM;
	}

	if(Info.width*Info.height != mWindowBuffer.width*mWindowBuffer.height)
	{
		mWindowBuffer = Info;

		if(!!mPicBuf)
		{
			free(mPicBuf);
			mPicBuf = NULL;
		}
	}
	
	if(mPicBuf == NULL)
	{
		DEBUG("%d,%d,%d",miVideoPixfmt, Info.width , Info.height);
		mPicBuf  = malloc(PlayCore::GetInstance()->avpicture_get_size(miVideoPixfmt, Info.width , Info.height)); //avpicture_get_size(miVideoPixfmt, mRectDst.width() , mRectDst.height());

		if(mPicBuf == NULL)
		{
			ERROR("PicBuf 	= malloc(Size) Failed");
			return ERR_OUT_OF_MEMORY;
		}
	}
			
	if(mRectDst != Rect)
	{
		mRectDst = Rect;

		DEBUG("format %d w %d h %d",mWindowBuffer.format,mWindowBuffer.width,mWindowBuffer.height);
		
		PlayCore::GetInstance()->avpicture_fill((AVPicture *)mScaleFrame, (unsigned char*)mPicBuf,miVideoPixfmt, Rect.width(), Rect.height());
	}

	return ERR_NONE;
}

void* CAndroidVideoSurfaceRender::GetSurfacePtr(const ANativeWindow_Buffer &Info)
{
   return Info.bits;
}


void CAndroidVideoSurfaceRender::SetDisplayType(DisplayType DType)
{
	if((DType < DT_Auto_Type && DType > DT_16_9_Type) || mDisplayType == DType)
		return ;
	
	mLock.Lock();
	
	mDisplayType = DType;

	mLock.Unlock();
}

int CAndroidVideoSurfaceRender::ShowPicture (AVFrame *pFrame) 
{
	SkRect  RectDst;
	void*   PtrSurface;
	int 	Pitch = 0;
	ANativeWindow_Buffer Info;

	mLock.Lock();
	
	if(pFrame == NULL || pFrame->data[0] == NULL || pFrame->data[1] == NULL || pFrame->data[2] == NULL)
	{
		DEBUG("pFrame %d, pFrame->data[0] %d,pFrame->data[1] %d,pFrame->data[2] %d",pFrame,pFrame->data[0],pFrame->data[1],pFrame->data[2]);

		mLock.Unlock();
		
		return ERR_INVALID_PARAM;
	}
	
	if(mSurface == NULL)
	{
		DEBUG("mSurface == NULL");
		mLock.Unlock();
		return ERR_NONE;
	}

	if (ANativeWindow_lock(mWindow,&Info,NULL) < 0)
	{
		DEBUG("ANativeWindow_lock< 0");	
		mLock.Unlock();
		return ERR_NONE;
	}
	
	DEBUG("ANativeWindow_Buffer format %d,w %d,h %d",Info.format,Info.width,Info.height);	
	Pitch 		= GetSurfacePitch(Info);
	
	PtrSurface 	= GetSurfacePtr(Info);
	
	if(PtrSurface == NULL)
	{
		DEBUG("PtrSurface is NULL");
		ANativeWindow_unlockAndPost(mWindow);
		mLock.Unlock();
		return ERR_NONE;		
	}

	RectDst = MakeDstRect(Info,mDisplayType);
	
	if(MakeAVFrame(Info,RectDst) != ERR_NONE)
	{
		ANativeWindow_unlockAndPost(mWindow);
		mLock.Unlock();
		return ERR_NONE;
	}
	
	DEBUG("miVideoWidth %d,miVideoHeight %d,RectDst.width() %d,RectDst.height() %d",miVideoWidth ,miVideoHeight ,(int)RectDst.width(),(int)RectDst.height());

	I420Scale(pFrame->data[0],pFrame->linesize[0],
			  pFrame->data[1],pFrame->linesize[1],
			  pFrame->data[2],pFrame->linesize[2],
			  miVideoWidth,miVideoHeight,
			  (unsigned char*)mScaleFrame->data[0],mScaleFrame->linesize[0],
			  (unsigned char*)mScaleFrame->data[1],mScaleFrame->linesize[1],
			  (unsigned char*)mScaleFrame->data[2],mScaleFrame->linesize[2],
			  RectDst.width(),RectDst.height(),kFilterNone);

	memset(PtrSurface,0,Info.width*Info.height*2);

	yuv420_2_rgb565((unsigned char*)PtrSurface+(int)RectDst.fLeft*2+(int)RectDst.fTop*Pitch, 
			mScaleFrame->data[0],
			mScaleFrame->data[1],
			mScaleFrame->data[2],
			RectDst.width(),
			RectDst.height(),
			mScaleFrame->linesize[0],
			mScaleFrame->linesize[1], 
			Pitch,			
			yuv2rgb565_table,
			mDither++);
		
	if (ANativeWindow_unlockAndPost(mWindow)<0)
	{
		ERROR("ANativeWindow_unlockAndPost(mWindow) < 0");	
		mLock.Unlock();
		return ERR_NONE;
	}

	mLock.Unlock();
	
    return ERR_NONE;
}

void  CAndroidVideoSurfaceRender::Flush()
{
	void*   PtrSurface;
	ANativeWindow_Buffer Info;

	if(mWindow == NULL)
	{
		ERROR("mWindow == NULL");
		return ;
	}

	if (ANativeWindow_lock(mWindow,&Info,NULL) < 0)
	{
		ERROR("mSurface->lock(&Info) < 0");	
		return ;
	}

	DEBUG("format %d w %d h %d",Info.format,Info.width,Info.height);
	
	PtrSurface 	= GetSurfacePtr(Info);
	
	if(!!PtrSurface){
		memset(PtrSurface,0,Info.width*Info.height*2);
	}
		
	if (ANativeWindow_unlockAndPost(mWindow)<0)
	{
		ERROR("ANativeWindow_unlockAndPost(mWindow) < 0");	
		return ;
	}
}

bool CAndroidVideoSurfaceRender::IsInit()
{
	return mInitialized;
}

int  CAndroidVideoSurfaceRender::Reset () 
{
	mInitialized = false;

	DEBUG("Reset mPicBuf 0x%x",mPicBuf);

	if(mPicBuf)
	{
		free(mPicBuf);
		mPicBuf = NULL;
	}

	DEBUG("Reset mScaleFrame 0x%x",mScaleFrame);
		
	if(mScaleFrame)
	{
		PlayCore::GetInstance()->av_free(mScaleFrame);
		mScaleFrame = NULL;
	}

	DEBUG("Reset finish");
	
    return ERR_NONE;
}
