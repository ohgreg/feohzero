#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "config.h"
#include "eval.h"
#include "moves.h"
#include "print.h"
#include "search.h"
#include "transposition.h"
#include "zobrist.h"

int main(int argc, char *argv[]) {
  (void)argc;

  Config config;
  init_config(&config);

  if (!parse_args(argv, &config)) {
    return config.help ? 0 : 1; // exit if help or parse error
  }

  srand(config.seed); // set seed for RNG

  init_moves();   // move generation LUTs
  init_eval();    // evaluation piece-square tables
  init_zobrist(); // zobrist hashing keys

  init_tt(config.tt_size_mb); // allocate transposition table

  Board board;
  board.key = (U64)0;

  if (!load_fen(&board, config.fen)) {
    fprintf(stderr, "Error! Invalid FEN string\n");
    clear_tt();
    return 1;
  }

  if (!config.quiet) {
    print_board(&board);
  }

  MoveList list = {0};
  initial_list(&board, &list, config.moves); // parse or generate starting moves

  if (list.count == 0) {
    fprintf(stderr, "Error! There no valid moves\n");
    clear_tt();
    return 1;
  }

  if (!config.quiet) {
    printf("\nmove list:\n");
    print_move_list(&list);
    printf("\n");
  }

  SearchResult result;
  ids_search(&board, config.depth, list, config.timeout, &result); // run search

  if (!config.quiet) {
    printf("best move: ");
    print_move(&result.best_move);
    double seconds = result.elapsed_time / 1000.0;
    int nps = (seconds > 0) ? (int)(result.nodes / seconds) : 0;
    printf(", depth: %d, nodes: %d, time: %dms%s, nps: %d\n\n",
           result.sol_depth, result.nodes, result.elapsed_time,
           result.timeout ? " (timeout)" : "", nps);
  } else {
    print_move(&result.best_move); // output move only in quiet mode
    printf("\n");
  }

  clear_tt(); // free transposition table memory

  return 0;
}
