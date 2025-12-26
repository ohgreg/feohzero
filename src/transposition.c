#include "transposition.h"

#include <stdlib.h>

/* transposition table */
static TTentry *tt = NULL;

/* transposition table's size */
static size_t tt_size;

/* bitmask for fast modulo, used as key & tt_mask */
static size_t tt_mask;

void init_tt(size_t size) {
  clear_tt();
  size_t entries = size * 1024 * 1024 / sizeof(TTentry);
  if (entries == 0)
    return;

  // round to power of 2
  size_t tt_size = entries;
  tt_size |= tt_size >> 1;
  tt_size |= tt_size >> 2;
  tt_size |= tt_size >> 4;
  tt_size |= tt_size >> 8;
  tt_size |= tt_size >> 16;

  if (sizeof(size_t) == 8) {
    tt_size |= tt_size >> 32;
  }

  tt_size = tt_size - (tt_size >> 1);

  tt = calloc(tt_size, sizeof(TTentry));
}

void store_tt(U64 key, int depth, int score, Move best_move, Node node_type) {
  if (tt_size == 0 || tt == NULL)
    return;

  size_t index = key & tt_mask;
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

  size_t index = key & tt_mask;
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
