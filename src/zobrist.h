#ifndef FEOHZERO_ZOBRIST_H
#define FEOHZERO_ZOBRIST_H

#include "types.h"

/* initializes zobrist hashing keys with random values */
void init_zobrist(void);

/* returns the zobrist key for a piece of given side and type at a position */
U64 get_zobrist_piece(Turn side, PieceType piece, int pos);

/* returns the zobrist key for a specific castling rights configuration */
U64 get_zobrist_castling(int pos);

/* returns the zobrist key for a specific en passant square */
U64 get_zobrist_enpassant(int pos);

/* returns the zobrist key for toggling the side to move */
U64 get_zobrist_side(void);

/* incrementally updates the board's zobrist hash key after applying a move */
void fast_board_key(Board *board, const Move *move);

#endif
