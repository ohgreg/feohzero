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
    start = loadFEN("r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1");
    TEST_ASSERT_NOT_NULL(start);
    // start->ep_square = 40;
    print_board(start);

    MoveList list;
    list.count = 0;
    generate_moves(&list, start);
    apply_move(start, &list.moves[16]);
    print_board(start);

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
