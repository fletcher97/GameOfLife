#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "life.h"
#include "thread.h"

extern pthread_mutex_t doWork;
extern pthread_cond_t waitWork;
extern pthread_cond_t waitComplete;

static int get_neighbor_count(life_t *life, int x, int y)
{
	int c = 0;
	for (int i = -1; i < 2; i++)
		for (int j = -1; j < 2; j++)
		{
			if (!i && !j)
				continue;
			int cx = (x+j) % life->sym.width;
			if (cx < 0)
				cx += life->sym.width;
			int cy = (y+i) % life->sym.height;
			if (cy < 0)
				cy += life->sym.height;
			if (((life->sym.map[(cy * life->sym.width + cx) / 8] & (1 << (7 - (cy * life->sym.width + cx) % 8)))))
				c++;
		}
	return c;
}

void calc_map(life_t *life, int start, int count)
{
	for (int i = start; i < (start + count) && i < (life->sym.height * life->sym.width); i++)
	{
		int c = get_neighbor_count(life, i%life->sym.width, i/life->sym.width);
		if (c == 3)
			life->sym.nmap[i / 8] |= (1 << (7 - i % 8));
		else if (c == 2)
			life->sym.nmap[i / 8] |= ((life->sym.map[i / 8] & (1 << (7 - i % 8))));
	}
}

void next_gen_task(life_t *life)
{
	printf("TASK GEN\n");
	int bits_req = life->sym.height * life->sym.width;
	int bytes_req = ceil(bits_req / 8.0);
	life->sym.nmap = calloc(1, bytes_req);
	int step = ceil(bits_req / THREAD_COUNT);
	for(int i = 0; i < THREAD_COUNT; i++)
	{
		printf("  ADD GEN\n");
		add_task(life, (task_t){.count = step, .start = step * i});
	}
	pthread_mutex_lock(&doWork);
	while (life->threads.completed != THREAD_COUNT)
	{
		pthread_cond_wait(&waitComplete, &doWork);
	}
	life->threads.completed = 0;
	pthread_mutex_unlock(&doWork);
	free(life->sym.map);
	life->sym.map = life->sym.nmap;
	life->sym.nmap = NULL;
	printf("TASK COMPLETE\n");
}

void next_gen(life_t *life)
{
	int bits_req = life->sym.height * life->sym.width;
	int bytes_req = ceil(bits_req / 8.0);
	life->sym.nmap = calloc(1, bytes_req);
	calc_map(life, 0, life->sym.width * life->sym.height);
	free(life->sym.map);
	life->sym.map = life->sym.nmap;
	life->sym.nmap = NULL;
}
