#include "common.h"
#include "util_log.h"
#include "masterclock.h"
#include "android_audioopensles_render.h"


CAndroidAudioOpenSLESRender::CAndroidAudioOpenSLESRender (CMasterClock* Clock)
{
   	mEngineObject		= NULL;
	mEngineEngine		= NULL;
	
	// output mix interfaces
	mOutputMixObject	= NULL;
	mOutputMixERb		= NULL;
	
	// buffer queue player interfaces
	mPlayerObject		= NULL;
	mPlayerPlay			= NULL;
	mPlayerBufferQueue	= NULL;
	mPlayerMuteSolo		= NULL;
	mPlayerVolume		= NULL;
   

	mState				= AR_NONE;
	mbFlushed			= true;
	memset(mSliceBuf,0,BUFFER_SIZE);
	CBaseAudioRender::SetClock(Clock);
	INFO("0x%x",this);
}

CAndroidAudioOpenSLESRender::~CAndroidAudioOpenSLESRender () 
{
	INFO("0x%x",this);
	Close () ;

	Reset ();
}

int CAndroidAudioOpenSLESRender::Reset () 
{
	CBaseAudioRender::BufferReset();
	
    return ERR_NONE;
}

ARState CAndroidAudioOpenSLESRender::GetState()
{
	return mState;
}

bool CAndroidAudioOpenSLESRender::IsOpened()
{
	if(mEngineObject != NULL && mEngineEngine != NULL)
	{
        return true;
    }
	
	return false;
}

int CAndroidAudioOpenSLESRender::Open (AudioFormat audioFormat) 
{
	SLresult result;
    SLint32 flags;
    SLDataFormat_PCM format_pcm;
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq;
	int format = 0;
	
    INFO ("open(%u, %d, %d, %d)", audioFormat.SampleRate, audioFormat.Channels, audioFormat.Bits);

    if(mEngineObject != NULL && mEngineEngine != NULL)
	{
        return ERR_NONE;
    }

    Close();

    switch((audioFormat.Bits-1)/8)
	{
	case 0: // SAMPLE_FMT_U8
	    {
			format = SL_PCMSAMPLEFORMAT_FIXED_8;
	        INFO("SL_PCMSAMPLEFORMAT_FIXED_8");
	        break;
		}
	case 1: // SAMPLE_FMT_S16
	    {
			format = SL_PCMSAMPLEFORMAT_FIXED_16;
	        INFO("SL_PCMSAMPLEFORMAT_FIXED_16");
	        break;
		}
	default:
	   {
	   		ERROR(("audio track format %d is not supported"), (audioFormat.Bits-1)/8);
	        return ERR_DEVICE_ERROR;
	   }
    }

    // create engine
    result = slCreateEngine(&mEngineObject, 0, NULL, 0, NULL, NULL);
    if(result)
	{
        ERROR("slCreateEngine %d",result);
        return ERR_DEVICE_ERROR;
    }

    // realize the engine
    result = (*mEngineObject)->Realize(mEngineObject, SL_BOOLEAN_FALSE);//fix
    if(result)
	{
        ERROR("Realize %d",result);
		Close ();
        return ERR_DEVICE_ERROR;
    }
    

    // get the engine interface, which is needed in order to create other objects
    result = (*mEngineObject)->GetInterface(mEngineObject, SL_IID_ENGINE, &mEngineEngine);
    if(result)
	{
        ERROR("mEngineObject::GetInterface SL_IID_ENGINE %d",result);
		Close ();
        return ERR_DEVICE_ERROR;
    }

    // create output mix, with environmental reverb specified as a non-required interface
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    result = (*mEngineEngine)->CreateOutputMix(mEngineEngine, &mOutputMixObject, 1, ids, req);
    if(result)
	{
        ERROR("mEngineEngine::CreateOutputMix %d",result);
		Close();
        return ERR_DEVICE_ERROR;
    }


    // realize the output mix
    result = (*mOutputMixObject)->Realize(mOutputMixObject, SL_BOOLEAN_FALSE);
    if(result)
	{
        ERROR("mOutputMixObject::Realize %d",result);
        Close();
        return ERR_DEVICE_ERROR;
    }

    // configure audio source
    loc_bufq.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
    loc_bufq.numBuffers = 2;
    format_pcm.formatType		= SL_DATAFORMAT_PCM;
    format_pcm.numChannels		= ((audioFormat.Bits+7)/8) >=2 ? 2 : 1;
    format_pcm.samplesPerSec   	= audioFormat.SampleRate*1000;
    format_pcm.bitsPerSample    = format;
    format_pcm.containerSize    = format;
    format_pcm.channelMask      = ((audioFormat.Bits+7)/8) >=2 ? SL_SPEAKER_FRONT_LEFT|SL_SPEAKER_FRONT_RIGHT : SL_SPEAKER_FRONT_CENTER;
    format_pcm.endianness       = SL_BYTEORDER_LITTLEENDIAN;       
    SLDataSource audioSrc 		= {&loc_bufq, &format_pcm};
    
    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, mOutputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};
    // create audio player
    const SLInterfaceID ids2[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND,
            /*SL_IID_MUTESOLO,*/ SL_IID_VOLUME};
    const SLboolean req2[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
            /*SL_BOOLEAN_TRUE,*/ SL_BOOLEAN_TRUE};
	
    INFO("CreateAudioPlayer  formatType[%d] numChannels[%d] samplesPerSec[%d] bitsPerSample[%d] containerSize[%d] channelMask[%d] endianness[%d]",
        format_pcm.formatType,format_pcm.numChannels,format_pcm.samplesPerSec,
        format_pcm.bitsPerSample,format_pcm.containerSize,format_pcm.channelMask,
        format_pcm.endianness); 


    result = (*mEngineEngine)->CreateAudioPlayer(mEngineEngine, &mPlayerObject, &audioSrc, &audioSnk, 3, ids2, req2);
	
    if(result)
	{
        ERROR("mEngineEngine::CreateAudioPlayer %d",result);
        Close();
        return ERR_DEVICE_ERROR;
    }
    // realize the player
    result = (*mPlayerObject)->Realize(mPlayerObject, SL_BOOLEAN_FALSE);
    if(result)
	{
        ERROR("mPlayerObject::Realize %d",result);
        Close();
        return ERR_DEVICE_ERROR;
    }
    // get the play interface
    result = (*mPlayerObject)->GetInterface(mPlayerObject, SL_IID_PLAY, &mPlayerPlay);
    if(result)
	{
        ERROR("mPlayerObject::GetInterface mPlayerPlay  %d",result);
        Close();
        return ERR_DEVICE_ERROR;
    }
    // get the buffer queue interface
    result = (*mPlayerObject)->GetInterface(mPlayerObject, SL_IID_BUFFERQUEUE,&mPlayerBufferQueue);
    if(result)
	{
        ERROR("mPlayerObject::GetInterface mPlayerBufferQueue %d",result);
        Close();
        return ERR_DEVICE_ERROR;
    }
    // register callback on the buffer queue
    result = (*mPlayerBufferQueue)->RegisterCallback(mPlayerBufferQueue, CAndroidAudioOpenSLESRender::CallbackWrapper, this);
    if(result)
	{
        ERROR("mPlayerBufferQueue::RegisterCallback callback, is %d",result);
        Close();
        return ERR_DEVICE_ERROR;
    }

    // get the volume interface
    result = (*mPlayerObject)->GetInterface(mPlayerObject, SL_IID_VOLUME, &mPlayerVolume);
    if(result)
	{
        ERROR("mPlayerObject::GetInterface mPlayerVolume %d",result);
        Close();
        return ERR_DEVICE_ERROR;
    }

	mBufferScaledTime = CalculateScaledTime(audioFormat,mBufferLength);
	mBufferScaledAdjust= mBufferScaledTime/mBufferLength;
	
	INFO("mBufferScaledTime %f",mBufferScaledTime);
	
	mState = AR_OPENED;
	
    return ERR_NONE;
}

void CAndroidAudioOpenSLESRender::Start () 
{	
    SLresult result;

    DEBUG ("CAndroidAudioOpenSLESRender::start mPlayerPlay 0x%x");

	if(mbFlushed)
	{
		if(mCurBuffer)
		{
			ReleaseBuffer(mCurBuffer);
			
			mCurBuffer = NULL;
		}
			
		if(mBufferFillFirst != mBufferFillLast && mCurBuffer == NULL)
		{
			mCurBuffer		= mBufferFillFirst;
			mBufferFillFirst= mCurBuffer->Next;
			mCurBuffer->Next= NULL;
			
			if(mBufferFillFirst == NULL)
				mBufferFillLast = NULL;
		}
		
		if(mCurBuffer == NULL)
		{
			ERROR("Start mCurBuffer is NULL");
			return ;
		}

		
		result = (*mPlayerBufferQueue)->Enqueue(mPlayerBufferQueue, (unsigned char*)mCurBuffer->Ptr, mCurBuffer->LeftBytes );
						
		if (result != SL_RESULT_SUCCESS)
		{
			ERROR("Enqueue err");
			Close();
			return ;
		}

		mbFlushed = false;
	}

    if(mPlayerPlay)
	{		
        result = (*mPlayerPlay)->SetPlayState(mPlayerPlay, SL_PLAYSTATE_PLAYING);

		if(result == 0)
		{
			mState = AR_RUNNING;
        }
		else
	    {
	    	ERROR("Start error");
		}
    }
}

void     CAndroidAudioOpenSLESRender::Stop () 
{
    SLresult result;

    DEBUG("CAndroidAudioOpenSLESRender::stop");

    if (mPlayerPlay)
	{
        result = (*mPlayerPlay)->SetPlayState(mPlayerPlay, SL_PLAYSTATE_STOPPED);
		
        if(result == 0)
		{
			mState = AR_STOP;
        }  
		else
		{
            ERROR("stop error");
		}
    }
}

void     CAndroidAudioOpenSLESRender::Flush () 
{
    DEBUG("CAndroidAudioOpenSLESRender::flush");
	
	Reset();

	if(!!mPlayerBufferQueue)
	{
		(*mPlayerBufferQueue)->Clear(mPlayerBufferQueue);
		mbFlushed = true;
	}
}

void     CAndroidAudioOpenSLESRender::Pause () 
{
    SLresult result;

    DEBUG("CAndroidAudioOpenSLESRender::pause");
	
    if (mPlayerPlay)
	{
        result = (*mPlayerPlay)->SetPlayState(mPlayerPlay, SL_PLAYSTATE_PAUSED);

		if(result == 0)
		{
			mState = AR_PAUSE;
        }
		else
		{
            ERROR("pause error");
		}
    }
}

void     CAndroidAudioOpenSLESRender::Close () 
{
   DEBUG("Close");
    // destroy buffer queue audio player object, and invalidate all associated interfaces
    if (mPlayerObject != NULL)
	{
        (*mPlayerObject)->Destroy(mPlayerObject);
        mPlayerObject = NULL;
        mPlayerPlay = NULL;
        mPlayerBufferQueue = NULL;
        mPlayerMuteSolo = NULL;
        mPlayerVolume = NULL;
    }


    // destroy output mix object, and invalidate all associated interfaces
    if (mOutputMixObject != NULL) 
	{
        (*mOutputMixObject)->Destroy(mOutputMixObject);
        mOutputMixObject = NULL;
        mOutputMixERb = NULL;
    }
    
    
    // destroy engine object, and invalidate all associated interfaces
    if (mEngineObject != NULL) 
	{
        (*mEngineObject)->Destroy(mEngineObject);
        mEngineObject = NULL;
        mEngineEngine = NULL;
    }

	mState = AR_CLOSE;
}

void CAndroidAudioOpenSLESRender::IsRenderFinish(int* IsFinish)
{	
	if(IsFinish == NULL)
		return ;
	
	*IsFinish = CBaseAudioRender::IsListEmpty();

	DEBUG("IsRenderFinish %d",*IsFinish);
}

int CAndroidAudioOpenSLESRender::Write (const AudioSample& SrcAudioSample) 
{
    int Ret = ERR_NONE;

	if(!IsOpened())
	{
		return ERR_DEVICE_ERROR;
	}
	
	if(SrcAudioSample.Ptr == NULL || SrcAudioSample.Size <= 0)
	{
		return ERR_INVALID_PARAM;
	}
	
	DEBUG("audioSample.Ptr 0x%x,size %d,Used %d Limit %d",SrcAudioSample.Ptr,SrcAudioSample.Size,mBufferUsed,mBufferLimit);

	Ret = AddBufferToList(SrcAudioSample);
	
    return Ret;
}

void  CAndroidAudioOpenSLESRender::SetVolume (float left, float right) 
{
    DEBUG("setVolume(%f, %f)", left, right);

}

void CAndroidAudioOpenSLESRender::CallbackWrapper(SLAndroidSimpleBufferQueueItf bq, void *User) 
{
    SLresult result;
		
    CAndroidAudioOpenSLESRender *	This = (CAndroidAudioOpenSLESRender *)User;

	static int nEmptyCount = 0;
	if(User == NULL)
		return ;
	
	This->mLock.Lock();
	
	DEBUG("mBufferFillFirst 0x%x,mBufferFillLast 0x%x",This->mBufferFillFirst,This->mBufferFillLast);

	if(This->mCurBuffer)
	{
		This->ReleaseBuffer(This->mCurBuffer);
			
		This->mCurBuffer = NULL;
	}
		
	if(This->mBufferFillFirst != This->mBufferFillLast && This->mCurBuffer == NULL)
	{
		This->mCurBuffer		= This->mBufferFillFirst;
		This->mBufferFillFirst	= This->mCurBuffer->Next;
		This->mCurBuffer->Next	= NULL;
		
		if(This->mBufferFillFirst == NULL)
			This->mBufferFillLast = NULL;
	}
	
	if(This->mCurBuffer)
	{
		DEBUG("This->mCurBuffer->LeftBytes %d !!",This->mCurBuffer->LeftBytes);

		nEmptyCount = 0;
		
		result = (*bq)->Enqueue(bq, (unsigned char*)This->mCurBuffer->Ptr, This->mCurBuffer->LeftBytes );
		
        if (result != SL_RESULT_SUCCESS)
		{
            DEBUG("(*bq)->Enqueue(bq, (short *)(is->audio_buf),  %d", result);
        }
		else
		{
			//can't releaseBuffer here!!!		
        }  
	}
	else
	{
		if(nEmptyCount %50 ==0)
		{
			INFO("BufferList  IS NULL nEmptyCount %d",nEmptyCount);
		}

		nEmptyCount++;
				
		result = (*bq)->Enqueue(bq, (unsigned char*)This->mSliceBuf, BUFFER_SIZE );
		
        if (result != SL_RESULT_SUCCESS)
		{
            DEBUG("(*bq)->Enqueue(bq, (short *)(is->audio_buf),  %d", result);
        }
		else
		{
			//can't releaseBuffer here!!!		
        }  
	}
		
	DEBUG("CallbackWrapper finish");
			
	This->mLock.Unlock();
}
