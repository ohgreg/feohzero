#ifndef BOARD_H
#define BOARD_H
#define MAX_MOVES_PER_GAME 512

#define CANNOT_CASTLE 0
#define CAN_CASTLE_OO 1
#define CAN_CASTLE_OOO 2
#define w 0
#define b 1
#define CAN_CASTLE (CAN_CASTLE_OO | CAN_CASTLE_OOO)

typedef unsigned long long U64;
#define NAME "FeohZero"


typedef unsigned long long U64;

typedef struct 
{
    // use 12 
    U64 wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK;

    U64 white_occupied;
    U64 black_occupied;
    U64 occupied;

    unsigned char toMove;

    unsigned char castle_white;
    unsigned char castle_black;
    U64 en_passant_square;


    int halfMove;
    int fullMove;

} Board;

typedef struct
{
    unsigned char castle_white;
    unsigned char castle_black;

    
    // the en passant index
    // represents where a pawn can capture to, rather than
    // where the pawn actually is
    int en_passant_square;


} MoveInfo;



#endif