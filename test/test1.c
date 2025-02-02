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
    start = loadFEN("rnbqkbnr/pppp1ppp/8/3Pp3/8/8/PPP1PPPP/RNBQKBNR w KQkq e6 0 1");
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
