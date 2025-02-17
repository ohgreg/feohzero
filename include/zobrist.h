#ifndef ZOBRIST_H
#define ZOBRIST_H

#include "types.h"

U64 rand64();
void init_zobrist();
U64 update_piece_key(PieceType piece, int sq, Turn turn);
U64 update_castling_key(Board *board);
U64 update_side_key();
U64 update_en_passant_key(Board *board);
U64 update_board_key(Board *board);

#endif