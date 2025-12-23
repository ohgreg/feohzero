#ifndef FEOHZERO_ZOBRIST_H
#define FEOHZERO_ZOBRIST_H

#include "types.h"

/* initializes zobrist hashing keys */
void init_zobrist(void);

/* updates the board key using XOR operations */
void fast_board_key(Board *board, const Move *move);

#endif
