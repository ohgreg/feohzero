#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "book.h"
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
  init_zobrist();     // zobrist hashing keys

  Board board;
  board.key = (U64)0;

  if (!load_fen(&board, config.fen)) {
    fprintf(stderr, "Error! Invalid FEN string\n");
    return 1;
  }

  if (!config.quiet) {
    print_board(&board);
    printf("\n");
  }

  Move book_move;
  if (probe_book(&board, &book_move)) {
    printf("book move found: ");
    print_move(&board, &book_move, move_to_uci);
    printf("\n");
    return 0;
  }

  init_moves(); // move generation LUTs

  // parse or generate starting moves
  MoveList list = {0};
  list.moves = NULL;
  initial_list(&board, &list, config.moves, config.str_to_move);

  if (list.count == 0) {
    fprintf(stderr, "Error! There are no valid moves\n");
    free(list.moves);
    return 1;
  }

  if (!config.quiet) {
    printf("board position:\n%s\n\n", config.fen);
    print_move_list(&board, &list, config.move_to_str);
    printf("\n");
  }

  init_eval();                // evaluation piece-square tables
  init_tt(config.tt_size_kb); // allocate transposition table

  SearchResult result;
  ids_search(&board, config.depth, list, config.timeout, &result); // run search

  if (!config.quiet) {
    printf("best move: ");
    print_move(&board, &result.best_move, config.move_to_str);
    int nps = (result.elapsed > 0)
                  ? (int)((result.nodes * 1000ULL) / result.elapsed)
                  : 0;
    printf(", depth: %d, nodes: %d, time: %dms%s, nps: %d\n\n",
           result.sol_depth, result.nodes, result.elapsed,
           result.timeout ? " (timeout)" : "", nps);
  } else {
    // output move only in quiet mode
    print_move(&board, &result.best_move, config.move_to_str);
    printf("\n");
  }

  free(list.moves);
  free_tt(); // free transposition table memory

  return 0;
}
