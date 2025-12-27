#ifndef FEOHZERO_TRANSPOSITION_H
#define FEOHZERO_TRANSPOSITION_H

#include <stddef.h>

#include "types.h"

/* initializes transposition table (size in kb) */
void init_tt(size_t size_kb);

/* stores an entry in the transposition table, replaces old entries if needed */
void store_tt(U64 key, int depth, int score, Move best_move, Node node_type);

/* probes the transposition table for a key */
TTentry *probe_tt(U64 key);

/* clears all entries in the transposition table */
void clear_tt(void);

/* frees the transposition table memory */
void free_tt(void);

#endif
