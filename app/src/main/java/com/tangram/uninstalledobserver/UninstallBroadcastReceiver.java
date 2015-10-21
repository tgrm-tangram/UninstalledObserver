package com.tangram.uninstalledobserver;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

/**
 * 开机 启动卸载监听
 * @author wanglx
 *
 */
public class UninstallBroadcastReceiver extends BroadcastReceiver{

	@Override
	public void onReceive(Context arg0, Intent arg1) {
		Log.e("+++++++++++++++++++++++", "开机了");
		if(UninstalledObserver.isEnable(arg0)){
			UninstalledObserver.resetPid(arg0);
			UninstalledObserver.init(UninstalledObserver.getUrl(arg0), arg0);
		}
	}
}
