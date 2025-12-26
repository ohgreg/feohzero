#ifndef FEOHZERO_MOVES_H
#define FEOHZERO_MOVES_H

#include "types.h"

/* initializes LUT attacks for each piece */
void init_moves(void);

/* returns 1 if the current side's king is under attack, 0 otherwise */
int is_king_in_check(const Board *board);

/* generates all possible and saves them in a list, given a board position */
void generate_moves(const Board *board, MoveList *list);

/* translates a SAN move string to a Move structure,
 * setting only from, to, and promo fields */
Move san_to_move(const Board *board, const char *move_str);

/* translates a UCI move string to a Move structure,
 * setting only from, to, and promo fields */
Move uci_to_move(const Board *board, const char *move_str);

/* generates initial move list given a string to Move translation function,
 * filtering for valid moves only */
void initial_list(const Board *board, MoveList *list, const char *move_str,
                  StrToMoveFunc str_to_move);

/* converts a Move to SAN notation string */
char *move_to_san(const Board *board, const Move *move);

/* converts a Move to UCI notation string */
char *move_to_uci(const Board *board, const Move *move);

#endif
