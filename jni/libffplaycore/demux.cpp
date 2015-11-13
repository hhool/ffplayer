#include <unistd.h>
#include "demux.h"
#include "playcore.h"
#include "baseplayer.h"
#include "basedecoder.h"
#include "masterclock.h"

bool CDemux::mbInterrupt = false;

#define MAX_VIDEO_SIZE DEFAULT_MAX_SIZE*2
#define MAX_AUDIO_SIZE DEFAULT_MAX_SIZE
#define MAX_CACHE_SIZE (DEFAULT_MAX_SIZE)/5

CDemux::CDemux (CBasePlayer* Player) : 			mPlayer(Player),
												msUrl(""),
												mOperationLock("CDemux"),
												mpFormatCtx(NULL),
												miVideoStream(-1),
												miAudioStream(-1),
												mAudioQueue(MAX_AUDIO_SIZE),
												mVideoQueue(MAX_VIDEO_SIZE),
												mMaxCache(MAX_CACHE_SIZE),//must le to mAudioQueue maxsize
												mSyncPTS(INVALID_TIME_VALUE),
												mbSynced(false),
												mbAudioReadyForPlay(false),
												mbPaused(false),
												mAOutPlay(NULL),
												mVOutPlay(NULL),
												mpClock(NULL),
												mbQuit(false),
												mSeekable(false),
												mCanPause(false),
												mbSeek(false),
												mDCS(DCS_NONE),
												mbOpened(false),
												mFirstCache(true)
{
	assert(mPlayer != NULL);
	INFO("0x%x",this);

}

CDemux::~CDemux ()
{
	INFO("0x%x",this);
}

int CDemux::SetDataSource(const char *url)
{
	int ret = ERR_NONE;
	
    if (url == NULL)
    {
        ERROR ("url == NULL!");
        return ERR_INVALID_PARAM;
    }

    mOperationLock.Lock ();
    Reset();
    msUrl = url;
    ret = OpenInputFile (msUrl);
	if(ret >= 0)
	{
		mbOpened = true;
	}
    mOperationLock.Unlock ();
    return ret;
}

int CDemux::Start ()
{
    mOperationLock.Lock ();
	
    mVideoQueue.init (MAX_VIDEO_SIZE);
    mAudioQueue.init (MAX_AUDIO_SIZE);
    mbQuit 		= false;
	mbInterrupt = false;

	mbPaused 	= false;
	mIsPaused 	= false;
	mbLow 		= false;
	mbReachEnd 	= false;
	mbReadOk 	= false;
	mPercent 	= -1;
	
    ThreadExec ();
    
    mOperationLock.Unlock ();
	
    return ERR_NONE;
}

void CDemux::Interrupt()
{
	mbQuit 		= true;//for quit player when opening
	mbInterrupt = true;

    mVideoQueue.flush ();
    mAudioQueue.flush ();
}

int CDemux::Stop ()
{
	mbQuit 		= true;//for quit player when opening
	mbInterrupt = true;
	
    mVideoQueue.flush ();
    mAudioQueue.flush ();
			
    DEBUG ("begin wait thread!");
    ThreadWait ();
    DEBUG ("end wait thread!");
  
    return ERR_NONE;
}


int CDemux::Seek (double pos)
{
    //pos is seconds based.
    int ret = 0;
    mOperationLock.Lock ();
    mbQuit 			= true;
	mbInterrupt 	= true;
    mVideoQueue.flush ();
    mAudioQueue.flush ();
    INFO("begin wait thread!");
    ThreadWait ();
    INFO ("end wait thread!");
    mSyncPTS 		= INVALID_TIME_VALUE;
	mbSynced 		= false;
	mbAudioReadyForPlay = true;
	if(miAudioStream >= 0)
	{
		mbAudioReadyForPlay = false;
	}

    mVideoQueue.init (MAX_VIDEO_SIZE);
    mAudioQueue.init (MAX_AUDIO_SIZE);
    mbQuit = false;
	mbInterrupt = false;

	mIsPaused = false;
	mbLow = false;
	mbReachEnd = false;
	mbReadOk = false;
	mPercent = -1;
	
    if (AvSeekFrame (pos) < 0)
    {
        ERROR ("seek fail!");
		ret = ERR_NOT_SUPPORTED;
    }
	
	mbSeek = true;
    ThreadExec ();

    mOperationLock.Unlock ();
    return ret;
}
bool CDemux::IsSeekable()
{
	//todo:hhool

	return true;
}
bool CDemux::IsCanPause()
{
	//todo:hhool

	return true;
}

int CDemux::DecodeInterruptCB(void* pThis)
{
	CDemux* This =(CDemux*)pThis;
	if(pThis == NULL)
		return false;

	CBasePlayer* pPlayer = This->mPlayer;
	if(pPlayer == NULL)
		return false;

	UtilThread* pThread = pPlayer->GetThreadById();
	if(pThread == NULL)
		return false;
	
	if(pThread->IsNeedQuit())
	{
		INFO("mbQuit");
		return true;
	}

	if(This->mbQuit)
	{
		INFO("mbQuit");
		return true;
	}
		
	This->CheckQueueData(This);
		
	if(pPlayer->GetPlayMode()== MEDIA_PLAY_MODE_TIMESHIFT)
	{
		if(!pPlayer->IsTShiftRun())
		{
			INFO("Record has stop");
			return true;
		}
		else 
			return false;
	}
	return false;
}

int CDemux::OpenInputFile (string url)
{
    AVFormatContext *pFormatCtx;
    int video_index = -1;
    int audio_index = -1;
		
    PlayCore::GetInstance()->url_set_interrupt_cb(CDemux::DecodeInterruptCB,(void*)this);
    // Open video file
    if(PlayCore::GetInstance()->av_open_input_file(&pFormatCtx, url.c_str (), NULL, 0, NULL)!=0)
    {
        ERROR ("Couldn't open file:%s", url.c_str ());
		if(!mbInterrupt)
			mPlayer->Notify(MEDIA_ERROR,MEDIA_OPENING,ERR_NOT_SUPPORTED);
        return ERR_FILE_NOT_FOUND;
    }

    // Retrieve stream information
    if(PlayCore::GetInstance()->av_find_stream_info(pFormatCtx)<0)
    {
        WARN("Couldn't find stream information!");
        PlayCore::GetInstance()->av_close_input_file (pFormatCtx);
		if(!mbInterrupt)
			mPlayer->Notify(MEDIA_ERROR,MEDIA_OPENING,ERR_NOT_SUPPORTED);
        return ERR_INVALID_DATA;
    }

    // Find the first video stream
    for(int i = 0; i < pFormatCtx->nb_streams; i++)
    {
        if(pFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO && video_index < 0)
        {
            video_index=i;
        }
        if(pFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_AUDIO && audio_index < 0)
        {
            audio_index=i;
        }
    }

    if (audio_index < 0 && video_index < 0)
    {
        WARN ("there is no audio or video stream in the file, quit!");
        PlayCore::GetInstance()->av_close_input_file (pFormatCtx);
		if(!mbInterrupt)
			mPlayer->Notify(MEDIA_ERROR,MEDIA_OPENING,ERR_NOT_SUPPORTED);
        return ERR_INVALID_DATA;
    }
	
    // Open audio codec
    if(audio_index >= 0)
    {
        AVCodecContext *codecCtx = pFormatCtx->streams[audio_index]->codec;
		
        AVCodec *codec = PlayCore::GetInstance()->avcodec_find_decoder(codecCtx->codec_id);
		
		INFO("ACODEC ID %d",codecCtx->codec_id);
		
        if(!codec || (PlayCore::GetInstance()->avcodec_open(codecCtx, codec) < 0))
        {
            ERROR("Unsupported codec!");
			
            PlayCore::GetInstance()->av_close_input_file (pFormatCtx);
			
			if(!mbInterrupt)
				mPlayer->Notify(MEDIA_ERROR,MEDIA_OPENING,ERR_NOT_SUPPORTED);
			
            return ERR_NOT_SUPPORTED;
        }
		
        CAudioDecoderImp::SetTimebase (pFormatCtx->streams[audio_index]->time_base);
		
		mbAudioReadyForPlay = false;
		
	    miAudioStream 	= audio_index;
    }

    // Open video codec
    if(video_index >= 0)
    {
        AVCodecContext *codecCtx = pFormatCtx->streams[video_index]->codec;
		
        AVCodec *codec = PlayCore::GetInstance()->avcodec_find_decoder(codecCtx->codec_id);
		
		INFO("VCODEC ID %d width %d, height %d bit_rate %d",codecCtx->codec_id,codecCtx->width,codecCtx->height,codecCtx->bit_rate);
		
		if(!codec || (PlayCore::GetInstance()->avcodec_open(codecCtx, codec) < 0))
        {
            ERROR("Unsupported codec!");

			if (miAudioStream >= 0 &&  pFormatCtx->streams[miAudioStream]->codec != NULL)
		    {
				PlayCore::GetInstance()->avcodec_close(pFormatCtx->streams[miAudioStream]->codec);

				miAudioStream = -1;
		    }
			
            PlayCore::GetInstance()->av_close_input_file (pFormatCtx);
			if(!mbInterrupt)
				mPlayer->Notify(MEDIA_ERROR,MEDIA_OPENING,ERR_NOT_SUPPORTED);
			
            return ERR_NOT_SUPPORTED;
        }

		codecCtx->get_buffer 		= CVideoDecoderImp::our_get_buffer;
		
        codecCtx->release_buffer 	= CVideoDecoderImp::our_release_buffer;

		miVideoStream 	= video_index;
    }   

    // If reach here, there is a video or audio or both streams is got.
    mpFormatCtx 	= pFormatCtx;
	mSyncPTS 		= INVALID_TIME_VALUE;
	mbSynced 		= false;
	mDCS			= DCS_NONE;

	if(mPlayer->GetPlayMode()== MEDIA_PLAY_MODE_ONDEMAND)
	{
		mMaxCache = DEFAULT_MAX_SIZE;
	}
	if(miAudioStream >= 0 && miVideoStream < 0)
	{
		mAudioQueue.init(DEFAULT_MAX_SIZE/2);
		mMaxCache = DEFAULT_MAX_SIZE/4;
	}

	mCanPause = false;
	DEBUG("file_size %d",mpFormatCtx->file_size);
	if(mpFormatCtx->file_size>0)
	{
		mCanPause = true;
	}
	
    return ERR_NONE;
}

int CDemux::Reset ()
{
	INFO("");
	mVideoQueue.flush ();
	mAudioQueue.flush ();

	if (mpFormatCtx)
	{		
		if (miVideoStream >= 0 &&  mpFormatCtx->streams[miVideoStream]->codec != NULL)
		{
			PlayCore::GetInstance()->avcodec_close(mpFormatCtx->streams[miVideoStream]->codec);
		}
		if (miAudioStream >= 0 &&  mpFormatCtx->streams[miAudioStream]->codec != NULL)
		{
			PlayCore::GetInstance()->avcodec_close(mpFormatCtx->streams[miAudioStream]->codec);
		}
		
		PlayCore::GetInstance()->av_close_input_file (mpFormatCtx);
		mpFormatCtx = NULL;
	}
    miVideoStream 	= -1;
    miAudioStream 	= -1;
    mSyncPTS 		= INVALID_TIME_VALUE;
	mbPaused 		= false;
    mVideoQueue.init (MAX_VIDEO_SIZE);
    mAudioQueue.init (MAX_AUDIO_SIZE);
    mbQuit 			= false;
	mbInterrupt 	= false;
	mbAudioReadyForPlay = false;
	mbReachEnd 		= false;
	mFirstCache 	= true;
	mbOpened		= false;
	
    return ERR_NONE;
}

int CDemux::CheckQueueData(CDemux* This,bool bCheckLow)
{
	//get all size of data exsit in AV Queue
	int MaxSize = mFirstCache?This->mMaxCache:This->mMaxCache*2;
	
	int DataSize = This->mAudioQueue.getDataSize()+ This->mVideoQueue.getDataSize();

	int NowPercent = (DataSize*100)/MaxSize;

	if(!mbOpened)
		return ERR_NONE;

	if(This->mbInterrupt)
		return ERR_NONE;
	
	DEBUG("WT: DataSize %d MaxSize %d NowPercent %d",DataSize,MaxSize,NowPercent);
	//a/v is not synced,todo:fix can not sync when the data of cache reach max
	if(!This->IsSynced())
	{
		This->mDCS = DCS_BUFFING_BEFORE_SYNC;

		This->mIsPaused = This->IsPaused();

		DEBUG("WT: mDCS = BEFORE_SYNC mIsPaused %d NowPercent %d DataSize %d mbReachEnd %d VQueueSize %d",\
			This->mIsPaused,NowPercent,DataSize,This->mbReachEnd,This->mVideoQueue.getDataSize());
		//read end of file and the v stream is process over and size is zero before sync
		if(This->mbReachEnd&&This->mVideoQueue.getDataSize()<=0)
		{
			INFO("WT: CDemux::mbReachEnd");
			return ERR_END_DEMUX_PLAY;
		}

		if(This->mPercent != NowPercent)
		{
			This->mPlayer->Notify(MEDIA_SYNCING,NowPercent,DataSize);
			This->mPercent = NowPercent;
		}
	}
	else if(This->mDCS == DCS_BUFFING_BEFORE_SYNC)//has a/v sync to notify sync message
	{
		This->mDCS = DCS_SYNC;

		This->mIsPaused = This->IsPaused();

		DEBUG("WT: mDCS = DCS_SYNC mIsPaused %d NowPercent %d DataSize %d",\
			This->mIsPaused,NowPercent,DataSize);
				
		mPlayer->Notify(MEDIA_SYNC,This->mPercent,0);

		This->mDCS = DCS_BUFFING_AFTER_SYNC;
	}
	else if(This->mDCS == DCS_BUFFING_AFTER_SYNC)//after sync to continue to cache media data to MaxSize of cache
	{			
		DEBUG("WT:  AFTER_SYNC NowPercent %d mbAudioReadyForPlay %d DataSize %d mbReachEnd %d",\
			NowPercent,This->mbAudioReadyForPlay,DataSize,This->mbReachEnd);
		
		//not reach end and Percent little 99
		if(This->mbReachEnd == false && NowPercent < 99)
		{
			if(NowPercent < 0)
				NowPercent = 0;

			if(NowPercent > 99)
				NowPercent = 99;

			if(NowPercent != This->mPercent)
			{
				This->mPlayer->Notify(MEDIA_BUFFERING,NowPercent,DataSize);
				This->mPercent = NowPercent;
			}
				
		}//not reach end and percent big than 99 and audio not ready for paly
		else if(This->mbReachEnd == false && (This->miAudioStream >= 0 && !This->mbAudioReadyForPlay))
		{
			if(NowPercent < 0)
				NowPercent = 0;

			if(NowPercent > 99)
				NowPercent = 99;

			if(NowPercent != This->mPercent)
			{
				This->mPlayer->Notify(MEDIA_BUFFERING,NowPercent,DataSize);
				This->mPercent = NowPercent;
			}
		}
		else
		{
			This->mDCS = DCS_CHECK_LOW;

			NowPercent = 100;

			This->mPercent = -1;

			INFO("mbSeek = %d mIsPaused = %d",This->mbSeek,This->mIsPaused);
							
			if(This->mbSeek)
			{					
				This->mPlayer->Notify(MEDIA_SEEK_COMPLETE,NowPercent,DataSize);

				This->mbSeek = false;
			}
			else
			{
				This->mbLow = false;
				
				This->mPlayer->Notify(MEDIA_BUFFERING,NowPercent,DataSize);
			}
			//to restore play state				
			if(!This->mIsPaused)
			{
				INFO("WT:  AFTER_SYNC mIsPaused %d NowPercent %d DataSize%d MaxSize %d",\
					This->mIsPaused,NowPercent,DataSize,MaxSize);
				
				//StartPlay Auto
				if(!This->mpClock->IsRun())
				{
					This->mpClock->Start();
					mFirstCache = false;
				}

			}
			if(This->mbReachEnd)
			{
				INFO("WT: CDemux::mbReachEnd");
				return ERR_END_DEMUX_PLAY;
			}
		}
	}
	else if(This->mDCS == DCS_CHECK_LOW)//if not reach end to check data percent
	{
		DEBUG("WT:  DCS_CHECK_LOW NowPercent %d, mbReachEnd %d",NowPercent,This->mbReachEnd);
		if(!bCheckLow)
		{
			return ERR_NONE;
		}
		//if  ReachEnd break;
		if(This->mbReachEnd)
		{
			INFO("WT: CDemux::mbReachEnd");
			return ERR_END_DEMUX_PLAY;
		}
		
		if(NowPercent <= 1)
		{						
			This->mDCS = DCS_BUFFING_AFTER_SYNC;

			This->mIsPaused = This->IsPaused();

			DEBUG("WT:  DCS_CHECK_LOW mIsPaused %d NowPercent %d DataSize%d MaxSize %d mpClock->IsRun() %d",\
				This->mIsPaused,NowPercent,DataSize,MaxSize,This->mpClock->IsRun());

			if(This->mpClock->IsRun())
			{
				INFO("WT:  DCS_CHECK_LOW  mpClock->Stop()");
				
				This->mpClock->Stop();

				This->mbAudioReadyForPlay = false;
			}

			This->mPercent = -1;

			if(!This->mbQuit)
			{
				INFO("WT:  DCS_CHECK_LOW mPlayer->Notify");				
				This->mPlayer->Notify(MEDIA_BUFFERING,NowPercent,DataSize);

				This->mbLow = true;
			}
		}
	}
	else
	{
		ERROR("err case mDCS  %d",This->mDCS);
	}

	return ERR_NONE;
}

void CDemux::UpdateDuration(bool bForce)
{
	AVFormatContext *pFormatCtx = mpFormatCtx;

	if(pFormatCtx && mPlayer &&  mPlayer->GetPlayMode()== MEDIA_PLAY_MODE_TIMESHIFT)
	{
		int64_t oldfilesize = pFormatCtx->file_size;
				
		int64_t filesize = PlayCore::GetInstance()->url_fsize(pFormatCtx->pb);
		
		if((filesize - oldfilesize)>=1*1024*1024 || bForce)
		{
			PlayCore::GetInstance()->av_update_timings(pFormatCtx);	
			INFO("durtioan %f,oldfilesize %lld ,newfilesize %lld",(float)pFormatCtx->duration/AV_TIME_BASE,oldfilesize,filesize);
		}
	}
}

void CDemux::ThreadEntry ()
{
    AVFormatContext *pFormatCtx 		= mpFormatCtx;
    AVPacket pkt1, *packet 				= &pkt1;
	bool 			bReadOk 			= false;
	static bool 	bNeedUpdateDuration = true;

    if (pFormatCtx == NULL || mPlayer == NULL)
    {
        ERROR ("WT: pFormatCtx == NULL!");
        return;
    }

	mPlayer->AttachThread(this);

	memset((void*)packet,0,sizeof(AVPacket));

    for(;;)
    {
        if(mbQuit)
        {
        	DEBUG("WT: CDemux::mbQuit");
            break;
        }
	
		if(mPlayer->GetPlayMode()== MEDIA_PLAY_MODE_TIMESHIFT)
		{
			if(mPlayer->IsTShiftRun())
			{				
				UpdateDuration();
			}
			else if(bNeedUpdateDuration)
			{
				INFO("final update duration");
				bNeedUpdateDuration = false;
				UpdateDuration(true);
			}
		}

		if(CheckQueueData(this)==ERR_END_DEMUX_PLAY)
		{	
			INFO("WT:DEMUX FINISH");
			break;
		}

		bReadOk = false;
		// if not ReachEnd and the packet is no Data
		// av_read_frame fail, reach end of file or an error occur!"
        if(!mbReachEnd && packet->data == NULL)
        {
        	int ret = PlayCore::GetInstance()->av_read_frame(pFormatCtx, packet);
			
        	if( ret < 0)
        	{						
				bReadOk   = false;

				INFO("WT:  mDCS = %d Reach end ret %d mIsPaused %d mPercent %d  mpClock->IsRun() %d pFormatCtx->pb %d pFormatCtx->pb->error %d",\
					mDCS,ret,mIsPaused,mPercent,mpClock->IsRun(),
					pFormatCtx->pb,pFormatCtx->pb?pFormatCtx->pb->error:100000);
									
        		if (ret == AVERROR_EOF || PlayCore::GetInstance()->url_feof(pFormatCtx->pb))                
				{
					mbReachEnd = true;

					INFO("WT:  mDCS = %d Reach end mIsPaused %d mPercent %d mpClock->IsRun() %d mbReachEnd %d",\
						mDCS,mIsPaused,mPercent,mpClock->IsRun(),mbReachEnd);
        		}
				else if (mPlayer->GetPlayMode()!= MEDIA_PLAY_MODE_TIMESHIFT && pFormatCtx->pb && pFormatCtx->pb->error)   
				{
					mbReachEnd = true;
									
					INFO("WT: mDCS == %d Reach end err %d mIsPaused %d mPercent %dmpClock->IsRun() %d mbReachEnd %d",mDCS,pFormatCtx->pb->error,mIsPaused,mPercent,mpClock->IsRun(),mbReachEnd);
					if(!mbInterrupt)
						mPlayer->Notify(MEDIA_ERROR,MEDIA_PLAYING,0);
				}
				
        	}
			else
			{
				mbReachEnd = false;
				
				DEBUG("WT: Reach end mbReachEnd %d",mbReachEnd);
			}
        }

		if(packet->data != NULL)
		{
			bReadOk   = true;
		}
        // Is this a packet from the video stream?
        if(bReadOk)
        {
        	if(packet->stream_index == miVideoStream)
	        {
				if(!mVideoQueue.isFullWithDataSize(packet->size))
	            {
   		        	DEBUG ("put video packet, pts: %lld, size:%d", packet->pts, packet->size);
	            	mVideoQueue.put (packet, packet->size);
					memset((void*)packet,0,sizeof(AVPacket));
				}
				else
				{
					usleep(100*1000); //Reduce CPU usage
				}	
	        }
	        else if(packet->stream_index == miAudioStream)
	        {
				if(!mAudioQueue.isFullWithDataSize(packet->size))
	            {
   		   			DEBUG ("put audio packet, pts: %lld, size:%d", packet->pts, packet->size);
	            	mAudioQueue.put (packet, packet->size);
					memset((void*)packet,0,sizeof(AVPacket));
				}
				else
				{
					usleep(100*1000);//Reduce CPU usage
				}
	        }
	        else
	        {
	        	DEBUG("WT: Loop av_free_packet");
	            PlayCore::GetInstance()->av_free_packet(packet);
				memset((void*)packet,0,sizeof(AVPacket));
	        }
        }
    }
	//release the packet at the end
	if (packet->data != NULL) 
	{
		DEBUG("WT: END Loop av_free_packet");
        PlayCore::GetInstance()->av_free_packet(packet);
	}
	
	//sync 不成功解码线程退出的条件	 
	if(mDCS == DCS_BUFFING_BEFORE_SYNC)
	{
		INFO ("WT: END! NOTSYNC FLUSH PACKET TO VIDEO!");
		mVideoQueue.flush();			
		
		mSyncPTS = INVALID_TIME_VALUE;
	}

	INFO ("WT: END! INSERT NULL PACKET TO VIDEO!");
	mVideoQueue.put ((AVPacket*)&CPacketQueue::mNullPacket, 0); 

	INFO ("WT: END OF DEMUX,WAIT VIDEO TO FINISH");	
	if(mVOutPlay)
		mVOutPlay->Wait();

	//sync 不成功解码线程退出的条件
	if(mDCS == DCS_BUFFING_BEFORE_SYNC)
	{
		INFO ("WT: END! NOTSYNC FLUSH PACKET TO AUDIO!");
		
		mAudioQueue.flush();

		mSyncPTS = 0.0;
	}

	INFO ("WT: END! INSERT NULL PACKET TO AUDIO!");	 
	mAudioQueue.put ((AVPacket*)&CPacketQueue::mNullPacket, 0);
	
	INFO ("WT: END OF DEMUX,WAIT AUDIO TO FINISH mbQuit %d",mbQuit); 	 
	if(mAOutPlay)
		mAOutPlay->Wait();

	if(mpClock)
		mpClock->Stop();
	//1.cannot sync after seek 
	//2.a/v decoder not quit by user;
	if((mDCS == DCS_BUFFING_BEFORE_SYNC && mbSeek)
		||(mVOutPlay&&mVOutPlay->ThreadExitReason()!=CBaseDecoder::ER_QUIT)
		||(mAOutPlay&&mAOutPlay->ThreadExitReason()!=CBaseDecoder::ER_QUIT))
	{
		INFO ("WT: END OF DEMUX,MEDIA_PLAYBACK_COMPLETE");
		mPlayer->Notify(MEDIA_PLAYBACK_COMPLETE,0,0);
	}
	
    INFO ("WT: END OF DEMUX");
	mPlayer->DetachThread();
}

AVPacket CDemux::GetVideoPacket ()
{
    return mVideoQueue.get ();
}

AVPacket CDemux::GetAudioPacket ()
{
    return mAudioQueue.get ();
}

AVFormatContext* CDemux::GetFormatContext ()
{
    return mpFormatCtx;
}
int CDemux::GetVideoStreamIndex ()
{
    return miVideoStream;
}
int CDemux::GetAudioStreamIndex ()
{
    return miAudioStream;
}

int CDemux::AvSeekFrame (double pos)
{
	DEBUG("CDemux::AvSeekFrame mpFormatCtx 0x%x",mpFormatCtx);

    if (mpFormatCtx == NULL)
    {
        ERROR ("mpFormatCtx == NULL");
        return ERR_INVALID_DATA;
    }
    else
    {
    	int64_t seek_target = pos * AV_TIME_BASE;
		int stream_index = -1;
		int seek_flags = AVSEEK_FLAG_BACKWARD;//rel < 0 ? AVSEEK_FLAG_BACKWARD : 0;
		
		if(mpFormatCtx->iformat->flags & AVFMT_TS_DISCONT)
			seek_flags |=AVSEEK_FLAG_BYTE;

		if (miVideoStream >= 0)
			stream_index = miVideoStream;
		else if (miAudioStream >= 0)
			stream_index = miAudioStream;

		DEBUG("CDemux::AvSeekFrame stream_index %d",stream_index);

		if(mpFormatCtx->iformat->flags & AVFMT_TS_DISCONT)
		{
			INFO("seek_target %lld",seek_target);
			if(mpFormatCtx->duration <= 0)
				return ERR_NOT_SUPPORTED;
						
			int64_t size = PlayCore::GetInstance()->url_fsize(mpFormatCtx->pb);
			INFO("filesize %lld duration %lld",size,mpFormatCtx->duration);
			if(size <= 0)
				return ERR_NOT_SUPPORTED;
			
			seek_target = (seek_target/(float)mpFormatCtx->duration)*size;
		}
		else if(stream_index >= 0)
		{
			seek_target = PlayCore::GetInstance()->av_rescale_q (seek_target, AV_TIME_BASE_Q, mpFormatCtx->streams[stream_index]->time_base);
		}
		INFO("seek_target %lld,seek_flags %d",seek_target,seek_flags);
		
		if(PlayCore::GetInstance()->av_seek_frame (mpFormatCtx, stream_index, seek_target, seek_flags) < 0)
		{
			ERROR("%s: error while seeking", mpFormatCtx->filename);
			return ERR_NOT_SUPPORTED;
		}
    }
    return ERR_NONE;
}
void   CDemux::SetSyncPTS(double SyncPTS)
{
	DEBUG("mSyncPTS %f",SyncPTS);

	mSyncPTS = SyncPTS;
}
double   CDemux::GetSyncPTS()
{
	return mSyncPTS;
}
void   CDemux::SetSynced(bool bSynced)
{
	DEBUG("mbSynced %d",bSynced);

	mbSynced = bSynced;
}
bool   CDemux::IsSynced()
{
	return mbSynced;
}
void   CDemux::SetAudioReadForPlay(bool bReady)
{
	INFO("mbAudioReadyForPlay %d",bReady);

	mbAudioReadyForPlay = bReady;
}

bool CDemux::IsEmptyAudioQueue()
{
	return mAudioQueue.isEmpty();
}

bool CDemux::IsEmptyVideoQueue()
{
	return mVideoQueue.isEmpty();
}

bool   CDemux::IsAudioReadyForPlay()
{
	return mbAudioReadyForPlay;
}
int CDemux::Pause (bool bPause)
{
	DEBUG("mbPaused %d ThreadIsRunning %d",mbPaused,ThreadIsRunning());
	if(!ThreadIsRunning())
		return false;

	mbPaused = bPause;

	return ERR_NONE;
}

bool CDemux::IsPaused()
{
	DEBUG("mbPaused %d ThreadIsRunning %d",mbPaused,ThreadIsRunning());
	if(!ThreadIsRunning())
		return true;
	
	return mbPaused;
}
void CDemux::SetAudioOutPlay(CBaseDecoder* AOP)
{
	mAOutPlay = AOP;
}
void CDemux::SetVideoOutPlay(CBaseDecoder* VOP)
{
	mVOutPlay = VOP;
}
void CDemux::SetMasterClock (CMasterClock* clock)
{
	mpClock = clock;
}


