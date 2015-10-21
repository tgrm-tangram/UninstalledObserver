/* 头文件begin */
#include <jni.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/inotify.h>
#include <sys/stat.h>

#include <android/log.h>
/* 头文件end */

/* 宏定义begin */
//清0宏
#define MEM_ZERO(pDest, destSize) memset(pDest, 0, destSize)

//LOG宏定义
#define LOG_INFO(tag, msg) __android_log_write(ANDROID_LOG_INFO, tag, msg)
#define LOG_DEBUG(tag, msg) __android_log_write(ANDROID_LOG_DEBUG, tag, msg)
#define LOG_WARN(tag, msg) __android_log_write(ANDROID_LOG_WARN, tag, msg)
#define LOG_ERROR(tag, msg) __android_log_write(ANDROID_LOG_ERROR, tag, msg)
/* 宏定义end */

#ifndef _Included_main_activity_UninstalledObserverActivity
#define _Included_main_activity_UninstalledObserverActivity
#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     UninstalledObserverActivity
 * Method:    init
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT int JNICALL Java_com_tangram_uninstalledobserver_UninstalledObserver_init(JNIEnv *env, jobject obj,jstring url, jstring appDataPath, jstring userSerial);

#ifdef __cplusplus
}
#endif
#endif
