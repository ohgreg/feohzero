#ifndef FEOHZERO_EVAL_H
#define FEOHZERO_EVAL_H

#include "types.h"

/* initializes the evaluation tables */
void init_eval(void);

/* evaluates a board position */
int eval(const Board *board);

#endif
