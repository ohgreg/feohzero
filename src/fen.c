#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// function to load the chess position from the input FEN.
Board *loadFEN(char *fen) {
    Board *board = malloc(sizeof(Board));
    if (board == NULL) {
        fprintf(stderr, "Failed to allocate board memory!");
        exit(1);
    }
    memset(board, 0, sizeof(Board));

    int rank = 7;
    int file = 0;

    int last = 0;
    // STEP 1: read each rank one by one to load the actual position:
    for (size_t i = 0; i < strlen(fen); i++) {
        if (rank < 0) break;

        last = i + 1;
        char c = fen[i];
        int index = (8 * rank) + file;
        // decrement rank whenever we find backslash, or space for the last rank
        if (c == ' ' || c == '/') {
            file = 0;
            rank--;
            continue;
        }
        switch(c) {
            case 'p':
                board->pieces[BLACK][PAWN] |= ((U64)1 << index);
                break;
            case 'n':
                board->pieces[BLACK][KNIGHT] |= ((U64)1 << index);
                break;
            case 'b':
                board->pieces[BLACK][BISHOP] |= ((U64)1 << index);
                break;
            case 'r':
                board->pieces[BLACK][ROOK] |= ((U64)1 << index);
                break;
            case 'q':
                board->pieces[BLACK][QUEEN] |= ((U64)1 << index);
                break;
            case 'k':
                board->pieces[BLACK][KING] |= ((U64)1 << index);
                break;
            case 'P':
                board->pieces[WHITE][PAWN] |= ((U64)1 << index);
                break;
            case 'N':
                board->pieces[WHITE][KNIGHT] |= ((U64)1 << index);
                break;
            case 'B':
                board->pieces[WHITE][BISHOP] |= ((U64)1 << index);
                break;
            case 'R':
                board->pieces[WHITE][ROOK] |= ((U64)1 << index);
                break;
            case 'Q':
                board->pieces[WHITE][QUEEN] |= ((U64)1 << index);
                break;
            case 'K':
                board->pieces[WHITE][KING] |= ((U64)1 << index);
                break;
            default:
                if ('0' < c && c < '9') {
                    int skip = c - 48;
                    file += skip;
                    continue;
                }
        }
        file++;
    }

    // STEP 2: read whose turn it is to play:
    switch (fen[last]){
        case 'w':
            board->turn = WHITE;
            break;
        case 'b':
            board->turn = BLACK;
            break;
        default:
            printf("Couldnt get next move!\n");
            exit(1);
    }
    last++;

    // skip whitespace
    while (fen[last] == ' ') last++;
    
    // STEP 3: read castling availability:
    while (fen[last] != ' ') {
        if (fen[last] == '-') {
            board->castle_white = CANNOT_CASTLE;
            board->castle_black = CANNOT_CASTLE;
            last++;
            break;
        }
        if (fen[last] == 'K') {
            board->castle_white = CAN_CASTLE_OO;
            if (fen[last + 1] == 'Q') {
                board->castle_white = CAN_CASTLE;
                last++;
            }
        } else if (fen[last] == 'Q')
            board->castle_white = CAN_CASTLE_OOO;
        else if (fen[last] == 'k') {
            board->castle_black = CAN_CASTLE_OO;
            if (fen[last + 1] == 'q') {
                board->castle_black = CAN_CASTLE;
                last++;
            }
        } else if (fen[last] == 'q')
            board->castle_black = CAN_CASTLE_OOO;

        last++;
    }

    // skip whitespace
    while (fen[last] == ' ')
        last++;

    // STEP4: get enpassant square
    if (fen[last] == '-')
        board->ep_square = 64;
    else {
        int en_passant_index = fen[last] - 97;
        last++;
        if ('0' < fen[last] && fen[last] < '9')
            en_passant_index += (fen[last] - 49) * 8;
        else
            fprintf(stderr, "Invalid En Passant square!\n");
        board->ep_square = en_passant_index;
    }
    last++;

    // skip whitespace
    while (fen[last] == ' ')
        last++;

    // STEP5: Get halfmove counter
    int halfCounter = 0;
    while ('0' <= fen[last] && fen[last] <= '9') {
        halfCounter = 10 * halfCounter + (fen[last] - '0');
        last++;
    }
    board->half_move = halfCounter;

    // skip whitespace
    while (fen[last] == ' ')
        last++;

    // STEP6: get fullmove counter
    int fullCounter = 0;
    while ('0' <= fen[last] && fen[last] <= '9') {
        fullCounter = 10 * fullCounter + (fen[last] - '0');
        last++;
    }

    board->full_move = fullCounter;
    update_occupied(board);

    return board;
}
