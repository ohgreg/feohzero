#include "board.h"
#include <stdio.h>
#include <string.h>

void print_board(const Board *board) {
    printf("\n");
    for (int rank = 7; rank >= 0; rank--) {
        printf("%d   ", rank + 1); // rank numbers
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            char piece_char = ' ';
            const char *piece_color = "\033[0m"; // reset color
            const char *bg_color = (rank + file) % 2 ? "\033[47m" : "\033[40m";

            const char piece_chars[12] = {'P', 'N', 'B', 'R', 'Q', 'K',
                                          'p', 'n', 'b', 'r', 'q', 'k'};

            // check if any piece is in square and assign its char
            for (int i = 0; i < 6; i++) {
                if (board->pieces[0][i] & ((U64)1 << square)) {
                    piece_char = piece_chars[i];
                    piece_color = "\033[34m";
                } else if (board->pieces[1][i] & ((U64)1 << square)) {
                    piece_char = piece_chars[i + 6];
                    piece_color = "\033[31m";
                }
            }

            printf("%s\033[1m%s %c \033[0m", piece_color, bg_color,
                   piece_char); // print square
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

// update which squares are occupied in the board and by what color
void update_occupied(Board *board) {
    for (int i = 0; i < 6; i++) {
        board->occupied[0] |= board->pieces[0][i];
        board->occupied[1] |= board->pieces[1][i];
        board->occupied[2] |= board->pieces[0][i] | board->pieces[1][i];
    }
}

void apply_move(Board *board, Move *move) {
    // "turn off" the bit at start_pos of the corresponding piece and update the
    // occupied variable Also, "turn on" the bit at final_pos.
    board->half_move++;
    board->full_move++;

    Turn turn = board->turn;
    PieceType piece = 0;

    // look for the piece which is at move->start_pos
    for (int i = 0; i < 6; i++) {
        if (IS_SET_BIT(board->pieces[turn][i], move->start_pos)) {
            piece = i;
            break;
        }
    }
    if (piece == PAWN) board->half_move = 0;
    CLEAR_BIT(board->pieces[turn][piece], move->start_pos);

    // check for promotion
    // if move->promo != 0 then obviously piece has to be a pawn. Perhaps
    // add a failsafe check for this ?
    switch (move->promo) {
    case 0:
        ENABLE_BIT(board->pieces[turn][piece], move->final_pos);
        break;
    case KNIGHT:
        ENABLE_BIT(board->pieces[turn][KNIGHT], move->final_pos);
        break;
    case BISHOP:
        ENABLE_BIT(board->pieces[turn][BISHOP], move->final_pos);
        break;
    case ROOK:
        ENABLE_BIT(board->pieces[turn][ROOK], move->final_pos);
        break;
    case QUEEN:
        ENABLE_BIT(board->pieces[turn][QUEEN], move->final_pos);
        break;
    }

    // check for capturing: We will have to turn off that bit.
    // check if the piece to capture is occupied by a black piece.
    if (IS_SET_BIT(board->occupied[!turn], move->final_pos)) {
        for (int i = 0; i < 5; i++) {
            CLEAR_BIT(board->pieces[!turn][i], move->final_pos);
        }
    }

    // update the occupied bitboards
    CLEAR_BIT(board->occupied[2], move->start_pos);
    ENABLE_BIT(board->occupied[2], move->final_pos);
    CLEAR_BIT(board->occupied[turn], move->start_pos);
    ENABLE_BIT(board->occupied[turn], move->final_pos);
    CLEAR_BIT(board->occupied[!turn], move->final_pos);

    // SOS IMPORTANT: en_passant_square and castling rights must be updated here

    board->turn = !board->turn;
}

int abs(int x){
    if(x > 0) 
        return x;
    return -x; 
}

// util function for translateMove. If rank/file is -1, it will not be taken into account.
int findOrigin(Board *Board, int pieceRank, int pieceFile, PieceType piece, int final_rank, int final_file) {
    
    int start_pos;
    Turn turn = Board->turn;

    // different "modes" of the function.
    // Mode 1: No file and no rank given. Regular piece move (with no similiar pieces)
    if(pieceRank == -1 && pieceFile == -1) {
        switch(piece) {
            // Check the knight moveset and see how originated
            case KNIGHT:
                for(int i=-2; i<=2; i++){
                    if(i == 0)
                        continue;
                    for(int j=-2; j<=2; j++){
                        //i*j == 0 is not worth it for performance probably has loops
                        if(j == 0 || abs(i) == abs(j) || final_rank+i > 7 || final_rank+i < 0 || final_file+j > 7 || final_file+j < 0)
                            continue;
                        start_pos = 8*(final_rank+i) + final_file+j;
                        if(IS_SET_BIT(Board->pieces[turn][KNIGHT], start_pos))
                            return start_pos;
                    }
                }
                break;
            case BISHOP:
                int dir[4] = {0};
                for(int i=1; i<=7; i++){
                    // check top right diagonal movement
                    if(final_rank+i <= 7 && final_file+i <= 7 && dir[0] == 0){
                        start_pos = 8*(final_rank+i) + final_file+i;
                        if(IS_SET_BIT(Board->occupied[2], start_pos) && !IS_SET_BIT(Board->pieces[turn][BISHOP], start_pos))
                            dir[0] = 1;
                        else
                            return start_pos;
                    }
                    else
                        dir[0] = 1;

                    // check top left diagonal movement
                    if(final_rank+i <= 7 && final_file-i >= 0 && dir[1] == 0){
                        start_pos = 8*(final_rank+i) + final_file-i;
                        if(IS_SET_BIT(Board->occupied[2], start_pos) && !IS_SET_BIT(Board->pieces[turn][BISHOP], start_pos))
                            dir[1] = 1;
                        else
                            return start_pos;
                    }
                    else
                        dir[1] = 1;

                    // check bottom left diagonal movement
                    if(final_rank-i >= 0 && final_file-i >=0 && dir[2] == 0){
                        start_pos = 8*(final_rank-i) + final_file-i;
                        if(IS_SET_BIT(Board->occupied[2], start_pos) && !IS_SET_BIT(Board->pieces[turn][BISHOP], start_pos))
                            dir[2] = 1;
                        else
                            return start_pos;
                    }
                    else
                        dir[2] = 1;

                    // check bottom right diagonal movement
                    if(final_rank-i >= 0 && final_file+i <= 7 && dir[3] == 0){
                        start_pos = 8*(final_rank-i) + final_file+i;
                        if(IS_SET_BIT(Board->occupied[2], start_pos) && !IS_SET_BIT(Board->pieces[turn][BISHOP], start_pos))
                            dir[3] = 1;
                        else
                            return start_pos;
                    }
                    else
                        dir[3] = 1;
                }


            break;
        }
    }



}

// move to translate moves from standard chess notation to bitboard logic 
Move translateMove(const char* moveStr, Board *Board){

    // IMPORTANT REMINDER that this function most likely doesnt have castling and en passant implemented.
    // After making the correspdoning functions, modify this one to support those as well. 


    int size = strlen(moveStr);
    int isPawn = 0;
    PieceType piece = 0;
    int start_rank;
    int start_file;
    int final_rank;
    int final_file;
    int index;
    Turn turn = Board->turn;
    Move move = {0};

    // logic to capture whether we have mate, check, or neither. 
    if(moveStr[size-1] == '+')
        move.check = 1;
    else if(moveStr[size-1] == '#')
        move.mate = 1;

    // logic to check if we have promotion or not
    // this can probably be optimized further, since most likely the '=' will be at the 3rd or 5th position of the moveStr (indexing from 1)
    // or we can limit the search to only if there exists a pawn at the 7th rank, or better yet a pawn move at rank 8. To discuss 
    char toPromote = 0;
    if(size>=4) {
        for(int i=size-1; i>=2; i--){
            if(moveStr[i] == '='){
                toPromote = moveStr[i+1];
            }
        }
    }

    // Check what piece to promote to: 
    switch(toPromote) {
        case 0:
            move.promo = 0;
            break;
        case 'N':
            move.promo = KNIGHT;
            break;
        case 'B':
            move.promo = BISHOP;
            break;
        case 'R':
            move.promo = ROOK;
            break;
        case 'Q':
            move.promo = QUEEN;
            break;
    }

    if(moveStr[0] >= 'a' && moveStr[0] <= 'z')
        isPawn = 1; 
    
    switch(isPawn) {
        case 1:
            // check second element. If digit, it's a regular pawn move, no captures.
            if(moveStr[1] >= '0' && moveStr[1] <= '9' ) {
                // file is straight forward
                start_file = moveStr[0] - 'a';
                // for rank, we have to check if it's 2 steps forward or one. Minimize checks by checking only for '4' (white) and '5' (black)
                if(moveStr[1] == '4' && turn == WHITE_TO_MOVE) {
                    //an easy check would simply be to check if there's any piece at x3. 
                    if(IS_SET_BIT(Board->occupied[2], (8*2)+start_file))
                        start_rank = 2;
                    else
                        start_rank = 1;
                }
                // same logic for black
                else if(moveStr[1] == '5' && turn == BLACK_TO_MOVE) {
                    if(IS_SET_BIT(Board->occupied[2], (8*5)+start_file))
                        start_rank = 5;
                    else
                        start_rank = 6;
                }
                else
                    start_rank = moveStr[1] - '1';
                
                final_file = start_file;
                final_rank = moveStr[1]- '1';

            }
            else {
                //if a pawn move doesnt have a digit at the second character, it's a capture. 
                start_file = moveStr[0] - 'a';
                final_file = moveStr[2] - 'a';
                final_rank = moveStr[3] - '1';
                start_rank = moveStr[3] - '2';

                
            }
            move.start_pos = (start_rank * 8) + start_file;
            move.final_pos = (final_rank * 8) + final_file;
            break;
        case 0:
            // find which piece is moving 
            switch(moveStr[0]) {
                case 'N': 
                    piece = KNIGHT;
                case 'B':
                    piece = BISHOP;
                case 'R':
                    piece = ROOK;
                case 'Q': 
                    piece = QUEEN;
                case 'K': 
                    piece = KING;
            }
            // check for regular piece move
            if(size == 3 || (size == 4 && moveStr[1] == 'x')){
                final_rank = moveStr[size-1] - '1';
                final_file = moveStr[size-2] - 'a';
                move.start_pos = findOrigin(Board, -1, -1, piece, final_rank, final_file);
            }
                


        break;
    }
        
    


    return move;
}