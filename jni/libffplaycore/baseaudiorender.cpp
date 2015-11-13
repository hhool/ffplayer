#include "common.h"
#include "playcore.h"
#include "util_log.h"
#include "util_lock.h"
#include "masterclock.h"
#include "baseaudiorender.h"


CBaseAudioRender::CBaseAudioRender():mLock("BaseAudioRender")
{
	mBufferLimit		= MAX_BUFFER_NUM;
	mBufferUsed			= 0;	
	mBufferFillFirst 	= NULL;
	mBufferFillLast		= NULL;
	
	mBufferFreeFirst	= NULL;
	mBufferFreeLast		= NULL;
	
	mCurBuffer			= NULL;

	mBufferLength		= BUFFER_SIZE;
	mFillPos			= BUFFER_SIZE;
	mBytes				= 0;

	mFillLastTime		= TIME_UNKNOWN;

	mBufferScaledTime	= TIME_UNKNOWN;
	INFO("0x%x",this);
}

CBaseAudioRender::~CBaseAudioRender()
{
	INFO("0x%x",this);

	BufferReset();
}

void CBaseAudioRender::SetClock(CMasterClock* Clock)
{
	mClock = Clock;
}

double	CBaseAudioRender::CalculateScaledTime(AudioFormat AFormat,int Length)
{
	int ByteSizePerSample = (AFormat.Bits*AFormat.Channels)>>3;
	int nSamplesPerSec	  = AFormat.SampleRate;
	int nAvgBytesPerSec	  = ByteSizePerSample*nSamplesPerSec;
	INFO("ByteSizePerSample %d, nSamplesPerSec %d,nAvgBytesPerSec %d",ByteSizePerSample,nSamplesPerSec,nAvgBytesPerSec);
	return (double)Length/(double)nAvgBytesPerSec;
}

waveBuffer*	CBaseAudioRender::GetBuffer()
{
	waveBuffer* Buffer;

	Buffer = mBufferFreeFirst;
	
	if (Buffer)
	{
		mBufferFreeFirst = Buffer->Next;
		if (Buffer == mBufferFreeLast)
			mBufferFreeLast = NULL;
	}
	
	if (!Buffer)
	{
		void* Ptr = malloc(mBufferLength);

		if(!Ptr)
		{
			ERROR("Ptr IS NULL");
		}
		else
		{
			Buffer = (waveBuffer*)malloc(sizeof(waveBuffer));
		
			if(!Buffer)
			{
				ERROR("Buffer IS NULL");
				free(Ptr);
			}

			if(Buffer)
			{
				Buffer->Ptr = Ptr;
				Buffer->LeftBytes = 0;
			}
			else
			{
				if(mBufferLimit > mBufferUsed)
				{
					mBufferLimit = mBufferUsed;
				}
				else 
				{
					if(mBufferLimit>4)
					{
						mBufferLimit--;
					}
				}
			}
		}
	}
	
	if(Buffer)
	{
		mBufferUsed++;
		Buffer->Next 	= NULL;
		Buffer->ReadPos = 0;
		Buffer->RefTime = TIME_UNKNOWN;
	}
	
	DEBUG("Buffer 0x%x,mBufferUsed %d",Buffer,mBufferUsed);
	
	return Buffer;
}

void CBaseAudioRender::ReleaseBuffer(waveBuffer* Buffer,bool bUpdateTime)
{
	Buffer->Next 		= NULL;
	Buffer->Bytes		= 0;
	Buffer->ReadPos 	= 0;
	Buffer->LeftBytes 	= 0;

	if(Buffer->RefTime >= 0.0)
	{
		double ClockTime = PlayCore::GetInstance()->av_gettime() / 1000000.0;
		
		mClock->SetOriginClock(Buffer->RefTime,ClockTime);
		DEBUG("mClock 0x%x,RefTime %f",mClock,Buffer->RefTime);
	}
	
	if (mBufferFreeLast)
		mBufferFreeLast->Next = Buffer;
	else
		mBufferFreeFirst = Buffer;
	
	mBufferFreeLast = Buffer;
	
	mBufferUsed--;
}


int CBaseAudioRender::AddBufferToList(const AudioSample& ASample)
{
	int Ret = ERR_NONE;

	waveBuffer* Buffer = NULL;
	
	int DstLength 	= 0;
	
	int SrcLength 	= ASample.Size;
	
	int Length 		= 0;

	int SrcIndex	= 0;

	double AudioSamplePTS = ASample.PTS;
	
	mLock.Lock();
	
	if(mBufferUsed > mBufferLimit)
	{
		DEBUG("mBufferUsed(%d)>mBufferLimit(%d)",mBufferUsed,mBufferLimit);
		mLock.Unlock();
		return ERR_BUFFER_FULL;
	}

	while (SrcLength > 0)
	{
		if (mFillPos >= mBufferLength)
		{
			if(Buffer)
			{					
				Buffer->LeftBytes = mBufferLength;
				DEBUG("Finish Buffer 0x%x,Buffer->Ptr 0x%x,Buffer->ReadPos %d,Buffer->LeftBytes %d",Buffer,Buffer->Ptr,Buffer->ReadPos,Buffer->LeftBytes);
			}
			
			Buffer = GetBuffer();
			
			if (!Buffer)
			{
				ERROR("Buffer IS NULL !!");
				
				Ret = ERR_OUT_OF_MEMORY;
				
				break;
			}

			if (mBufferFillLast)
			{
				waveBuffer* Last = mBufferFillLast;

				if(Last->RefTime >= 0.0)
				{
					mFillLastTime = Last->RefTime;
				}
				else
				{
					if(mFillLastTime >= 0.0)
						mFillLastTime += mBufferScaledTime;
					
					Last->RefTime = mFillLastTime;
				}
				
				mBufferFillLast->Next = Buffer;
			}
			else
			{
				mBufferFillFirst = Buffer;
			}

			if(AudioSamplePTS >= 0.0)
			{
				Buffer->RefTime = AudioSamplePTS - (mFillPos*mBufferScaledAdjust);
				AudioSamplePTS = TIME_UNKNOWN;
			}
			
			mBufferFillLast = Buffer;
			mFillPos 		= 0;
			Buffer->Bytes	= mBytes;
		}
		else
			Buffer = mBufferFillLast;
		
		DstLength = mBufferLength - mFillPos;

		if(SrcLength > DstLength)
			Length 	= DstLength;
		else
			Length	= SrcLength;
				
		memcpy((unsigned char*)Buffer->Ptr+mFillPos,(unsigned char*)ASample.Ptr+SrcIndex,Length);

		mBytes		+= Length;
		mFillPos 	+= Length;
		SrcLength  	-= Length;
		SrcIndex	+= Length;
		
		if(SrcLength == 0)
		{
			Buffer->LeftBytes = mFillPos;
			DEBUG("Buffer 0x%x,Buffer->Ptr 0x%x,Buffer->ReadPos %d,Buffer->LeftBytes %d",Buffer,Buffer->Ptr,Buffer->ReadPos,Buffer->LeftBytes);
			break;
		}
	}

	mLock.Unlock();

	return Ret;
}

void CBaseAudioRender::BufferReset()
{
	mLock.Lock();

	if(mCurBuffer != NULL)
	{
		ReleaseBuffer(mCurBuffer);
		mCurBuffer = NULL;
	}
	
	while (mBufferFillFirst)
	{
		waveBuffer* Buffer 	= mBufferFillFirst;
		mBufferFillFirst 	= Buffer->Next;
		ReleaseBuffer(Buffer);
	}

	mBufferFillLast 	= mBufferFillFirst;
	
	while (mBufferFreeFirst)
	{
		waveBuffer* Buffer 	= mBufferFreeFirst;
		mBufferFreeFirst 	= Buffer->Next;
		free(Buffer->Ptr);
		free(Buffer);
	}
	mBufferFreeLast		= mBufferFreeFirst;

	mBufferUsed 		= 0;
	
	mBufferLength		= BUFFER_SIZE;
	mFillPos			= BUFFER_SIZE;
	mBytes				= 0;
	
	mLock.Unlock();
}
bool CBaseAudioRender::IsListEmpty()
{
	bool IsEmpty = false;
	
	mLock.Lock();

	if(mBufferFillFirst == mBufferFillLast || mBufferFillFirst == NULL)
	{
		IsEmpty = true;
	}
	DEBUG("IsRenderFinish %d",IsEmpty);
	
	mLock.Unlock();

	return IsEmpty;
}

