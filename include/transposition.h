#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#include "types.h"

typedef enum { EXACT, LOWER, UPPER } Node;

typedef struct {
    U64 key;
    int depth;
    int score;
    Move best_move;
    Node node_type;
} TTentry;

#endif