#ifndef FEOHZERO_PRINT_H
#define FEOHZERO_PRINT_H

#include "types.h"

/* prints a board struct */
void print_board(const Board *board);

/* prints a bitboard */
void print_bitboard(const U64 board);

/* prints a move struct */
void print_move(Move *move);

/* prints a move list struct */
void print_move_list(MoveList *list);

/* prints a PeSTO table */
void print_pesto_tables(int table[6][64]);

#endif
