#if !defined(GUI_H)
#define GUI_H

#include "life.h"

#define CELL_SIZE 5
#define BORDER_SIZE 1

#define SCALE 5

#define COLOR_DEAD 0x424242
#define COLOR_LIFE 0x2a8e2a
#define COLOR_BORDER 0x2a2a2a

void render(life_t *life);

#endif // GUI_H
