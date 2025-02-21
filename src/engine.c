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

int choose_move(char *fen, char *moves, int timeout) {
    (void)timeout;

    init_LUT();
    init_tables();
    init_zobrist();
<<<<<<< HEAD
    init_transposition_table(1024 / sizeof(TTentry));
=======
    init_transposition_table(1024 * 1024 * 1024 / sizeof(TTentry));
>>>>>>> e2b7c50d0d1c8d4a4fd8aed3854035330817802d

    Board board;
    board.key = 0ULL;
    loadFEN(&board, fen);

    MoveList list = first_list(moves, &board);
<<<<<<< HEAD
    Move best = iterative_deepening_search(&board, 6, list);
    // board.key = update_board_key(&board);
    // printf("U64 key of board is: %" PRIu64 "\n", board.key);
=======
    Move best = iterative_deepening_search(&board, 8, list);
    print_move(&best);
>>>>>>> e2b7c50d0d1c8d4a4fd8aed3854035330817802d
    for (int i = 0; i < list.count; i++) {
        if (move_equals(&best, &list.moves[i])) {
            return i;
        }
    }
    clear_transposition_table();
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
    srand(2400118+2400141);

    printf("%d\n", choose_move(argv[1], argv[2], timeout));

    return 0;
}
