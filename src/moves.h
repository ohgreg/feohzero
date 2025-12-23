#ifndef FEOHZERO_MOVES_H
#define FEOHZERO_MOVES_H

#include "types.h"

/* initializes LUT attacks for each piece */
void init_moves(void);

/* returns 1 if the current side's king is under attack, 0 otherwise */
int is_king_in_check(Board *board);

/* generates all possible and saves them in a list, given a board position */
void generate_moves(Board *board, MoveList *list);

/* translates a move from string containing the move in Algebraic notation to
 * the move struct */
Move translate_move(Board *board, const char *move_str);

/* creares a move list from a string of moves in Algebraic notation */
MoveList initial_list(Board *board, const char *move_str);

#endif
