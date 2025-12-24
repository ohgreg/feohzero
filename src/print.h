#ifndef FEOHZERO_PRINT_H
#define FEOHZERO_PRINT_H

#include "types.h"

/* prints a board struct */
void print_board(const Board *board);

/* prints a move struct */
void print_move(Move *move);

/* prints a move list struct */
void print_move_list(MoveList *list);

/* prints a bitboard (debug) */
void print_bitboard_debug(const U64 board);

/* prints a move struct (debug) */
void print_move_debug(Move *move);

/* prints a move list struct (debug) */
void print_move_list_debug(MoveList *list);

/* prints a PeSTO table (debug) */
void print_pesto_tables_debug(int table[6][64]);

#endif
