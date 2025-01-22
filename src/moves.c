#include "moves.h"
#include "board.h"

void init_knight_LUT(LUT *lut) {
    for (int square = 0; square < 64; square++) {
        uint64_t pos = (U64)1 << square;
        uint64_t moves = 0;

        // calculate all possible knight moves
        moves |= (pos << 17) & ~FILE_A;  // up 2, left 1
        moves |= (pos << 15) & ~FILE_H;  // up 2, right 1
        moves |= (pos << 10) & ~FILE_AB; // up 1, left 2
        moves |= (pos << 6) & ~FILE_GH;  // up 1, right 2
        moves |= (pos >> 17) & ~FILE_H;  // down 2, right 1
        moves |= (pos >> 15) & ~FILE_A;  // down 2, left 1
        moves |= (pos >> 10) & ~FILE_GH; // down 1, right 2
        moves |= (pos >> 6) & ~FILE_AB;  // down 1, left 2

        lut->knight[square] = moves;
    }
}

void init_king_LUT(LUT *lut) {
    for (int square = 0; square < 64; square++) {
        uint64_t pos = (U64)1 << square;
        uint64_t moves = 0;

        // Calculate all possible king moves
        moves |= (pos << 8);           // up
        moves |= (pos >> 8);           // down
        moves |= (pos << 1) & ~FILE_A; // right
        moves |= (pos >> 1) & ~FILE_H; // left
        moves |= (pos << 9) & ~FILE_A; // up - right
        moves |= (pos << 7) & ~FILE_H; // up - left
        moves |= (pos >> 9) & ~FILE_H; // down - left
        moves |= (pos >> 7) & ~FILE_A; // down - right

        lut->king[square] = moves;
    }
}

U64 generate_knight_moves(Board *board, LUT *lut, int pos) {
    U64 moves = lut->knight[pos];
    moves &= ~board->occupied[board->turn];
    return moves;
}

U64 generate_king_moves(Board *board, LUT *lut, int pos) {
    U64 moves = lut->king[pos];
    moves &= ~board->occupied[board->turn];
    return moves;
}

U64 generate_pawn_moves(Board *board, int pos) {
    U64 moves = 0;
    int turn = board->turn;
    int dir = turn ? -1 : 1;

    // set en passant as occupied
    if (board->ep_square != 64)
        ENABLE_BIT(board->occupied[!turn], board->ep_square);

    // capture diagonal left
    if (IS_SET_BIT(~(turn ? FILE_A : FILE_H) & board->occupied[!turn],
                   pos + 7 * dir)) {
        ENABLE_BIT(moves, pos + 7 * dir);
    }

    // capture diagonal right
    if (IS_SET_BIT(~(turn ? FILE_H : FILE_A) & board->occupied[!turn],
                   pos + 9 * dir)) {
        ENABLE_BIT(moves, pos + 9 * dir);
    }

    // clear en passant
    CLEAR_BIT(board->occupied[!turn], board->ep_square);

    // move 1 forward
    if (!IS_SET_BIT(board->occupied[2], pos + 8 * dir)) {
        ENABLE_BIT(moves, pos + 8 * dir);
        // move 2 forward
        if (IS_SET_BIT(turn ? RANK_7 : RANK_2, pos) &&
            !IS_SET_BIT(board->occupied[2], pos + 16 * dir)) {
            ENABLE_BIT(moves, pos + 16 * dir);
        }
    }

    return moves;
}
