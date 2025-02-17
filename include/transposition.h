#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#include <stddef.h>
#include "types.h"
#include <stdlib.h>

typedef enum { EXACT, LOWER, UPPER } Node;

typedef struct {
    U64 key;
    int depth;
    int score;
    Move best_move;
    Node node_type;
} TTentry;

void init_transposition_table(size_t size);
void tt_store(uint64_t key, int depth, int score, Move best_move, Node node_type);
TTentry *tt_probe(U64 key);
void clear_transposition_table();

#endif