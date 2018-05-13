#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "thread-pool.h"

#define POOL_USED 0
#define POOL_DESTROY 1

static ThreadPool *pool = NULL;

static void ThreadProcessor(struct _ThreadWorker *worker)
{
	printf("worker thread id:%ld, client ip:%s, port:%u, data:%s\n", pthread_self(), 
		worker->client_ip, worker->client_port, worker->user_input);
}

void ThreadPoolInit(unsigned int thread_num)
{
	pool = (ThreadPool*)malloc(sizeof(ThreadPool));
	if(NULL == pool){
		printf("pool malloc failed\n");
		exit(-1);
	}

	int err = pthread_attr_init(&pool->attr);
	if(err != 0)
		exit(-1);

	err = pthread_attr_setdetachstate(&pool->attr, PTHREAD_CREATE_DETACHED);
	if(err != 0)
		exit(-1);

	pthread_mutex_init(&(pool->queue_lock), NULL);
	pthread_cond_init(&(pool->queue_ready), NULL);

	pool->queue_head = NULL;

	pool->max_thread_num = thread_num;

	pool->cur_queue_size = 0;

	pool->shutdown = POOL_USED;

	pool->thread_id = (pthread_t *)malloc(pool->max_thread_num * sizeof(pthread_t));
	int i = 0;
	for(; i < pool->max_thread_num; i++){
		int ret = pthread_create(&pool->thread_id[i], NULL, &ThreadRoutine, NULL);
		if(ret != 0){
			printf("thread %d create failed, return value:%d\n", i, ret);
		}
	}
}

void ThreadPoolDestroy()
{
	if(NULL == pool || POOL_DESTROY == pool->shutdown)
		return;

	pool->shutdown = POOL_DESTROY;
	pthread_cond_broadcast(&pool->queue_ready);

#if 0
	int i = 0;
	for(; i < pool->max_thread_num; i++){
		pthread_join(pool->thread_id[i], NULL);
	}
#endif

	free(pool->thread_id);

	pthread_mutex_lock(&pool->queue_lock);

	ThreadWorker *worker = NULL;
	while(NULL != pool->queue_head){
		worker = pool->queue_head;
		pool->queue_head = pool->queue_head->next;
		free(worker);
		worker = NULL;
	}
	pool->cur_queue_size = 0;
	pthread_mutex_unlock(&pool->queue_lock);

	pthread_mutex_destroy(&pool->queue_lock);
	pthread_cond_destroy(&pool->queue_ready);

	pthread_attr_destroy(&pool->attr);

	free(pool);
	pool = NULL;
}

void PoolAddWorker(WorkerData* data, const char *client_ip, unsigned int client_port, pthread_t client_tid)
{
	ThreadWorker *worker = (ThreadWorker *)calloc(1, sizeof(ThreadWorker));
	if(NULL == worker){
		return;
	}

	memcpy(worker->client_ip, client_ip, sizeof(worker->client_ip)-1);
	worker->client_port = client_port;
	worker->client_tid = client_tid;
	memcpy(worker->user_input, data->buffer, sizeof(worker->user_input)-1);
	worker->proc = ThreadProcessor;
	worker->next = NULL;

	pthread_mutex_lock(&pool->queue_lock);
	ThreadWorker *worker_member = pool->queue_head;
	if(NULL != worker_member){
		while(NULL != worker_member->next){
			worker_member = worker_member->next;
		}
		worker_member->next = worker;
	}
	else{
		pool->queue_head = worker;
	}

	pool->cur_queue_size++;
	pthread_mutex_unlock(&pool->queue_lock);

	pthread_cond_signal(&(pool->queue_ready));
}

void* ThreadRoutine(void *arg)
{
	while(1){
		pthread_mutex_lock(&pool->queue_lock);

		while(pool->cur_queue_size == 0 && pool->shutdown != POOL_DESTROY){
			pthread_cond_wait(&pool->queue_ready, &pool->queue_lock);
		}

		if(pool->shutdown == POOL_DESTROY){
			pthread_mutex_unlock(&pool->queue_lock);
			pthread_exit(NULL);
		}

		pool->cur_queue_size--;
		ThreadWorker *worker = pool->queue_head;
		pool->queue_head = pool->queue_head->next;
		pthread_mutex_unlock(&pool->queue_lock);

		worker->proc(worker);

		free(worker);
		worker = NULL;
	}

	pthread_exit(NULL);
}

