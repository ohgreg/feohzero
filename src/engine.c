#include "board.h"
#include "fen.h"
#include "moves.h"
#include <stdio.h>
#include <stdlib.h>

int choose_move(char *fen, char *moves, int timeout) {
    (void)fen;
    (void)moves;
    (void)timeout;
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: <fen> <moves> <timeout>\n");
        return 1;
    }

    char *endptr;
    int timeout = strtol(argv[3], &endptr, 10);
    if (*endptr != '\0') {
        fprintf(stderr, "Provide a valid integer!\n");
        return 1;
    }

    init_LUT();

    char *fen = argv[1];
    Board *start = loadFEN(fen);
    print_board(start);

    // ./engine "3p3p/8/1p6/8/1P1Q2p1/2PpP3/8/8 w KQkq - 0 1" 0 0
    print_bitboard(generate_queen_moves(start, 27));

    (void)choose_move(argv[1], argv[2], timeout);

    return 0;
}
