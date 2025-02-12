#include "board.h"
#include "eval.h"
#include "fen.h"
#include "moves.h"
#include "search.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int choose_move(char *fen, char *moves, int timeout) {
    (void)fen;
    (void)moves;
    (void)timeout;

    init_LUT();
    init_tables();

    Board board;
    loadFEN(&board, fen);
    MoveList list = first_list(moves, &board);
    print_move_list(&list);

    Move best = iterative_deepening_search(&board, 7, list);
    
    apply_move(&board, &best);
    print_board(&board);
    print_move(&best);
    


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

    //printf("Pog move is:\n");
    //Move move = translate_move("c3", &start);
    //print_move(&move);

    
    
    

    return 0;
}
