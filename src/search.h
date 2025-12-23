#ifndef FEOHZERO_SEARCH_H
#define FEOHZERO_SEARCH_H

#include "types.h"

/* returns 1 if two moves are equal, 0 otherwise */
int equal_moves(const Move *a, const Move *b);

/* depth-limited search (dls) with alpha-beta pruning */
int dls_search(Board *board, int depth, int is_root, Move *best_move, int alpha,
               int beta, MoveList start_list, Move previous_best, int timeout);

/* iterative deepening search (ids) for a given board */
Move ids_search(Board *board, int max_depth, MoveList start_list, int timeout);

#endif
