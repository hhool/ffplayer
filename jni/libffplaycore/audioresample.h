#ifndef __AUDIO_RESAMPLE_H__
#define __AUDIO_RESAMPLE_H__

#include "ffmpeg.h"
#include "audio.h"


class CAudioResample {
public:
  CAudioResample(const AudioFormat SrcAudioFormat, const AudioFormat DstAudioFormat);

  CAudioResample();

  ~CAudioResample();

  //
  int  Init();

  //
  int  Resample(int16_t* Output, int16_t* Input, int InputAvailbleSize);
  
  AudioFormat GetDestAudioFormat();

  void    SetDestAudioFormat(const AudioFormat af);

  AudioFormat GetSrcAudioFormat();

  void    SetSrcAudioFormat(const AudioFormat af);

  bool    IsEqualSrcAudioFormat(const AudioFormat af);

  bool    IsEqualDstAudioFromat(const AudioFormat af);

  static bool  IsEqualAudioFormat(const AudioFormat& SrcAudioFormat, const AudioFormat& DstAudioFormat);
private:
  // The container of the resample format information.
  ReSampleContext* mResampleHandle;
  AudioFormat     mSrcFormat;
  AudioFormat     mDstFormat;
  int         mSrcBytesPerSample;
  int        mDstBytesPerSample;
};


#endif
