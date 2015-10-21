package com.tangram.uninstalledobserver;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

/**
 * @author wanglx
 * @note 监听此应用是否被卸载，若被卸载则弹出卸载反馈
 * @note 由于API17加入多用户支持，原有命令在4.2及更高版本上执行时缺少userSerial参数，特此修改
 *
 */

public class UninstalledObserverDemoActivity extends Activity
{
	private Button bt1, bt2, bt3, bt4;
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        bt1 = (Button)findViewById(R.id.button1);
        bt2 = (Button)findViewById(R.id.button2);
        bt3 = (Button)findViewById(R.id.button3);
        bt4 = (Button)findViewById(R.id.button4);
        UninstalledObserver.init("http://www.baidu.com", this);
        bt1.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View arg0) {
				UninstalledObserver.stop(UninstalledObserverDemoActivity.this);
				Log.e("UninstalledObserver", "关闭");
			}
		});
        bt2.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View arg0) {
				UninstalledObserver.init("http://www.baidu.com", UninstalledObserverDemoActivity.this);
				Log.e("UninstalledObserver", "打开");
			}
		});
        bt3.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View arg0) {
				UninstalledObserver.setEnable(true, UninstalledObserverDemoActivity.this);
				Log.e("UninstalledObserver", "使能开");
			}
		});
        bt4.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View arg0) {
				UninstalledObserver.setEnable(false, UninstalledObserverDemoActivity.this);
				Log.e("UninstalledObserver", "使能关");
			}
		});
    }
}