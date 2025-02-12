#include "moves.h"
#include "board.h"
#include <stdio.h>
#include <string.h> 

const U64 bishop_magic[64] = {
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

const U64 rook_magic[64] = {
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

const int bishop_shift[64] = {
    58, 59, 59, 59, 59, 59, 59, 58, 59, 59, 59, 59, 59, 59, 59, 59,
    59, 59, 57, 57, 57, 57, 59, 59, 59, 59, 57, 55, 55, 57, 59, 59,
    59, 59, 57, 55, 55, 57, 59, 59, 59, 59, 57, 57, 57, 57, 59, 59,
    59, 59, 59, 59, 59, 59, 59, 59, 58, 59, 59, 59, 59, 59, 59, 58};

const int rook_shift[64] = {
    52, 53, 53, 53, 53, 53, 53, 52, 53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53, 53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53, 53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53, 52, 53, 53, 53, 53, 53, 53, 52,
};

int bishop_offset[64];
int rook_offset[64];

U64 bishop_mask[64];
U64 rook_mask[64];

LUT lut;

U64 slide(U64 occupied, int truncate, int pos, int directions[4][2]) {
    U64 mask = 0;
    int rank = pos / 8;
    int file = pos % 8;
    for (int i = 0; i < 4; i++) {
        U64 prev = 0;
        for (int j = 1; j < 9; j++) {
            int r = rank + directions[i][0] * j;
            int f = file + directions[i][1] * j;
            if (r < 0 || f < 0 || r > 7 || f > 7) {
                if (truncate) {
                    mask &= ~prev;
                }
                break;
            }
            U64 bit = ((U64)1 << (8 * r + f));
            mask |= bit;
            if (bit & occupied) {
                break;
            }
            prev = bit;
        }
    }
    return mask;
}

int square_cnt(U64 value, int squares[64]) {
    int i = 0;
    while (value) {
        squares[i++] = pop_lsb(&value);
    }
    return i;
}

void init_knight_LUT(void) {
    for (int square = 0; square < 64; square++) {
        uint64_t pos = (U64)1 << square;
        uint64_t moves = 0;

        // calculate all possible knight moves
        moves |= (pos << 17) & ~FILE_A;  // up 2, left 1
        moves |= (pos << 15) & ~FILE_H;  // up 2, right 1
        moves |= (pos << 10) & ~FILE_AB; // up 1, left 2
        moves |= (pos << 6) & ~FILE_GH;  // up 1, right 2
        moves |= (pos >> 17) & ~FILE_H;  // down 2, right 1
        moves |= (pos >> 15) & ~FILE_A;  // down 2, left 1
        moves |= (pos >> 10) & ~FILE_GH; // down 1, right 2
        moves |= (pos >> 6) & ~FILE_AB;  // down 1, left 2

        lut.knight[square] = moves;
    }
}

void init_king_LUT(void) {
    for (int square = 0; square < 64; square++) {
        uint64_t pos = (U64)1 << square;
        uint64_t moves = 0;

        // Calculate all possible king moves
        moves |= (pos << 8);           // up
        moves |= (pos >> 8);           // down
        moves |= (pos << 1) & ~FILE_A; // right
        moves |= (pos >> 1) & ~FILE_H; // left
        moves |= (pos << 9) & ~FILE_A; // up - right
        moves |= (pos << 7) & ~FILE_H; // up - left
        moves |= (pos >> 9) & ~FILE_H; // down - left
        moves |= (pos >> 7) & ~FILE_A; // down - right

        lut.king[square] = moves;
    }
}

int init_LUT(void) {
    int bishop_directions[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
    int rook_directions[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    for (int i = 0; i < 64; i++) {
        bishop_mask[i] = slide((U64)0, 1, i, bishop_directions);
        rook_mask[i] = slide((U64)0, 1, i, rook_directions);
    }

    int offset;
    int squares[64];

    // bishop LUT
    offset = 0;
    for (int square = 0; square < 64; square++) {
        int count = square_cnt(bishop_mask[square], squares);
        int n = 1 << count;
        for (int i = 0; i < n; i++) {
            U64 occupied = 0;
            for (int j = 0; j < count; j++) {
                if (i & (1 << j)) {
                    occupied |= ((U64)1 << squares[j]);
                }
            }
            U64 value = slide(occupied, 0, square, bishop_directions);
            int index =
                (occupied * bishop_magic[square]) >> bishop_shift[square];
            U64 prev = lut.bishop[offset + index];
            if (prev && prev != value) {
                printf("Invalid bishop LUT\n");
            }
            lut.bishop[offset + index] = value;
        }
        bishop_offset[square] = offset;
        offset += 1 << (64 - bishop_shift[square]);
    }

    // rook LUT
    offset = 0;
    for (int square = 0; square < 64; square++) {
        int count = square_cnt(rook_mask[square], squares);
        int n = 1 << count;
        for (int i = 0; i < n; i++) {
            U64 occupied = 0;
            for (int j = 0; j < count; j++) {
                if (i & (1 << j)) {
                    occupied |= ((U64)1 << squares[j]);
                }
            }
            U64 value = slide(occupied, 0, square, rook_directions);
            int index = (occupied * rook_magic[square]) >> rook_shift[square];
            U64 prev = lut.rook[offset + index];
            if (prev && prev != value) {
                printf("Invalid rook LUT\n");
            }
            lut.rook[offset + index] = value;
        }
        rook_offset[square] = offset;
        offset += 1 << (64 - rook_shift[square]);
    }

    init_king_LUT();
    init_knight_LUT();

    return 0;
}

U64 generate_pawn_moves(Board *board, int pos) {
    U64 moves = 0;
    int turn = board->turn; // 0 for white, 1 for black
    int dir = turn ? -1 : 1;

    // capture diagonal left
    if (is_set_bit(~(turn ? FILE_A : FILE_H) & board->occupied[!turn],
                   pos + 7 * dir)) {
        enable_bit(&moves, pos + 7 * dir);
    }

    // capture diagonal right
    if (is_set_bit(~(turn ? FILE_H : FILE_A) & board->occupied[!turn],
                   pos + 9 * dir)) {
        enable_bit(&moves, pos + 9 * dir);
    }

    // move 1 forward
    if (!is_set_bit(board->occupied[2], pos + 8 * dir)) {
        enable_bit(&moves, pos + 8 * dir);
        // move 2 forward
        if (is_set_bit(turn ? RANK_7 : RANK_2, pos) &&
            !is_set_bit(board->occupied[2], pos + 16 * dir)) {
            enable_bit(&moves, pos + 16 * dir);
        }
    }

    return moves;
}

U64 generate_bishop_moves(Board *board, int pos) {
    return lut.bishop[(((board->occupied[2] & bishop_mask[pos]) *
                        bishop_magic[pos]) >>
                       bishop_shift[pos]) +
                      bishop_offset[pos]];
}

U64 generate_rook_moves(Board *board, int pos) {
    return lut
        .rook[(((board->occupied[2] & rook_mask[pos]) * rook_magic[pos]) >>
               rook_shift[pos]) +
              rook_offset[pos]];
}

U64 generate_queen_moves(Board *board, int pos) {
    return generate_bishop_moves(board, pos) | generate_rook_moves(board, pos);
}

U64 generate_piece_moves(Board *board, int piece, int pos) {
    U64 moves;
    switch (piece) {
    case PAWN:
        return generate_pawn_moves(board, pos);
    case KNIGHT:
        moves = lut.knight[pos];
        break;
    case BISHOP:
        moves = generate_bishop_moves(board, pos);
        break;
    case ROOK:
        moves = generate_rook_moves(board, pos);
        break;
    case QUEEN:
        moves = generate_queen_moves(board, pos);
        break;
    case KING:
        moves = lut.king[pos];
        break;
    default:
        return 0;
    }

    return moves & ~board->occupied[board->turn];
}

U64 attacked_squares(Board *board) {
    U64 moves = 0;
    for (int piece = 0; piece < 6; piece++) {
        U64 pieces = board->pieces[board->turn][piece];
        while (pieces) {
            moves |= generate_piece_moves(board, piece, pop_lsb(&pieces));
        }
    }
    return moves;
}

void generate_castling_moves(MoveList *list, Board *board) {
    int turn = board->turn;

    CastleRights rights = turn ? board->castle_black : board->castle_white;

    if (rights == 0) return;

    Board temp = *board;
    temp.turn = !temp.turn;
    U64 attacked = attacked_squares(&temp);
    U64 occupied = board->occupied[2] & ~board->pieces[turn][KING];

    if ((rights & CAN_CASTLE_OO) &&
        !((occupied | attacked) & (turn ? BSHORT : WSHORT))) {
        list->moves[list->count++] = (Move){
            turn ? 60 : 4, turn ? 62 : 6, 0, KING, NONE, CASTLING, board->ep_square, board->castle_white, board->castle_black, 2100};
    }
    if ((rights & CAN_CASTLE_OOO) &&
        !(occupied & (turn ? BLONG_OCCUPIED : WLONG_OCCUPIED)) &&
        !(attacked & (turn ? BLONG_ATTACKED : WLONG_ATTACKED))) {
        list->moves[list->count++] = (Move){
            turn ? 60 : 4, turn ? 58 : 2, 0, KING, NONE, CASTLING, board->ep_square, board->castle_white, board->castle_black, 2000};
    }
}

void generate_en_passant(MoveList *list, Board *board) {
    if (board->ep_square == 64) return;
    Turn turn = board->turn;
    U8 ep = board->ep_square;
    int dir = (turn ? 1 : -1);

    Move move = {0, ep, 0, PAWN, NONE, CAPTURE_MOVE | EN_PASSANT, ep, board->castle_white, board->castle_black, -1000};

    // en passant right capture
    if(is_set_bit(~(turn ? FILE_A : FILE_H) & board->pieces[turn][PAWN], ep + 9 * dir)){
        move.from = ep + 9 * dir;
        if (!is_illegal_move(board, &move)) list->moves[list->count++] = move;
    }

    // en passant left capture
    if(is_set_bit(~(turn ? FILE_H : FILE_A) & board->pieces[turn][PAWN], ep + 7 * dir)){
        move.from = ep + 7 * dir;
        if (!is_illegal_move(board, &move)) list->moves[list->count++] = move;
    }
}

int king_in_check(Board *board) {
    // find the king
    int king_pos = lsb(board->pieces[!board->turn][KING]);

    // check if the king is attacked
    return is_set_bit(attacked_squares(board), king_pos);
}

int is_illegal_move(Board *board, Move *move) {
    Board temp = *board;
    apply_move(&temp, move);
    return king_in_check(&temp);
}

void generate_moves(MoveList *list, Board *board) {
    Turn turn = board->turn;

    for (int piece = 0; piece < 6; piece++) {
        U64 pieces = board->pieces[turn][piece];
        while (pieces != 0) {
            int from = pop_lsb(&pieces);
            U64 moves = generate_piece_moves(board, piece, from);
            while (moves != 0) {
                int to = pop_lsb(&moves);
                Move move = {from, to, 0, piece, NONE, NORMAL_MOVE, board->ep_square, board->castle_white, board->castle_black, 0};
                if (is_illegal_move(board, &move)) continue;

                // check if move is a capture and add flag
                if ((board->occupied[!turn] & ((U64)1 << to)) != 0) {
                    move.flags |= CAPTURE_MOVE;
                    move.score += 1000;
                }
                    

                // check pawn move flags:
                if (piece == PAWN) {
                    // check for double pawn push
                    if(to-from == (turn ? -16 : 16)) move.flags |= DOUBLE_PAWN_PUSH;

                    // check for promotion
                    if(is_set_bit(turn ? RANK_1 : RANK_8, to)){
                        // check all promotions
                        for (int promo = KNIGHT; promo <= QUEEN; promo++) {
                            move.promo = promo;
                            move.flags |= PROMOTION;
                            move.score += promo * 1000;
                            list->moves[list->count++] = move;
                        }
                        continue;
                    }
                }
                list->moves[list->count++] = move;
            }
        }
    }

    generate_castling_moves(list, board);
    generate_en_passant(list, board);
}

void print_move(Move *move) {
    const char *pieces[] = {"Pawn", "Knight", "Bishop",
                            "Rook", "Queen",  "King"};

    // print piece type
    printf("%-6s", pieces[move->piece]);

    // print move details
    printf(" %c%d -> %c%d", 'a' + (move->from % 8), 1 + move->from / 8,
           'a' + (move->to % 8), 1 + move->to / 8);

    // handle promotion
    if (move->promo != 0) {
        printf(" Promo: %s", pieces[move->promo]);
    }

    // handle castling
    if (move->flags & CASTLING) {
        printf(" Castling");
    }

    // handle en passant
    if (move->flags & EN_PASSANT){
        printf(" En Passant");
    }

    printf("\n");
}

void print_move_list(MoveList *list) {
    printf("\n");
    for (int i = 0; i < list->count; i++) {
        printf("%-2d. ", i);
        print_move(&list->moves[i]);
    }
}

Move translate_move(const char *moveStr, Board *Board) {

    int size = strlen(moveStr);
    Turn turn = Board->turn;
    PieceType piece = 0;
    PieceType promote = 0;
    int is_capture = 0;
    int start_rank = -1;
    int start_file = -1;
    int final_rank = -1;
    int final_file = -1;

    int castle_count = 0;

    Move move = {0, 0, 0, PAWN, NONE, NORMAL_MOVE, Board->ep_square, Board->castle_white, Board->castle_black, 0};

    for (int i = 0; i < size; i++) {
        switch (moveStr[i]) {
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
            is_capture = 1;
            move.flags |= CAPTURE_MOVE;
            move.score += 1000;
            break;
        case '=': {
            char c = '0';
            if (i + 1 <= size) {
                c = moveStr[i + 1];
                i++;
            }

            if (c == 'N') {
                move.promo = KNIGHT;
            } else if (c == 'B') {
                move.promo = BISHOP;
            } else if (c == 'R') {
                move.promo = ROOK;
            } else if (c == 'Q') {
                move.promo = QUEEN;
            }
            move.flags |= PROMOTION;
            move.score += move.promo * 1000;
            break;
        }
        case '+':
            //move->flags = 1;
            // check means good move 
            move.score += 3000;
            break;
        case '#':
            //move->flags = 2;
            break;
        case 'O': 
            castle_count++;
            break;
        default: {
            char c = moveStr[i];
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

    if(castle_count != 0) {
        MoveList list;
        list.count = 0;
        generate_castling_moves(&list, Board);
        for(int i = 0; i < list.count; i++) {
            if(list.moves[i].score == 2100 && castle_count == 2) {
                move = list.moves[i];
                break; 
            }
            else if(list.moves[i].score == 2000 && castle_count == 3) {
                move = list.moves[i];
                break; 
            }
        }
        return move;
    }
     
    if(move.piece == PAWN && move.to - move.from == (turn ? -16 : 16)) 
        move.flags |= DOUBLE_PAWN_PUSH;
    if(move.piece == PAWN && move.to == Board->ep_square) {
        move.flags |= EN_PASSANT;
        move.score = -1000;
    }

    move.to = (final_rank * 8) + final_file;
    move.from = (start_rank * 8) + start_file;

    if(start_rank == -1 && start_file == -1) { 
        MoveList list;
        list.count = 0;
        generate_moves(&list, Board);
        for(int i=0; i<list.count; i++) {
            Move b = list.moves[i];
            if((move.to == b.to) && (move.piece == b.piece)) 
                move.from = b.from;
        }
    }
    else if(start_rank == -1 && start_file != -1) { 
        MoveList list;
        list.count = 0;
        generate_moves(&list, Board);
        for(int i=0; i<list.count; i++) {
            Move b = list.moves[i];
            if((move.to == b.to) && (move.piece == b.piece) && move.from % 8 == start_file) 
                move.from = b.from;
        }
    }
    else if(start_rank == -1 && start_file != -1) { 
        MoveList list;
        list.count = 0;
        generate_moves(&list, Board);
        for(int i=0; i<list.count; i++) {
            Move b = list.moves[i];
            if((move.to == b.to) && (move.piece == b.piece) && move.from / 8 == start_rank) 
                move.from = b.from;
        }
    }

    (void)turn;
    (void)piece;
    (void)promote;
    (void)is_capture;
    (void)start_rank;
    (void)start_file;

    return move;
}

MoveList first_list(const char* moveStr, Board *Board) {

    int cnt = 0;
    MoveList list;
    list.count = 0;
    size_t size = strlen(moveStr);
    char buffer[10];
    for(size_t i=0; i<size+1; i++) {
        char c = moveStr[i];
        if(c != ' ' && c != '\0') {
            buffer[cnt] = c;
            cnt++;
        }
        else {
            Move temp = translate_move(buffer, Board);
            list.moves[list.count++] = temp;
            memset(buffer, 0, sizeof(buffer));
            cnt = 0;
        }
    }
    

    return list;
}
