#include <stdio.h>

#include "mlx.h"

#include "life.h"
#include "gui.h"

static void	my_mlx_pixel_put(img_t *img, int x, int y, int color)
{
	char	*dst;

	if (color == 0xffc0ff)
		return ;
	dst = img->addr + (y * img->line + x * (img->bpp / 8));
	*(unsigned int *)dst = color;
}

static void	put_cell(life_t *life, int x, int y, int img_id)
{
	// printf("put cell %d: x-%d, y-%d\n", y * life->sym.width + x, x, y);
	for (int i = 0; i < ((CELL_SIZE + (BORDER_SIZE * 2)) * SCALE); i++)
		for (int j = 0; j < ((CELL_SIZE + (BORDER_SIZE * 2)) * SCALE); j++)
		{
			unsigned c;
			if (i < BORDER_SIZE * SCALE || i >= (BORDER_SIZE + CELL_SIZE) * SCALE || j < BORDER_SIZE * SCALE || j >= (BORDER_SIZE + CELL_SIZE) * SCALE)
				c = COLOR_BORDER;
			else if (life->sym.map[(y * life->sym.width + x) / 8] & (1 << (7 - (y * life->sym.width + x) % 8)))
				c = COLOR_LIFE;
			else
				c = COLOR_DEAD;
			my_mlx_pixel_put(&life->screen.img[img_id],
				x * ((CELL_SIZE + (BORDER_SIZE * 2)) * SCALE) + j,
				y * ((CELL_SIZE + (BORDER_SIZE * 2)) * SCALE) + i,
				c);
		}
}

void render(life_t *life)
{
	static int img_id = 1;

	img_id = !img_id;
	// static int cell = -1;
	// cell = (cell + 1) % 64;
	// printf("put cell %d: x-%d, y-%d, bit:%d\n", cell/8 * life->sym.width + cell%8, cell%8, cell/8, life->sym.map[(cell/8 * life->sym.width + cell%8) / 8] & (1 << (7 - (cell/8 * life->sym.width + cell%8) % 8)));
	// for (int i = 0; i < cell; i++)
	// 	put_cell(life, i%8, i/8, img_id);
	for (int i = 0; i < life->sym.height; i++)
		for (int j = 0; j < life->sym.width; j++)
			put_cell(life, j, i, img_id);
	mlx_put_image_to_window(life->mlx, life->screen.win, life->screen.img[img_id].img, 0, 0);
}
