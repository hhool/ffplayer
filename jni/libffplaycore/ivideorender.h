#ifndef __IVIDEORENDER__
#define __IVIDEORENDER__

#include "ffmpeg.h"

typedef enum _DisplayType {
  DT_Auto_Type = 0x0,
  DT_4_3_Type = 0x1,
  DT_16_9_Type = 0x2
}DisplayType;

class IVideoRender {
public:
  virtual int  ShowPicture(AVFrame *pFrame) = 0;
  virtual void Flush() = 0;
  virtual int  Init(AVStream *vstream) = 0;
  virtual bool IsInit() = 0;
  virtual bool GetVideoSize(int* w, int* h) = 0;
  virtual int  Reset() = 0;
  virtual void SetDisplayType(DisplayType DType) = 0;
};

#endif

