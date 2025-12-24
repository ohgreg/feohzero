#include "moves.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "constants.h"
#include "types.h"
#include "utils.h"

/* NOTE: the magic bitboard precomputation is from:
 * https://github.com/fogleman/MisterQueen/,
 * the rest of the move generation is crafted from scratch
 * The initial FeohZero project used function pointer tables, in
 * order to make the code less repetitive, but this version is
 * using loop unrolling to enable inlining. */

/* precomputed magic numbers for bishop attacks generation */
static const U64 bishop_magic[64] = {
    0x010a0a1023020080ULL, 0x0050100083024000ULL, 0x8826083200800802ULL,
    0x0102408100002400ULL, 0x0414242008000000ULL, 0x0414242008000000ULL,
    0x0804230108200880ULL, 0x0088840101012000ULL, 0x0400420202041100ULL,
    0x0400420202041100ULL, 0x1100300082084211ULL, 0x0000124081000000ULL,
    0x0405040308000411ULL, 0x01000110089c1008ULL, 0x0030108805101224ULL,
    0x0010808041101000ULL, 0x2410002102020800ULL, 0x0010202004098180ULL,
    0x1104000808001010ULL, 0x274802008a044000ULL, 0x1400884400a00000ULL,
    0x0082000048260804ULL, 0x4004840500882043ULL, 0x0081001040680440ULL,
    0x4282180040080888ULL, 0x0044200002080108ULL, 0x2404c80a04002400ULL,
    0x2020808028020002ULL, 0x0129010050304000ULL, 0x0008020108430092ULL,
    0x005600450c884800ULL, 0x005600450c884800ULL, 0x001004501c200301ULL,
    0xa408025880100100ULL, 0x1042080300060a00ULL, 0x4100a00801110050ULL,
    0x11240100c40c0040ULL, 0x24a0281141188040ULL, 0x08100c4081030880ULL,
    0x020c310201002088ULL, 0x006401884600c280ULL, 0x1204028210809888ULL,
    0x8000a01402005002ULL, 0x041d8a021a000400ULL, 0x041d8a021a000400ULL,
    0x000201a102004102ULL, 0x0408010842041282ULL, 0x000201a102004102ULL,
    0x0804230108200880ULL, 0x0804230108200880ULL, 0x8001010402090010ULL,
    0x0008000042020080ULL, 0x4200012002440000ULL, 0x80084010228880a0ULL,
    0x4244049014052040ULL, 0x0050100083024000ULL, 0x0088840101012000ULL,
    0x0010808041101000ULL, 0x1090c00110511001ULL, 0x2124000208420208ULL,
    0x0800102118030400ULL, 0x0010202120024080ULL, 0x00024a4208221410ULL,
    0x010a0a1023020080ULL};

/* precomputed magic numbers for rook attacks generation */
static const U64 rook_magic[64] = {
    0x0080004000608010ULL, 0x2240100040012002ULL, 0x008008a000841000ULL,
    0x0100204900500004ULL, 0x020008200200100cULL, 0x40800c0080020003ULL,
    0x0080018002000100ULL, 0x4200042040820d04ULL, 0x10208008a8400480ULL,
    0x4064402010024000ULL, 0x2181002000c10212ULL, 0x5101000850002100ULL,
    0x0010800400080081ULL, 0x0012000200300815ULL, 0x060200080e002401ULL,
    0x4282000420944201ULL, 0x1040208000400091ULL, 0x0010004040002008ULL,
    0x0082020020804011ULL, 0x0005420010220208ULL, 0x8010510018010004ULL,
    0x05050100088a1400ULL, 0x0009008080020001ULL, 0x2001060000408c01ULL,
    0x0060400280008024ULL, 0x9810401180200382ULL, 0x0200201200420080ULL,
    0x0280300100210048ULL, 0x0000080080800400ULL, 0x0002010200081004ULL,
    0x8089000900040200ULL, 0x0040008200340047ULL, 0x0400884010800061ULL,
    0xc202401000402000ULL, 0x0800401301002004ULL, 0x4c43502042000a00ULL,
    0x0004a80082800400ULL, 0xd804040080800200ULL, 0x060200080e002401ULL,
    0x0203216082000104ULL, 0x0000804000308000ULL, 0x004008100020a000ULL,
    0x1001208042020012ULL, 0x0400220088420010ULL, 0x8010510018010004ULL,
    0x8009000214010048ULL, 0x6445006200130004ULL, 0x000a008402460003ULL,
    0x0080044014200240ULL, 0x0040012182411500ULL, 0x0003102001430100ULL,
    0x4c43502042000a00ULL, 0x1008000400288080ULL, 0x0806003008040200ULL,
    0x4200020801304400ULL, 0x8100640912804a00ULL, 0x300300a043168001ULL,
    0x0106610218400081ULL, 0x008200c008108022ULL, 0x0201041861017001ULL,
    0x00020010200884e2ULL, 0x0205000e18440001ULL, 0x202008104a08810cULL,
    0x800a208440230402ULL};

/* bit shifts for indexing bishop LUT */
static const int bishop_shift[64] = {
    58, 59, 59, 59, 59, 59, 59, 58, 59, 59, 59, 59, 59, 59, 59, 59,
    59, 59, 57, 57, 57, 57, 59, 59, 59, 59, 57, 55, 55, 57, 59, 59,
    59, 59, 57, 55, 55, 57, 59, 59, 59, 59, 57, 57, 57, 57, 59, 59,
    59, 59, 59, 59, 59, 59, 59, 59, 58, 59, 59, 59, 59, 59, 59, 58};

/* bit shifts for indexing rook LUT */
static const int rook_shift[64] = {
    52, 53, 53, 53, 53, 53, 53, 52, 53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53, 53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53, 53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53, 52, 53, 53, 53, 53, 53, 53, 52};

/* LUT offsets for bishop moves */
static int bishop_offset[64];

/* LUT offsets for rook moves */
static int rook_offset[64];

/* bishop full attack mask */
static U64 bishop_tmask[64];

/* rook full attack mask */
static U64 rook_tmask[64];

/* bishop full attack mask with truncate */
static U64 bishop_mask[64];

/* rook full attack mask with truncate */
static U64 rook_mask[64];

/* LUT for all pieces */
static LUT lut;

/* generates sliding piece full attack mask given a position,
 * with an option for truncate ends */
static U64 slide(U64 occupied, int truncate, int pos, int directions[4][2]) {
  U64 mask = 0;
  int rank = pos / 8;
  int file = pos % 8;

  // iterate through each given movement direction
  for (int i = 0; i < 4; i++) {
    if (directions[i][0] == 0 && directions[i][1] == 0)
      continue; // skip invalid directions
    U64 prev = 0;
    for (int j = 1; j < 9; j++) {
      int r = rank + directions[i][0] * j;
      int f = file + directions[i][1] * j;
      if (r < 0 || f < 0 || r > 7 || f > 7) {
        if (truncate) {
          mask &= ~prev; // remove last occupied square if truncating
        }
        break;
      }
      U64 bit = ((U64)1 << (8 * r + f));
      mask |= bit;
      if (bit & occupied) {
        break; // stop at first occupied square
      }
      prev = bit;
    }
  }

  return mask;
}

/* counts set bits in a bitboard and store their positions */
static int square_count(U64 value, int squares[64]) {
  int i = 0;
  while (value) {
    squares[i++] = pop_lsb(&value);
  }
  return i;
}

void init_moves(void) {
  int bishop_directions[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
  int rook_directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

  // initialize bishop and rook full attack masks
  for (int i = 0; i < 64; i++) {
    bishop_tmask[i] = slide((U64)0, 0, i, bishop_directions);
    bishop_mask[i] = slide((U64)0, 1, i, bishop_directions);
    rook_tmask[i] = slide((U64)0, 0, i, rook_directions);
    rook_mask[i] = slide((U64)0, 1, i, rook_directions);
  }

  int offset;
  int squares[64];

  // bishop LUT
  offset = 0; // reset offset for bishop
  for (int square = 0; square < 64; square++) {
    int count = square_count(bishop_mask[square], squares);
    int n = 1 << count; // number of combinations
    for (int i = 0; i < n; i++) {
      U64 occupied = 0;
      for (int j = 0; j < count; j++) {
        if (i & (1 << j)) {
          enable_bit(&occupied, squares[j]); // set occupied bits
        }
      }
      U64 value =
          slide(occupied, 0, square, bishop_directions); // calculate move
      int index = (occupied * bishop_magic[square]) >>
                  bishop_shift[square];   // index in LUT
      lut.bishop[offset + index] = value; // store index in LUT
    }
    bishop_offset[square] = offset;             // save offset
    offset += 1 << (64 - bishop_shift[square]); // update offset
  }

  // rook LUT
  offset = 0; // reset offset for rook
  for (int square = 0; square < 64; square++) {
    int count = square_count(rook_mask[square], squares);
    int n = 1 << count; // number of combinations
    for (int i = 0; i < n; i++) {
      U64 occupied = 0;
      for (int j = 0; j < count; j++) {
        if (i & (1 << j)) {
          enable_bit(&occupied, squares[j]); // set occupied bits
        }
      }
      U64 value = slide(occupied, 0, square, rook_directions); // calculate move
      int index =
          (occupied * rook_magic[square]) >> rook_shift[square]; // index in LUT
      lut.rook[offset + index] = value; // store move in LUT
    }
    rook_offset[square] = offset;             // save offset
    offset += 1 << (64 - rook_shift[square]); // update offset
  }

  // knight LUT
  for (int square = 0; square < 64; square++) {
    U64 pos = (U64)1 << square;
    U64 moves = 0;

    // calculate all possible knight moves
    moves |= (pos << 17) & ~FILE_A;  // up 2, left 1
    moves |= (pos << 15) & ~FILE_H;  // up 2, right 1
    moves |= (pos << 10) & ~FILE_AB; // up 1, left 2
    moves |= (pos << 6) & ~FILE_GH;  // up 1, right 2
    moves |= (pos >> 17) & ~FILE_H;  // down 2, right 1
    moves |= (pos >> 15) & ~FILE_A;  // down 2, left 1
    moves |= (pos >> 10) & ~FILE_GH; // down 1, right 2
    moves |= (pos >> 6) & ~FILE_AB;  // down 1, left 2

    lut.knight[square] = moves; // store move in LUT
  }

  // king LUT
  for (int square = 0; square < 64; square++) {
    U64 pos = (U64)1 << square;
    U64 moves = 0;

    // calculate all possible king moves
    moves |= (pos << 8);           // up
    moves |= (pos >> 8);           // down
    moves |= (pos << 1) & ~FILE_A; // right
    moves |= (pos >> 1) & ~FILE_H; // left
    moves |= (pos << 9) & ~FILE_A; // up - right
    moves |= (pos << 7) & ~FILE_H; // up - left
    moves |= (pos >> 9) & ~FILE_H; // down - left
    moves |= (pos >> 7) & ~FILE_A; // down - right

    lut.king[square] = moves; // store move in LUT
  }

  // pawn LUTs
  for (int square = 0; square < 64; square++) {
    U64 pos = ((U64)1 << square);

    // captures
    lut.pawn_attack[WHITE][square] =
        ((pos & ~FILE_H) << 9) |
        ((pos & ~FILE_A) << 7); // capture right and left
    lut.pawn_attack[BLACK][square] =
        ((pos & ~FILE_A) >> 9) |
        ((pos & ~FILE_H) >> 7); // capture right and left

    // single pawn push
    lut.pawn_push[WHITE][square] = (pos << 8);
    lut.pawn_push[BLACK][square] = (pos >> 8);

    // double pawn pushes
    lut.pawn_double_push[WHITE][square] = ((pos & RANK_2) << 16);
    lut.pawn_double_push[BLACK][square] = ((pos & RANK_7) >> 16);
  }
}

/* generates a bitboard of squares attacked by pawns at the given position */
static inline U64 generate_pawn_attacks(Board *board, int pos) {
  return lut.pawn_attack[board->turn][pos];
}

/* generates a bitboard of squares attacked by knights at the given position */
static inline U64 generate_knight_attacks(Board *board, int pos) {
  (void)board;
  return lut.knight[pos];
}

/* generates a bitboard of squares attacked by bishops at the given position */
static inline U64 generate_bishop_attacks(Board *board, int pos) {
  return lut
      .bishop[(((board->occupied[2] & bishop_mask[pos]) * bishop_magic[pos]) >>
               bishop_shift[pos]) +
              bishop_offset[pos]];
}

/* generates a bitboard of squares attacked by rooks at the given position */
static inline U64 generate_rook_attacks(Board *board, int pos) {
  return lut.rook[(((board->occupied[2] & rook_mask[pos]) * rook_magic[pos]) >>
                   rook_shift[pos]) +
                  rook_offset[pos]];
}

/* generates a bitboard of squares attacked by queen at the given position */
static inline U64 generate_queen_attacks(Board *board, int pos) {
  return lut.bishop[(((board->occupied[2] & bishop_mask[pos]) *
                      bishop_magic[pos]) >>
                     bishop_shift[pos]) +
                    bishop_offset[pos]] |
         lut.rook[(((board->occupied[2] & rook_mask[pos]) * rook_magic[pos]) >>
                   rook_shift[pos]) +
                  rook_offset[pos]];
}

/* generates a bitboard of squares attacked by king at the given position */
static inline U64 generate_king_attacks(Board *board, int pos) {
  (void)board;
  return lut.king[pos];
}

/* array of function pointers to generate attacks for each piece type */
static inline U64 generate_piece_attacks(PieceType piece, Board *board,
                                         int pos) {
  switch (piece) {
  case PAWN:
    return generate_pawn_attacks(board, pos);
  case KNIGHT:
    return generate_knight_attacks(board, pos);
  case BISHOP:
    return generate_bishop_attacks(board, pos);
  case ROOK:
    return generate_rook_attacks(board, pos);
  case QUEEN:
    return generate_queen_attacks(board, pos);
  case KING:
    return generate_king_attacks(board, pos);
  case NONE:
    return 0;
  }
}

/* generates all attack squares controlled by the opponent,
 * after removing the king */
static inline U64 generate_opponent_attacks(Board *board) {
  Turn turn = board->turn;
  int king_pos = lsb(board->pieces[turn][KING]);
  U64 res = 0;

  // create a temporary board without the king
  Board temp = *board;
  temp.turn = !turn;
  temp.pieces[turn][KING] = 0;
  clear_bit(&temp.occupied[turn], king_pos);
  clear_bit(&temp.occupied[2], king_pos);

  // generate attacks for all opponent pieces
  U64 pieces = temp.pieces[!turn][PAWN];
  while (pieces)
    res |= generate_pawn_attacks(&temp, pop_lsb(&pieces));

  pieces = temp.pieces[!turn][KNIGHT];
  while (pieces)
    res |= generate_knight_attacks(&temp, pop_lsb(&pieces));

  pieces = temp.pieces[!turn][BISHOP];
  while (pieces)
    res |= generate_bishop_attacks(&temp, pop_lsb(&pieces));

  pieces = temp.pieces[!turn][ROOK];
  while (pieces)
    res |= generate_rook_attacks(&temp, pop_lsb(&pieces));

  pieces = temp.pieces[!turn][QUEEN];
  while (pieces)
    res |= generate_queen_attacks(&temp, pop_lsb(&pieces));

  pieces = temp.pieces[!turn][KING];
  while (pieces)
    res |= generate_king_attacks(&temp, pop_lsb(&pieces));

  return res;
}

/* generates a bitboard of pieces giving check to the king */
static inline U64 generate_checkers(Board *board) {
  Turn turn = board->turn;
  int king_pos = lsb(board->pieces[turn][KING]);
  U64 res = 0;
  Turn opp = !turn;

  // check for attacking opponent pieces
  // NOTE: all check moves are symmetrical in respect to the king
  res |= generate_pawn_attacks(board, king_pos) & board->pieces[opp][PAWN];

  res |= generate_knight_attacks(board, king_pos) & board->pieces[opp][KNIGHT];

  res |= generate_bishop_attacks(board, king_pos) & board->pieces[opp][BISHOP];

  res |= generate_rook_attacks(board, king_pos) & board->pieces[opp][ROOK];

  res |= generate_queen_attacks(board, king_pos) & board->pieces[opp][QUEEN];

  return res;
}

int is_king_in_check(Board *board) { return generate_checkers(board) != 0; }

/* generates a bitboard of squares that can block a check,
 * given the position of the checker */
static inline U64 generate_blockmask(Board *board, int pos) {
  Turn turn = board->turn;
  int king_pos = lsb(board->pieces[turn][KING]);
  U64 res = 0;

  enable_bit(&res, pos);

  // check bishop sliding mask
  if (is_set_bit(bishop_tmask[pos], king_pos)) {
    res |= generate_bishop_attacks(board, pos) &
           generate_bishop_attacks(board, king_pos);
  }

  // check rook sliding mask
  if (is_set_bit(rook_tmask[pos], king_pos)) {
    res |= generate_rook_attacks(board, pos) &
           generate_rook_attacks(board, king_pos);
  }

  clear_bit(&res, king_pos);

  return res;
}

/* generates a bitboard of pieces that are pinned by the opponent's
 * sliding pieces */
static inline U64 generate_pinned(Board *board) {
  Turn turn = board->turn;
  int king_pos = lsb(board->pieces[turn][KING]);
  U64 res = 0;

  // check for diagonal pinners
  U64 pinners = board->pieces[!turn][BISHOP] | board->pieces[!turn][QUEEN];
  while (pinners) {
    int pinner_pos = pop_lsb(&pinners);
    if (is_set_bit(bishop_tmask[pinner_pos], king_pos)) {
      U64 between = generate_bishop_attacks(board, king_pos) &
                    generate_bishop_attacks(board, pinner_pos);
      if (pop_count(between) && (between & board->occupied[turn])) {
        enable_bit(&res, lsb(between));
      }
    }
  }

  // check for orthogonal pinners
  pinners = board->pieces[!turn][ROOK] | board->pieces[!turn][QUEEN];
  while (pinners) {
    int pinner_pos = pop_lsb(&pinners);
    if (is_set_bit(rook_tmask[pinner_pos], king_pos)) {
      U64 between = generate_rook_attacks(board, king_pos) &
                    generate_rook_attacks(board, pinner_pos);
      if (pop_count(between) && (between & board->occupied[turn])) {
        enable_bit(&res, lsb(between));
      }
    }
  }

  return res;
}

/* generates a bitboard mask for the squares that a pinned piece
 * can move to, given its position */
static inline U64 generate_pinmask(Board *board, int pos) {
  Turn turn = board->turn;
  int king_pos = lsb(board->pieces[turn][KING]);
  U64 res = 0;

  // check diagonal rays
  U64 slide = bishop_tmask[pos];
  if (is_set_bit(slide, king_pos)) {
    res = slide & bishop_tmask[king_pos] & generate_bishop_attacks(board, pos);

    clear_bit(&res, king_pos);
    return res;
  }

  // check orthogonal rays
  slide = rook_tmask[pos];
  if (is_set_bit(slide, king_pos)) {
    res = slide & rook_tmask[king_pos] & generate_rook_attacks(board, pos);

    clear_bit(&res, king_pos);
    return res;
  }

  return res;
}

/* generates all possible moves for a pawn at a given position */
static inline U64 generate_pawn_moves(Board *board, int pos) {
  Turn turn = board->turn;
  U64 empty = ~board->occupied[2];

  U64 res = 0;

  // capture moves
  res = lut.pawn_attack[turn][pos] & board->occupied[!turn];

  // single push
  U64 single = lut.pawn_push[turn][pos] & empty;

  // add single push, check for double
  res |= single |
         (lut.pawn_double_push[turn][pos] & empty & ((single) ? ~(U64)0 : 0));

  return res;
}

void generate_moves(Board *board, MoveList *list) {
  Turn turn = board->turn;
  U64 moves = 0, pieces = 0;
  int from = 0, to = 0;
  Move move;

  // STEP 1: generate king moves
  // NOTE: the king can move to free squares that are not controlled by the
  // opponent
  U64 opponent_attacks = generate_opponent_attacks(board);
  from = lsb(board->pieces[turn][KING]);
  moves = generate_piece_attacks(KING, board, from) & ~board->occupied[turn] &
          ~opponent_attacks;
  while (moves) {
    to = pop_lsb(&moves);
    move = (Move){from,
                  to,
                  0,
                  KING,
                  NONE,
                  NORMAL_MOVE,
                  board->ep_square,
                  board->castle_white,
                  board->castle_black,
                  0};
    list->moves[list->count++] = move;
  }

  // STEP 2: determine valid moves based on check scenarios
  // NOTE: if there is one piece checking the king, then moves are filtered to
  // prevent the check if there are multiple such pieces, then only the king
  // might be moved
  U64 valid = ~(U64)0;
  U64 checkers = generate_checkers(board);
  if (checkers != 0) {
    if (pop_count(checkers) == 1) {
      valid = generate_blockmask(
          board, lsb(checkers)); // block mask for single checker
    } else if (pop_count(checkers) > 1) {
      return; // return if multiple checkers
    }
  }

  // STEP 3: generate moves for knight, bishop, rook, and queen
  // NOTE: Move generation follows a filtering pipeline: raw attacks are
  // retrieved from LUTs, then bitwise ANDed with masks to exclude friendly
  // pieces, satisfy check-evasion requirements, and respect absolute pins
  U64 pinned = generate_pinned(board);

  // knights
  pieces = board->pieces[turn][KNIGHT];
  while (pieces) {
    from = pop_lsb(&pieces);
    moves =
        generate_knight_attacks(board, from) & ~board->occupied[turn] & valid;
    if (is_set_bit(pinned, from))
      moves &= generate_pinmask(board, from);
    while (moves) {
      to = pop_lsb(&moves);
      move = (Move){from,
                    to,
                    0,
                    KNIGHT,
                    NONE,
                    NORMAL_MOVE,
                    board->ep_square,
                    board->castle_white,
                    board->castle_black,
                    0};
      if (is_set_bit(board->occupied[!turn], to)) {
        move.flags |= CAPTURE_MOVE;
        move.score += 1000 - 10 * KNIGHT;
      }
      list->moves[list->count++] = move;
    }
  }

  // bishops
  pieces = board->pieces[turn][BISHOP];
  while (pieces) {
    from = pop_lsb(&pieces);
    moves =
        generate_bishop_attacks(board, from) & ~board->occupied[turn] & valid;
    if (is_set_bit(pinned, from))
      moves &= generate_pinmask(board, from);
    while (moves) {
      to = pop_lsb(&moves);
      move = (Move){from,
                    to,
                    0,
                    BISHOP,
                    NONE,
                    NORMAL_MOVE,
                    board->ep_square,
                    board->castle_white,
                    board->castle_black,
                    0};
      if (is_set_bit(board->occupied[!turn], to)) {
        move.flags |= CAPTURE_MOVE;
        move.score += 1000 - 10 * BISHOP;
      }
      list->moves[list->count++] = move;
    }
  }

  // rooks
  pieces = board->pieces[turn][ROOK];
  while (pieces) {
    from = pop_lsb(&pieces);
    moves = generate_rook_attacks(board, from) & ~board->occupied[turn] & valid;
    if (is_set_bit(pinned, from))
      moves &= generate_pinmask(board, from);
    while (moves) {
      to = pop_lsb(&moves);
      move = (Move){from,
                    to,
                    0,
                    ROOK,
                    NONE,
                    NORMAL_MOVE,
                    board->ep_square,
                    board->castle_white,
                    board->castle_black,
                    0};
      if (is_set_bit(board->occupied[!turn], to)) {
        move.flags |= CAPTURE_MOVE;
        move.score += 1000 - 10 * ROOK;
      }
      list->moves[list->count++] = move;
    }
  }

  // queens
  pieces = board->pieces[turn][QUEEN];
  while (pieces) {
    from = pop_lsb(&pieces);
    moves =
        generate_queen_attacks(board, from) & ~board->occupied[turn] & valid;
    if (is_set_bit(pinned, from))
      moves &= generate_pinmask(board, from);
    while (moves) {
      to = pop_lsb(&moves);
      move = (Move){from,
                    to,
                    0,
                    QUEEN,
                    NONE,
                    NORMAL_MOVE,
                    board->ep_square,
                    board->castle_white,
                    board->castle_black,
                    0};
      if (is_set_bit(board->occupied[!turn], to)) {
        move.flags |= CAPTURE_MOVE;
        move.score += 1000 - 10 * QUEEN;
      }
      list->moves[list->count++] = move;
    }
  }

  // STEP 4: generate moves for pawns
  // NOTE: the code repetitiveness in handling pawn moves separately is
  // intentional for optimization purposes
  pieces = board->pieces[turn][PAWN]; // get pawns
  while (pieces) {
    from = pop_lsb(&pieces);
    moves = generate_pawn_moves(board, from); // potential moves

    // handle pinned pawns
    if (is_set_bit(pinned, from)) {
      moves &= generate_pinmask(board, from); // restrict moves
    }

    // apply valid mask
    moves &= valid;
    while (moves) {
      to = pop_lsb(&moves);
      move = (Move){from,
                    to,
                    0,
                    PAWN,
                    NONE,
                    NORMAL_MOVE,
                    board->ep_square,
                    board->castle_white,
                    board->castle_black,
                    0};

      // check for capture
      if (is_set_bit(board->occupied[!turn], to)) {
        move.flags |= CAPTURE_MOVE;           // set capture flag
        move.score += 1000 - 10 * move.piece; // adjust score
      }

      // check for double pawn push
      if (to - from == (turn ? -16 : 16))
        move.flags |= DOUBLE_PAWN_PUSH;

      // check for promotion
      if (is_set_bit(turn ? RANK_1 : RANK_8, to)) {
        // check all promotions
        for (int promo = QUEEN; promo >= KNIGHT; promo--) {
          move.promo = promo;         // set promotion type
          move.flags |= PROMOTION;    // set promotion flag
          move.score += promo * 1000; // adjust score
          list->moves[list->count++] = move;
        }
        continue; // skip to next pawn
      }

      list->moves[list->count++] = move;
    }
  }

  // STEP 5: generate castling moves
  if (checkers == 0) { // return if the king is in check
    CastleRights rights = turn ? board->castle_black : board->castle_white;
    if (rights != 0) { // return if there are no castle rights
      U64 occupied = board->occupied[2] & ~board->pieces[turn][KING];

      // check for kingside castling
      if ((rights & CAN_CASTLE_OO) &&
          !((occupied | opponent_attacks) & (turn ? BSHORT : WSHORT))) {
        list->moves[list->count++] = (Move){turn ? 60 : 4,
                                            turn ? 62 : 6,
                                            0,
                                            KING,
                                            NONE,
                                            CASTLING,
                                            board->ep_square,
                                            board->castle_white,
                                            board->castle_black,
                                            2100};
      }

      // check for queenside castling
      if ((rights & CAN_CASTLE_OOO) &&
          !(occupied & (turn ? BLONG_OCCUPIED : WLONG_OCCUPIED)) &&
          !(opponent_attacks & (turn ? BLONG_ATTACKED : WLONG_ATTACKED))) {
        list->moves[list->count++] = (Move){turn ? 60 : 4,
                                            turn ? 58 : 2,
                                            0,
                                            KING,
                                            NONE,
                                            CASTLING,
                                            board->ep_square,
                                            board->castle_white,
                                            board->castle_black,
                                            2000};
      }
    }
  }

  // STEP 6: generate en passant moves
  if (board->ep_square == 64)
    return; // return if there is no en passant square

  U8 ep = board->ep_square;

  // determine valid en passant moves based on pawn positions
  moves = lut.pawn_attack[!turn][ep] & board->pieces[turn][PAWN];

  // iterate over all possible en passant moves
  while (moves) {
    Move move = {pop_lsb(&moves),
                 ep,
                 0,
                 PAWN,
                 NONE,
                 EN_PASSANT,
                 ep,
                 board->castle_white,
                 board->castle_black,
                 -1000};

    // create a temporary board state and apply the en passant move
    Board temp = *board;
    apply_move(&temp, &move);
    temp.turn = turn;

    // check if the move leaves the king in check
    if (generate_checkers(&temp) != 0)
      continue;

    list->moves[list->count++] = move;
  }
}

/* translates a SAN move string to a Move structure,
 * setting only from, to, and promo fields */
static Move translate_move(Board *board, const char *move_str) {
  // STEP 1: initialize variables
  int size = strlen(move_str);
  Turn turn = board->turn;
  int start_rank = -1;
  int start_file = -1;
  int final_rank = -1;
  int final_file = -1;
  int castle = 0;

  Move move = {0};
  move.piece = PAWN;
  move.promo = NONE;

  // STEP 2: iterate through each character of the move string
  for (int i = 0; i < size; i++) {
    switch (move_str[i]) {
    // set piece type
    case 'N':
      move.piece = KNIGHT;
      break;
    case 'B':
      move.piece = BISHOP;
      break;
    case 'R':
      move.piece = ROOK;
      break;
    case 'Q':
      move.piece = QUEEN;
      break;
    case 'K':
      move.piece = KING;
      break;
    case 'x':
      /* capture indicator - ignore for now */
      break;
    // handle promotion
    case '=': {
      char c = '0';
      if (i + 1 <= size) {
        c = move_str[i + 1];
        i++;
      }

      // set promotion piece based on character
      if (c == 'N') {
        move.promo = KNIGHT;
      } else if (c == 'B') {
        move.promo = BISHOP;
      } else if (c == 'R') {
        move.promo = ROOK;
      } else if (c == 'Q') {
        move.promo = QUEEN;
      }
      break;
    }
    // handle check
    case '+':
    case '#':
      /* check/checkmate indicators - ignore */
      break;
    case 'O':
      castle++;
      break;
    default: {
      char c = move_str[i];
      if ('1' <= c && c <= '8') {
        start_rank = final_rank;
        final_rank = c - '1';
      } else if ('a' <= c && c <= 'h') {
        start_file = final_file;
        final_file = c - 'a';
      }
    }
    }
  }

  // STEP 3: handle castling moves
  if (castle == 2) {
    move.from = turn ? 60 : 4;
    move.to = turn ? 62 : 6;
    return move;
  } else if (castle == 3) {
    move.from = turn ? 60 : 4;
    move.to = turn ? 58 : 2;
    return move;
  }

  // STEP 4: set the target square for the move
  move.to = 8 * final_rank + final_file;

  // STEP 5: Create a mask for the starting square
  U64 start_mask = ~(U64)0;
  if (start_rank != -1)
    start_mask &= RANK_1 << (8 * start_rank);
  if (start_file != -1)
    start_mask &= FILE_A << start_file;

  // STEP 6: handle en passant for pawns
  if (move.to == board->ep_square && move.piece == PAWN) {
    move.from = lsb(lut.pawn_attack[!turn][move.to] &
                    board->pieces[turn][PAWN] & start_mask);
    return move;
  }

  // STEP 7: handle pawn moves
  if (move.piece == PAWN) {
    U64 pieces = board->pieces[turn][move.piece] & start_mask;
    while (pieces) {
      int piece_pos = pop_lsb(&pieces);
      U64 moves = generate_pawn_moves(board, piece_pos);
      if (is_set_bit(moves, move.to)) {
        move.from = piece_pos; // set source position for pawn
        break;
      }
    }
    return move;
  }

  // STEP 8: handle moves for all the other pieces
  move.from = lsb(generate_piece_attacks(move.piece, board, move.to) &
                  board->pieces[turn][move.piece] & start_mask);

  return move;
}

void initial_list(Board *board, MoveList *list, const char *moves_str) {
  list->count = 0;

  // generate all legal moves
  MoveList legal_moves;
  legal_moves.count = 0;
  generate_moves(board, &legal_moves);

  // allocate memory for temp string
  char *temp = malloc(strlen(moves_str) + 1);
  if (temp == NULL) {
    return; // return empty list in case of allocation fail
  }

  strcpy(temp, moves_str);

  // go through each move
  char *token = strtok(temp, " "); // tokenize moves
  while (token != NULL) {
    Move parsed = translate_move(board, token);

    // find a matching legal move
    for (int i = 0; i < legal_moves.count; i++) {
      Move *legal = &legal_moves.moves[i];
      if (legal->from == parsed.from && legal->to == parsed.to) {
        list->moves[list->count++] = *legal;
        break;
      }
    }

    token = strtok(NULL, " ");
  }

  free(temp);
}
