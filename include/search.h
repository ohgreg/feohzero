#ifndef SEARCH_H
#define SEARCH_H

#include "types.h"

// function prototypes
int equal_moves(const Move *m1, const Move *m2);
int compare_moves(const void *a, const void *b);
int dls_search(Board *board, int depth, int is_root, Move *best_move, int alpha, int beta, MoveList startList, Move previousBest);
Move ids_search(Board *board, int max_depth, MoveList startList, int timeout);

#endif
