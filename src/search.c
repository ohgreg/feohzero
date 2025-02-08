#include "search.h"
#include "board.h"
#include "moves.h"
#include "limits.h"


int depth_limited_search(Board *board, int depth, int is_root, Move *best_move) { 
    //base case (leaf)
    if(depth == 0)
        return eval(board);
    
    int best_current_score = (board->turn ? INT_MAX : INT_MIN);
    Move best_current_move = {0};

    MoveList list;
    list.count = 0;
    generate_moves(&list, board);
    for(int i=0; i<list.count; i++) {
        apply_move(board, &list.moves[i]);
        int recScore = depth_limited_search(board, depth-1, 0, NULL);
        undo_move(board, &list.moves[i]);
        switch (board->turn) {
            case WHITE: 
                if(recScore > best_current_score) {
                    best_current_move = list.moves[i]; 
                    best_current_score = recScore;
                }
                break;
            case BLACK: 
                if(recScore < best_current_score) {
                    best_current_move = list.moves[i]; 
                    best_current_score = recScore;
                }
                break;
        }
    }

    if(is_root == 1) 
        *best_move = best_current_move;
    
    return best_current_score;
}

Move iterative_deepening_search(Board *board, int max_depth) { 
    Move best_move = {0}; // Initialize to a default move

    int best_score = (board->turn ? INT_MAX : INT_MIN);
    
    for (int depth = 1; depth <= max_depth; depth++) {
        Move curr_move;
        int curr_score = depth_limited_search(board, depth, 1, &curr_move);
        
        switch (board->turn) {
            case WHITE: 
                if(curr_score > best_score) {
                    best_move = curr_move; 
                    best_score = curr_score;
                }
                break;
            case BLACK: 
                if(curr_score < best_score) {
                    best_move = curr_move; 
                    best_score = curr_score;
                }
                break;
        }
    }

    return best_move;
}