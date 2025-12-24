#ifndef FEOHZERO_SEARCH_H
#define FEOHZERO_SEARCH_H

#include "types.h"

/* iterative deepening search (ids) for a given board */
void ids_search(Board *board, int max_depth, MoveList start_list, int timeout,
                SearchResult *result);

#endif
