#include <stdlib.h>
#include <stdio.h>

#include "life.h"
#include "thread.h"
#include "logic.h"

pthread_mutex_t doWork;
pthread_cond_t waitWork;
pthread_cond_t waitComplete;

task_t tasks[THREAD_COUNT * 5];

static void *wait_work(void *l)
{
	static int i = 0;
	pthread_mutex_lock(&doWork);
	int id = i++;
	pthread_mutex_unlock(&doWork);
	life_t *life = (life_t*)l;
	task_t task;
	while(42)
	{
		pthread_mutex_lock(&doWork);
		while (!life->threads.task_count)
		{
			pthread_cond_wait(&waitWork, &doWork);
		}
		if (life->threads.done)
			break;
		printf("%d: work\n", id);
		task = tasks[0];
		for(int i = 0; i < life->threads.task_count - 1; i++)
			tasks[i] = tasks[i + 1];
		life->threads.task_count--;
		pthread_mutex_unlock(&doWork);
		calc_map(life, task.start, task.count);
		pthread_mutex_lock(&doWork);
		life->threads.completed++;
		pthread_cond_signal(&waitComplete);
		pthread_mutex_unlock(&doWork);
	}
	return NULL;
}

void add_task(life_t *life, task_t task)
{
	pthread_mutex_lock(&doWork);
	tasks[life->threads.task_count] = task;
	life->threads.task_count++;
	pthread_mutex_unlock(&doWork);
	pthread_cond_signal(&waitWork);
}

void init_threads(life_t *life)
{
	pthread_mutex_init(&doWork, NULL);
	pthread_cond_init(&waitWork, NULL);
	pthread_cond_init(&waitComplete, NULL);
	pthread_mutex_lock(&doWork);
	life->threads.threads = calloc(THREAD_COUNT, sizeof(pthread_t));
	for (int i = 0; i < THREAD_COUNT; i++)
		pthread_create(&life->threads.threads[i], NULL, wait_work, life);
	pthread_mutex_unlock(&doWork);
}

void join_threads(life_t *life)
{
	for (int i = 0; i < THREAD_COUNT; i++)
		pthread_cond_signal(&waitWork);
	for (int i = 0; i < THREAD_COUNT; i++)
	{
		pthread_join(life->threads.threads[i], NULL);
	}
	free(life->threads.threads);
	pthread_mutex_destroy(&doWork);
	pthread_cond_destroy(&waitWork);
	pthread_cond_destroy(&waitComplete);
}
