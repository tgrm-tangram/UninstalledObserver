package com.tangram.uninstalledobserver;

import android.annotation.TargetApi;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.PackageInfo;
import android.os.Build;
import android.text.TextUtils;
import android.util.Log;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

/**
 * 卸载反馈
 * @author wanglx
 *
 */
public class UninstalledObserver {
	private static final String TAG = "UninstalledObserver";
	private final static String VERSION_NAME = "versionName";
	private final static String MODEL = "model";
    private final static String OS = "os";
    private final static String ANDROID = "android";
	private final static String VERSION = "version";
	private final static String URL = "url";

    /**
	  * 初始化卸载监听进程
	  * @param url 反馈网页链接
	  * @param context 上下文
	  * @return pid
	  */
	public static int init(String url, Context context){
		if(TextUtils.isEmpty(url) || context == null){
			Log.e(TAG, "init fail: url or context is null");
			return -1;
		}
		SharedPreferences setting = context.getSharedPreferences("UninstalledObserver", Context.MODE_PRIVATE);
		int mObserverProcessPid =setting.getInt("ObserverProcessPid", -1);
		boolean enable = setting.getBoolean("ObserverEnable", true);
		if(!enable){
			return -1;
		}
		if(mObserverProcessPid != -1){
			return mObserverProcessPid;
		}
		String urlAddParam = getUrlAddParamer(url, context);
		try{
			if (Build.VERSION.SDK_INT < 17){ // API level小于17，不需要获取userSerialNumber
			    mObserverProcessPid = init(urlAddParam,context.getFilesDir().getParent(), null);
			}else{// 否则，需要获取userSerialNumber
			    mObserverProcessPid = init(urlAddParam, context.getFilesDir().getParent(), getUserSerial(context));
			}
		}catch(Exception e){
			e.printStackTrace();
		}
		//Toast.makeText(context, urladdParamer + "\n"+ mObserverProcessPid +"\n"+ context.getFilesDir().getParent(), Toast.LENGTH_LONG).show();
		Log.e(TAG,  urlAddParam + "\n"+ mObserverProcessPid +"\n"+ context.getFilesDir().getParent());
		SharedPreferences.Editor edit = setting.edit();
		edit.putInt("ObserverProcessPid", mObserverProcessPid);
		edit.commit();
		return mObserverProcessPid;
	}
	/**
	 * 添加参数
	 * @param url 反馈网页链接
	 * @param context 上下文
	 * @return 添加参数后的url
	 *
	 */
	private static String getUrlAddParamer(String url, Context context) {
		SharedPreferences setting = context.getSharedPreferences("UninstalledObserver", Context.MODE_PRIVATE);
		boolean isInit = setting.getBoolean("isInit", false);
		if(!isInit){
			try {
				SharedPreferences.Editor edit = setting.edit();
				PackageInfo packInfo = context.getPackageManager().getPackageInfo(context.getPackageName(), 0);
				edit.putString(VERSION_NAME, packInfo.versionName);
                edit.putString(MODEL, Build.MODEL);
				edit.putString(VERSION, Build.VERSION.RELEASE);
				edit.putString(URL, url);
				edit.putBoolean("isInit", true);
				edit.commit();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return url + "?" + MODEL + "=" + setting.getString(MODEL, "")
				+ "&" + OS + "=" + ANDROID + setting.getString(VERSION, "")
				+ "&" + VERSION + "=" + setting.getString(VERSION_NAME, "");
	}
	
	/**
	 * 获取卸载反馈url
	 * @param context 上下文
	 * @return 完整url
	 */
	public static String getUrl(Context context){
		SharedPreferences setting = context.getSharedPreferences("UninstalledObserver", Context.MODE_PRIVATE);
		return setting.getString(URL, setting.getString(URL, ""));
	}
	
	// 由于targetSdkVersion低于17，只能通过反射获取
	@TargetApi(Build.VERSION_CODES.JELLY_BEAN_MR1)
	private static String getUserSerial(Context context){
	    Object userManager = context.getSystemService(Context.USER_SERVICE);
	    if (userManager == null){
	        Log.e(TAG, "userManager not exsit !!!");
	        return null;
	    }
	    try{
	        Method myUserHandleMethod = android.os.Process.class.getMethod("myUserHandle", (Class<?>[]) null);
	        Object myUserHandle = myUserHandleMethod.invoke(android.os.Process.class, (Object[]) null);
	        Method getSerialNumberForUser = userManager.getClass().getMethod("getSerialNumberForUser", myUserHandle.getClass());
	        long userSerial = (Long) getSerialNumberForUser.invoke(userManager, myUserHandle);
	        return String.valueOf(userSerial);
	    } catch (NoSuchMethodException | IllegalArgumentException | IllegalAccessException | InvocationTargetException e){
	        Log.e(TAG, "", e);
	    }
        return null;
	}
	/**
	 * 停止监听
	 */
	public static void stop(Context context){
		SharedPreferences setting = context.getSharedPreferences("UninstalledObserver", Context.MODE_PRIVATE);
		int mObserverProcessPid = setting.getInt("ObserverProcessPid", -1);
		if(mObserverProcessPid != -1){
			android.os.Process.killProcess(mObserverProcessPid);
			SharedPreferences.Editor edit = setting.edit();
			edit.putInt("ObserverProcessPid", -1);
			edit.commit();
		}
	}
	
	public static void resetPid(Context context){
		SharedPreferences setting = context.getSharedPreferences("UninstalledObserver", Context.MODE_PRIVATE);
		SharedPreferences.Editor edit = setting.edit();
		edit.putInt("ObserverProcessPid", -1);
		edit.commit();
	}
	/**
	 * 设置监听使能 默认为true成
	 * @param b 使能
	 */
	public static void setEnable(boolean b, Context context){
		SharedPreferences setting = context.getSharedPreferences("UninstalledObserver", Context.MODE_PRIVATE);
		SharedPreferences.Editor edit = setting.edit();
		edit.putBoolean("ObserverEnable", b);
		edit.commit();
		if(!b){
			stop(context);
		}
	}
	
	public static boolean isEnable(Context context){
		SharedPreferences setting = context.getSharedPreferences("UninstalledObserver", Context.MODE_PRIVATE);
		return setting.getBoolean("ObserverEnable", true);
	}
	
    private static native int init(String url , String dataPath, String userSerial);
    
    static
    {
        Log.d(TAG, "load lib --> uninstalled_observer");
        System.loadLibrary("uninstalled_observer");
    }
}
