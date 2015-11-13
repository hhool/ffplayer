package com.pbi.live;

import java.io.File;
import java.util.Arrays;
import java.util.Comparator;

import com.pbi.live.R;

import android.app.ListActivity;
import android.content.Intent;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;
import android.widget.ListView;
import android.widget.TextView;

public class FFPlayFileExplorer extends ListActivity {

private static final String TAG = "FFMpegFileExplorer";
	
	private String 			mRoot = "/sdcard";
	private TextView 		mTextViewLocation;
	private File[]			mFiles;
	private String			mCurFolder = null;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.ffplay_file_explorer);
		mTextViewLocation = (TextView) findViewById(R.id.textview_path);
		getDirectory(mRoot);
	}
	
	protected static boolean checkExtension(File file) {
		return true;
	}
	
	private void sortFilesByDirectory(File[] files) {
		Arrays.sort(files, new Comparator<File>() {

			public int compare(File f1, File f2) {
				return Long.valueOf(f1.length()).compareTo(f2.length());
			}
			
		});
	}

	private void getDirectory(String dirPath) {
		try {
			mTextViewLocation.setText("Location: " + dirPath);
			mCurFolder = dirPath;
			File f = new File(dirPath);
			File[] temp = f.listFiles();
			
			sortFilesByDirectory(temp);
			
			File[] files = null;
			if(!dirPath.equals(mRoot)) {
				files = new File[temp.length + 1];
				System.arraycopy(temp, 0, files, 1, temp.length);
				files[0] = new File(f.getParent());
			} else {
				files = temp;
			}
			
			mFiles = files;
			setListAdapter(new FileExplorerAdapter(this, files, temp.length == files.length));
		} 
		catch(Exception ex)
		{
			//FFMpegMessageBox.show(this, "Error", ex.getMessage());
		}
	}
	
	@Override
	protected void onListItemClick(ListView l, View v, int position, long id) {
		File file = mFiles[position];

		if (file.isDirectory()) {
			if (file.canRead())
				getDirectory(file.getAbsolutePath());
			else {
				
			}
		} else {
			if(!checkExtension(file)) {
				
				return;
			}
			
			startPlayer(file.getAbsolutePath());
		}
	}
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if(!mCurFolder.equals(mRoot))
		{
			File file = new File(mCurFolder);
			if( file.canRead())
			{
				getDirectory(file.getParent());
				return true;
			}
		}
		return super.onKeyDown(keyCode, event);
		
	}
	private void startPlayer(String filePath) {
    	Intent i = new Intent(this, VideoPlayerActivity.class);
    	i.putExtra("file", filePath);
    	startActivity(i);
    }
}
