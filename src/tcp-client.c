#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

#include "worker-data.h"

#define PORT 8888

int s_socket_fd = -1;

void sig_init(int sig)
{
	if(s_socket_fd != -1)
		close(s_socket_fd);
	exit(0);
}

void process_conn_client(int socket_fd)
{
	ssize_t size = 0;
	WorkerData data;

	while(1){
		memset(data.buffer, 0, sizeof(data.buffer));
		size = read(0, data.buffer, sizeof(data.buffer));
		if(size > 0){
			write(socket_fd, (void*)&data, sizeof(WorkerData));
			memset(data.buffer, 0, sizeof(data.buffer));
			size = read(socket_fd, (void*)&data, sizeof(WorkerData));
			write(1, data.buffer, sizeof(WorkerData));
		}
	}
}

int main(int argc, char **argv)
{
	signal(SIGINT, &sig_init);

	s_socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s_socket_fd < 0){
		printf("socket error\n");
		return -1;
	}

	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);

	inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

	int connect_err = connect(s_socket_fd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));

	if(connect_err == 0){
		printf("connect success\n");
		process_conn_client(s_socket_fd);
	}
	else{
		printf("connect error\n");
	}

	return 0;
}


