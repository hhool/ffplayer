#include "common.h"
#if PLATFORM >= 14
#include <system/audio.h>
#endif
#include "util_log.h"
#include "masterclock.h"
#include "android_audiotrack_render.h"

using namespace android;

#define DUMP_FILE 0
#define DUMP_OUT_START_STOP_FILE 0
#define DUMP_IN_START_STOP_FILE 0

#if DUMP_FILE
static FILE* fdump = NULL;
#endif

#if DUMP_IN_START_STOP_FILE
static FILE* fdump_in_start_stop_clip = NULL;
static int nin_start_count = 0;
#endif

#if DUMP_OUT_START_STOP_FILE
static FILE* fdump_out_start_stop_clip = NULL;
static int nout_start_count = 0;
#endif

CAndroidAudioTrackRender::CAndroidAudioTrackRender (CMasterClock* Clock)
{
    mTrack = NULL;
#if    PLATFORM < 14
    mStreamType = AudioSystem::MUSIC;
#else
    mStreamType = AUDIO_STREAM_MUSIC;
#endif
    mLeftVolume 		= 1.0;
    mRightVolume 		= 1.0;
    mLatency = 0;
    mMsecsPerFrame 		= 0;
    mNumFramesWritten 	= 0;
   
	mMinFrameCount 		= 16384;

	mState				= AR_NONE;

#if DUMP_FILE
	if(fdump==NULL)
	{
		fdump = fopen("/mnt/sdcard/1/outdump.pcm","wb");
	}
#endif
	INFO("0x%x",this);
	CBaseAudioRender::SetClock(Clock);
}

CAndroidAudioTrackRender::~CAndroidAudioTrackRender () 
{
	INFO("0x%x",this);
	Close () ;

	Reset ();

#if DUMP_FILE
	if(fdump != NULL)
	{
		fclose(fdump);
		fdump = NULL;
	}
#endif

}

int CAndroidAudioTrackRender::Reset () 
{
	CBaseAudioRender::BufferReset();
	
    return ERR_NONE;
}

ARState CAndroidAudioTrackRender::GetState()
{
	return mState;
}

bool CAndroidAudioTrackRender::IsOpened()
{
	if(mTrack != NULL)
		return true;
	
	return false;
}

int  CAndroidAudioTrackRender::Open (AudioFormat audioFormat) 
{
	int format = 0;
    
    INFO ("open(%u, %d, %d, %d)", audioFormat.SampleRate, audioFormat.Channels, audioFormat.Bits);
	
    switch((audioFormat.Bits-1)/8)
	{
		case 0: // SAMPLE_FMT_U8
		{
#if PLATFORM < 14
			format = AudioSystem::PCM_8_BIT;
#elif PLATFORM >= 14
		format = AUDIO_FORMAT_PCM_8_BIT;
#endif
			INFO("PCM_8_BIT");
			break;
		}
		case 1: // SAMPLE_FMT_S16
		{
#if PLATFORM < 14
			format = AudioSystem::PCM_16_BIT;
#elif PLATFORM >= 14
			format = AUDIO_FORMAT_PCM_16_BIT;
#endif
			INFO("PCM_16_BIT");
			break;
		}
		default:
		{
			if (mTrack)
			{
				WARN("format NO match device and delete mTrack");
				Close();
			}
			return ERR_DEVICE_ERROR;
		}
	}

    if (mTrack)
	{
		if (audioFormat.SampleRate != mTrack->getSampleRate() || format != mTrack->format() || audioFormat.Channels != mTrack->channelCount())
		{
			WARN(("new Track param not equal to pre param settings,to  delete mTrack and create new one;"));
			Close();
		}
	}
#if    PLATFORM >=9 && PLATFORM < 16
	if(mTrack->getMinFrameCount(&mMinFrameCount,mStreamType,audioFormat.SampleRate)!= NO_ERROR)
	{
		return ERR_DEVICE_ERROR;
	}
#elif PLATFORM >= 16
	if(mTrack->getMinFrameCount(&mMinFrameCount,(audio_stream_type_t)mStreamType,audioFormat.SampleRate)!= NO_ERROR)
	{
		return ERR_DEVICE_ERROR;
	}	
#endif

	status_t ret;

    INFO ("frameCount %d",mMinFrameCount);
#if    PLATFORM < 14
    if(!mTrack)
    {
    	mTrack = new AudioTrack();

		ret = mTrack->set(
					mStreamType,
					audioFormat.SampleRate,
					format,
					audioFormat.Channels,
					mMinFrameCount,
					0 /* flags */,
					CallbackWrapper,
					this);
	
		INFO ("First mTrack->set success ? %s",(ret == NO_ERROR)?"yes":"No");
		
		if (ret != NO_ERROR)
		{
			//AudioSystem::CHANNEL_OUT_STEREO : AudioSystem::CHANNEL_OUT_MONO,
			int chl = (audioFormat.Channels== 2) ? 12 : 4;

			ret = mTrack->set(
						mStreamType,
						audioFormat.SampleRate,
						format,
						chl,//(audioFormat.Channels == 2) ? AudioSystem::CHANNEL_OUT_STEREO : AudioSystem::CHANNEL_OUT_MONO,
						mMinFrameCount,
						0 /* flags */,
						CallbackWrapper,
						this);
			

			INFO ("Second mTrack->set success ? %s",(ret == NO_ERROR)?"yes":"No");
		}
    }
#elif PLATFORM>=16
	if(!mTrack)
	{
    	int chl = (audioFormat.Channels>= 2) ? AUDIO_CHANNEL_OUT_STEREO : AUDIO_CHANNEL_OUT_MONO;

    	mTrack = new AudioTrack();

		ret = mTrack->set(
					(audio_stream_type_t)mStreamType,
					audioFormat.SampleRate,
					(audio_format_t)format,
					chl,
					mMinFrameCount,
					(audio_output_flags_t)0 /* flags */,
					CallbackWrapper,
					this);
	}
#else 
    if(!mTrack)
	{
    	int chl = (audioFormat.Channels>= 2) ? AUDIO_CHANNEL_OUT_STEREO : AUDIO_CHANNEL_OUT_MONO;

    	mTrack = new AudioTrack();

		INFO("new mTrack 0x%x",mTrack);

		ret = mTrack->set(
					mStreamType,
					audioFormat.SampleRate,
					format,
					chl,
					mMinFrameCount,
					0 /* flags */,
					CallbackWrapper,
					this);
	}
#endif
    if (ret != NO_ERROR)
    {
    	ERROR ("Unable to set audio track");
    	Close();
		return ERR_DEVICE_ERROR;
    }
    if ((mTrack->initCheck() != NO_ERROR)) 
	{
        ERROR ("Unable to create audio track");
        Close();
        return ERR_DEVICE_ERROR;
    }
	
    INFO ("setVolume: mLeftVolume=%f, mRightVolume=%f", mLeftVolume, mRightVolume);
    mTrack->setVolume(mLeftVolume, mRightVolume);
    mMsecsPerFrame = 1.e3 / (float) audioFormat.SampleRate;
    mLatency = mTrack->latency();

	mState = AR_OPENED;
	
    return ERR_NONE;
}

void     CAndroidAudioTrackRender::Start () 
{
    DEBUG ("CAndroidAudioTrackRender::start");
	
    if (mTrack && mState != AR_RUNNING) 
	{
        mTrack->setVolume(mLeftVolume, mRightVolume);
        mTrack->start();
        mTrack->getPosition(&mNumFramesWritten);
		mState = AR_RUNNING;

#if DUMP_IN_START_STOP_FILE
		if(fdump_in_start_stop_clip==NULL)
		{
			char filename[128]={0};
			sprintf(filename,"/mnt/sdcard/1/1_%d_in_dump.pcm",nin_start_count++);
			fdump_in_start_stop_clip = fopen(filename,"wb");
		}
#endif
#if DUMP_OUT_START_STOP_FILE
		if(fdump_out_start_stop_clip==NULL)
		{
			char filename[128]={0};
			sprintf(filename,"/mnt/sdcard/1/1_%d_out_dump.pcm",nout_start_count++);
			fdump_out_start_stop_clip = fopen(filename,"wb");
		}
#endif

		DEBUG("mBufferLength %d,mBufferLimit %d,mBufferUsed %d,mBufferFillFirst 0x%x,\
			  mBufferFillLast 0x%x,mBufferFreeFirst 0x%x,mBufferFreeLast 0x%x\
			  mCurBuffer 0x%x,mFillPos %d,mBytes %d",\
			  mBufferLength,mBufferLimit,mBufferUsed,mBufferFillFirst,\
			  mBufferFillLast,mBufferFreeFirst,mBufferFreeLast,\
			  mCurBuffer,mFillPos,mBytes);

    }
}

void     CAndroidAudioTrackRender::Stop () 
{
    DEBUG("CAndroidAudioTrackRender::stop");
	
    if (mTrack && mState != AR_STOP) 
	{
		mTrack->stop();
		mState = AR_STOP;
#if DUMP_IN_START_STOP_FILE
		if(fdump_in_start_stop_clip)
		{
			fclose(fdump_in_start_stop_clip);
			fdump_in_start_stop_clip = NULL;
		}
#endif
#if DUMP_OUT_START_STOP_FILE
		if(fdump_out_start_stop_clip)
		{
			fclose(fdump_out_start_stop_clip);
			fdump_out_start_stop_clip = NULL;
		}
#endif
    }
}

void     CAndroidAudioTrackRender::Flush () 
{
    DEBUG("CAndroidAudioTrackRender::flush");
	Reset();
	
	if (mTrack) 
		mTrack->flush();
}

void     CAndroidAudioTrackRender::Pause () 
{
    DEBUG("CAndroidAudioTrackRender::pause");
    if (mTrack && mState == AR_RUNNING) 
	{
		mTrack->pause();
		mState = AR_PAUSE;
    }
}

void     CAndroidAudioTrackRender::Close () 
{
    DEBUG("CAndroidAudioTrackRender::Close mTrack 0x%x",mTrack);
    if(mTrack)
	{
    	DEBUG("CAndroidAudioTrackRender::Close before delete mTrack 0x%x",mTrack);
		INFO("del mTrack 0x%x",mTrack);
		delete mTrack;
		DEBUG("CAndroidAudioTrackRender::Close after");
		mTrack = NULL;
		mState = AR_CLOSE;
	}
    DEBUG("CAndroidAudioTrackRender::Close finish");
}

void CAndroidAudioTrackRender::IsRenderFinish(int* IsFinish)
{	
	if(IsFinish == NULL)
		return ;
	
	*IsFinish = CBaseAudioRender::IsListEmpty();

	DEBUG("IsRenderFinish %d",*IsFinish);
}

int CAndroidAudioTrackRender::Write (const AudioSample& SrcAudioSample) 
{
    int Ret = ERR_NONE;

	if(mTrack == NULL)
	{
		return ERR_DEVICE_ERROR;
	}
	
	if(SrcAudioSample.Ptr == NULL || SrcAudioSample.Size <= 0)
	{
		return ERR_INVALID_PARAM;
	}
	
	DEBUG("SrcAudioSample.Ptr 0x%x,size %d,Used %d Limit %d",SrcAudioSample.Ptr,SrcAudioSample.Size,mBufferUsed,mBufferLimit);

#if DUMP_IN_START_STOP_FILE
	if(fdump_in_start_stop_clip==NULL)
	{
		char filename[128]={0};
		sprintf(filename,"/mnt/sdcard/1/1_%d_in_dump.pcm",nin_start_count++);
		fdump_in_start_stop_clip = fopen(filename,"wb");
	}
#endif
#if DUMP_OUT_START_STOP_FILE
	if(fdump_out_start_stop_clip==NULL)
	{
		char filename[128]={0};
		sprintf(filename,"/mnt/sdcard/1/1_%d_out_dump.pcm",nout_start_count++);
		fdump_out_start_stop_clip = fopen(filename,"wb");
	}
#endif

	Ret = AddBufferToList(SrcAudioSample);
	
    return Ret;
}

void  CAndroidAudioTrackRender::SetVolume (float left, float right) 
{
    DEBUG("setVolume(%f, %f)", left, right);

	mLeftVolume = left;

	mRightVolume = right;

	if (mTrack) 
	{
        mTrack->setVolume(left, right);
    }
}

void CAndroidAudioTrackRender::CallbackWrapper(int event, void *cookie, void *info) 
{
	DEBUG("CallbackWrapper cookie 0x%x,info 0x%x",cookie,info);

    if (event != AudioTrack::EVENT_MORE_DATA)
	{
        return;
    }
			
    CAndroidAudioTrackRender *	This = (CAndroidAudioTrackRender *)cookie;
    AudioTrack::Buffer *		AudioTrackBuffer = (AudioTrack::Buffer *)info;

	unsigned char *DstPtr 		= (unsigned char *)AudioTrackBuffer->raw;
	int			   DstSize 		= AudioTrackBuffer->size;
	int			   DstWritePos 	= 0;

	DEBUG("CallbackWrapper DstPtr 0x%x,DstSize %d,DstWritePos %d",DstPtr,DstSize,DstWritePos);

	if(DstSize <= 0)
		return ;
	
	This->mLock.Lock();
	
	do
	{
		if(This->mCurBuffer)
		{
			DEBUG("This->mCurBuffer->LeftBytes %d !!",This->mCurBuffer->LeftBytes);
			
			if(This->mCurBuffer->LeftBytes > 0)
			{	
				int Size = DstSize;
					
				if(Size > This->mCurBuffer->LeftBytes)
				{
					Size = This->mCurBuffer->LeftBytes;
				}

				memcpy(DstPtr+DstWritePos,(unsigned char*)This->mCurBuffer->Ptr+This->mCurBuffer->ReadPos,Size);
#if DUMP_FILE				
				if(fdump)
				{
					fwrite((unsigned char*)This->mCurBuffer->Ptr+This->mCurBuffer->ReadPos,1,Size,fdump);
				}
#endif
#if	DUMP_OUT_START_STOP_FILE
				if(fdump_out_start_stop_clip)
				{
					fwrite((unsigned char*)This->mCurBuffer->Ptr+This->mCurBuffer->ReadPos,1,Size,fdump_out_start_stop_clip);
				}
#endif

				This->mCurBuffer->LeftBytes	-=Size;
				This->mCurBuffer->ReadPos	+=Size;
				DstWritePos					+=Size;
				DstSize						-=Size;
			}

			if(This->mCurBuffer->LeftBytes <= 0)
			{
				//Release Cur Buffer
				This->ReleaseBuffer(This->mCurBuffer);
				
				This->mCurBuffer = NULL;
			}
		}

		AudioTrackBuffer->size = DstWritePos;

		DEBUG("AudioTrackBuffer->size %d,DstWritePos %d mBufferFillFirst 0x%x,mBufferFillLast 0x%x",\
			AudioTrackBuffer->size,DstWritePos,
			This->mBufferFillFirst,
			This->mBufferFillLast);

		if(This->mBufferFillFirst != This->mBufferFillLast && This->mCurBuffer == NULL)
		{
			This->mCurBuffer 		= This->mBufferFillFirst;
			This->mBufferFillFirst 	= This->mCurBuffer->Next;
			This->mCurBuffer->Next 	= NULL;
			if(This->mBufferFillFirst == NULL)
				This->mBufferFillLast = This->mBufferFillFirst;
		}
		
	}while(DstWritePos < DstSize && This->mCurBuffer != NULL);

	DEBUG("AudioTrackBuffer->size %d,DstWritePos %d FINAL!!",AudioTrackBuffer->size,DstWritePos);
			
	This->mLock.Unlock();
}
