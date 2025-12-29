#ifndef FEOHZERO_CONSTANTS_H
#define FEOHZERO_CONSTANTS_H

#include <limits.h>

/* bitboard for file A (leftmost file) */
#define FILE_A 0x0101010101010101ULL
/* bitboard for file B */
#define FILE_B 0x0202020202020202ULL
/* bitboard for file G */
#define FILE_G 0x4040404040404040ULL
/* bitboard for file H (rightmost file) */
#define FILE_H 0x8080808080808080ULL

/* bitboard for files G and H combined */
#define FILE_GH 0xC0C0C0C0C0C0C0C0ULL
/* bitboard for files A and B combined */
#define FILE_AB 0x0303030303030303ULL

/* bitboard for rank 1 (white's back rank) */
#define RANK_1 0x00000000000000FFULL
/* bitboard for rank 2 */
#define RANK_2 0x000000000000FF00ULL
/* bitboard for rank 7 */
#define RANK_7 0x00FF000000000000ULL
/* bitboard for rank 8 (black's back rank) */
#define RANK_8 0xFF00000000000000ULL

/* default capacity for move lists */
#define DEFAULT_MOVELIST_CAPACITY 64

/* squares that must be empty for white kingside castling */
#define WSHORT 0x70ULL
/* squares that must be empty for black kingside castling */
#define BSHORT 0x7000000000000000ULL
/* squares that must be empty for white queenside castling */
#define WLONG_OCCUPIED 0x1EULL
/* squares that must not be attacked for white queenside castling */
#define WLONG_ATTACKED 0x1CULL
/* squares that must be empty for black queenside castling */
#define BLONG_OCCUPIED 0x1E00000000000000ULL
/* squares that must not be attacked for black queenside castling */
#define BLONG_ATTACKED 0x1C00000000000000ULL

/* represents infinity in evaluation scores */
#define INF INT_MAX
/* represents the base value for a checkmate */
#define MATE_SCORE 300000
/* represents the boundary to distinguish mate scores from normal eval */
#define MATE_THRESHOLD (MATE_SCORE - 256)
/* penalty for doubled pawns in the middlegame */
#define DOUBLED_PAWN_MG_PENALTY 10
/* penalty for doubled pawns in the endgame */
#define DOUBLED_PAWN_EG_PENALTY 20
/* bonus for king safety in the middlegame */
#define KING_SAFETY_BONUS_MG 20
/* bonus for king safety in the endgame */
#define KING_SAFETY_BONUS_EG 15
/* penalty for king on open file in the middlegame */
#define OPEN_FILE_KING_MG_PENALTY 25
/* penalty for king on open file in the endgame */
#define OPEN_FILE_KING_EG_PENALTY 40

/* move ordering boost for the best move from previous search */
#define BEST_MOVE_BOOST 40000
/* move ordering boost for the previous iteration's best move */
#define PREVIOUS_BEST_BOOST 20000

/* macros for stringifying config values */
#define XSTR(x) STR(x)
#define STR(x) #x

/* config default fen (start position) */
#define DEFAULT_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
/* config default timeout */
#define DEFAULT_TIMEOUT 1000
/* config default search depth */
#define DEFAULT_DEPTH 10
/* config default transposition table size */
#define DEFAULT_TT_SIZE_KB 16384
/* config default seed */
#define DEFAULT_SEED 259

#endif
