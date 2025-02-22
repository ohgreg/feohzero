#include "zobrist.h"

#include <stdlib.h>

#include "types.h"
#include "utils.h"

// generate random U64 number
U64 rand64(void) {
    return rand() ^ ((U64)rand() << 15) ^ ((U64)rand() << 30) ^ ((U64)rand() << 45) ^ ((U64)rand() << 60);
}

// initialize arrays to hold zobrist keys
U64 zobrist_pieces[2][6][64];
U64 zobrist_castling[16];
U64 zobrist_enpassant[65];
U64 zobrist_side;

// init zobrist keys
void init_zobrist(void) {
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 6; j++)
            for (int k = 0; k < 64; k++) zobrist_pieces[i][j][k] = rand64();
    for (int i = 0; i < 4; i++) zobrist_castling[i] = rand64();
    for (int i = 0; i < 65; i++) zobrist_enpassant[i] = rand64();
    zobrist_side = rand64();
}

// XOR piece 
U64 update_piece_key(PieceType piece, int sq, Turn turn) {
    U64 key = 0ULL;
    key ^= zobrist_pieces[turn][piece][sq];
    return key;
}

// XOR castling rights
U64 update_castling_key(Board *board) {
    U64 key = 0ULL;
    int rights = ((int)board->castle_white << 2) | (int)board->castle_black;
    key ^= zobrist_castling[rights];
    return key;
}

// XOR turn
U64 update_side_key(void) {
    U64 key = 0ULL;
    key ^= zobrist_side;
    return key;
}

// XOR ep square
U64 update_en_passant_key(int sq) {
    U64 key = 0ULL;
    key ^= zobrist_enpassant[sq];
    return key;
}

// Combine all the above in one function (only called once, at fen.c)
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
    key ^= update_en_passant_key(board->ep_square);
    if(board->turn == BLACK)
        key ^= update_side_key();
    return key;
}

// XOR the info that changes from a move (fast version of above)
void fast_board_key(Board *board, const Move *move) {
    // Removes the key of the piece's previous position
    board->key ^= update_piece_key(move->piece, move->from, board->turn);

    // Update en passant key (XOR out old, XOR in new)
    board->key ^= update_en_passant_key(move->ep);
    board->key ^= update_en_passant_key(board->ep_square);

    // XOR castling rights (need temp board to xor old ones)
    if(((board->castle_black ^ move->castle_black) != 0) || ((board->castle_white ^ move->castle_white) != 0)) {
        Board temp;
        temp.castle_black = move->castle_black;
        temp.castle_white = move->castle_white;
        board->key ^= update_castling_key(&temp);
        board->key ^= update_castling_key(board);
    }

    // Special cases for pawn and king 
    switch(move->piece) {
        case PAWN:
            if(move->flags & EN_PASSANT) {
                board->key ^= update_piece_key(move->piece, move->to, board->turn);
                int sq = board->turn ? move->to + 8 : move->to - 8;
                board->key ^= update_piece_key(move->piece, move->to + sq, !board->turn);
            }
            if(move->flags & PROMOTION) {
                board->key ^= update_piece_key(move->promo, move->to, board->turn);
            }
            else {
                board->key ^= update_piece_key(move->piece, move->to, board->turn);
            }
            //could also just use the flag but yeah
            if(move->captured != NONE) {
                board->key ^= update_piece_key(move->captured, move->to, !board->turn);
            }
        break;

        case KING:
            if (move->flags & CASTLING) {
                if(move->to == (board->turn ? 62 : 6)) {
                    board->key ^= update_piece_key(KING, move->to, board->turn);
                    board->key ^= update_piece_key(ROOK, move->to + 1, board->turn);
                    board->key ^= update_piece_key(ROOK, move->to - 1, board->turn);
                }
                else if(move->to == (board->turn ? 58 : 2)) {
                    board->key ^= update_piece_key(KING, move->to, board->turn);
                    board->key ^= update_piece_key(ROOK, move->to + 1, board->turn);
                    board->key ^= update_piece_key(ROOK, move->to - 2, board->turn);
                }
                break;
            }
            //no break cause may not be castle
            // also the comment below is genuinely needed or it fails
            /* fall through */
        default:
            board->key ^= update_piece_key(move->piece, move->to, board->turn);
            if(move->captured != NONE) {
                board->key ^= update_piece_key(move->captured, move->to, !board->turn);
            }
            board->key ^= update_side_key();

    }
}
