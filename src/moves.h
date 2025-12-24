#ifndef FEOHZERO_MOVES_H
#define FEOHZERO_MOVES_H

#include "types.h"

/* initializes LUT attacks for each piece */
void init_moves(void);

/* returns 1 if the current side's king is under attack, 0 otherwise */
int is_king_in_check(Board *board);

/* generates all possible and saves them in a list, given a board position */
void generate_moves(Board *board, MoveList *list);

/* generates initial move list from SAN notation strings, filtering for valid moves only */
void initial_list(Board *board, MoveList *list, const char *move_str);

#endif
