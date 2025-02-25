#ifndef ZOBRIST_H
#define ZOBRIST_H

#include "types.h"

// zobrist hashing keys
extern U64 zobrist_pieces[2][6][64];
extern U64 zobrist_castling[16];
extern U64 zobrist_enpassant[65];
extern U64 zobrist_side;

// function prototypes
U64 rand64(void);
void init_zobrist(void);
U64 update_piece_key(PieceType piece, int sq, Turn turn);
U64 update_castling_key(Board *board);
U64 update_side_key(void);
U64 update_en_passant_key(int sq);
U64 update_board_key(Board *board);
void fast_board_key(Board *board, const Move *move);

#endif
