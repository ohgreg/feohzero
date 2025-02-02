#ifndef MOVES_H
#define MOVES_H

#include "board.h"

// define bitboards for A, B, G, H file
#define FILE_A 0x0101010101010101ULL
#define FILE_B 0x0202020202020202ULL
#define FILE_G 0x4040404040404040ULL
#define FILE_H 0x8080808080808080ULL

// define bitboards for 2, 7 rank
#define RANK_1 0x00000000000000FFULL
#define RANK_2 0x000000000000FF00ULL
#define RANK_7 0x00FF000000000000ULL
#define RANK_8 0xFF00000000000000ULL

// define the castling masks
#define WSHORT 0x70ULL
#define BSHORT 0x7000000000000000ULL
#define WLONG_OCCUPIED 0x1EULL
#define WLONG_ATTACKED 0x1CULL
#define BLONG_OCCUPIED 0x1E00000000000000ULL
#define BLONG_ATTACKED 0x1C00000000000000ULL

// combined bitboards
#define FILE_GH (FILE_G | FILE_H)
#define FILE_AB (FILE_A | FILE_B)

extern const U64 rook_magic[64];
extern const U64 bishop_magic[64];

extern const int rook_shift[64];
extern const int bishop_shift[64];

extern int rook_offset[64];
extern int bishop_offset[64];

extern U64 rook_mask[64];
extern U64 bishop_mask[64];

// struct for LUTs
typedef struct {
    U64 knight[64];
    U64 king[64];
    U64 rook[102400];
    U64 bishop[5248];
} LUT;

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

#endif
