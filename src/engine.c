#include "board.h"
#include "parser.h"
#include "utils.h"
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

    char *fen = argv[1];
    Board *start = loadFEN(fen);
    print_board(start);

    Move *my_move = malloc(sizeof(Move));
    my_move->start_pos = 1;
    my_move->final_pos = 18;
    my_move->promo = 0;

    apply_move(start, my_move);
    print_board(start);


    (void)choose_move(argv[1], argv[2], timeout);

    return 0;
}
