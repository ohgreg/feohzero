#include "fen.h"
#include "unity.h"
#include "unity_internals.h"

#include "print.h"
#include "moves.h"
#include "board.h"
#include "utils.h"

void setUp(void) {
    init_moves();
}

void tearDown(void) {
    // clean stuff up here
}

void test(void) {
    Board board;
    if (!loadFEN(&board, "rnbqkbnr/ppppp1p1/1N5p/2N2p2/4P3/2P5/PP1P1PPP/R1BQKB1R w KQkq - 0 1")) {
        TEST_FAIL_MESSAGE("Failed to load FEN");
    }

    MoveList list = initial_list(&board, "Nba4");
    print_move_list(&list);
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test);
    return UNITY_END();
}
