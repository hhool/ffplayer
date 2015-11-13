#ifndef __BASE_AUDIO_RENDER_H__
#define __BASE_AUDIO_RENDER_H__


#define BUFFER_SIZE 8192
#define MAX_BUFFER_NUM 16


typedef struct waveBuffer
{
	void* 		   Ptr;
	int			   ReadPos;
	int			   LeftBytes;
	int 		   Bytes;
	waveBuffer	  *Next;
	double		   RefTime;
}waveBuffer;

class UtilSingleLock;
class CMasterClock;
class CBaseAudioRender
{

public:
	CBaseAudioRender();
	~CBaseAudioRender();
public:
	void SetClock(CMasterClock*	Clock);
	
protected:
	waveBuffer* GetBuffer();
	void		ReleaseBuffer(waveBuffer* Buffer,bool bUpdateTime = false);
	void		BufferReset();
	int 		AddBufferToList(const AudioSample& ASample);
	double		CalculateScaledTime(AudioFormat AFormat,int Length);
	bool		IsListEmpty();
protected:
	CMasterClock* mClock;
	int 		  mBufferLimit;
	int 		  mBufferUsed;
	waveBuffer*   mBufferFillFirst;
	waveBuffer*   mBufferFillLast;
	
	waveBuffer*   mBufferFreeFirst;
	waveBuffer*   mBufferFreeLast;
		
	waveBuffer*   mCurBuffer;
	int 		  mBufferLength;
	int 		  mFillPos;
	int 		  mBytes;

	double		  mFillLastTime;
	double 		  mBufferScaledTime;
	double		  mBufferScaledAdjust;

	UtilSingleLock	mLock;
};


#endif

