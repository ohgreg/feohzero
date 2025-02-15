#ifndef MOVES_H
#define MOVES_H

#include "types.h"

extern const U64 rook_magic[64];
extern const U64 bishop_magic[64];

extern const int rook_shift[64];
extern const int bishop_shift[64];

extern int rook_offset[64];
extern int bishop_offset[64];

extern U64 rook_mask[64];
extern U64 bishop_mask[64];

extern LUT lut;

// function prototypes
U64 slide(U64 occupied, int truncate, int pos, int directions[4][2]);
int square_cnt(U64 value, int squares[64]);

void init_knight_LUT(void);
void init_king_LUT(void);
int init_LUT(void);

U64 generate_pawn_moves(Board *board, int pos);
U64 generate_bishop_moves(Board *board, int pos);
U64 generate_rook_moves(Board *board, int pos);
U64 generate_queen_moves(Board *board, int pos);
U64 generate_piece_moves(Board *board, int piece, int pos);
void generate_castling_moves(MoveList *list, Board *board);
void generate_en_passant(MoveList *list, Board *board);
int king_in_check(Board *board);
int is_illegal_move(Board *board, Move *move);
void generate_moves(MoveList *list, Board *board);

void print_move(Move *move);
void print_move_list(MoveList *list);
Move translate_move(const char *moveStr, Board *Board);
MoveList first_list(const char *moveStr, Board *Board);

#endif
