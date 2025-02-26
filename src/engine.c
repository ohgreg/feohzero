#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "eval.h"
#include "fen.h"
#include "moves.h"
#include "search.h"
#include "transposition.h"
#include "zobrist.h"

#if DEBUG == 1
    #include "print.h"
#endif

int choose_move(char *fen, char *moves, int timeout) {
    srand(2400118 + 2400141);

    // initialize important tables
    init_moves();
    init_eval();
    init_zobrist();
    init_tt(16 * 1024 * 1024 / sizeof(TTentry)); // 16 MB

    Board board;
    board.key = (U64)0;
    loadFEN(&board, fen);

    MoveList list = initial_list(&board, moves);
    Move best = ids_search(&board, 50, list, timeout);

    #if DEBUG == 1
        print_board(&board);
        print_move(&best);
    #endif

    clear_tt();

    for (int i = 0; i < list.count; i++) {
        if (equal_moves(&best, &list.moves[i])) {
            return i;
        }
    }

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

    printf("%d\n", choose_move(argv[1], argv[2], timeout));

    return 0;
}
