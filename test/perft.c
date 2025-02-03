#include "board.h"
#include "moves.h"
#include "fen.h"
#include "unity.h"
#include "unity_internals.h"
#include <stdlib.h>
#include <time.h>

typedef struct {
    char *fen;
    int depth;
    int expected_result;
} PerftTestCase;

Board * board;

PerftTestCase test_cases[] = {
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 1, 20},
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 2, 400},
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 3, 8902},
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 4, 197281},
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 5, 4865609},
};

const int num_test_cases = sizeof(test_cases) / sizeof(test_cases[0]);


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

void perft_tests(void) {
    for (int i = 0; i < num_test_cases; i++) {
        PerftTestCase *test = &test_cases[i];

        board = loadFEN(test->fen);
        TEST_ASSERT_NOT_NULL(board);

        printf("\nRunning test %d: Depth %d\n", i + 1, test->depth);

        // measure time
        clock_t start = clock();
        int total_moves = perft(board, test->depth);
        clock_t end = clock();
        double time_taken = (double)(end - start) / CLOCKS_PER_SEC;

        // validate the result
        TEST_ASSERT_EQUAL_INT_MESSAGE(
            test->expected_result,
            total_moves,
            "Perft test failed"
        );

        // uutput results
        printf("Total moves generated: %d (Expected: %d)\n", total_moves, test->expected_result);
        printf("Time taken: %f seconds\n", time_taken);

        free(board);
    }
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(perft_tests);
    return UNITY_END();
}
