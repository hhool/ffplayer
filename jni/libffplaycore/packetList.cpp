#include "packetList.h"
#include "playcore.h"
#include "util_log.h"

const AVPacket CPacketQueue::mNullPacket = {0,0,0,0,0,0,0,0,0,0,0};

CPacketQueue::CPacketQueue (int size) :
    miMaxSize(size), miSize(0), mbToFlush(false), 
    mLock("AVPacket"), mCondEmpty(), mCondFull()
{
    INFO ("0x%x",this);
    mPktList.clear ();
}

CPacketQueue::~CPacketQueue ()
{
    INFO ("0x%x DataSize %d",this,getDataSize());
}
    
void CPacketQueue::init (int MaxSize)
{
    mLock.Lock ();
    miSize = 0;
    list<pair<AVPacket, int> >::iterator it = mPktList.begin ();
    for(; it != mPktList.end (); it++)
    {
		PlayCore::GetInstance()->av_free_packet (&it->first);
    }
    mPktList.clear ();
    mbToFlush = false;
	miMaxSize = MaxSize;
    mLock.Unlock ();
}

AVPacket CPacketQueue::get ()
{
    AVPacket packet = mNullPacket;
    mLock.Lock ();
    for (;;)
    {
        if (mbToFlush)
        {
            mLock.Unlock ();
            DEBUG ("flush return");
            return mNullPacket;
        }
        if (mPktList.size () <= 0)
        {
            //if (miSize <= 0) {
            mCondEmpty.Wait (mLock.GetMutex());
            VERBOSE ("<<get mCondEmpty signal! miSize=%d", miSize);
        }
        else
        {
            break;
        }
    }
    if (mPktList.size () <= 0)
    {
        ERROR ("concurrent error! should not reach here! miSize=%d, mPktList.size()=%d\n", miSize, mPktList.size());
        mLock.Unlock ();
        return mNullPacket;
    }
    pair<AVPacket, int> item = mPktList.front();
    mPktList.pop_front ();
    packet = item.first;
    miSize -= item.second;
    //DEBUG ("miSize = %d, out queue size=%d", miSize, item.second);
    mCondFull.Signal ();
    mLock.Unlock ();
    return packet;
}

/** 
 * 
 * @param pkt 
 * @param size , if size == -1, <miSize>, <miMaxSize> stands for packet count.
 *               miSize = mPktList.size(). we should add 1 to miSize.
 *               else the size stands for the size of the <pkt>.
 * 
 * @return 
 */
int CPacketQueue::put (AVPacket* pkt, int size)
{
    if (size == -1)
    	size = 1;

    //todo: first av_dup_packet, will not copy data, why???
    if (PlayCore::GetInstance()->av_dup_packet(pkt) < 0)
    {
        ERROR ("dup packet fail! maybe this could be optimization! no need to dup?\n");
        return -1;
    }

    mLock.Lock ();

    for (;;)
    {
        if (mbToFlush)
        {
            mLock.Unlock ();
			if(pkt&&pkt->data)
            	PlayCore::GetInstance()->av_free_packet(pkt);
            DEBUG ("flush return");
            return -1;
        }
        if (miSize + size > miMaxSize)
        {
            VERBOSE (">>get mCondFull signal! FULL miSize=%d, size=%d miMaxSize %d", miSize, size,miMaxSize);
            mCondFull.Wait (mLock.GetMutex());
            VERBOSE (">>get mCondFull signal! miSize=%d, size=%d", miSize, size);
        }
        else
        {
            break;
        }
    }

    pair<AVPacket, int> item = make_pair (*pkt, size);
    mPktList.push_back (item);
    miSize += size;
    //DEBUG ("miSize = %d, in queue size=%d", miSize, size);
    mCondEmpty.Signal ();
    mLock.Unlock ();
    return 0;
}

bool CPacketQueue::isEmpty()
{
	bool bEmpty = false;
	
	mLock.Lock ();

	if (miSize <= 0)
	{
		bEmpty = true;
	}

	mLock.Unlock ();

	return bEmpty;
}

bool CPacketQueue::isFullWithDataSize(int size)
{
	bool bFull = false;
	
	mLock.Lock ();

	if (miSize + size > miMaxSize)
	{
		bFull = true;
	}

	mLock.Unlock ();

	return bFull;
}
int CPacketQueue::getDataSize()
{	
	int size = 0;
	mLock.Lock ();
	size =  miSize;
	mLock.Unlock ();
	return size;
}
int CPacketQueue::getMaxSize()
{
	return miMaxSize;
}
void CPacketQueue::flush ()
{
    mLock.Lock ();
    mbToFlush = true;
    DEBUG ("send mCondFull Signal");
    mCondFull.Signal ();
    DEBUG ("send mCondEmpty Signal");
    mCondEmpty.Signal ();
    DEBUG ("after send out all Signal");
    miSize = 0;
    list<pair<AVPacket, int> >::iterator it = mPktList.begin ();
    int i = 0;
    for(; it != mPktList.end (); it++) {
        //DEBUG ("i=%d, second=%d", i++, it->second);
        PlayCore::GetInstance()->av_free_packet (&it->first);
    }
    mPktList.clear ();
    mLock.Unlock ();
}

