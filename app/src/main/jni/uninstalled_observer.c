/* 头文件begin */
#include "uninstalled_observer.h"
#include <time.h>
/* 头文件end */

#ifdef __cplusplus
extern "C"
{
#endif

static char TAG[] = "UninstalledObserver.init";
static jboolean isCopy = JNI_TRUE;

/*
 *获取当前时间
 */
int get_time(char *buffer)
{
	time_t now;
	struct tm *time_now;
	time(&now);
	time_now = localtime(&now);
	return sprintf(buffer,"%d-%d-%d %d:%d:%d",(1900+time_now->tm_year),(1 + time_now->tm_mon), time_now->tm_mday, time_now->tm_hour, time_now->tm_min, time_now->tm_sec);
}
/*
 * Class:     main_activity_UninstalledObserverActivity
 * Method:    init
 * Signature: ()V
 * return: 子进程pid
 */
JNIEXPORT int JNICALL Java_com_tangram_uninstalledobserver_UninstalledObserver_init(JNIEnv *env, jobject obj,jstring url, jstring appDataPath, jstring userSerial)
{
	char APP_FILES_DIR[512];
	char APP_OBSERVED_FILE[512];
	char APP_LOCK_FILE[512];
	char UNINSTALL_URL[512];
    char APP_FILES_LIB[512];
    char NEW_TIME[32];
    jstring tag = (*env)->NewStringUTF(env, TAG);
    const char* pUrl = (*env)->GetStringUTFChars(env, url, &isCopy);
	const char* pDataPath = (*env)->GetStringUTFChars(env, appDataPath, &isCopy);

	strcpy(APP_FILES_DIR, pDataPath);
	strcat(APP_FILES_DIR,"/files");

    strcpy(APP_FILES_LIB, pDataPath);
    strcat(APP_FILES_LIB,"/lib");

	strcpy(APP_OBSERVED_FILE, APP_FILES_DIR);
	strcat(APP_OBSERVED_FILE,"/observedFile");
	
	strcpy(APP_LOCK_FILE, APP_FILES_DIR);
	strcat(APP_LOCK_FILE,"/lockFile");
	
	LOG_DEBUG((*env)->GetStringUTFChars(env, tag, &isCopy)
		, (*env)->GetStringUTFChars(env, (*env)->NewStringUTF(env, APP_FILES_DIR), &isCopy));
	LOG_DEBUG((*env)->GetStringUTFChars(env, tag, &isCopy)
		, (*env)->GetStringUTFChars(env, (*env)->NewStringUTF(env, APP_OBSERVED_FILE), &isCopy));
	LOG_DEBUG((*env)->GetStringUTFChars(env, tag, &isCopy)
		, (*env)->GetStringUTFChars(env, (*env)->NewStringUTF(env, APP_LOCK_FILE), &isCopy));
	LOG_DEBUG((*env)->GetStringUTFChars(env, tag, &isCopy)
		, (*env)->GetStringUTFChars(env, url, &isCopy));
    LOG_DEBUG((*env)->GetStringUTFChars(env, tag, &isCopy)
            , (*env)->GetStringUTFChars(env, (*env)->NewStringUTF(env, "init observer"), &isCopy));

    // fork子进程，以执行轮询任务
    pid_t pid = fork();
    if (pid < 0)
    {
        LOG_ERROR((*env)->GetStringUTFChars(env, tag, &isCopy)
                , (*env)->GetStringUTFChars(env, (*env)->NewStringUTF(env, "fork failed !!!"), &isCopy));
        exit(1);
    }
    else if (pid == 0)
    {
        // 若监听文件所在文件夹不存在，创建
        FILE *p_filesDir = fopen(APP_FILES_DIR, "r");
        if (p_filesDir == NULL)
        {
            int filesDirRet = mkdir(APP_FILES_DIR, S_IRWXU | S_IRWXG | S_IXOTH);
            if (filesDirRet == -1)
            {
                LOG_ERROR((*env)->GetStringUTFChars(env, tag, &isCopy)
                        , (*env)->GetStringUTFChars(env, (*env)->NewStringUTF(env, "mkdir failed !!!"), &isCopy));
                LOG_ERROR((*env)->GetStringUTFChars(env, tag, &isCopy)
                                       , (*env)->GetStringUTFChars(env, (*env)->NewStringUTF(env, strerror(errno)), &isCopy));
                exit(1);
            }
        }

        // 若被监听文件不存在，创建文件
        FILE *p_observedFile = fopen(APP_OBSERVED_FILE, "r");
        if (p_observedFile == NULL)
        {
            p_observedFile = fopen(APP_OBSERVED_FILE, "w");
        }
        fclose(p_observedFile);

        LOG_DEBUG((*env)->GetStringUTFChars(env, tag, &isCopy)
                    , (*env)->GetStringUTFChars(env, (*env)->NewStringUTF(env, "observed by child process"), &isCopy));

        // 分配空间，以便读取event
        void *p_buf = malloc(sizeof(struct inotify_event));
        if (p_buf == NULL)
        {
            LOG_ERROR((*env)->GetStringUTFChars(env, tag, &isCopy)
                    , (*env)->GetStringUTFChars(env, (*env)->NewStringUTF(env, "malloc failed !!!"), &isCopy));
            exit(1);
        }
        // 分配空间，以便打印mask
        int maskStrLength = 7 + 10 + 1;// mask=0x占7字节，32位整形数最大为10位，转换为字符串占10字节，'\0'占1字节
        char *p_maskStr = malloc((size_t) maskStrLength);
        if (p_maskStr == NULL)
        {
            free(p_buf);
            LOG_ERROR((*env)->GetStringUTFChars(env, tag, &isCopy)
                    , (*env)->GetStringUTFChars(env, (*env)->NewStringUTF(env, "malloc failed !!!"), &isCopy));
            exit(1);
        }

        // 开始监听
        LOG_DEBUG((*env)->GetStringUTFChars(env, tag, &isCopy)
                , (*env)->GetStringUTFChars(env, (*env)->NewStringUTF(env, "start observe"), &isCopy));

        // 初始化
        int fileDescriptor = inotify_init();
        if (fileDescriptor < 0)
        {
            free(p_buf);
            free(p_maskStr);
			
            LOG_ERROR((*env)->GetStringUTFChars(env, tag, &isCopy)
                    , (*env)->GetStringUTFChars(env, (*env)->NewStringUTF(env, "inotify_init failed !!!"), &isCopy));

            exit(1);
        }

        // 添加被监听文件到监听列表
        int watchDescriptor = inotify_add_watch(fileDescriptor, APP_OBSERVED_FILE, IN_ALL_EVENTS);
        if (watchDescriptor < 0)
        {
            free(p_buf);
            free(p_maskStr);

            LOG_ERROR((*env)->GetStringUTFChars(env, tag, &isCopy)
                    , (*env)->GetStringUTFChars(env, (*env)->NewStringUTF(env, "inotify_add_watch failed !!!"), &isCopy));

            exit(1);
        }

        while(1)
        {
            // read会阻塞进程
            read(fileDescriptor, p_buf, sizeof(struct inotify_event));
            // 打印mask
            snprintf(p_maskStr, (size_t) maskStrLength, "mask=0x%x", ((struct inotify_event *) p_buf)->mask);
            LOG_DEBUG((*env)->GetStringUTFChars(env, tag, &isCopy)
                    , (*env)->GetStringUTFChars(env, (*env)->NewStringUTF(env, p_maskStr), &isCopy));
            // 若文件被删除，可能是已卸载，还需进一步判断app文件夹是否存在
            if (IN_DELETE_SELF == ((struct inotify_event *) p_buf)->mask)
            {
                FILE *p_appDir = fopen(APP_FILES_LIB, "r");
                // 确认已卸载
                if (p_appDir == NULL)
                {
                    inotify_rm_watch(fileDescriptor, (uint32_t) watchDescriptor);
                     LOG_ERROR((*env)->GetStringUTFChars(env, tag, &isCopy)
                                                    , (*env)->GetStringUTFChars(env, (*env)->NewStringUTF(env, "is uninstalled"), &isCopy));
                    break;
                }
                // 未卸载，可能用户执行了"清除数据"
                else
                {
                    fclose(p_appDir);
                    // 重新创建被监听文件，并重新监听
                    FILE *pObservedFile = fopen(APP_OBSERVED_FILE, "w");
                    fclose(pObservedFile);

                    int descriptor = inotify_add_watch(fileDescriptor, APP_OBSERVED_FILE, IN_ALL_EVENTS);
                     LOG_ERROR((*env)->GetStringUTFChars(env, tag, &isCopy)
                                                                        , (*env)->GetStringUTFChars(env, (*env)->NewStringUTF(env, "don't uninstall"), &isCopy));
                    if (descriptor < 0)
                    {
                        free(p_buf);
                        free(p_maskStr);
                        LOG_ERROR((*env)->GetStringUTFChars(env, tag, &isCopy)
                                , (*env)->GetStringUTFChars(env, (*env)->NewStringUTF(env, "inotify_add_watch failed !!!"), &isCopy));
                        exit(1);
                    }
                }
            }
        }
        // 释放资源
        free(p_buf);
        free(p_maskStr);
        // 停止监听
        LOG_DEBUG((*env)->GetStringUTFChars(env, tag, &isCopy)
                , (*env)->GetStringUTFChars(env, (*env)->NewStringUTF(env, "stop observe"), &isCopy));

    	strcpy(UNINSTALL_URL, pUrl);
    	strcat(UNINSTALL_URL, "&deldate=");
    	get_time(NEW_TIME);
    	strcat(UNINSTALL_URL, NEW_TIME);
        LOG_DEBUG((*env)->GetStringUTFChars(env, tag, &isCopy)
        , (*env)->GetStringUTFChars(env, (*env)->NewStringUTF(env, UNINSTALL_URL), &isCopy));
        if (userSerial == NULL)
        {
            // 执行命令am start -a android.intent.action.VIEW -d $(url)
            execlp("am", "am", "start", "-a", "android.intent.action.VIEW", "-d", UNINSTALL_URL, (char *)NULL);
        }
        else
        {
            // 执行命令am start --user userSerial -a android.intent.action.VIEW -d $(url)
            execlp("am", "am", "start", "--user", (*env)->GetStringUTFChars(env, userSerial, &isCopy), "-a", "android.intent.action.VIEW", "-d", UNINSTALL_URL, (char *)NULL);
        }
        // 执行命令失败log
        LOG_ERROR((*env)->GetStringUTFChars(env, tag, &isCopy)
                , (*env)->GetStringUTFChars(env, (*env)->NewStringUTF(env, "exec AM command failed !!!"), &isCopy));
    }
    else
    {
        // 父进程直接退出，使子进程被init进程领养，以避免子进程僵死，同时返回子进程pid
        return pid;
    }
    return 0;
}

#ifdef __cplusplus
}
#endif
