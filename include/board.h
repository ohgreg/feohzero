#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

typedef uint64_t U64;
typedef uint16_t U16;
typedef uint8_t U8;

// enum for piece types
typedef enum {
    PAWN = 0,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
} PieceType;

// enum for castling rights (2 bits for each player)
typedef enum {
    CANNOT_CASTLE = 0, // 00
    CAN_CASTLE_OO = 1,   // 01
    CAN_CASTLE_OOO = 2,  // 10
    CAN_CASTLE = 3       // 11 (both sides)
} CastleRights;

// enum for move type (1 bit in total)
typedef enum {
    WHITE_TO_MOVE = 0,
    BLACK_TO_MOVE = 1
} Turn;

// define structure for representing the board state
typedef struct {
    // use 2 arrays of 6 for black and white
    U64 white[6];
    U64 black[6];

    U64 white_occupied;
    U64 black_occupied;
    U64 occupied;

    Turn turn : 1;                 // 1 bit
    CastleRights castle_white : 2; // 2 bits
    CastleRights castle_black : 2; // 2 bits

    U8 ep_square;
    U8 half_move;
    U16 full_move;
} Board;

typedef struct {

    unsigned int start_pos : 6;
    unsigned int final_pos : 6;
    unsigned int promo : 3;

} Move;



/*  ### FUNCTIONS ###  */



#endif
