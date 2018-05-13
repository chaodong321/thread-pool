#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "tcp-server.h"
#include "thread-pool.h"
#include "worker-data.h"

typedef struct _ConnectData{
	int accept_fd;
	struct sockaddr_in client_addr;
	struct _ConnectData *next;
}ConnectData;

static ConnectData *s_conn_data = NULL;

#define SERVER_PORT 8888
#define BACKLOG 5

static int s_socket_fd = -1;

/*
 *功能：tcp处理线程接收数据请求，并添加到线程池处理队列中
 */
static void* tcp_proc(void *arg)
{
	ConnectData *conn_data = (ConnectData *)arg;
	ssize_t size = 0;
	WorkerData data;

	pthread_t tid = pthread_self();

	while(1){
		memset(&data, 0, sizeof(WorkerData));
		size = read(conn_data->accept_fd, &data, sizeof(WorkerData));

		printf("thread id:%ld -- recv data size:%lu\n", tid, size);
		
		if(size <= 0)
			goto THREAD_EXIT;

		//添加到线程池队列
		PoolAddWorker(&data, inet_ntoa(conn_data->client_addr.sin_addr), conn_data->client_addr.sin_port, tid);

		memset(&data, 0, sizeof(WorkerData));
		sprintf(data.buffer, "add to woker to pool\n");
		write(conn_data->accept_fd, &data, sizeof(WorkerData));
	}

THREAD_EXIT:
	printf("thread:%ld, exit\n", tid);
	close(conn_data->accept_fd);
	conn_data->accept_fd = -1;
	pthread_exit((void*)0);
	return NULL;
}

/*
 *功能：tcp初始化
 *返回值：
 *  非0：初始化成功,返回服务端socket文件描述符
 *  -1:初始化失败
 */
int tcp_init()
{
	s_socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s_socket_fd < 0){
		printf("%s: socket error\n", __func__);
		return -1;
	}

	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(SERVER_PORT);
	int bind_err = bind(s_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(bind_err < 0){
		printf("%s: bind error\n", __func__);
		return -1;
	}

	int listen_err = listen(s_socket_fd, BACKLOG);
	if(listen_err < 0){
		printf("%s: listen error\n", __func__);
		return -1;
	}
	printf("tcp init success\n");
	return s_socket_fd;
}

void tcp_exit()
{
	while(NULL != s_conn_data){
		ConnectData *connect_data = s_conn_data;
		s_conn_data = s_conn_data->next;
		if(connect_data->accept_fd != -1){
			close(connect_data->accept_fd);
		}
		free(connect_data);
		connect_data = NULL;
	}
	
	if(s_socket_fd != -1){
		close(s_socket_fd);
	}

	printf("tcp exit\n");
}

int tcp_accept()
{
	if(s_socket_fd == -1){
		return -1;
	}
	
	while(1){

		ConnectData *conn_data = (ConnectData *)malloc(sizeof(ConnectData));
		if(NULL == s_conn_data){
			s_conn_data = conn_data;
		}
		else{
			conn_data->next = s_conn_data;
			s_conn_data = conn_data;
		}

		socklen_t addrlen = sizeof(struct sockaddr);
		conn_data->accept_fd = accept(s_socket_fd, (struct sockaddr*)&conn_data->client_addr, &addrlen);

		if(conn_data->accept_fd < 0){
			printf("%s: accept error\n", __func__);
			continue;
		}

		printf("accept client, addr:%s, port:%d\n", inet_ntoa(conn_data->client_addr.sin_addr), conn_data->client_addr.sin_port);

		pthread_t tid;
		int create_err = pthread_create(&tid, NULL, tcp_proc, (void*)conn_data);
		if(create_err < 0){
			printf("%s: thread id：%ld create error:%d\n", __func__, tid, create_err);
			continue;
		}

		int detach_err = pthread_detach(tid);
		if(detach_err < 0){
			printf("%s: thread id：%ld detach error:%d\n", __func__, tid, detach_err);
			continue;
		}
	}
	return 0;
}

