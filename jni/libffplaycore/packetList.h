#ifndef __PACKET_LIST_H__
#define __PACKET_LIST_H__

#include "ffmpeg.h"
#include <stdio.h>
#include "util_lock.h"
#include "config/stl_config.h"

#define DEFAULT_MAX_SIZE (1024*1024)

class CPacketQueue
{
private:
    int miMaxSize, miSize;
    bool mbToFlush;
    list<pair<AVPacket, int> > mPktList;
    UtilSingleLock mLock;
    UtilCond mCondEmpty;
    UtilCond mCondFull;
public:
    CPacketQueue (int maxSize=DEFAULT_MAX_SIZE);
    ~CPacketQueue ();
    
    void      init (int MaxSize=DEFAULT_MAX_SIZE);
    AVPacket  get ();
    int       put (AVPacket* pkt, int size = -1);
	bool 	  isFullWithDataSize(int size);
	bool	  isEmpty();
    void      flush ();
	int		  getDataSize();
	int		  getMaxSize();
    static const AVPacket mNullPacket;
};

#endif
