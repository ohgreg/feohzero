#ifndef MOVES_H
#define MOVES_H

#include "types.h"

extern const U64 bishop_magic[64];
extern const U64 rook_magic[64];

extern const int bishop_shift[64];
extern const int rook_shift[64];

extern int bishop_offset[64];
extern int rook_offset[64];

extern U64 bishop_lmask[64];
extern U64 bishop_rmask[64];
extern U64 bishop_tmask[64];
extern U64 bishop_mask[64];
extern U64 rook_lmask[64];
extern U64 rook_rmask[64];
extern U64 rook_tmask[64];
extern U64 rook_mask[64];

extern LUT lut;

// function prototypes
U64 slide(U64 occupied, int truncate, int pos, int directions[4][2]);
int square_count(U64 value, int squares[64]);

void init_LUT(void);

static inline U64 generate_pawn_attacks(Board *board, int pos) {
    return lut.pawn[board->turn][pos];
}

static inline U64 generate_knight_attacks(Board *board, int pos) {
    (void)board;
    return lut.knight[pos];
}

static inline U64 generate_bishop_attacks(Board *board, int pos) {
    return lut.bishop[(((board->occupied[2] & bishop_mask[pos]) * bishop_magic[pos]) >> bishop_shift[pos]) + bishop_offset[pos]];
}

static inline U64 generate_rook_attacks(Board *board, int pos) {
    return lut.rook[(((board->occupied[2] & rook_mask[pos]) * rook_magic[pos]) >> rook_shift[pos]) + rook_offset[pos]];
}

static inline U64 generate_queen_attacks(Board *board, int pos) {
    return lut.bishop[(((board->occupied[2] & bishop_mask[pos]) * bishop_magic[pos]) >> bishop_shift[pos]) + bishop_offset[pos]]
            | lut.rook[(((board->occupied[2] & rook_mask[pos]) * rook_magic[pos]) >> rook_shift[pos]) + rook_offset[pos]];
}

static inline U64 generate_king_attacks(Board *board, int pos) {
    (void)board;
    return lut.king[pos];
}

extern U64 (*generate_piece_attacks[6])(Board *, int);
extern U64 (*generate_sliding_attacks[2])(Board *, int);

U64 generate_pawn_moves(Board *board, int pos);

U64 generate_opponent_attacks(Board *board);
U64 generate_checkers(Board *board);
U64 generate_blockmask(Board *board, int pos);
U64 generate_pinned(Board *board);
U64 generate_pinmask(Board *board, int pos);

void generate_castling(MoveList *list, Board *board, U64 checkers, U64 opponent_attacks);
void generate_en_passant(MoveList *list, Board *board);

void generate_moves(MoveList *list, Board *board);

Move translate_move(const char *moveStr, Board *Board);
MoveList first_list(const char *moveStr, Board *Board);

#endif
