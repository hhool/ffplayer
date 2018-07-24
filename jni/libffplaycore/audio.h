#ifndef __AUDIO_H__
#define __AUDIO_H__

#define AUDIOFMT_PCM      0x01

typedef struct AudioFormat {
  int    SampleRate;
  int    Channels;
  int    Bits;
}AudioFormat;

typedef struct AudioSample {
  void*  Ptr;
  int    Size;
  double PTS;
}AudioSample;
#endif