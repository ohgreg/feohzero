#include "board.h"
#include <stdio.h>

void print_board(const Board *board) {
    printf("\n");
    for (int rank = 7; rank >= 0; rank--) {
        printf("%d   ", rank + 1); // rank numbers
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            char piece = ' ';
            const char *piece_color = "\033[0m"; // reset color
            const char *bg_color = (rank + file) % 2 ? "\033[47m" : "\033[40m";

            const char pieces[12] = {'P', 'N', 'B', 'R', 'Q', 'K',
                                     'p', 'n', 'b', 'r', 'q', 'k'};

            // check if any piece is in square and assign its char
            for (int i = 0; i < 6; i++) {
                if (board->white[i] & ((U64)1 << square)) {
                    piece = pieces[i];
                    piece_color = "\033[34m";
                } else if (board->black[i] & ((U64)1 << square)) {
                    piece = pieces[i + 6];
                    piece_color = "\033[31m";
                }
            }

            printf("%s\033[1m%s %c \033[0m", piece_color, bg_color,
                   piece); // print square
        }

        // print board details
        switch (rank) {
        case 7:
            printf("\tTo move: %s", board->turn == 0 ? "White" : "Black");
            break;
        case 6:
            printf("\tCastling rights: %s %s %s %s",
                   board->castle_white & 1 ? "K" : "-",
                   board->castle_white & 2 ? "Q" : "-",
                   board->castle_black & 1 ? "k" : "-",
                   board->castle_black & 2 ? "q" : "-");
            break;
        case 5:
            printf("\tEn passant: %d", board->ep_square);
            break;
        case 4:
            printf("\tHalfmove clock: %d", board->half_move);
            break;
        case 3:
            printf("\tFullmove number: %d", board->full_move);
            break;
        }

        printf("\n");
    }
    printf("\n     a  b  c  d  e  f  g  h\n"); // file letters
}


//update which squares are occupied in the board and by what color
void update_occupied(Board *board) {
    for(int i=0; i<6; i++){
        board->white_occupied |= board->white[i];
        board->black_occupied |= board->black[i];
        board->occupied = board->occupied | board->white[i] | board->black[i];
    }
}

void apply_move(Board *board, Move *move) {
    //"turn off" the bit at start_pos of the corresponding piece and update the occupied variable
    //Also, "turn on" the bit at final_pos. 
    board->half_move++;
    board->full_move++;

    PieceType piece = 0;

    switch(board->turn) {
    case WHITE_TO_MOVE:
        //look for the piece which is at move->start_pos
        for(int i=0; i<6; i++){
            if((board->white[i] & ((U64)1 << move->start_pos)) != 0)
                piece = i;
        }
        if(piece == PAWN)
            board->half_move = 0;

        board->white[piece] ^= ((U64)1 << move->start_pos);
        board->white_occupied ^= ((U64)1 << move->start_pos);
        //check for promotion
        //if move->promo != 0 then obviously piece has to be a pawn. Perhaps add a failsafe check for this ? 
        switch(move->promo) {
            case 0:
                board->white[piece] |= ((U64)1 << move->final_pos);
                break;
            case KNIGHT:
                board->white[KNIGHT] |= ((U64)1 << move->final_pos);
                break;
            case BISHOP:
                board->white[BISHOP] |= ((U64)1 << move->final_pos);
                break;
            case ROOK:
                board->white[ROOK] |= ((U64)1 << move->final_pos);
                break;
            case QUEEN:
                board->white[QUEEN] |= ((U64)1 << move->final_pos);
                break;
        }
        //check for capturing: We will have to turn off that bit.
        //check if the piece to capture is occupied by a black piece.
        if((board->black_occupied & (1 << move->final_pos)) != 0){
            for(int i=0; i<6; i++)
                board->black[i] ^= (1 << move->final_pos);
        }
        break;

    //identical logic for black
    case BLACK_TO_MOVE:
        //look for the piece which is at move->start_pos
        for(int i=0; i<6; i++){
            if((board->black[i] & ((U64)1 << move->start_pos)) != 0)
                piece = i;
        }
        if(piece == PAWN)
            board->half_move = 0;

        board->black[piece] ^= ((U64)1 << move->start_pos);
        board->black_occupied ^= ((U64)1 << move->start_pos);
        switch(move->promo) {
            case 0:
                board->black[piece] |= ((U64)1 << move->final_pos);
                break;
            case KNIGHT:
                board->black[KNIGHT] |= ((U64)1 << move->final_pos);
                break;
            case BISHOP:
                board->black[BISHOP] |= ((U64)1 << move->final_pos);
                break;
            case ROOK:
                board->black[ROOK] |= ((U64)1 << move->final_pos);
                break;
            case QUEEN:
                board->black[QUEEN] |= ((U64)1 << move->final_pos);
                break;
        }
        //check if the piece to capture is occupied by a black piece.
        if((board->black_occupied & (1 << move->final_pos)) != 0){
            for(int i=0; i<6; i++)
                board->black[i] ^= (1 << move->final_pos);
        }
        break;
    }
    //update the general occupied variable
    board->occupied ^= ((U64)1 << move->start_pos);
    board->turn = ~board->turn;
}