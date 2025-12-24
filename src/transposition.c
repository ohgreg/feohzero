#include "transposition.h"

#include <stdlib.h>

/* transposition table */
static TTentry *tt = NULL;

/* transposition table's size */
static size_t tt_size;

void init_tt(size_t size) {
  clear_tt();
  tt_size = size * 1024 * 1024 / sizeof(TTentry);
  if (tt_size == 0)
    return;
  tt = calloc(tt_size, sizeof(TTentry));
}

void store_tt(U64 key, int depth, int score, Move best_move, Node node_type) {
  if (tt_size == 0 || tt == NULL)
    return;

  size_t index = key % tt_size;
  TTentry *entry = &tt[index];

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
  if (tt_size == 0 || tt == NULL)
    return NULL;

  size_t index = key % tt_size;
  TTentry *entry = &tt[index];

  return (entry->key == key) ? entry : NULL;
}

void clear_tt(void) {
  tt_size = 0;
  if (tt == NULL)
    return;

  free(tt);
  tt = NULL;
}
