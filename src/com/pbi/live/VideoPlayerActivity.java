package com.pbi.live;

import java.util.LinkedList;
import android.app.Activity;

import com.pbi.live.R;
import com.pbi.player.LivePlayer.TimeShiftInfo;

import android.content.ContentResolver;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.view.Window;
import android.view.WindowManager;

public class VideoPlayerActivity extends Activity {
	
	private final static String TAG = "VideoPlayerActivity";
	
	private VideoView vv = null;
	
	public static LinkedList<MovieInfo> playList = new LinkedList<MovieInfo>();
	public class MovieInfo{
		String displayName;  
		String path;
	}

	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
    	
    	requestWindowFeature(Window.FEATURE_NO_TITLE);
    	requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);
    	setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
    	
        super.onCreate(savedInstanceState);  
        setContentView(R.layout.main);
        
        
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        
        processIntentData(getIntent());
    }
    
    protected String getSchemePath(Uri uri)
	{
		if(uri != null)
		{
			if("content".equals(uri.getScheme()))
			{
				ContentResolver contentResolver = getContentResolver();
				Cursor c = contentResolver.query(uri, null, null, null, null);
				if(c.moveToFirst())
				{
					return c.getString(c.getColumnIndex(MediaStore.Video.Media.DATA));
				}
			}
			else if("file".equals(uri.getScheme()))
			{
				return uri.getPath();
//				String path = uri.toString();
//				//先对字符串进行解编码，若本身不是编码方式原文输出
//				path = Uri.decode(path);				
//				path = this.encodeFileName(path, "/");
//				URI fileURI = URI.create(path);
//				File file = new File(fileURI);
//				
//				return file.getPath();
			}
			else if("http".equals(uri.getScheme()))
			{
				return uri.toString();
			}
		}
		
		return "";
	}
	
	/**
	 * 对文件夹或者文件特殊字符进行遍历编码
	 * @author song.lj
	 * @return String
	 */
	private String encodeFileName(String path, String regular){
	    String[] parts = path.split(regular);
	    StringBuffer sb = new StringBuffer();
	    sb.append(parts[0]);
	    for(int i=1; i<parts.length; i++){
	    	sb.append(regular);
	    	sb.append(Uri.encode(parts[i]));
	    }
	    
	    return sb.toString();
	}
	
    @SuppressWarnings("null")
	protected void processIntentData(Intent intent)
	{
		String path = null;
		if (intent.getAction() != null&& intent.getAction().equals(Intent.ACTION_VIEW )) 
		{
            /* Started from external application */
            path = getSchemePath(intent.getData());
            
            if (path == null || path.length() <= 1)
            {
            	//finish();
            	return ;
            }
		}
		else
		{
			Bundle bundle = intent.getExtras();
			if(bundle == null)
			{
//				this.finish();
				return ;
			}
			path = bundle.getString("file");
		}
		
		if(vv == null)
		{
			vv = (VideoView)findViewById(R.id.vv);
			TimeShiftInfo TShiftInfo = new TimeShiftInfo();
	    	
			TShiftInfo.mbAllowTimeShift = true;
			TShiftInfo.mFileName		= "/sdcard/ptvlive.cache";
			TShiftInfo.mFileSize		= 100*1024*1024;
	    	
			vv.SetTimeShiftInfo(TShiftInfo);
			vv.setVideoPath(path);
		}
		else
		{
			TimeShiftInfo TShiftInfo = new TimeShiftInfo();
	    	
			TShiftInfo.mbAllowTimeShift = true;
			TShiftInfo.mFileName		= "/sdcard/ptvlive.cache";
			TShiftInfo.mFileSize		= 100*1024*1024;
			vv.setVideoPath(path);
		}
	}
}
