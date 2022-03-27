#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "mlx.h"

#include "mlx_keys.h"
#include "life.h"
#include "gui.h"
#include "logic.h"
#include "thread.h"

static int get_map_size(char *path, life_t *life)
{
	FILE *f = fopen(path, "r");
	if (!f)
		return -1;
	for (char c = getc(f); c != EOF; c = getc(f))
	{
		if (c == '\n')
		{
			life->sym.height++;
		}
		if (!life->sym.height)
			life->sym.width++;
	}
	fclose(f);
	return life->sym.height && life->sym.width;
}

static int read_map(life_t *life, char *path)
{
	if (!get_map_size(path, life))
		return 0;
	FILE *f = fopen(path, "r");
	if (!f)
		return -1;
	int bits_req = life->sym.height * life->sym.width;
	int bytes_req = ceil(bits_req / 8.0);
	life->sym.map = calloc(1, bytes_req);
	int line_adj = 0;
	for (int i = 0; i - line_adj < bits_req; i++)
	{
		char c = getc(f);
		if (c == '1')
		{
			life->sym.map[(i - line_adj) / 8] |= (1 << (7 - (i - line_adj)%8));
		}
		else if (c == '\n')
			line_adj++;
		else if(c != '0')
		{
			fclose(f);
			return 0;
		}
	}
	fclose(f);
	return 1;
}

static int setup(life_t *life, char *path)
{
	life->mlx = mlx_init();
	if (!life->mlx)
		return 0;
	if (!read_map(life, path))
		return 0;
	life->screen.width = life->sym.width * ((CELL_SIZE + (BORDER_SIZE * 2)) * SCALE);
	life->screen.height = life->sym.height * ((CELL_SIZE + (BORDER_SIZE * 2)) * SCALE);
	life->screen.win = mlx_new_window(life->mlx, life->screen.width, life->screen.height, "Game of Life");
	life->screen.img = calloc(2, sizeof *life->screen.img);
	life->screen.img[0].img = mlx_new_image(life->mlx, life->screen.width, life->screen.height);
	life->screen.img[1].img = mlx_new_image(life->mlx, life->screen.width, life->screen.height);
	life->screen.img[0].addr = mlx_get_data_addr(life->screen.img[0].img, &life->screen.img[0].bpp, &life->screen.img[0].line, &life->screen.img[0].endian);
	life->screen.img[1].addr = mlx_get_data_addr(life->screen.img[1].img, &life->screen.img[1].bpp, &life->screen.img[1].line, &life->screen.img[1].endian);
	init_threads(life);
	return 1;
}

static void clean(life_t *life)
{
	if (!life)
		return;
	if (life->screen.img)
	{
		mlx_destroy_image(life->mlx, life->screen.img[0].img);
		mlx_destroy_image(life->mlx, life->screen.img[1].img);
		free(life->screen.img);
	}
	if (life->screen.win)
	{
		mlx_destroy_window(life->mlx, life->screen.win);
	}
	if (life->sym.map)
		free(life->sym.map);
	if (life->mlx)
		mlx_destroy_display(life->mlx);
	life->threads.done = 1;
	join_threads(life);
	free(life);
}

static int stop_hook(life_t *life)
{
	clean(life);
	exit(1);
	return 1;
}

static int update(life_t *life)
{
	clock_t		current = clock();

	current = clock();
	while ((double)(current - life->sym.last_tick) / CLOCKS_PER_SEC > (1.0 / TICK_RATE) * 1)
	{
		next_gen_task(life);
		render(life);
		life->sym.last_tick += (1.0 / TICK_RATE) * 1000000;
	}
	return 1;
}

static int on_key_press(int key, life_t *life)
{
	if (key == MLX_KEY_ESC)
		stop_hook(life);
	return 1;
}

int main(int argc, char **argv)
{
	life_t *life = calloc(1, sizeof *life);
	if (!life || argc != 2 || !setup(life, argv[1]))
	{
		clean(life);
		return 1;
	}
	mlx_loop_hook(life->mlx, update, life);
	mlx_hook(life->screen.win, 2, (1L << 0), on_key_press, life);
	mlx_hook(life->screen.win, 17, (1L << 17), stop_hook, life);
	render(life);
	mlx_loop(life->mlx);
	return 0;
}
