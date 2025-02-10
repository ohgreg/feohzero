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

int depth_limited_search(Board *board, int depth, int is_root, Move *best_move, int alpha, int beta) { 
    int side = board->turn;
    //base case (leaf)
    if(depth == 0)
        return eval(board);
    
    int best_current_score = (board->turn ? INT_MAX : INT_MIN);
    Move best_current_move = {0};

    MoveList list;
    list.count = 0;
    generate_moves(&list, board);
    qsort(list.moves, list.count, sizeof(Move), sort_moves);

    if (list.count == 0)
        return (board->turn ? INT_MAX : INT_MIN);

    for(int i=0; i<list.count; i++) {
        apply_move(board, &list.moves[i]);
        int recScore = depth_limited_search(board, depth-1, 0, NULL, alpha, beta);
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

Move iterative_deepening_search(Board *board, int max_depth) { 
    Move best_move = {0};
    for (int depth = 1; depth <= max_depth; depth++) {
        Move curr_move;
        depth_limited_search(board, depth, 1, &curr_move, INT_MIN, INT_MAX);
        best_move = curr_move;
    }
    return best_move;
}