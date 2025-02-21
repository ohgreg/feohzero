#include "board.h"

#include "utils.h"
#include "zobrist.h"

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
        if (move->to == (turn ? 62 : 6)) {  // short castling
            clear_bit(&board->pieces[turn][ROOK], move->to + 1);
            enable_bit(&board->pieces[turn][ROOK], move->to - 1);
            clear_bit(&board->occupied[turn], move->to + 1);
            clear_bit(&board->occupied[2], move->to + 1);
            enable_bit(&board->occupied[turn], move->to - 1);
            enable_bit(&board->occupied[2], move->to - 1);
        } else if (move->to == (turn ? 58 : 2)) {  // long castling
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
        if (turn == 0)
            board->castle_white = CANNOT_CASTLE;
        else
            board->castle_black = CANNOT_CASTLE;
    } else if (piece == ROOK && (board->castle_black != CANNOT_CASTLE || board->castle_white != CANNOT_CASTLE)) {
        if (turn == 0) {  // white
            if (move->from == 0) board->castle_white &= ~CAN_CASTLE_OOO;
            if (move->from == 7) board->castle_white &= ~CAN_CASTLE_OO;
        } else {  // black
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
        // enable_bit(&board->pieces[turn][piece], move->from);
    }

    // always change this
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
    board->castle_white = move->castle_white;
    board->castle_black = move->castle_black;
}
