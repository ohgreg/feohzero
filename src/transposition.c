#include "transposition.h"

#include <stdlib.h>


TTentry *transposition_table;
size_t tt_size;

// init transpo table 
void init_transposition_table(size_t size) {
    tt_size = size;
    transposition_table = calloc(tt_size, sizeof(TTentry));
}

// Function to store entry (move) in TT (replace old entries with newer ones if higher depth)
void tt_store(uint64_t key, int depth, int score, Move best_move, Node node_type) {
    size_t index = key % tt_size;
    TTentry *entry = &transposition_table[index];
    // replace with new info
    if (entry->depth <= depth) {
        entry->key = key;
        entry->depth = depth;
        entry->score = score;
        entry->best_move = best_move;
        entry->node_type = node_type;
    }
}

// Function to probe Transpo table, or in other words check for the existence of a key (chess position) in table
TTentry *tt_probe(U64 key) {
    size_t index = key % tt_size;
    TTentry *entry = &transposition_table[index];
    return (entry->key == key) ? entry : NULL;
}

// free memory 
void clear_transposition_table() {
    free(transposition_table);
}
