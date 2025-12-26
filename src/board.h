#ifndef FEOHZERO_BOARD_H
#define FEOHZERO_BOARD_H

#include "types.h"

/* loads a chess position in FEN format to a board.
 * returns 1 on success, 0 on failure */
int load_fen(Board *board, const char *fen);

/* applies a move, changing the chess position */
void apply_move(Board *board, Move *move);

/* reverts a move, apply_move() mirror */
void undo_move(Board *board, Move *move);

#endif
