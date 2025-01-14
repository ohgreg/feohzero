#ifndef UTILS_H
#define UTILS_H

#include "board.h"

void print_board(const Board *board);
void apply_move(Board *board, Move *move);
void update_occupied(Board *board);

#endif
