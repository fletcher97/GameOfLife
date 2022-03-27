#if !defined(LOGGIC_H)
#define LOGGIC_H

#include "life.h"

void next_gen(life_t *life);
void next_gen_task(life_t *life);
void calc_map(life_t *life, int start, int count);

#endif // LOGGIC_H
