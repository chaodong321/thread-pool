#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "util-cmdline.h"
#include "util-pidfile.h"
#include "util-coredump.h"
#include "util-daemon.h"
#include "tcp-server.h"
#include "thread-pool.h"

static prog_cmd s_instance = {0};

void sig_init(int sig)
{
	//通知线程关闭

	
	tcp_exit();
	ThreadPoolDestroy();
	remove_pidfile(PID_FILE_PATH);
	exit(0);
}

int main(int argc, char **argv)
{
	if(parse_commandline(argc, argv, &s_instance) != 0)
		return 0;

	if(!set_coredump()){
		printf("set coredump size failed\n");
	}

	if(s_instance.daemon){
		daemonize();
	}

	if(is_pid_running(PID_FILE_PATH) != 0)
		return 0;
	if(create_pidfile(PID_FILE_PATH) != 0)
		return 0;
	
	signal(SIGINT, &sig_init);

	ThreadPoolInit(3);

	if(tcp_init() < 0){
		return 0;
	}

	if(tcp_accept() < 0){
		return 0;
	}

	tcp_exit();
	ThreadPoolDestroy();
	remove_pidfile(PID_FILE_PATH);
	return 0;
}

