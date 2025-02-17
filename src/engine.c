#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>

#include "eval.h"
#include "fen.h"
#include "moves.h"
#include "search.h"
#include "zobrist.h"
#include "board.h"


int choose_move(char *fen, char *moves, int timeout) {
    (void)timeout;

    init_LUT();
    init_tables();
    init_zobrist();

    Board board;
    board.key = 0ULL;
    loadFEN(&board, fen);
    // printf("U64 key of board is: %" PRIu64 "\n", board.key);
    // print_board(&board);
    // Move move = {2, 18, 0, KNIGHT, NONE, NORMAL_MOVE, board.ep_square, board.castle_white, board.castle_black, 0};
    // apply_move(&board, &move);
    // board.key = update_board_key(&board);
    // printf("U64 key of board is: %" PRIu64 "\n", board.key);
    // print_board(&board);
    // undo_move(&board, &move);
    // board.key = update_board_key(&board);
    // printf("U64 key of board is: %" PRIu64 "\n", board.key);
    // print_board(&board);
    MoveList list = first_list(moves, &board);
    Move best = iterative_deepening_search(&board, 4, list);

    for (int i = 0; i < list.count; i++) {
        if (move_equals(&best, &list.moves[i])) {
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
    //srand(time(NULL));

    printf("%d\n", choose_move(argv[1], argv[2], timeout));

    return 0;
}
