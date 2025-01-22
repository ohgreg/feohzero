#ifndef MOVES_H
#define MOVES_H

#include "board.h"

// define bitboards for A, B, G, H file
#define FILE_A 0x0101010101010101ULL
#define FILE_B 0x0202020202020202ULL
#define FILE_G 0x4040404040404040ULL
#define FILE_H 0x8080808080808080ULL

// define bitboards for 2, 7 rank
#define RANK_2 0x000000000000FF00ULL
#define RANK_7 0x00FF000000000000ULL

// combined bitboards
#define FILE_GH (FILE_G | FILE_H)
#define FILE_AB (FILE_A | FILE_B)

// struct for LUTs
typedef struct {
    U64 knight[64];
    U64 king[64];
    U64 *rook;
    U64 *bishop;
} LUT;

// function prototypes
void init_knight_LUT(LUT *lut);
void init_king_LUT(LUT *lut);

U64 generate_pawn_moves(Board *board, int pos);
U64 generate_knight_moves(Board *board, LUT *lut, int pos);
U64 generate_king_moves(Board *board, LUT *lut, int pos);
// U64 generate_bishop_moves(Board *board, int pos);
// U64 generate_rook_moves(Board *board, int pos);
// U64 generate_queen_moves(Board *board, int pos);

#endif
