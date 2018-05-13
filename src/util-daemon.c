#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#include "util-daemon.h"

#define DAEMON_DIR "/var/log/tcp-proxy/"

static volatile sig_atomic_t sigflag = 0;

static void signal_handler_sigusr1 (int signo)
{
    sigflag = 1;
}


static void tell_waiting_parent(pid_t pid)
{
	kill(pid, SIGUSR1);
}


/*
 *功能：设置程序的终端输入、输出、错误到/dev/null中
 */
static void setup_logging()
{
	int fd = open("/dev/null", O_RDWR);
    if (fd < 0)
        return;
    (void)dup2(fd, 0);
    (void)dup2(fd, 1);
    (void)dup2(fd, 2);
    close(fd);
}


/*
 *功能：等待子进程变为守护进程后退出，若子进程结束则waitpid回收子进程资源，防止僵尸进程
 */
static void wait_for_child (pid_t pid)
{
    int status;
    printf("Daemon: Parent waiting for child to be ready...\n");
    /* Wait until child signals is ready */
    while (sigflag == 0) {
        if (waitpid(pid, &status, WNOHANG)) {
            /* Check if the child is still there, otherwise the parent should exit */
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                printf("Child died unexpectedly\n");
                exit(0);
            }
        }
        /* sigsuspend(); */
        sleep(1);
    }
}

/*
 *功能：让程序变为守护进程
 */
void daemonize()
{
	signal(SIGUSR1, signal_handler_sigusr1);
	pid_t pid = fork();
	if(pid < 0){
		printf("%s: fork error\n", __func__);
		exit(0);
	}
	else if(pid == 0){
		umask(022);

		pid_t sid = setsid();

		if(sid < 0){
			printf("%s: set sid error\n", __func__);
			exit(0);
		}

		if(access(DAEMON_DIR, F_OK) < 0){
			if(mkdir(DAEMON_DIR, 0755) < 0){
				printf("%s: can't create daemon dir:%s\n", __func__, DAEMON_DIR);
				exit(0);
			}
		}

		if(chdir(DAEMON_DIR) < 0){
			printf("%s: change daemon dir failed\n", __func__);
			exit(0);
		}

		setup_logging();

		tell_waiting_parent(getppid());

		printf("daemon is running\n");
		return;
	}

	printf("%s: Parent is waiting for child to be ready\n", __func__);
	wait_for_child(pid);

	printf("Child is ready, parent exiting\n");
	exit(0);
}

