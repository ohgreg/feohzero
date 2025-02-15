#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <limits.h>

// define bitboards for A, B, G, H file
#define FILE_A 0x0101010101010101ULL
#define FILE_B 0x0202020202020202ULL
#define FILE_G 0x4040404040404040ULL
#define FILE_H 0x8080808080808080ULL

// define combined files bitboards
#define FILE_GH (FILE_G | FILE_H)
#define FILE_AB (FILE_A | FILE_B)

// define bitboards for 1, 2, 7, 8 rank
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

// define eval constants
#define INF INT_MAX

#endif
