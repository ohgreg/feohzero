#include "transposition.h"

#include <stdlib.h>

/* transposition table */
static TTentry *transposition_table = NULL;

/* transposition table's size */
static size_t tt_size;

void init_tt(size_t size) {
  if (transposition_table != NULL) {
    clear_tt();
  }
  tt_size = size;
  transposition_table = malloc(tt_size * sizeof(TTentry));
}

void store_tt(U64 key, int depth, int score, Move best_move, Node node_type) {
  if (tt_size == 0 || transposition_table == NULL)
    return;

  size_t index = key % tt_size;
  if (index >= tt_size)
    return;

  TTentry *entry = &transposition_table[index];

  // update entry if new depth is greater
  if (entry->depth <= depth) {
    entry->key = key;
    entry->depth = depth;
    entry->score = score;
    entry->best_move = best_move;
    entry->node_type = node_type;
  }
}

TTentry *probe_tt(U64 key) {
  size_t index = key % tt_size;
  TTentry *entry = &transposition_table[index];
  return (entry->key == key) ? entry : NULL;
}

void clear_tt(void) { free(transposition_table); }
