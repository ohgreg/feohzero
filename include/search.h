#ifndef SEARCH_H
#define SEARCH_H

#include "types.h"

// function prototypes
int move_equals(const Move *m1, const Move *m2);
int sort_moves(const void *a, const void *b);
Move iterative_deepening_search(Board *board, int max_depth, MoveList startList);
int depth_limited_search(Board *board, int depth, int is_root, Move *best_move, int alpha, int beta,
                         MoveList startList, Move previousBest);

#endif
