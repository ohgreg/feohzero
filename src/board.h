#ifndef BOARD_H
#define BOARD_H

#include "types.h"

// function prototypes
int loadFEN(Board *board, char *fen);
void apply_move(Board *board, Move *move);
void undo_move(Board *board, Move *move);

#endif
