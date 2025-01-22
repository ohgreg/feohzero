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

    char *fen = argv[1];
    Board *start = loadFEN(fen);
    // print_board(start);

    Move *my_move = malloc(sizeof(Move));
    my_move->start_pos = 12;
    my_move->final_pos = 28;
    my_move->promo = 0;

    apply_move(start, my_move);
    print_board(start);
    printf("\n%llx\n\n", generate_pawn_moves(start, 51));

    my_move->start_pos = 51;
    my_move->final_pos = 35;
    my_move->promo = 0;

    apply_move(start, my_move);
    print_board(start);
    printf("\n%llx\n\n", generate_pawn_moves(start, 28));

    my_move->start_pos = 28;
    my_move->final_pos = 35;
    my_move->promo = 0;

    apply_move(start, my_move);
    print_board(start);
    LUT lut;
    init_knight_LUT(&lut);
    printf("\n%llx\n\n", generate_knight_moves(start, &lut, 62));

    my_move->start_pos = 62;
    my_move->final_pos = 45;
    my_move->promo = 0;

    apply_move(start, my_move);
    print_board(start);

    init_king_LUT(&lut);
    printf("\n%llx\n\n", generate_king_moves(start, &lut, 4));

    (void)choose_move(argv[1], argv[2], timeout);

    return 0;
}
