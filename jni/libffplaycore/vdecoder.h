#ifndef __AVRENDER_STUB_H__
#define __AVRENDER_STUB_H__

#include "basedecoder.h"

class CVideoStubRender : public CStubRender
{
public:
    virtual ~CVideoStubRender () {}
    virtual int ShowPicture (AVFrame *pFrame) = 0;
	virtual int Flush() = 0;
};

class CVideoDecoder : public CBaseDecoder {
public:
	CVideoDecoder();
	~CVideoDecoder();
public:
    void ThreadEntry ();
	void ReDrawPictrue();
protected:
	void Reset();
private:
	bool mbFirstKeyFrame;
	bool mbNeedReSync;
	bool mbNextFrame;
	AVFrame* mpFrame;
};

#endif
