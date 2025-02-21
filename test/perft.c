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
    // https://gist.github.com/peterellisjones/8c46c28141c162d1d8a0f0badbc9cff9?permalink_comment_id=3775288
    {"Position 1", "r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 2", 1, 8},
    {"Position 2", "8/8/8/2k5/2pP4/8/B7/4K3 b - d3 0 3", 1, 8},
    {"Position 3", "r1bqkbnr/pppppppp/n7/8/8/P7/1PPPPPPP/RNBQKBNR w KQkq - 2 2", 1, 19},
    {"Position 4", "r3k2r/p1pp1pb1/bn2Qnp1/2qPN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQkq - 3 2", 1, 5},
    {"Position 5", "2kr3r/p1ppqpb1/bn2Qnp1/3PN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQ - 3 2", 1, 44},
    {"Position 6", "rnb2k1r/pp1Pbppp/2p5/q7/2B5/8/PPPQNnPP/RNB1K2R w KQ - 3 9", 1, 39},
    {"Position 7", "2r5/3pk3/8/2P5/8/2K5/8/8 w - - 5 4", 1, 9},
    // {"Position 8", "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 3, 62379}, // --
    {"Position 9", "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 3, 89890},
    {"Position 10", "3k4/3p4/8/K1P4r/8/8/8/8 b - - 0 1", 6, 1134888},
    {"Position 11", "8/8/4k3/8/2p5/8/B2P2K1/8 w - - 0 1", 6, 1015133},
    {"Position 12", "8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1", 6, 1440467},
    {"Position 13", "5k2/8/8/8/8/8/8/4K2R w K - 0 1", 6, 661072},
    {"Position 14", "3k4/8/8/8/8/8/8/R3K3 w Q - 0 1", 6, 803711},
    // {"Position 15", "r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1", 4, 1274206}, // --

    // https://www.chessprogramming.net/perfect-perft/
    {"Start position", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 1, 20},
    {"Start position", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 2, 400},
    {"Start position", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 3, 8902},
    {"Start position", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 4, 197281},
    // {"Start position", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 5, 4865609}, // --
    // {"Start position", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 6, 119060324}, // --
    {"Good test position", "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 2, 2039},
    // {"Good test position", "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 3, 97862}, // --
    // {"Good test position", "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 4, 4085603}, // --
    // {"Good test position", "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 5, 193690690}, // --
    {"Illegal en passant move #1", "3k4/3p4/8/K1P4r/8/8/8/8 b - - 0 1", 6, 1134888},
    {"Illegal en passant move #2", "8/8/4k3/8/2p5/8/B2P2K1/8 w - - 0 1", 6, 1015133},
    {"En passant capture checks oppenent", "8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1", 6, 1440467},
    {"Short castling gives check", "5k2/8/8/8/8/8/8/4K2R w K - 0 1", 6, 661072},
    {"Long castling gives check", "3k4/8/8/8/8/8/8/R3K3 w Q - 0 1", 6, 803711},
    // {"Castle rights", "r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1", 4, 1274206}, // --
    // {"Castling prevented", "r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1", 4, 1720476}, // --
    // {"Promote out of check", "2K2r2/4P3/8/8/8/8/8/3k4 w - - 0 1", 6, 3821001}, // --
    // {"Discovered check", "8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1", 5, 1004658}, // --
    {"Promote to give check", "4k3/1P6/8/8/8/8/K7/8 w - - 0 1", 6, 217342},
    {"Under promote to give check", "8/P1k5/K7/8/8/8/8/8 w - - 0 1", 6, 92683},
    {"Self stalemate", "K1k5/8/P7/8/8/8/8/8 w - - 0 1", 6, 2217},
    {"Stalemate and checkmate #1", "8/k1P5/8/1K6/8/8/8/8 w - - 0 1", 7, 567584},
    {"Stalemate and checkmate #2", "8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1", 4, 23527}
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
