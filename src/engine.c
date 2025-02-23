#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "board.h"
#include "eval.h"
#include "fen.h"
#include "moves.h"
#include "search.h"
#include "transposition.h"
#include "zobrist.h"
#include "print.h"

int choose_move(char *fen, char *moves, int timeout) {
    (void)timeout;

    // initialize important tables
    init_LUT();
    init_tables();
    init_zobrist();
    init_transposition_table(1024 * 1024 * 1024 / sizeof(TTentry));     // 1 GB 

    Board board;
    board.key = 0ULL;
    loadFEN(&board, fen);

    MoveList list = first_list(moves, &board);
    Move best = iterative_deepening_search(&board, 9, list);
    print_move(&best);
    for (int i = 0; i < list.count; i++) {
        if (move_equals(&best, &list.moves[i])) {
            return i;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    
    // check for correct usage of program
    if (argc != 4) {
        fprintf(stderr, "Usage: <fen> <moves> <timeout>\n");
        return 1;
    }

    char *endptr;
    // get timeout (3rd argument)
    int timeout = strtol(argv[3], &endptr, 10);
    if (*endptr != '\0') {
        fprintf(stderr, "Provide a valid integer!\n");
        return 1;
    }
    // set seed for rand()
    srand(2400118+2400141);


    printf("%d\n", choose_move(argv[1], argv[2], timeout));

    clear_transposition_table();
    return 0;
}
