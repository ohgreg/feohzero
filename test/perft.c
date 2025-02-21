#include "board.h"
#include "moves.h"
#include "fen.h"
#include "unity.h"
#include "unity_internals.h"
#include <time.h>

typedef struct {
    char *name;
    char *fen;
    int depth;
    int expected_result;
} PerftTestCase;

PerftTestCase testcases[] = {
    // https://www.chessprogramming.net/perfect-perft/
    // {"Stockfish", "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 5, 193690690},
    {"Start position", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 1, 20}, // --
    {"Start position", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 2, 400}, // --
    {"Start position", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 3, 8902}, // --
    {"Start position", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 4, 197281}, // --
    // {"Start position", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 5, 4865609},
    {"Good test position", "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 2, 2039}, // --
    // {"Illegal en passant move #1", "3k4/3p4/8/K1P4r/8/8/8/8 b - - 0 1", 6, 1134888},
    // {"Illegal en passant move #2", "8/8/4k3/8/2p5/8/B2P2K1/8 w - - 0 1", 6, 1015133},
    // {"En passant capture checks oppenent", "8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1", 6, 1440467},
    {"Short castling gives check", "5k2/8/8/8/8/8/8/4K2R w K - 0 1", 6, 661072}, // --
    {"Long castling gives check", "3k4/8/8/8/8/8/8/R3K3 w Q - 0 1", 6, 803711}, // --
    // {"Castle rights", "r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1", 4, 1274206},
    // {"Castling prevented", "r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1", 4, 1720476},
    // {"Promote out of check", "2K2r2/4P3/8/8/8/8/8/3k4 w - - 0 1", 6, 3821001},
    // {"Discovered check", "8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1", 5, 1004658},
    {"Promote to give check", "4k3/1P6/8/8/8/8/K7/8 w - - 0 1", 6, 217342}, // --
    {"Under promote to give check", "8/P1k5/K7/8/8/8/8/8 w - - 0 1", 6, 92683}, // --
    {"Self stalemate", "K1k5/8/P7/8/8/8/8/8 w - - 0 1", 6, 2217}, // --
    {"Stalemate and checkmate #1", "8/k1P5/8/1K6/8/8/8/8 w - - 0 1", 7, 567584}, // --
    {"Stalemate and checkmate #2", "8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1", 4, 23527} // --
};

const int len = sizeof(testcases) / sizeof(testcases[0]);

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

        apply_move(board, move);

        nodes += perft(board, depth - 1);

        undo_move(board, move);
    }

    return nodes;
}

void perft_tests(void) {
    for (int i = 0; i < len; i++) {
        PerftTestCase *test = &testcases[i];
        Board board;

        printf("\n[TEST %d] %s (Depth: %d)\nFEN: %s\n", i + 1, test->name, test->depth, test->fen);

        if (!loadFEN(&board, test->fen)) {
            TEST_FAIL_MESSAGE("Failed to load FEN");
        }

        // print_board(&board);

        clock_t start = clock();
        int total_moves = perft(&board, test->depth);
        clock_t end = clock();
        double total_time = (double)(end - start) / CLOCKS_PER_SEC;
        double nodes_per_sec = (total_moves / total_time);

        printf("Generated Moves: %d (Expected: %d)\n", total_moves, test->expected_result);
        printf("Total Time: %.4fs | NPS: %.0f\n", total_time, nodes_per_sec);

        TEST_ASSERT_EQUAL_INT_MESSAGE(
            test->expected_result,
            total_moves,
            "Perft test failed!"
        );
    }
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(perft_tests);
    return UNITY_END();
}
