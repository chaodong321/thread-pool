#ifndef _THREAD_POOL_
#define _THREAD_POOL_

#ifdef __cplusplus
extern "C"{
#endif

#include <pthread.h>

#include "worker-data.h"

typedef struct _ThreadWorker
{
	char client_ip[16];
	unsigned int client_port;
	pthread_t client_tid;
	char user_input[256];
	void (*proc)(struct _ThreadWorker *worker);
	struct _ThreadWorker *next;
}ThreadWorker;

typedef struct _ThreadPool
{
	pthread_attr_t attr;
	
	pthread_mutex_t queue_lock;
	pthread_cond_t queue_ready;

	ThreadWorker *queue_head;

	int shutdown;

	pthread_t *thread_id;

	unsigned int max_thread_num;

	unsigned int cur_queue_size;
}ThreadPool;

void ThreadPoolInit(unsigned int thread_num);
void ThreadPoolDestroy();
void PoolAddWorker(WorkerData* data, const char *client_ip, unsigned int client_port, pthread_t client_tid);
void* ThreadRoutine(void *arg);

#ifdef __cplusplus
}
#endif

#endif