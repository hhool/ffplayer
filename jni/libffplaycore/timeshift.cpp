#include "timeshift.h"
#include "playcore.h"
#include "baseplayer.h"
#include "demux.h"


#define RECV_BUFSIZE 65536

CTimeShiftManage::CTimeShiftManage (CBasePlayer* Player) :	mPlayer(Player),
															mTimeShiftFile(NULL),
															mCurSize(0),
															mState(TSHIFT_NOP),
															mbQuit(false)
{
	assert(mPlayer != NULL);
	
	INFO("0x%x",this);
}

CTimeShiftManage::~CTimeShiftManage ()
{
	INFO("0x%x",this);
	remove(mTShiftInfo.mFileName);
	INFO("end 0x%x",this);
}

int CTimeShiftManage::Reset ()
{	
	mbQuit			= false;
	mState			= TSHIFT_NOP;
	mCurSize		= 0;
	mTimeShiftFile	= NULL;
	remove(mTShiftInfo.mFileName);
	
    return ERR_NONE;
}

void CTimeShiftManage::SetTimeShiftInfo(TimeShiftInfo TShiftInfo)
{
	memcpy(&mTShiftInfo,&TShiftInfo,sizeof(TimeShiftInfo));
	INFO("Allow %d,name %s,size %d",mTShiftInfo.mbAllowTimeShift,mTShiftInfo.mFileName,mTShiftInfo.mFileSize);
}

int CTimeShiftManage::SetDataSource(const char *url)
{
	mStrDataSource = url;
}

bool CTimeShiftManage::IsAlowTimeShift()
{
	return mTShiftInfo.mbAllowTimeShift;
}

bool CTimeShiftManage::IsDataAvailable()
{
	if(mCurSize >= VAILD_DATASIZE)
		return true;

	return false;
}

char* CTimeShiftManage::GetTimeShiftURL()
{
	INFO("%s",mTShiftInfo.mFileName);
	return mTShiftInfo.mFileName;
}
int CTimeShiftManage::Start()
{
	mbQuit 		= false;
	mCurSize 	= 0;
			
	ThreadExec ();

	return ERR_NONE;
}

int CTimeShiftManage::Stop()
{
	mbQuit 		= true;
	
	ThreadWait ();

	return ERR_NONE;
}

bool CTimeShiftManage::IsNeedQuit()
{
	if(mbQuit)
	{
		INFO("this 0x%x , mbQuit %d",this,mbQuit);
	}
	return mbQuit;
}

bool CTimeShiftManage::IsRun()
{
	return ThreadIsRunning();
}

void CTimeShiftManage::ThreadEntry ()
{
	int64_t nMaxTimeShiftSize 		= mTShiftInfo.mFileSize;
	int		nRecvBufSize			= sizeof(unsigned char)*RECV_BUFSIZE;
	unsigned char *Buffer			= (unsigned char *)malloc(nRecvBufSize);
	int     trycount = 0;
	mPlayer->AttachThread(this);

	if(!(mTimeShiftFile = fopen(mTShiftInfo.mFileName,"wb")))
	{
		if(!mbQuit)
		{
			ERROR("can't create timeshift file %s",mTShiftInfo.mFileName);	
			mState = TSHIFT_OPEN_DST_ERR;
			mPlayer->Notify(MEDIA_ERROR,MEDIA_TIMESHIFT,TSHIFT_OPEN_DST_ERR);
		}
		goto DONE;
	}

	if(PlayCore::GetInstance()->url_fopen(&mpIO, mStrDataSource.c_str (),AVIO_RDONLY)<0)
    {
		if(!mbQuit)
		{
	        ERROR ("Couldn't open file:%s", mStrDataSource.c_str ());
			mState = TSHIFT_OPEN_SRC_ERR;
			mPlayer->Notify(MEDIA_ERROR,MEDIA_TIMESHIFT,TSHIFT_OPEN_SRC_ERR);
		}

		goto DONE;
    }

	mPlayer->Notify(MEDIA_TIMESHIFT,TSHIFT_BEGIN_RECORD,0);
	
   	for(;;)
   	{
   		if(mbQuit)
        {
        	INFO("mbQuit");
            break;
        }
		
		int nRead = ((mCurSize+nRecvBufSize)>nMaxTimeShiftSize)?(nMaxTimeShiftSize-mCurSize):nRecvBufSize;
		int nReaded =mpIO->read_packet(mpIO->opaque,Buffer,nRead);

		if(nReaded >= 0)
		{					
			int n = 0;				
			if((n = fwrite(Buffer,1,nReaded,mTimeShiftFile)) != nReaded)
			{					
				//write err;	
				ERROR("can't write timeshift file %s",mTShiftInfo.mFileName);		
				mState = TSHIFT_WRITE_DST_ERR;
				mPlayer->Notify(MEDIA_ERROR,MEDIA_TIMESHIFT,TSHIFT_WRITE_DST_ERR);
				break;
			}
			else
			{	
				mCurSize += nReaded;
				mPlayer->Notify(MEDIA_TIMESHIFT,TSHIFT_PROGRESS,mCurSize);
				usleep(1000);
			}

			trycount = 0;
			
			if(mCurSize >= nMaxTimeShiftSize)
			{		
				fflush(mTimeShiftFile);
				INFO("finish timeshift file %s mCurSize %lld nMaxTimeShiftSize %lld",mTShiftInfo.mFileName,mCurSize,nMaxTimeShiftSize);	
				mState = TSHIFT_END_RECORD;	
				mPlayer->Notify(MEDIA_TIMESHIFT,TSHIFT_END_RECORD,0);
				break;
			}				
		}		
		else	
		{		
			if(!mbQuit)
			{
				trycount++;
				if(trycount>=5) //bugfix
				{
					ERROR("can't read netdata for timeshift file %s",mTShiftInfo.mFileName);
					mState = TSHIFT_READ_SRC_ERR;		
					mPlayer->Notify(MEDIA_TIMESHIFT,TSHIFT_READ_SRC_ERR,0);
					break;
				}
				else
				{
					usleep(500*1000);
					continue;
				}
				
			}
				
		}
   	}
DONE:
	if(mTimeShiftFile != NULL)
	{
		fclose(mTimeShiftFile);
		mTimeShiftFile = NULL;
	}

	if (mpIO != NULL)
	{				
		PlayCore::GetInstance()->url_fclose(mpIO);
		mpIO = NULL;
	}
	free(Buffer);
	
   	mPlayer->DetachThread();
}


