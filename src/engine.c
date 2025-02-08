#include "board.h"
#include "eval.h"
#include "fen.h"
#include "moves.h"
#include <stdio.h>
#include <stdlib.h>

int choose_move(char *fen, char *moves, int timeout) {
    (void)fen;
    (void)moves;
    (void)timeout;

    init_LUT();
    init_tables();

    Board board;
    loadFEN(&board, fen);

    MoveList list;
    list.count = 0;
    generate_moves(&list, &board);
    print_move_list(&list);
    apply_move(&board, &list.moves[34]);
    print_board(&board);
    undo_move(&board, &list.moves[34]);
    print_board(&board);
    (void)board;

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
    loadFEN(&start, argv[1]);
    print_board(&start);


    (void)choose_move(argv[1], argv[2], timeout);

    return 0;
}
