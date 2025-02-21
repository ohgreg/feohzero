#include "zobrist.h"

#include <stdlib.h>

#include "types.h"
#include "utils.h"

// generate random U64 number
U64 rand64(void) {
    return rand() ^ ((U64)rand() << 15) ^ ((U64)rand() << 30) ^ ((U64)rand() << 45) ^ ((U64)rand() << 60);
}

U64 zobrist_pieces[2][6][64];
U64 zobrist_castling[16];
U64 zobrist_enpassant[65];
U64 zobrist_side;

void init_zobrist(void) {
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 6; j++)
            for (int k = 0; k < 64; k++) zobrist_pieces[i][j][k] = rand64();
    for (int i = 0; i < 4; i++) zobrist_castling[i] = rand64();
    for (int i = 0; i < 65; i++) zobrist_enpassant[i] = rand64();
    zobrist_side = rand64();
}

U64 update_piece_key(PieceType piece, int sq, Turn turn) {
    U64 key = 0ULL;
    key ^= zobrist_pieces[turn][piece][sq];
    return key;
}

U64 update_castling_key(Board *board) {
    U64 key = 0ULL;
    int rights = ((int)board->castle_white << 2) | (int)board->castle_black;
    key ^= zobrist_castling[rights];
    return key;
}

U64 update_side_key(void) {
    U64 key = 0ULL;
    key ^= zobrist_side;
    return key;
}

U64 update_en_passant_key(Board *board) {
    U64 key = 0ULL;
    key ^= zobrist_enpassant[board->ep_square];
    return key;
}

U64 update_board_key(Board *board) {
    U64 key = 0ULL;
    for (PieceType piece = PAWN; piece <= KING; piece++) {
        U64 temp = board->pieces[WHITE][piece];
        while (temp != 0) {
            int sq = pop_lsb(&temp);
            key ^= update_piece_key(piece, sq, WHITE);
        }
        temp = board->pieces[BLACK][piece];
        while (temp != 0) {
            int sq = pop_lsb(&temp);
            key ^= update_piece_key(piece, sq, BLACK);
        }
    }
    key ^= update_castling_key(board);
    key ^= update_side_key();
    if (board->turn == BLACK) key ^= update_en_passant_key(board);
    return key;
}
