#include "transposition.h"

#include <stdlib.h>
#include <string.h>

/* transposition table */
static TTentry *tt = NULL;

/* transposition table's size */
static size_t tt_size = 0;

/* bitmask for fast modulo, used as key & tt_mask */
static size_t tt_mask = 0;

/* rounds down to nearest power of 2 */
static size_t round_pow2(size_t n) {
  if (n == 0) return 0;

  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  if (sizeof(size_t) == 8) {
    n |= n >> 32;
  }
  return n - (n >> 1);
}

void init_tt(size_t size_kb) {
  size_t entries = size_kb * 1024 / sizeof(TTentry);
  if (entries == 0) {
    clear_tt();
    return;
  }

  size_t new_size = round_pow2(entries);
  if (tt != NULL && new_size <= tt_size) {
    tt_size = new_size;
    tt_mask = tt_size - 1;
    memset(tt, 0, tt_size * sizeof(TTentry));
    return;
  }

  if (tt != NULL) {
    free(tt);
  }

  tt = calloc(new_size, sizeof(TTentry));
  if (tt == NULL) {
    tt_size = 0;
    tt_mask = 0;
    return;
  }

  tt_size = new_size;
  tt_mask = tt_size - 1;
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
  if (tt != NULL) {
    memset(tt, 0, tt_size * sizeof(TTentry));
  }
}

void free_tt(void) {
  if (tt != NULL) {
    free(tt);
    tt = NULL;
  }
  tt_size = 0;
  tt_mask = 0;
}
