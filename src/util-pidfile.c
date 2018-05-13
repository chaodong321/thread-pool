#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include "util-pidfile.h"

int create_pidfile(const char *pid_file)
{
	char pid_val[16];
	size_t pid_len = snprintf(pid_val, sizeof(pid_val), "%d", getpid());
	if(pid_len < 0){
		printf("%s: get pid error\n", __func__);
		goto ERROR;
	}

	int pid_fd = open(pid_file, O_CREAT | O_TRUNC | O_NOFOLLOW | O_WRONLY, 0644);
	if(pid_fd < 0){
		printf("%s: error to open pid file:%s, error no:%s\n", __func__, pid_file, strerror(errno));
		goto ERROR;
	}

	ssize_t r = write(pid_fd, pid_val, pid_len);
	if(r == -1){
		printf("%s: unable to pid file\n", __func__);
		goto ERROR;
	}
	else if((size_t)r != pid_len){
		printf("%s: unable to pid file, wrote:%ld\n", __func__, r);
		goto ERROR;
	}

	return 0;

ERROR:
	return -1;
}

void remove_pidfile(const char *pid_file)
{
	if(pid_file != NULL){
		unlink(pid_file);
	}
}

/*
 *功能：判断文件内容的pid是否正在运行
 *返回值：
 *0：不在运行
 *-1：正在运行
 */
int is_pid_running(const char *pid_file)
{
	if(access(pid_file, F_OK) == 0){
		FILE *fd = fopen(pid_file, "r");
		if(fd == NULL){
			printf("can't open pidfile\n");
			return -1;
		}

		int pid;
		if(fscanf(fd, "%d", &pid) == 1 && kill(pid, 0) == 0){
			fclose(fd);
			printf("program already running\n");
			return -1;
		}

		if(fd != NULL)
			fclose(fd);
	}
	return 0;
}

