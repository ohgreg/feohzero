#include "board.h"
#include "eval.h"
#include "fen.h"
#include "moves.h"
#include "search.h"
#include <stdio.h>
#include <stdlib.h>

int choose_move(char *fen, char *moves, int timeout) {
    (void)timeout;

    init_LUT();
    init_tables();

    Board board;
    loadFEN(&board, fen);

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

    Board start;
    loadFEN(&start, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    //print_board(&start);

    int move = choose_move(argv[1], argv[2], timeout);
    printf("%d\n", move);

    // MoveList list = first_list("a3 a4 b3 b4 c3 c4 d3 d4 e3 e4 f3 f4 g3 g4 h3 h4 Na3 Nc3 Nf3 Nh3", &start);
    // print_move_list(&list);

    // Move best = iterative_deepening_search(&start, 5, list);

    // for (int i = 0; i < list.count; i++) {
    //     if (move_equals(&best, &list.moves[i])) {
    //         printf("%d\n", i);
    //     }
    // }

    // print_move_list(&list);
    // print_move(&best);

    return 0;
}
