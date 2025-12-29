#include "book.h"

#include "book_data.h"

/* a 10-byte array type to represent a single book entry */
typedef U8 BookEntry[10];

/* generates a hash for the current board using the fixed_random table */
static U64 book_hash(const Board *board) {
  U64 key = 0;

  for (int side = 0; side < 2; side++) {
    for (int p = 0; p < 6; p++) {
      U64 bb = board->pieces[side][p];
      while (bb) {
        int sq = __builtin_ctzll(bb);
        int piece_idx = (side * 6) + p;
        key ^= fixed_random[piece_idx * 64 + sq];
        bb &= (bb - 1);
      }
    }
  }

  if (board->castle_white & 1)
    key ^= fixed_random[768];
  if (board->castle_white & 2)
    key ^= fixed_random[769];
  if (board->castle_black & 1)
    key ^= fixed_random[770];
  if (board->castle_black & 2)
    key ^= fixed_random[771];

  if (board->ep_square != 64) {
    key ^= fixed_random[772 + (board->ep_square % 8)];
  }

  if (board->turn == 1)
    key ^= fixed_random[780];

  return key;
}

int probe_book(const Board *board, Move *book_move) {
  if (book_bin_len == 0)
    return 0;

  U64 curr_key = book_hash(board);

  int low = 0, high = book_bin_len / sizeof(BookEntry) - 1;

  while (low <= high) {
    int mid = low + (high - low) / 2;

    const BookEntry *entry =
        (const BookEntry *)&book_bin[mid * sizeof(BookEntry)];

    U64 entry_key = *(U64 *)entry;

    if (entry_key == curr_key) {
      U16 raw = *(U16 *)&(*entry)[8];

      book_move->from = raw & 0x3F;
      book_move->to = (raw >> 6) & 0x3F;
      book_move->promo = (raw >> 12) & 0x7;

      book_move->ep = board->ep_square;
      book_move->castle_white = board->castle_white;
      book_move->castle_black = board->castle_black;
      book_move->score = 30000;
      book_move->captured = 6;
      book_move->flags = 0;

      for (int p = 0; p < 6; p++) {
        if (board->pieces[board->turn][p] & (1ULL << book_move->from))
          book_move->piece = p;
        if (board->pieces[!board->turn][p] & (1ULL << book_move->to))
          book_move->captured = p;
      }
      return 1;
    }

    if (entry_key < curr_key)
      low = mid + 1;
    else
      high = mid - 1;
  }

  return 0;
}
