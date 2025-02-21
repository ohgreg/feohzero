#include "fen.h"
#include "unity.h"
#include "unity_internals.h"

#include "print.h"
#include "moves.h"

void setUp(void) {
    init_LUT();
}

void tearDown(void) {
    // clean stuff up here
}

void test(void) {
    Board board;
    if (!loadFEN(&board, "8/8/1n2K3/8/8/1q6/8/5k2 w - - 0 1")) {
        TEST_FAIL_MESSAGE("Failed to load FEN");
    }

    print_board(&board);

    MoveList list;
    list.count = 0;

    generate_moves(&list, &board);
    print_move_list(&list);
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test);
    return UNITY_END();
}
