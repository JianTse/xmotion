#ifndef  __ANDROIDLOG_H__
#define  __ANDROIDLOG_H__

// Loging
#ifndef ANDROID
// 程序输出
#define  vPrintf(x,...) \
do \
{ \
	printf(x,##__VA_ARGS__); \
	printf("\n");\
} while (0)


#define LOGV vPrintf
#define LOGD vPrintf
#define LOGI vPrintf
#define LOGW vPrintf
#define LOGE vPrintf
#define LOG LOGV
#else

#ifndef LOG_TAG 
#define LOG_TAG "Motion"
#endif

#include <android/log.h>
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG,__VA_ARGS__) 
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG ,LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO  ,LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN  ,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  ,LOG_TAG, __VA_ARGS__)
#define LOG LOGV
#endif

#endif /* __ANDROID_LOG_H__ */

