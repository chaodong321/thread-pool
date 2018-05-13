#ifndef _THREAD_POOL_
#define _THREAD_POOL_

#ifdef __cplusplus
extern "c"{
#endif

#include <stdarg.h>
#include <syslog.h>

#define MAX_LOG_LEN 1024

/* 添加了打印语句所在的文件、行号、函数信息 */  
#define C_LOG(level, fmt, argc...) log_message(level, "[file:%s,line:%d,func:%s]" fmt, __FILE__,  __LINE__, __FUNCTION__, ##argc)

#ifdef __cplusplus
}
#endif

#endif
