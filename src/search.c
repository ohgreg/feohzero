#include "search.h"

#include <stdlib.h>
#include <time.h>

#include "board.h"
#include "constants.h"
#include "eval.h"
#include "moves.h"
#include "transposition.h"
#include "zobrist.h"

/* search stack for holding ply data */
typedef struct {
  int ply;
  Move previous_best;
  MoveList *start_list;
} SearchStack;

/* search info for tracking nodes and time constraints */
typedef struct {
  int nodes;
  clock_t start_time;
  int timeout;
  int stop;
} SearchInfo;

/* checks if the search has exceeded the time limit */
static inline int has_timed_out(SearchInfo *info) {
  if (info->stop)
    return 1;

  if ((info->nodes++ & 2047) != 0)
    return 0;

  if ((int)(((double)(clock() - info->start_time) / CLOCKS_PER_SEC) * 1000) >=
      info->timeout) {
    info->stop = 1;
    return 1;
  }

  return 0;
}

/* compares moves by score for sorting, returns highest score move */
static int compare_moves(const void *a, const void *b) {
  return ((const Move *)b)->score - ((const Move *)a)->score;
}

/* checks if two moves are equal */
static int equal_moves(const Move *a, const Move *b) {
  return (a->from == b->from) && (a->to == b->to) && (a->promo == b->promo);
}

/* depth-limited search (dls) with alpha-beta pruning */
static int dls_search(Board *board, int depth, Move *best_move, int alpha,
                      int beta, SearchStack *ss, SearchInfo *info) {
  // STEP 1: check for timeout or stop flag
  if (has_timed_out(info))
    return eval(board);

  // STEP 2: initialize alpha, beta and current side
  int side = board->turn;
  int initial_alpha = alpha;
  int initial_beta = beta;

  // STEP 3: probe the transposition table
  // NOTE: tt's depth has to be bigger depth or there's no point
  TTentry *tt_entry = probe_tt(board->key);
  if (tt_entry != NULL && tt_entry->depth >= depth) {
    if (tt_entry->node_type == EXACT)
      return tt_entry->score;
    else if (tt_entry->node_type == LOWER)
      alpha = (alpha > tt_entry->score) ? alpha : tt_entry->score;
    else if (tt_entry->node_type == UPPER)
      beta = (beta < tt_entry->score) ? beta : tt_entry->score;

    if (beta <= alpha)
      return tt_entry->score;
  }

  // STEP 4: recursion base case
  if (depth == 0)
    return eval(board);

  // STEP 5: generate moves based on ply
  MoveList list;
  if (ss->ply != 0) {
    list.count = 0;
    generate_moves(board, &list);

    // prioritize best move from transposition table
    if (tt_entry != NULL) {
      for (int j = 0; j < list.count; j++) {
        if (equal_moves(&list.moves[j], &tt_entry->best_move)) {
          list.moves[j].score = BEST_MOVE_BOOST;
          break;
        }
      }
    }
  } else {
    list = *ss->start_list; // use root move list

    // boost previous best move score
    if (ss->previous_best.score == PREVIOUS_BEST_BOOST) {
      for (int j = 0; j < list.count; j++) {
        if (equal_moves(&list.moves[j], &ss->previous_best)) {
          list.moves[j].score = PREVIOUS_BEST_BOOST;
          break;
        }
      }
    }
  }

  // STEP 6: sort moves based on heuristic scores
  qsort(list.moves, list.count, sizeof(Move), compare_moves);

  // STEP 7: handle end game conditions
  if (list.count == 0) {
    if (is_king_in_check(board))
      return (board->turn ? INF : -INF); // checkmate (cooked)
    return 0;                            // stalemate
  }

  // STEP 8: iterate through generated moves
  int best_current_score =
      (side == WHITE) ? -INF : INF;       // initialize best score
  Move best_current_move = list.moves[0]; // initialize best move

  // minimax logic for white and black respectively
  // NOTE: the code repetitiveness in handling white and black moves is
  // intentional for optimization purposes
  if (side == WHITE) {
    for (int i = 0; i < list.count; i++) {
      apply_move(board, &list.moves[i]);
      fast_board_key(board, &list.moves[i]);

      int rec_score =
          dls_search(board, depth - 1, NULL, alpha, beta, ss + 1, info);

      fast_board_key(board, &list.moves[i]);
      undo_move(board, &list.moves[i]);

      if (info->stop)
        return eval(board); // exit if timed out

      if (rec_score > best_current_score) {
        best_current_move = list.moves[i];
        best_current_score = rec_score;
      }
      if (rec_score > alpha)
        alpha = rec_score; // update alpha
      if (beta <= alpha)
        break; // prune the search
    }
  } else {
    for (int i = 0; i < list.count; i++) {
      apply_move(board, &list.moves[i]);
      fast_board_key(board, &list.moves[i]);

      int rec_score =
          dls_search(board, depth - 1, NULL, alpha, beta, ss + 1, info);

      fast_board_key(board, &list.moves[i]);
      undo_move(board, &list.moves[i]);

      if (info->stop)
        return eval(board); // exit if timed out

      if (rec_score < best_current_score) {
        best_current_move = list.moves[i];
        best_current_score = rec_score;
      }
      if (rec_score < beta)
        beta = rec_score; // update beta
      if (beta <= alpha)
        break; // prune the search
    }
  }

  // STEP 9: store the result in transposition table and update root move
  if (!info->stop) {
    Node node_type;
    if (best_current_score <= initial_alpha)
      node_type = UPPER; // upper bound
    else if (best_current_score >= initial_beta)
      node_type = LOWER; // lower bound
    else
      node_type = EXACT; // exact

    store_tt(board->key, depth, best_current_score, best_current_move,
             node_type);
    if (ss->ply == 0)
      *best_move = best_current_move; // update best move only at root
  }

  return best_current_score;
}

void ids_search(Board *board, int max_depth, MoveList start_list, int timeout,
                SearchResult *result) {
  // STEP 1: initialize search information
  SearchInfo info = {0};
  info.start_time = clock();
  info.timeout = timeout;
  info.stop = 0;

  // STEP 2: initialize search stack for each possible ply
  SearchStack ss[64];
  for (int i = 0; i < 64; i++) {
    ss[i].ply = i;
    ss[i].previous_best = (Move){0};
  }

  // STEP 3: reset search result
  ss[0].start_list = &start_list;

  result->best_move = (Move){0};
  result->sol_depth = 0;
  result->nodes = 0;
  result->timeout = 0;

  // STEP 4: iterate through each depth up to max_depth
  for (int depth = 1; depth <= max_depth; depth++) {
    Move curr_move = {0};
    dls_search(board, depth, &curr_move, -INF, INF, ss, &info);

    // check if search was aborted
    if (info.stop) {
      result->timeout = 1;
      break;
    }

    // update result after each completed depth
    result->best_move = curr_move;
    result->sol_depth = depth;
    ss[0].previous_best = curr_move;
    ss[0].previous_best.score = PREVIOUS_BEST_BOOST;
  }

  // STEP 5: update search result
  result->nodes = info.nodes;
  result->elapsed =
      (int)(((double)(clock() - info.start_time) / CLOCKS_PER_SEC) * 1000);
}
