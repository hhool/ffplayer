#include <common.h>
#include <libyuv.h>
#include <yuv2rgb.h>
#include <util_log.h>
#include <util_lock.h>
#include "android_videosurface_render.h"

using  namespace libyuv;

extern int		 g_nSDKVersion;

CAndroidVideoSurfaceRender::CAndroidVideoSurfaceRender ():mLock("SurfaceRender")
{
    mInitialized 		= false;
    mSurface 			= NULL;

    miVideoWidth 		= -1;
    miVideoHeight 		= -1;
	miVideoPixfmt	 	= PIX_FMT_NONE;
	mDither				= 0;
	mScaleFrame			= NULL;
	mPicBuf				= NULL;

	mDisplayType 		= DT_4_3_Type;
   	INFO("0x%x",this);
}

CAndroidVideoSurfaceRender::~CAndroidVideoSurfaceRender () 
{
	INFO("0x%x",this);
    Reset ();
}

int CAndroidVideoSurfaceRender::SetSurface (const android::sp<android::Surface>& surface) 
{
	mLock.Lock();
	
    if (surface != NULL)
	{
   		DEBUG("surface isValid %d",surface->isValid());
        mSurface = surface;
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

	INFO ("video = %d x %d", codecCtx->width, codecCtx->height);
   
	mRectDst.setEmpty();

	memset(&mSurfaceInfo,0,sizeof(android::Surface::SurfaceInfo));
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

int  CAndroidVideoSurfaceRender::GetSurfacePitch(const android::Surface::SurfaceInfo &Info)
{
#if PLATFORM < 8
	if(g_nSDKVersion == 4)
	{
		return Info.bpr;
	}

	return Info.bpr*2;
#elif PLATFORM >= 8
	return Info.s*2;
#endif
}

SkRect CAndroidVideoSurfaceRender::MakeDstRect(const android::Surface::SurfaceInfo &Info,const DisplayType DType)
{
	SkRect  DstRect;
	int DstX		= 0;
	int	DstY		= 0;
	int DstWidth 	= Info.w;
	int DstHeight	= Info.h;
	
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

int CAndroidVideoSurfaceRender::MakeAVFrame(const android::Surface::SurfaceInfo &Info,const SkRect Rect)
{
	int DstX		= 0;
	int	DstY		= 0;
	int DstWidth 	= Info.w;
	int DstHeight	= Info.h;
	
	DEBUG("Scale Before DstWidth %d DstHeight %d",DstWidth,DstHeight);

	if(miVideoPixfmt != PIX_FMT_YUV420P && miVideoPixfmt != PIX_FMT_YUVJ420P )
	{
		ERROR ("error! Video is not YUV420P! %d",miVideoPixfmt);
		return ERR_INVALID_PARAM;
	}

	if(Info.w*Info.h != mSurfaceInfo.w*mSurfaceInfo.h)
	{
		mSurfaceInfo = Info;

		if(!!mPicBuf)
		{
			free(mPicBuf);
			mPicBuf = NULL;
		}
	}
	
	if(mPicBuf == NULL)
	{
		mPicBuf  = malloc(PlayCore::GetInstance()->avpicture_get_size(miVideoPixfmt, Info.w , Info.h)); //avpicture_get_size(miVideoPixfmt, mRectDst.width() , mRectDst.height());

		if(mPicBuf == NULL)
		{
			ERROR("PicBuf 	= malloc(Size) Failed");
			return ERR_OUT_OF_MEMORY;
		}
	}
			
	if(mRectDst != Rect)
	{
		mRectDst = Rect;

		INFO("format %d w %d h %d",mSurfaceInfo.format,mSurfaceInfo.w,mSurfaceInfo.h);
		
		PlayCore::GetInstance()->avpicture_fill((AVPicture *)mScaleFrame, (unsigned char*)mPicBuf,miVideoPixfmt, mRectDst.width(), mRectDst.height());
	}

	return ERR_NONE;
}

void* CAndroidVideoSurfaceRender::GetSurfacePtr(const android::Surface::SurfaceInfo &Info)
{
#if PLATFORM < 8
    return (reinterpret_cast<int>(Info.bits) < 0x0200 ? Info.base : Info.bits);
#elif PLATFORM >= 8
    return Info.bits;
#endif
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
	android::Surface::SurfaceInfo Info;

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

	if (!mSurface->isValid())
	{
		DEBUG("!mSurface->isValid()");
		mLock.Unlock();
		return ERR_NONE;
	}
	if (mSurface->lock(&Info) < 0)
	{
		DEBUG("mSurface->lock(&mSurfaceInfo) < 0");	
		mLock.Unlock();
		return ERR_NONE;
	}
	
	Pitch 		= GetSurfacePitch(Info);
	
	PtrSurface 	= GetSurfacePtr(Info);
	
	if(PtrSurface == NULL)
	{
		DEBUG("PtrSurface is NULL");
		mSurface->unlockAndPost();
		mLock.Unlock();
		return ERR_NONE;		
	}

	RectDst = MakeDstRect(Info,mDisplayType);
	
	if(MakeAVFrame(Info,RectDst) != ERR_NONE)
	{
		DEBUG("MakeAVFrameBySurfaceInfo");	
		mSurface->unlockAndPost();
		mLock.Unlock();
		return ERR_NONE;
	}
		
	DEBUG("mRectDst.left() %d, mRectDst.top() %d,mRectDst.width() %d,mRectDst.height() %d" \
		"use asm  mScaleFrame 0x%x,miVideoWidth %d miVideoHeight %d "\
		"pFrame->data[0] 0x%x pFrame->linesize[0] %d,pFrame->linesize[1] %d " \
		"mScaleFrame->data[0] 0x%x,mScaleFrame->linesize[0] %d, mScaleFrame->linesize[1] %d " \
		"surface.w %d,surface.h %d mSurfaceInfo.s %d ",
		mRectDst.fLeft,mRectDst.fTop,mRectDst.width(),mRectDst.height(),
		mScaleFrame,miVideoWidth,miVideoHeight,
		pFrame->data[0],pFrame->linesize[0],pFrame->linesize[1],
		mScaleFrame->data[0] ,mScaleFrame->linesize[0], mScaleFrame->linesize[1],
		mSurfaceInfo.w,mSurfaceInfo.h,Pitch);

	I420Scale(pFrame->data[0],pFrame->linesize[0],
			  pFrame->data[1],pFrame->linesize[1],
			  pFrame->data[2],pFrame->linesize[2],
			  miVideoWidth,miVideoHeight,
			  (unsigned char*)mScaleFrame->data[0],mScaleFrame->linesize[0],
			  (unsigned char*)mScaleFrame->data[1],mScaleFrame->linesize[1],
			  (unsigned char*)mScaleFrame->data[2],mScaleFrame->linesize[2],
			  mRectDst.width(),mRectDst.height(),kFilterNone);

	memset(PtrSurface,0,mSurfaceInfo.w*mSurfaceInfo.h*2);

	yuv420_2_rgb565((unsigned char*)PtrSurface+(int)mRectDst.fLeft*2+(int)mRectDst.fTop*Pitch, 
			mScaleFrame->data[0],
			mScaleFrame->data[1],
			mScaleFrame->data[2],
			mRectDst.width(),
			mRectDst.height(),
			mScaleFrame->linesize[0],
			mScaleFrame->linesize[1], 
			Pitch,			
			yuv2rgb565_table,
			mDither++);
		
	if (mSurface->unlockAndPost() < 0)
	{
		DEBUG("mSurface->unlockAndPost() < 0");	
		mLock.Unlock();
		return ERR_NONE;
	}

	mLock.Unlock();
	
    return ERR_NONE;
}
void  CAndroidVideoSurfaceRender::Flush()
{
	void*   PtrSurface;
	
	if(mSurface == NULL)
	{
		ERROR("mSurface == NULL");
		return ;
	}

	if (!mSurface->isValid())
	{
		ERROR("!mSurface->isValid()");	
		return ;
	}
	if (mSurface->lock(&mSurfaceInfo) < 0)
	{
		ERROR("mSurface->lock(&mSurfaceInfo) < 0");	
		return ;
	}

	DEBUG("format %d w %d h %d",mSurfaceInfo.format,mSurfaceInfo.w,mSurfaceInfo.h);
	
	PtrSurface 	= GetSurfacePtr(mSurfaceInfo);
	
	if(!!PtrSurface)
	{
		memset(PtrSurface,0,mSurfaceInfo.w*mSurfaceInfo.h*2);
	}
		
	if (mSurface->unlockAndPost() < 0)
	{
		ERROR("mSurface->unlockAndPost() < 0");	
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
