#include "common.h"
#include "util_log.h"
#include "playcore.h"
#include "audioresample.h"

CAudioResample::CAudioResample()
{
	mResampleHandle = NULL;
	memset(&mSrcFormat,0,sizeof(AudioFormat));
	memset(&mDstFormat,0,sizeof(AudioFormat));	
	mSrcBytesPerSample = 0;
	mDstBytesPerSample = 0;
}

CAudioResample::CAudioResample(const AudioFormat SrcAudioFormat,const AudioFormat DstAudioFormat)
{
	mResampleHandle = NULL;
	
	mSrcFormat = SrcAudioFormat;
	mDstFormat = DstAudioFormat;

	mDstBytesPerSample = PlayCore::GetInstance()->av_get_bytes_per_sample((AVSampleFormat)((mDstFormat.Bits-1)/8)); 

	mSrcBytesPerSample = PlayCore::GetInstance()->av_get_bytes_per_sample((AVSampleFormat)((mSrcFormat.Bits-1)/8));
	
   	INFO("0x%x",this);
}

CAudioResample::~CAudioResample()
{
   	INFO("0x%x",this);

	if(mResampleHandle != NULL)
	{
		PlayCore::GetInstance()->audio_resample_close(mResampleHandle);
		mResampleHandle = NULL;
	}
}

int CAudioResample::Init()
{	
	if(mResampleHandle != NULL)
	{
		PlayCore::GetInstance()->audio_resample_close(mResampleHandle);
		mResampleHandle = NULL;
	}

	if(mDstFormat.SampleRate == 0 || mDstFormat.Channels == 0 || mDstFormat.Bits == 0)
	{
		return ERR_INVALID_PARAM;
	}

	if(mSrcFormat.SampleRate == 0 || mSrcFormat.Channels == 0 || mSrcFormat.Bits == 0)
	{
		return ERR_INVALID_PARAM;
	}

	if(mResampleHandle == NULL)
		mResampleHandle = PlayCore::GetInstance()->av_audio_resample_init(mDstFormat.Channels, mSrcFormat.Channels, 
																	 mDstFormat.SampleRate,mSrcFormat.SampleRate,
																	 (AVSampleFormat)((mDstFormat.Bits-1)/8),(AVSampleFormat)((mSrcFormat.Bits-1)/8),
																	 16,10,0,0.8);

	if(mResampleHandle == NULL)
		return ERR_DEVICE_ERROR;

	mDstBytesPerSample = PlayCore::GetInstance()->av_get_bytes_per_sample((AVSampleFormat)((mDstFormat.Bits-1)/8)); 

	mSrcBytesPerSample = PlayCore::GetInstance()->av_get_bytes_per_sample((AVSampleFormat)((mSrcFormat.Bits-1)/8));

	return ERR_NONE;
}

int CAudioResample::Resample(int16_t* Output,int16_t* Input, int InputAvailbleSize)
{
	if(mResampleHandle == NULL)
		return ERR_INVALID_DATA;

	int SampleNum =  PlayCore::GetInstance()->audio_resample(mResampleHandle, Output,Input,
												  InputAvailbleSize/(mSrcBytesPerSample*mSrcFormat.Channels));

	if(SampleNum > 0)
	{
		return SampleNum*mDstBytesPerSample*mDstFormat.Channels;
	}

	return ERR_INVALID_DATA;
}

AudioFormat CAudioResample::GetDestAudioFormat()
{
	return mDstFormat;
}

void CAudioResample::SetDestAudioFormat(const AudioFormat DstAudioFormat)
{
	mDstFormat = DstAudioFormat;
}

AudioFormat CAudioResample::GetSrcAudioFormat()
{
	return mSrcFormat;
}

void CAudioResample::SetSrcAudioFormat(const AudioFormat SrcAudioFormat)
{
	mSrcFormat = SrcAudioFormat;
}

bool CAudioResample::IsEqualSrcAudioFormat(const AudioFormat af)
{
	return IsEqualAudioFormat(af,mSrcFormat);
}

bool CAudioResample::IsEqualDstAudioFromat(const AudioFormat af)
{
	return IsEqualAudioFormat(af,mDstFormat);
}

bool CAudioResample::IsEqualAudioFormat(const AudioFormat& SrcAudioFormat,const AudioFormat& DstAudioFormat)
{
	if(SrcAudioFormat.SampleRate 	== DstAudioFormat.SampleRate 
	&& SrcAudioFormat.Channels 		== DstAudioFormat.Channels
	&& SrcAudioFormat.Bits 			== DstAudioFormat.Bits)
		return true;

	return false;
}


