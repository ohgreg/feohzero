#include "board.h"
#include <stdio.h>

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

void print_bitboard(const U64 board) {
    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            if (is_set_bit(board, 8 * rank + file)) {
                putchar('1');
                continue;
            }
            putchar('.');
        }
        putchar('\n');
    }
    putchar('\n');
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
    // update the full move and half move counters
    Turn turn = board->turn;

    if (turn) board->full_move++;
    board->half_move++;

    // remove the piece which is at move->from
    PieceType piece = move->piece;
    clear_bit(&board->pieces[turn][piece], move->from);

    // check for promotion, and add piece at move->to
    if (move->promo == 0) {
        enable_bit(&board->pieces[turn][piece], move->to);
    } else {
        enable_bit(&board->pieces[turn][move->promo], move->to);
    }

    // handle normal capture at move->to
    if (is_set_bit(board->occupied[!turn], move->to)) {
        for (int i = PAWN; i <= QUEEN; i++) {
            if (is_set_bit(board->pieces[!turn][i], move->to)) {
                clear_bit(&board->pieces[!turn][i], move->to);
                move->captured = i;
                break;
            }
        }
    // handle rook move in castling
    } else if (move->flags & CASTLING) {
        if (move->to == (turn ? 62 : 6)) { // short castling
            clear_bit(&board->pieces[turn][ROOK], move->to + 1);
            enable_bit(&board->pieces[turn][ROOK], move->to - 1);
            clear_bit(&board->occupied[turn], move->to + 1);
            clear_bit(&board->occupied[2], move->to + 1);
            enable_bit(&board->occupied[turn], move->to - 1);
            enable_bit(&board->occupied[2], move->to - 1);

        } else if (move->to == (turn ? 58 : 2)) { // long castling
            clear_bit(&board->pieces[turn][ROOK], move->to - 2);
            enable_bit(&board->pieces[turn][ROOK], move->to + 1);
            clear_bit(&board->occupied[turn], move->to - 2);
            clear_bit(&board->occupied[2], move->to - 2);
            enable_bit(&board->occupied[turn], move->to + 1);
            enable_bit(&board->occupied[2], move->to + 1);
        }
    }
    // handle en passant capture
    else if (move->flags & EN_PASSANT) {
        int sq = turn ? move->to + 8 : move->to - 8;
        clear_bit(&board->pieces[!turn][PAWN], sq);
        clear_bit(&board->occupied[!turn], sq);
        clear_bit(&board->occupied[2], sq);
        move->captured = PAWN;
    }



    // update en passant square
    if (move->flags & DOUBLE_PAWN_PUSH) {
        board->ep_square = (turn ? (move->to) + 8 : (move->to) - 8);
    } else {
        board->ep_square = 64;
    }

    // update castling rights in rook or king moves
    if (piece == KING && (board->castle_black != CANNOT_CASTLE || board->castle_white != CANNOT_CASTLE)) {
        if (turn == 0) board->castle_white = CANNOT_CASTLE;
        else board->castle_black = CANNOT_CASTLE;
    } else if (piece == ROOK && (board->castle_black != CANNOT_CASTLE || board->castle_white != CANNOT_CASTLE)) {
        if (turn == 0) { // white
            if (move->from == 0) board->castle_white &= ~CAN_CASTLE_OOO;
            if (move->from == 7) board->castle_white &= ~CAN_CASTLE_OO;
        } else { // black
            if (move->from == 56) board->castle_black &= ~CAN_CASTLE_OOO;
            if (move->from == 63) board->castle_black &= ~CAN_CASTLE_OO;
        }
    }

    // update castling rights if a rook is captured
    if (move->captured == ROOK) {
        if (!turn) {
            if (move->to == 56) board->castle_black &= ~CAN_CASTLE_OOO;
            if (move->to == 63) board->castle_black &= ~CAN_CASTLE_OO;
        } else {
            if (move->to == 0) board->castle_white &= ~CAN_CASTLE_OOO;
            if (move->to == 7) board->castle_white &= ~CAN_CASTLE_OO;
        }
    }

    // update occupied bitboards
    clear_bit(&board->occupied[!turn], move->to);
    clear_bit(&board->occupied[2], move->from);
    enable_bit(&board->occupied[2], move->to);
    clear_bit(&board->occupied[turn], move->from);
    enable_bit(&board->occupied[turn], move->to);

    // update turn
    ;
    board->turn = !board->turn;
}

void undo_move(Board *board, Move *move) {
    board->turn = !board->turn;

    Turn turn = board->turn;
    PieceType piece = move->piece;
    board->ep_square = move->ep;

    if (move->promo == 0) {
        clear_bit(&board->pieces[turn][piece], move->to);
    } else {
        clear_bit(&board->pieces[turn][move->promo], move->to);
        //enable_bit(&board->pieces[turn][piece], move->from);
    }

    //always change this
    enable_bit(&board->pieces[turn][piece], move->from);

    if (move->captured != NONE && (move->flags & EN_PASSANT) == 0) {
        enable_bit(&board->pieces[!turn][move->captured], move->to);
        enable_bit(&board->occupied[!turn], move->to);
    } else {
        clear_bit(&board->occupied[2], move->to);
    }



    if (move->flags & EN_PASSANT) {
        int sq = turn ? move->to + 8 : move->to - 8;
        if (sq >= 0 && sq < 64) {
            enable_bit(&board->pieces[!turn][PAWN], sq);
            enable_bit(&board->occupied[!turn], sq);
            clear_bit(&board->occupied[2], move->to);
        }
    } else if (move->flags & CASTLING) {
        if (move->to == (turn ? 62 : 6)) {
            enable_bit(&board->pieces[turn][ROOK], move->to + 1);
            clear_bit(&board->pieces[turn][ROOK], move->to - 1);
            clear_bit(&board->occupied[turn], move->to - 1);
            clear_bit(&board->occupied[2], move->to - 1);
            enable_bit(&board->occupied[2], move->to + 1);
            enable_bit(&board->occupied[turn], move->to + 1);
        } else if (move->to == (turn ? 58 : 2)) {
            enable_bit(&board->pieces[turn][ROOK], move->to - 2);
            clear_bit(&board->pieces[turn][ROOK], move->to + 1);
            clear_bit(&board->occupied[turn], move->to + 1);
            clear_bit(&board->occupied[2], move->to + 1);
            enable_bit(&board->occupied[2], move->to - 2);
            enable_bit(&board->occupied[turn], move->to - 2);
        }
    }

    enable_bit(&board->occupied[2], move->from);
    enable_bit(&board->occupied[turn], move->from);
    clear_bit(&board->occupied[turn], move->to);


    if (turn) board->full_move--;
    board->half_move--;
    board->ep_square = move->ep;
    board->castle_white = move->castleWhite;
    board->castle_black = move->castleBlack;
}

// int abs(int x){
//     if(x > 0)
//         return x;
//     return -x;
// }

// // util function for translateMove. If rank/file is -1, it will not be taken
// into account. int findOrigin(Board *Board, int pieceRank, int pieceFile,
// PieceType piece, int final_rank, int final_file) {

//     int from;
//     Turn turn = Board->turn;

//     // different "modes" of the function.
//     // Mode 1: No file and no rank given. Regular piece move (with no
//     similiar pieces) if(pieceRank == -1 && pieceFile == -1) {
//         switch(piece) {
//             // Check the knight moveset and see how originated
//             case KNIGHT:
//                 for(int i=-2; i<=2; i++){
//                     if(i == 0)
//                         continue;
//                     for(int j=-2; j<=2; j++){
//                         //i*j == 0 is not worth it for performance probably
//                         has loops if(j == 0 || abs(i) == abs(j) ||
//                         final_rank+i > 7 || final_rank+i < 0 || final_file+j
//                         > 7 || final_file+j < 0)
//                             continue;
//                         from = 8*(final_rank+i) + final_file+j;
//                         if(is_set_bit(&Board->pieces[turn][KNIGHT],
//                         from))
//                             return from;
//                     }
//                 }
//                 break;
//             case BISHOP:
//                 int dir[4] = {0};
//                 for(int i=1; i<=7; i++) {
//                     for(int j=0; j<4; j++) {
//                         int k1;
//                         int k2;
//                         switch (j) {
//                             case 0:
//                                 k1 = i;
//                                 k2 = i;
//                                 break;
//                             case 1:
//                                 k1 = i;
//                                 k2 = -i;
//                                 break;
//                             case 2:
//                                 k1 = -i;
//                                 k2 = -i;
//                                 break;
//                             case 3:
//                                 k1 = -i;
//                                 k2 = i;
//                         }
//                         if(final_rank+k1 <= 7 && final_file+k2 <= 7 && dir[j]
//                         == 0){ from = 8*(final_rank+k1) + final_file+k2;
//                         if(is_set_bit(&Board->occupied[2], from) &&
//                         !is_set_bit(&Board->pieces[turn][BISHOP], from))
//                             dir[j] = 1;
//                         else
//                             return from;
//                     }
//                     else
//                         dir[j] = 1;

//                     }
//                 }
//                 break;

//             case ROOK:

//         }
//     }

// }

// // move to translate moves from standard chess notation to bitboard logic
// Move translateMove(const char* moveStr, Board *Board){

//     // IMPORTANT REMINDER that this function most likely doesnt have castling
//     and en passant implemented.
//     // After making the correspdoning functions, modify this one to support
//     those as well.

//     int size = strlen(moveStr);
//     int isPawn = 0;
//     PieceType piece = 0;
//     int start_rank;
//     int start_file;
//     int final_rank;
//     int final_file;
//     int index;
//     Turn turn = Board->turn;
//     Move move = {0};

//     // logic to capture whether we have mate, check, or neither.
//     if(moveStr[size-1] == '+')
//         move.check = 1;
//     else if(moveStr[size-1] == '#')
//         move.mate = 1;

//     // logic to check if we have promotion or not
//     // this can probably be optimized further, since most likely the '=' will
//     be at the 3rd or 5th position of the moveStr (indexing from 1)
//     // or we can limit the search to only if there exists a pawn at the 7th
//     rank, or better yet a pawn move at rank 8. To discuss char toPromote = 0;
//     if(size>=4) {
//         for(int i=size-1; i>=2; i--){
//             if(moveStr[i] == '='){
//                 toPromote = moveStr[i+1];
//             }
//         }
//     }

//     // Check what piece to promote to:
//     switch(toPromote) {
//         case 0:
//             move.promo = 0;
//             break;
//         case 'N':
//             move.promo = KNIGHT;
//             break;
//         case 'B':
//             move.promo = BISHOP;
//             break;
//         case 'R':
//             move.promo = ROOK;
//             break;
//         case 'Q':
//             move.promo = QUEEN;
//             break;
//     }

//     if(moveStr[0] >= 'a' && moveStr[0] <= 'z')
//         isPawn = 1;

//     switch(isPawn) {
//         case 1:
//             // check second element. If digit, it's a regular pawn move, no
//             captures. if(moveStr[1] >= '0' && moveStr[1] <= '9' ) {
//                 // file is straight forward
//                 start_file = moveStr[0] - 'a';
//                 // for rank, we have to check if it's 2 steps forward or one.
//                 Minimize checks by checking only for '4' (white) and '5'
//                 (black) if(moveStr[1] == '4' && turn == WHITE_TO_MOVE) {
//                     //an easy check would simply be to check if there's any
//                     piece at x3. if(is_set_bit(&Board->occupied[2],
//                     (8*2)+start_file))
//                         start_rank = 2;
//                     else
//                         start_rank = 1;
//                 }
//                 // same logic for black
//                 else if(moveStr[1] == '5' && turn == BLACK_TO_MOVE) {
//                     if(is_set_bit(&Board->occupied[2], (8*5)+start_file))
//                         start_rank = 5;
//                     else
//                         start_rank = 6;
//                 }
//                 else
//                     start_rank = moveStr[1] - '1';

//                 final_file = start_file;
//                 final_rank = moveStr[1]- '1';

//             }
//             else {
//                 //if a pawn move doesnt have a digit at the second character,
//                 it's a capture. start_file = moveStr[0] - 'a'; final_file =
//                 moveStr[2] - 'a'; final_rank = moveStr[3] - '1'; start_rank =
//                 moveStr[3] - '2';

//             }
//             move.from = (start_rank * 8) + start_file;
//             move.to = (final_rank * 8) + final_file;
//             break;
//         case 0:
//             // find which piece is moving
//             switch(moveStr[0]) {
//                 case 'N':
//                     piece = KNIGHT;
//                 case 'B':
//                     piece = BISHOP;
//                 case 'R':
//                     piece = ROOK;
//                 case 'Q':
//                     piece = QUEEN;
//                 case 'K':
//                     piece = KING;
//             }
//             // check for regular piece move
//             if(size == 3 || (size == 4 && moveStr[1] == 'x')){
//                 final_rank = moveStr[size-1] - '1';
//                 final_file = moveStr[size-2] - 'a';
//                 move.from = findOrigin(Board, -1, -1, piece, final_rank,
//                 final_file);
//             }

//         break;
//     }

//     return move;
// }
