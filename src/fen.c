#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// i may have fucked up the includes by the way i dont think i need to include
// all of those but it kept giving errors idk

// function to load the chess position from the input FEN.
Board *loadFEN(char *fen) {

    // TO DO: check for invalid FEN format (leave for later)(for now assume it
    // is correct)
    Board *board = malloc(sizeof(Board));
    if (board == NULL) {
        fprintf(stderr, "Failed to allocate board memory!");
        exit(1);
    }
    memset(board, 0, sizeof(Board));

    int len = strlen(fen);
    int rank = 7;
    int file = 0;

    int last = 0;
    // STEP 1: read each rank one by one to load the actual position:
    // Note: discuss whether we should use white_occupied and black_occupied and
    // initialize them if so.

    for (int i = 0; i < len; i++) {

        if (rank < 0) break;

        last = i + 1;
        char c = fen[i];
        int index = (8 * rank) + file;
        // decrement rank whenever we find backslash (or space for the last
        // rank)
        if (c == ' ' || c == '/') {
            file = 0;
            rank--;
            continue;
        }

        if (c == 'p')
            board->pieces[1][0] |= ((U64)1 << index);
        else if (c == 'n')
            board->pieces[1][1] |= ((U64)1 << index);
        else if (c == 'b')
            board->pieces[1][2] |= ((U64)1 << index);
        else if (c == 'r')
            board->pieces[1][3] |= ((U64)1 << index);
        else if (c == 'q')
            board->pieces[1][4] |= ((U64)1 << index);
        else if (c == 'k')
            board->pieces[1][5] |= ((U64)1 << index);
        else if (c == 'P')
            board->pieces[0][0] |= ((U64)1 << index);
        else if (c == 'N')
            board->pieces[0][1] |= ((U64)1 << index);
        else if (c == 'B')
            board->pieces[0][2] |= ((U64)1 << index);
        else if (c == 'R')
            board->pieces[0][3] |= ((U64)1 << index);
        else if (c == 'Q')
            board->pieces[0][4] |= ((U64)1 << index);
        else if (c == 'K')
            board->pieces[0][5] |= ((U64)1 << index);
        else if ('0' < c && c < '9') {
            // care with this, may be 49? Indexing at 0
            int skip = c - 48;
            file += skip;
            continue;
        }
        file++;
    }

    // STEP 2: read whose turn it is to play:
    if (fen[last] == 'w')
        board->turn = 0;
    else if (fen[last] == 'b')
        board->turn = 1;
    else {
        printf("Couldnt get next move!\n");
        exit(1);
    }
    last++;

    // skip whitespace
    while (fen[last] == ' ')
        last++;

    // STEP 3: read castling availability:
    while (fen[last] != ' ') {
        if (fen[last] == '-') {
            board->castle_white = CANNOT_CASTLE;
            board->castle_black = CANNOT_CASTLE;
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
        board->ep_square = 0;
    else {
        int en_passant_index = fen[last] - 97;
        last++;
        if ('0' < fen[last] && fen[last] < '9')
            en_passant_index += (fen[last] - 49) * 8;
        else
            fprintf(stderr, "Invalid En Passant square!\n");
        board->ep_square |= (1 << en_passant_index);
    }
    last++;

    // skip whitespace
    while (fen[last] == ' ')
        last++;

    // STEP5: Get halfmove counter
    int halfCounter = 0;
    while ('0' < fen[last] && fen[last] < '9') {
        halfCounter = 10 * halfCounter + (fen[last] - '0');
        last++;
    }
    board->half_move = halfCounter;

    // skip whitespace
    while (fen[last] == ' ')
        last++;

    // STEP6: get fullmove counter
    int fullCounter = 0;
    while ('0' < fen[last] && fen[last] < '9') {
        fullCounter = 10 * fullCounter + (fen[last] - '0');
        last++;
    }

    board->full_move = fullCounter;

    update_occupied(board);

    return board;
}
