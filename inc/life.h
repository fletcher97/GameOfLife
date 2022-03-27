#if !defined(LIFE_H)
#define LIFE_H

#include <time.h>
#include <pthread.h>

#define TICK_RATE 30

typedef struct img_s
{
	void	*img;
	char	*addr;
	int		bpp;
	int		line;
	int		endian;
}				img_t;

typedef struct screen_s
{
	void	*win;
	int		width;
	int		height;
	img_t	*img;
}				screen_t;

typedef struct sym_s
{
	int width;
	int height;
	int gen_count;
	char *map;
	char *nmap;
	clock_t last_tick;
}				sym_t;

typedef struct pt_s
{
	pthread_t *threads;
	int gen;
	int task_count;
	int completed;
	int done;
}				pt_t;


typedef struct life_s
{
	void *mlx;
	screen_t screen;
	sym_t sym;
	pt_t threads;
}				life_t;

#endif // LIFE_H
