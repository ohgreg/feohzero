#include "search.h"

#include <stdlib.h>

#include "board.h"
#include "constants.h"
#include "eval.h"
#include "moves.h"
#include "transposition.h"
#include "zobrist.h"

// small helper function to be used in qsort(). Returns move with highest score.
int sort_moves(const void *a, const void *b) {
    const Move *move_a = a;
    const Move *move_b = b;
    return move_b->score - move_a->score;
}

// helper function to compare two moves (no need to check all struct members though)
int move_equals(const Move *m1, const Move *m2) {
    return (m1->from == m2->from) && (m1->to == m2->to) && (m1->promo == m2->promo);
}

// DFS but with depth limit
int depth_limited_search(Board *board, int depth, int is_root, Move *best_move, int alpha, int beta, MoveList startList, Move previousBest) {
    int side = board->turn;
    // starting alpha and beta for TT
    int alpha_original = alpha;
    int beta_original = beta;

    // probe tt table for
    TTentry *tt_entry = tt_probe(board->key);
    // tt depth has to be bigger depth or there's no point in getting that entry

    if (tt_entry != NULL && tt_entry->depth >= depth) {
        // this essentially mirrors the code below. It's just a DP table
        if (tt_entry->node_type == EXACT) {
            // if exact just return score
            //if (is_root && best_move != NULL) *best_move = tt_entry->best_move;       // i think this is useless?
            return tt_entry->score;
        } else if (tt_entry->node_type == LOWER) {
            // return at MOST tt_entry->score basically
            alpha = (alpha > tt_entry->score) ? alpha : tt_entry->score;
        } else if (tt_entry->node_type == UPPER) {
            // return at LEAST tt_entry->score basically
            beta = (beta < tt_entry->score) ? beta : tt_entry->score;
        }   // check for prune
        if (beta <= alpha) return tt_entry->score;
    }
    // base case: Return static evaluation of position
    if (depth == 0) return eval(board);

    

    MoveList list;
    list.count = 0;
    // check if it's root of position (starting position) to get the movelist from second argument or not
    if (!is_root) {
        generate_moves(&list, board);
        // Prioritize TT best move
        if (tt_entry != NULL) {
            for (int j = 0; j < list.count; j++) {
                if (move_equals(&list.moves[j], &tt_entry->best_move)) {
                    list.moves[j].score = 40000; // Highest priority
                    break;
                }
            }
        }
    } else {
        // uncomment this in prod code
        //list = startList;
        // if is_root, we have no transpo moves, so use PV node. (much better)
        generate_moves(&list, board);
        if (previousBest.score == 20000) {
            for (int j = 0; j < list.count; j++) {
                if (move_equals(&list.moves[j], &previousBest)) {
                    list.moves[j].score = 20000;  // Boost its score
                    break;
                }
            }
        }
    }
    // sort moves based on heuristic score
    qsort(list.moves, list.count, sizeof(Move), sort_moves);

    // this is draw or mate. Cooked if stalemate.
    if (list.count == 0) return (board->turn ? INF : -INF);

    int best_current_score = (side == WHITE) ? -INF : INF;
    Move best_current_move = list.moves[0];

    // Minimax logic for white and black respectively
    if (side == WHITE) {
        for (int i = 0; i < list.count; i++) {
            // make and unmake move, while also updating Zobrist key
            apply_move(board, &list.moves[i]);
            fast_board_key(board, &list.moves[i]);
            int recScore = depth_limited_search(board, depth - 1, 0, NULL, alpha, beta, startList, previousBest);
            fast_board_key(board, &list.moves[i]);
            undo_move(board, &list.moves[i]);
            // Check for better move
            if (recScore > best_current_score) {
                best_current_move = list.moves[i];
                best_current_score = recScore;
            }    // if better, update alpha
            if (recScore > alpha) alpha = recScore;
                // check for prune
            if (beta <= alpha) break;

        }
    }
        // same for black
    else {
        for (int i = 0; i < list.count; i++) {
            apply_move(board, &list.moves[i]);
            fast_board_key(board, &list.moves[i]);
            int recScore = depth_limited_search(board, depth - 1, 0, NULL, alpha, beta, startList, previousBest);
            fast_board_key(board, &list.moves[i]);
            undo_move(board, &list.moves[i]);

            if (recScore < best_current_score) {
                best_current_move = list.moves[i];
                best_current_score = recScore;
                if (recScore < beta) beta = recScore;
                if (beta <= alpha) break;
            }

        }
    }
   
    // store in tt table (this is wayback machine copy)
    Node node_type;
    if (best_current_score <= alpha_original) {
        node_type = UPPER;
    } else if (best_current_score >= beta_original) {
        node_type = LOWER;
    } else {
        node_type = EXACT;
    }

    tt_store(board->key, depth, best_current_score, best_current_move, node_type);

    // only change move if root of moves (doesnt even work otherwise)
    if (is_root == 1) *best_move = best_current_move;

    return best_current_score;
}

// IDS 
Move iterative_deepening_search(Board *board, int max_depth, MoveList startList) {
    Move best_move = {0};
    Move previousBest = {0};
    // call depth limited for each depth
    for (int depth = 1; depth <= max_depth; depth++) {
        Move curr_move;
        depth_limited_search(board, depth, 1, &curr_move, -INF, INF, startList, previousBest);
        best_move = curr_move;
        previousBest = curr_move;
        // give bonus to last best move
        previousBest.score = 20000;
    }
    return best_move;
}
