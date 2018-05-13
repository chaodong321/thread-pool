#include <stdio.h>
#include <time.h>

#include "util-log.h"

static const char *log_level_str[] = {
	[LOG_EMERG] = "EMERG",
	[LOG_ALERT] = "ALERT",
	[LOG_CRIT] = "CRIT",
	[LOG_ERR] = "ERR",
	[LOG_WARNING] = "WARNING",
	[LOG_NOTICE] = "NOTICE",
	[LOG_INFO] = "INFO",
	[LOG_DEBUG] = "DEBUG",
};

static void get_current_time(char *buf, int len)
{
	time_t timep;
	struct tm *p;
	
	time(&timep);
	p = gmtime(&timep);
	snprintf(buf, len - 1, "%d/%d/%d %d-%d-%d", (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
}

static void vlog_message(const int level, const char* fmt, va_list args)
{
	char buf[MAX_LOG_LEN+1] = {0};
	char time[128] = {0};

	get_current_time(time, sizeof(time) - 1);
	vsnprintf(buf, sizeof(buf), fmt, args);
	/* 可以在这里组装需要的信息 */
	fprintf(stderr, "%s[%s]%s\n", time, log_level_str[level], buf);  /* 时间，级别，位置，日志内容 */
	/* cat /var/log/messages 进行日志查看 */
	syslog(level, "%s", buf);
}

void log_message(const int level, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vlog_message(level, fmt, args);
	va_end(args);
}

