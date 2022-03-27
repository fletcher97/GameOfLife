#if !defined(THREAD_H)
#define THREAD_H

#include <pthread.h>

#define THREAD_COUNT 3

typedef struct task_s
{
	int start;
	int count;
}				task_t;

void add_task(life_t *life, task_t task);
void init_threads(life_t *life);
void join_threads(life_t *life);


#endif // THREAD_H
