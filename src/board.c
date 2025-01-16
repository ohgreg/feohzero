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

    board->turn = !board->turn;
}
