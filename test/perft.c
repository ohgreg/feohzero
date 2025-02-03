#include "board.h"
#include "moves.h"
#include "fen.h"
#include "unity.h"
#include "unity_internals.h"
#include <stdlib.h>
#include <time.h>

Board * board;

void setUp(void) {
    init_LUT();
}

void tearDown(void) {
    // clean stuff up here
}

int perft(Board *board, int depth) {
    if (depth == 0) {
        return 1;
    }

    MoveList list;
    list.count = 0;

    generate_moves(&list, board);

    int cnt = 0;
    // iterate over all moves in the list
    for (int i = 0; i < list.count; i++) {
        Move *move = &list.moves[i];

        // apply the move on a temp board
        Board new_board = *board;
        apply_move(&new_board, move);

        // generate moves for the next depth
        cnt += perft(&new_board, depth - 1);
    }

    return cnt;
}

void perft_test(void) {
    board = loadFEN("8/k1P5/8/1K6/8/8/8/8 w - - 0 1");
    TEST_ASSERT_NOT_NULL(board);
    // board->ep_square = 19;
    print_board(board);

    int depth = 7;

    printf("\nRunning depth: %d\n", depth);

    // Track the start time of the perft test
    clock_t start = clock();

    // Call the recursive perft function
    int total_moves = perft(board, depth);

    // Track the end time
    clock_t end = clock();

    // Calculate elapsed time
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;

    // Output results
    printf("Total moves generated: %d\n", total_moves);
    printf("Time taken: %f seconds\n", time_taken);

    free(board);
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(perft_test);
    return UNITY_END();
}
