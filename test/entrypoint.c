#include <stdio.h>
#include "fen.h"
#include "unity.h"
#include "unity_internals.h"

#include "print.h"
#include "moves.h"
#include "board.h"
#include "utils.h"

void setUp(void) {
    init_LUT();
}

void tearDown(void) {
    // clean stuff up here
}

int perft(Board *board, int depth) {
    if (depth == 0)
        return 1;

    MoveList list;
    list.count = 0;

    generate_moves(&list, board);
    // print_move_list(&list);

    int nodes = 0;
    for (int i = 0; i < list.count; i++) {
        Move *move = &list.moves[i];

        if (depth == 2 && ((move->flags & CASTLING) != 0) && (move->to == 62) && is_set_bit(board->pieces[0][PAWN], 24)) {
            apply_move(board, move);
            MoveList list2;
            list2.count = 0;
            generate_moves(&list2, board);
            print_move_list(&list2);
            undo_move(board, move);
        }

        apply_move(board, move);

        nodes += perft(board, depth - 1);

        undo_move(board, move);
    }

    return nodes;
}

void test(void) {
    Board board;
    if (!loadFEN(&board, "3r3k/6p1/7p/6P1/8/2nPPP2/r1p2K1P/2q5 w - - 0 42")) {
        TEST_FAIL_MESSAGE("Failed to load FEN");
    }

    print_board(&board);

    MoveList list;
    list.count = 0;

    generate_moves(&list, &board);

    print_move_list(&list);

    // int depth = 3;
    // int total = 0;

    // for (int i = 0; i < list.count; i++) {
    //     Move *move = &list.moves[i];

    //     apply_move(&board, move);

    //     // print_move(move);

    //     int nodes = perft(&board, depth - 1);
    //     // printf("Moves: %d\n", nodes);
    //     total += nodes;

    //     undo_move(&board, move);
    // }

    // printf("Total nodes: %d\n", total); // 97862
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test);
    return UNITY_END();
}
