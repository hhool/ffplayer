#include "common.h"
#include "playcore.h"
#include "vdecoder.h"

CVideoDecoder::CVideoDecoder()
{
	Reset();
	mpFrame				= PlayCore::GetInstance()->avcodec_alloc_frame();
    INFO ("0x%x",this);
}

CVideoDecoder::~CVideoDecoder()
{
    INFO ("0x%x",this);
	PlayCore::GetInstance()->av_free(mpFrame);
}

//此处参数设置适用于刚打开和seek后，不适合pause后play
void CVideoDecoder::Reset()
{
	mbFirstKeyFrame = true;
	mbNeedReSync	= false;
	mbNextFrame 	= true;
}

void CVideoDecoder::ReDrawPictrue()
{
	if(mpStubRender)
	{
		((CVideoStubRender*)mpStubRender)->ShowPicture (mpFrame);
	}
}

void CVideoDecoder::ThreadEntry () 
{
    CDemux* pDemux 					= mpDemux;
    AVStream* pStream 				= mpStream;
    CVideoStubRender* pVStubRender 	= (CVideoStubRender*)mpStubRender;
    CMasterClock* pClock 			= mpClock;
    int Ret 						= ERR_NONE;
	int bFrameFinished				= 0;
	AVCodecContext *pCodecContext	= NULL;
	AVPacket AVPkt;
	int64_t  PTS;
	
    if (pDemux == NULL || pStream == NULL || pVStubRender == NULL || pStream->codec == NULL) 
	{
        ERROR ("mpDecoder || mpStream || mpStubRender || mpStream->codec== NULL!");
        return;
    }
    pCodecContext = pStream->codec;

	memset((void*)&AVPkt,0,sizeof(AVPacket));

	DEBUG ("start video decoder loop!");	
	
    for(;;) 
	{
        if(mbQuit) 
		{
			INFO("mbQuit");
			PlayCore::GetInstance()->avcodec_flush_buffers(pCodecContext);
            break;
        }

		if(pDemux->GetSyncPTS()>=0.0)//已经设定了SyncPTS
		{						
			if(!pDemux->IsSynced())//没有被音频流Sync
			{
				DEBUG("NO FOUND SYNCED !!");
				usleep(25*1000);
				continue;
			}
			else if(!pClock->IsRun())//已经设定SyncPTs并且被其他流sync并且当前状态不是播放状态
			{
				usleep(1*1000);//Reduce CPU usage
				continue;
			}
		}
		else 
		{	
			DEBUG("TRY SHOW FIRST KEY VIDEO FRAME mbFirstKeyFrame %s",mbFirstKeyFrame?"true":"false");
		}
		
		if(mbNextFrame)
		{
			AVPkt = pDemux->GetVideoPacket ();
			if (AVPkt.data == 0 || AVPkt.size == 0) 
			{
				INFO ("get NULL packet! continue! must be end of pStream!!! stop thread!");
				break;
			}
		
			//如果A/V已经同步，但此时视频时间戳严重落后于系统时钟
			if(pDemux->IsSynced()&&mbNeedReSync)
			{
				double CurClock = pClock->GetCurrentClock ();
				double FramePTS = AVPkt.pts * av_q2d (pStream->time_base);
	    		double Diff = FramePTS - CurClock;
	            DEBUG ("V:SyncOut CurClock=%f, FramePTS=%f, Diff=%f", CurClock, FramePTS, Diff);
								
				if (Diff >= CMasterClock::AVThresholdSync) 
				{
		            DEBUG ("V:Sync CurClock=%f, FramePTS=%f, Diff=%f", CurClock, FramePTS, Diff);
				}
				else
				{
					PlayCore::GetInstance()->av_free_packet (&AVPkt);
					memset((void*)&AVPkt,0,sizeof(AVPacket));
					continue;
				}
				
			}
			
	        int len = CVideoDecoderImp::Decode (pCodecContext, mpFrame, &bFrameFinished, &AVPkt, &PTS);
			
	        if (len < 0 || bFrameFinished == 0) 
			{
	            WARN ("CVideoDecoderImp::Decode fail! continue to next packet!");
	            PlayCore::GetInstance()->av_free_packet (&AVPkt);
				memset((void*)&AVPkt,0,sizeof(AVPacket));
	            continue;
	        }

			DEBUG("mbFirstKeyFrame %d,key_frame %d,pict_type %d mbNeedReSync %d AV_PICTURE_TYPE_I %d",mbFirstKeyFrame,mpFrame->key_frame,mpFrame->pict_type,mbNeedReSync,AV_PICTURE_TYPE_I);
			//需要关键帧
			if(!pDemux->IsSynced())
			{
				//不是关键帧continue
				if(!(mpFrame->key_frame && mpFrame->pict_type == AV_PICTURE_TYPE_I))
				{
					DEBUG("Not Key frame continue");
					PlayCore::GetInstance()->av_free_packet (&AVPkt);
					memset((void*)&AVPkt,0,sizeof(AVPacket));
					continue;
				}
				else //第一次音视频Sync
				{
					INFO("First Sync after opened");
					mbFirstKeyFrame = true;
				}
					
			}
			else if(mbNeedReSync) //视频时间戳严重落后于系统时钟,第一个合适的关键帧
			{
				if(!(mpFrame->key_frame && mpFrame->pict_type == AV_PICTURE_TYPE_I))
				{
					DEBUG("Not Key frame continue");
					PlayCore::GetInstance()->av_free_packet (&AVPkt);
					memset((void*)&AVPkt,0,sizeof(AVPacket));
					continue;
				}
				else
				{
					INFO("mbNeedReSync %d",mbNeedReSync);
					mbNeedReSync 	= false;
					mbFirstKeyFrame = false;
				}
			}
			else//pDemux go on
			{
				mbFirstKeyFrame = false;
			}
		}
        if (pClock) 
		{
			double FramePTS = PTS * av_q2d (pStream->time_base);
			//渲染视频首个关键帧并且不是播放过程中视频解码落后于系统时钟。设定音频同步时间点
			if(mbFirstKeyFrame)
			{							
				Ret = pVStubRender->ShowPicture (mpFrame);

				INFO("IS Key frame mbFirstKeyFrame %d FramePTS %f ShowPicture Ret %d",mbFirstKeyFrame,FramePTS,Ret);
				
				//成功渲染视频
				if(Ret>=0 || Ret == ERR_DEVICE_NOSET)
				{
					//得到同步时间点，
					double SyncPTS = pDemux->GetSyncPTS();
					INFO("pDemux->GetSyncPTS() %f",SyncPTS);
					if(SyncPTS<0.0)
					{
						//设定同步时间点
						pDemux->SetSyncPTS(FramePTS);
						//设定时钟的起始时间点
						pClock->SetOriginClock (FramePTS);

						//surface fixed  
						pVStubRender->ShowPicture (mpFrame);

						//如果没有音频数据流设定为Synced;
						if(pDemux->GetAudioStreamIndex()<0)
						{
							pDemux->SetSynced(true);
						}

						mbFirstKeyFrame = false;
						mbNextFrame = true;
					}
				}
			}
            else 
			{
				double CurClock = pClock->GetCurrentClock ();
	            double Diff = FramePTS - CurClock;

				DEBUG ("V:CurClock=%f, FramePTS=%f, Diff=%f", CurClock, FramePTS, Diff);
				
				if (fabs(Diff) < CMasterClock::AVThresholdNoSync) 
				{
					mbNextFrame = true;
					
	                if (Diff <= 0) 
					{
	                    VERBOSE ("show it at once.");
	                } 
					else 
					{
	                    unsigned int usec = Diff * 1000 * 1000;
	                    VERBOSE ("wait %d usec", usec);
	                    usleep (usec);
	                }

	                Ret = pVStubRender->ShowPicture (mpFrame);
	            } 
				else
				{
	                if (Diff < 0) 
					{
						mbNextFrame = true;

						//如果系统时钟是以视频时钟为基准的，调整系统时钟
						if(pClock->GetClockType()==CMasterClock::CLOCK_VIDEO)
						{
							WARN("we reset master timer to video FramePTS");
                    		double ClockTime = PlayCore::GetInstance()->av_gettime() / 1000000.0;
                   			pClock->SetOriginClock (FramePTS, ClockTime);
                   			Ret = pVStubRender->ShowPicture (mpFrame);
						}
						else//否则是以其他(音频，系统时间)为系统时钟，此时视频严重落后于同步的时间范围值，
						{	//对于直播不需要设定需要查找最近的关键帧标识

							DEBUG("we need ReSync video FramePTS Diff %f",Diff);
						}
	                } 
					else 
					{
						WARN ("video FramePTS far early than curr_pts Diff %f",Diff);
						unsigned int usec = Diff * 1000 * 1000;
						mbNextFrame  = false;
						usleep (1*1000);
	                }
	            }
				DEBUG("ShowPicture Ret %d",Ret);
            }
        } 
		else 
		{
            ERROR ("you have not set Master Clock!!! will not show pictures!");
        }
		
        PlayCore::GetInstance()->av_free_packet (&AVPkt);
		memset((void*)&AVPkt,0,sizeof(AVPacket));
    }
	
	Reset();
	
	if (AVPkt.data != 0) 
	{
		PlayCore::GetInstance()->av_free_packet (&AVPkt);
	}
    DEBUG ("end of video out thread!");
}

