#include "eval.h"

int depth_limited_search(Board *board, int depth, int is_root, Move *best_move, int alpha, int beta, MoveList startList);
Move iterative_deepening_search(Board *board, int max_depth, MoveList startList);
int sort_moves(const void *a, const void *b);