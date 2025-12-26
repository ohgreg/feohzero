#ifndef FEOHZERO_PRINT_H
#define FEOHZERO_PRINT_H

#include "types.h"

/* prints a board struct */
void print_board(const Board *board);

/* prints a move struct given a move_to_str function */
void print_move(const Board *board, const Move *move,
                     MoveToStrFunc move_to_str);

/* prints a move list struct given a move_to_str function */
void print_move_list(const Board *board, const MoveList *list,
                     MoveToStrFunc move_to_str);

/* prints a bitboard (debug) */
void print_bitboard_debug(const U64 board);

/* prints a move struct (debug) */
void print_move_debug(const Move *move);

/* prints a move list struct (debug) */
void print_move_list_debug(const MoveList *list);

/* prints a PeSTO table (debug) */
void print_pesto_tables_debug(const int table[6][64]);

#endif
