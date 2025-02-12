#include "search.h"
#include "board.h"
#include "moves.h"
#include "limits.h"
#include <stddef.h>
#include <stdlib.h>

int sort_moves(const void *a, const void *b) {
    const Move *move_a = a;
    const Move *move_b = b;
    return move_b->score - move_a->score;
}

int move_equals(const Move *m1, const Move *m2) {
    return (m1->from == m2->from) &&
           (m1->to == m2->to) &&
           (m1->promo == m2->promo);
           // &&
           // (m1->piece == m2->piece) &&
           // (m1->captured == m2->captured) &&
           // (m1->flags == m2->flags) &&
           // (m1->ep == m2->ep) &&
           // (m1->castleWhite == m2->castleWhite) &&
           // (m1->castleBlack == m2->castleBlack);
}

int depth_limited_search(Board *board, int depth, int is_root, Move *best_move, int alpha, int beta, MoveList startList, Move previousBest) {
    int side = board->turn;
    //base case (leaf)
    if(depth == 0)
        return eval(board);

    int best_current_score = (board->turn ? INT_MAX : INT_MIN);
    Move best_current_move = {0};

    MoveList list;
    list.count = 0;
    if(is_root == 0)
        generate_moves(&list, board);
    else 
        list = startList;

    // after we're done testing, merge the 2 ifs of course
    if(is_root == 1) {
        if (previousBest.score == 20000) {
            for (int j = 0; j < list.count; j++) {
                if (move_equals(&list.moves[j], &previousBest)) {
                    list.moves[j].score = 20000; // Boost its score
                    break;
                }
            }
        }
    }

    qsort(list.moves, list.count, sizeof(Move), sort_moves);

    if (list.count == 0)
        return (board->turn ? INT_MAX : INT_MIN);

    for(int i=0; i<list.count; i++) {
        apply_move(board, &list.moves[i]);
        int recScore = depth_limited_search(board, depth-1, 0, NULL, alpha, beta, startList, previousBest);
        undo_move(board, &list.moves[i]);
        switch (side) {
            case WHITE:
                if(recScore > best_current_score) {
                    best_current_move = list.moves[i];
                    best_current_score = recScore;
                }
                if(recScore > alpha)
                    alpha = recScore;
                if (beta <= alpha) goto breakLoop;

                break;
            case BLACK:
                if(recScore < best_current_score) {
                    best_current_move = list.moves[i];
                    best_current_score = recScore;
                }
                if (recScore < beta)
                    beta = recScore;
                if(beta <= alpha) goto breakLoop;
                break;
        }
    }
    breakLoop:

    if(is_root == 1)
        *best_move = best_current_move;

    return best_current_score;
}

Move iterative_deepening_search(Board *board, int max_depth, MoveList startList) {
    Move best_move = {0};
    Move previousBest = {0};
    for (int depth = 1; depth <= max_depth; depth++) {
        Move curr_move;
        depth_limited_search(board, depth, 1, &curr_move, INT_MIN, INT_MAX, startList, previousBest);
        best_move = curr_move;
        previousBest = curr_move;
        previousBest.score = 20000;
    }
    return best_move;
}
