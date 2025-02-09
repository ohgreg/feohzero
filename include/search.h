#include "eval.h"

int depth_limited_search(Board *board, int depth, int is_root, Move *best_move, int alpha, int beta);
Move iterative_deepening_search(Board *board, int max_depth);
