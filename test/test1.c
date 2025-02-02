#include "board.h"
#include "moves.h"
#include "fen.h"
#include "unity.h"
#include "unity_internals.h"
#include <stdlib.h>

Board * start;

void setUp(void) {
    init_LUT();
}

void tearDown(void) {
    // clean stuff up here
}

void test_generate_moves(void) {
    start = loadFEN("r1b2rk1/ppp2ppp/2np1n2/2q1p1B1/2B1P3/bPNP4/P1PNQPPP/R3K2R w KQ - 0 1");
    TEST_ASSERT_NOT_NULL(start);
    print_board(start);

    MoveList list;
    list.count = 0;
    generate_moves(&list, start);

    print_move_list(&list);

    free(start);
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_generate_moves);
    return UNITY_END();
}
