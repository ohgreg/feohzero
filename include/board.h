#ifndef BOARD_H
#define BOARD_H

#include "types.h"

// function prototypes
void print_board(const Board *board);
void print_bitboard(const U64 board);
void apply_move(Board *board, Move *move);
void undo_move(Board *board, Move *move);
void update_occupied(Board *board);

#endif
