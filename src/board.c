#include "board.h"

#include "utils.h"
#include "zobrist.h"

// loads a chess position in FEN format to a board
int loadFEN(Board *board, char *fen) {
    *board = (Board){0};
    int rank = 7, file = 0, last = 0;

    // STEP 1: parse pieces' positions
    char c;
    // loop through each rank
    while (rank >= 0) {
        int index = 8 * rank + file;
        c = fen[last];

        if (c == '\0') {
            return 0;
        }
        if (c == ' ' || c == '/') {
            file = 0;
            rank--;
            last++;
            continue;
        } else if ('1' <= c && c <= '8') {
            file += c - '0';
            last++;
            continue;
        }
        int color = ('a' <= c && c <= 'z') ? BLACK : WHITE;
        int piece;
        switch (c | 32) {  // equivalent to tolower()
            case 'p':
                piece = PAWN;
                break;
            case 'n':
                piece = KNIGHT;
                break;
            case 'b':
                piece = BISHOP;
                break;
            case 'r':
                piece = ROOK;
                break;
            case 'q':
                piece = QUEEN;
                break;
            case 'k':
                piece = KING;
                break;
            default:
                last++;
                continue;
        }
        board->pieces[color][piece] |= ((U64)1 << index);
        file++;
        last++;
    }

    if (fen[last] == '\0') return 0;

    // STEP 2: parse turn
    board->turn = (fen[last] == 'w') ? WHITE : BLACK;
    last++;
    while (fen[last] == ' ') last++;

    // STEP 3: parse castling rights
    board->castle_white = board->castle_black = CANNOT_CASTLE;
    while (fen[last] != ' ') {
        switch (fen[last]) {
            case '-':
                break;
            case 'K':
                board->castle_white |= CAN_CASTLE_OO;
                break;
            case 'Q':
                board->castle_white |= CAN_CASTLE_OOO;
                break;
            case 'k':
                board->castle_black |= CAN_CASTLE_OO;
                break;
            case 'q':
                board->castle_black |= CAN_CASTLE_OOO;
                break;
            default:  // includes '\0'
                return 0;
        }
        last++;
    }
    while (fen[last] == ' ') last++;

    // STEP 4: parse en passant square
    board->ep_square = 64;
    while (fen[last] == '-') last++;
    if (fen[last] && fen[last + 1] && 'a' <= fen[last] && fen[last] <= 'h' && '1' <= fen[last + 1] && fen[last + 1] <= '8') {
        file = fen[last] - 'a';
        rank = fen[last + 1] - '1';
        board->ep_square = 8 * rank + file;
        last += 2;
    }
    while (fen[last] == ' ') last++;

    // STEP 5: parse halfmove counter
    board->half_move = 0;
    while ('0' <= fen[last] && fen[last] <= '9') {
        board->half_move = 10 * board->half_move + (fen[last] - '0');
        last++;
    }
    while (fen[last] == ' ') last++;

    // STEP 6: parse fullmove counter
    board->full_move = 0;
    while ('0' <= fen[last] && fen[last] <= '9') {
        board->full_move = 10 * board->full_move + (fen[last] - '0');
        last++;
    }

    // STEP 7: update occupied and zobrist key
    for (int i = 0; i < 6; i++) {
        board->occupied[WHITE] |= board->pieces[WHITE][i];
        board->occupied[BLACK] |= board->pieces[BLACK][i];
        board->occupied[2] |= board->pieces[WHITE][i] | board->pieces[BLACK][i];
    }

    // STEP 8: update zobrist key
    board->key = (U64)0;
    for (int piece = PAWN; piece <= KING; piece++) {
        U64 pieces = board->pieces[WHITE][piece];
        while (pieces) {
            int sq = pop_lsb(&pieces);
            board->key ^= zobrist_pieces[WHITE][piece][sq];
        }
        pieces = board->pieces[BLACK][piece];
        while (pieces) {
            int sq = pop_lsb(&pieces);
            board->key ^= zobrist_pieces[BLACK][piece][sq];
        }
    }
    board->key ^= zobrist_castling[((int)board->castle_white << 2) | (int)board->castle_black];
    board->key ^= zobrist_enpassant[board->ep_square];
    if (board->turn == BLACK) {
        board->key ^= zobrist_side;
    }

    return 1;
}

// applies a move, changing the chess position
void apply_move(Board *board, Move *move) {
    Turn turn = board->turn;

    // update full move and half move counters
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
        // loop through each piece type to find what piece was captured
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
        // move->to differents from the square the captured pawn is in!
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

// reverts a move, apply_move() mirror
void undo_move(Board *board, Move *move) {
    // revert turn
    board->turn = !board->turn;

    Turn turn = board->turn;
    PieceType piece = move->piece;

    // revert en passant square
    board->ep_square = move->ep;

    // revert promotion
    if (move->promo == 0) {
        clear_bit(&board->pieces[turn][piece], move->to);
    } else {
        clear_bit(&board->pieces[turn][move->promo], move->to);
    }

    // always change this
    enable_bit(&board->pieces[turn][piece], move->from);

    // revert captured piece
    if (move->captured != NONE && (move->flags & EN_PASSANT) == 0) {
        enable_bit(&board->pieces[!turn][move->captured], move->to);
        enable_bit(&board->occupied[!turn], move->to);
    } else {
        clear_bit(&board->occupied[2], move->to);
    }

    // revert en passant
    if (move->flags & EN_PASSANT) {
        int sq = turn ? move->to + 8 : move->to - 8;
        if (sq >= 0 && sq < 64) {
            enable_bit(&board->pieces[!turn][PAWN], sq);
            enable_bit(&board->occupied[!turn], sq);
            clear_bit(&board->occupied[2], move->to);
        }
    // revert castling
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

    // revert occupied bitboard
    enable_bit(&board->occupied[2], move->from);
    enable_bit(&board->occupied[turn], move->from);
    clear_bit(&board->occupied[turn], move->to);

    // revert misc info
    if (turn) board->full_move--;
    board->half_move--;
    board->castle_white = move->castle_white;
    board->castle_black = move->castle_black;
}
