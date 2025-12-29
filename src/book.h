#ifndef FEOHZERO_BOOK_H
#define FEOHZERO_BOOK_H

#include "types.h"

/* binary searches the book_bin array for a move matching the board position */
int probe_book(const Board *board, Move *book_move);

#endif
