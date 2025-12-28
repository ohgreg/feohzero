#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "eval.h"
#include "moves.h"
#include "search.h"
#include "transposition.h"
#include "zobrist.h"

/* global board state */
static Board board;
/* initialization flag */
static int initialized = 0;
/* last search result */
static SearchResult result;
/* buffer for UCI move string */
static char move_buf[8];

/* initializes the WASM engine, given transposition table size */
void wasm_init(int tt_size_kb) {
  if (initialized) {
    init_tt(tt_size_kb);
    return;
  }

  init_moves();
  init_eval();
  init_zobrist();

  initialized = 1;
}

/* clears the transposition table */
void wasm_clear(void) { clear_tt(); }

/* loads a FEN position, returns 1 on success, 0 on failure */
int wasm_load_fen(const char *fen) {
  if (!initialized || !fen)
    return 0;
  return load_fen(&board, fen);
}

/* runs search and returns best move as UCI string */
const char *wasm_search(int depth, int timeout) {
  if (!initialized)
    return "none";

  MoveList list = {0};
  generate_moves(&board, &list);

  if (list.count == 0) {
    free(list.moves);
    return "none";
  }

  ids_search(&board, depth, list, timeout, &result);

  char *uci = move_to_uci(&board, &result.best_move);
  strncpy(move_buf, uci, sizeof(move_buf) - 1);
  move_buf[sizeof(move_buf) - 1] = '\0';

  free(uci);
  free(list.moves);
  return move_buf;
}

/* returns the depth reached in last search */
int wasm_get_depth(void) { return result.sol_depth; }

/* returns the number of nodes searched */
int wasm_get_nodes(void) { return result.nodes; }

/* returns the time elapsed in ms */
int wasm_get_time(void) { return result.elapsed; }

/* returns nodes per second */
int wasm_get_nps(void) {
  return (result.elapsed > 0) ? (int)((result.nodes * 1000ULL) / result.elapsed)
                              : 0;
}

/* returns 1 if last search timed out, 0 otherwise */
int wasm_is_timeout(void) { return result.timeout; }

/* returns static evaluation of current position */
int wasm_eval(void) {
  if (!initialized)
    return 0;
  return eval(&board);
}
