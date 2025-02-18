#include "search.h"

#include <stdlib.h>

#include "board.h"
#include "constants.h"
#include "eval.h"
#include "moves.h"
#include "transposition.h"
#include "zobrist.h"

int sort_moves(const void *a, const void *b) {
    const Move *move_a = a;
    const Move *move_b = b;
    return move_b->score - move_a->score;
}

int move_equals(const Move *m1, const Move *m2) {
    return (m1->from == m2->from) && (m1->to == m2->to) && (m1->promo == m2->promo);
}

int depth_limited_search(Board *board, int depth, int is_root, Move *best_move, int alpha, int beta,
                         MoveList startList, Move previousBest) {
    int side = board->turn;
    // starting alpha and beta for TT
    int alpha_original = alpha; 
    int beta_original = beta;

    // probe tt table for 
    TTentry *tt_entry = tt_probe(board->key);
    // tt depth has to be bigger depth or there's no point in getting that entry
    if (tt_entry != NULL && tt_entry->depth >= depth) {

        if (tt_entry->node_type == EXACT) {
            // if exact just return score
            if (is_root && best_move != NULL) *best_move = tt_entry->best_move;
            return tt_entry->score;
        } else if (tt_entry->node_type == LOWER) {
            // return at MOST tt_entry->score basically
            alpha = (alpha > tt_entry->score) ? alpha : tt_entry->score;
        } else if (tt_entry->node_type == UPPER) {
            // return at LEAST tt_entry->score basically
            beta = (beta < tt_entry->score) ? beta : tt_entry->score;
        }
        if (beta <= alpha) return tt_entry->score;
    }

    if (depth == 0) return eval(board);

    int best_current_score = (side == WHITE) ? -INF : INF;
    Move best_current_move = {0};

    MoveList list;
    list.count = 0;
    if (!is_root) {
        generate_moves(&list, board);
        // Prioritize TT best move
        if (tt_entry != NULL && tt_entry->best_move.from != tt_entry->best_move.to) {
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

    qsort(list.moves, list.count, sizeof(Move), sort_moves);

    if (list.count == 0) return (board->turn ? INF : -INF);

    if (side == WHITE) {
        for (int i = 0; i < list.count; i++) {
            apply_move(board, &list.moves[i]);
            fast_board_key(board, &list.moves[i]);
            int recScore = depth_limited_search(board, depth - 1, 0, NULL, alpha, beta, startList, previousBest);
            fast_board_key(board, &list.moves[i]);
            undo_move(board, &list.moves[i]);
            
            if (recScore > best_current_score) {
                best_current_move = list.moves[i];
                best_current_score = recScore;
            }
            if (recScore > alpha) alpha = recScore;
            if (beta <= alpha) break;
            
        }
    }
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
    // for (int i = 0; i < list.count; i++) {
    //     apply_move(board, &list.moves[i]);
    //     fast_board_key(board, &list.moves[i]);
    //     int recScore = depth_limited_search(board, depth - 1, 0, NULL, alpha, beta, startList, previousBest);
    //     fast_board_key(board, &list.moves[i]);
    //     undo_move(board, &list.moves[i]);
    //     if (side == WHITE) {
    //         if (recScore > best_current_score) {
    //             best_current_move = list.moves[i];
    //             best_current_score = recScore;
    //         }
    //         if (recScore > alpha) alpha = recScore;
    //         if (beta <= alpha) break;
    //     }
    //     else if (recScore < best_current_score) {
    //         best_current_move = list.moves[i];
    //         best_current_score = recScore;
    //         if (recScore < beta) beta = recScore;
    //         if (beta <= alpha) break;
    //     }
        
    // }

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

    if (is_root == 1) *best_move = best_current_move;

    return best_current_score;
}

Move iterative_deepening_search(Board *board, int max_depth, MoveList startList) {
    Move best_move = {0};
    Move previousBest = {0};
    for (int depth = 1; depth <= max_depth; depth++) {
        Move curr_move;
        depth_limited_search(board, depth, 1, &curr_move, -INF, INF, startList, previousBest);
        best_move = curr_move;
        previousBest = curr_move;
        previousBest.score = 20000;
    }
    return best_move;
}
