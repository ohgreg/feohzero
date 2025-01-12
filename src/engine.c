#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "board.h"




int choose_move(char * fen, char * moves, int timeout) {
    (void)fen; (void)moves; (void)timeout;
    return 0;
}



//function to load the chess position from the input FEN.
Board *loadFEN(char *fen){

    

    //TO DO: check for invalid FEN format (leave for later)(for now assume it is correct)
    Board *board = malloc(sizeof(Board));
    if(board == NULL){
        fprintf(stderr, "Failed to allocate board memory!");
        exit(1);
    }
    memset(board, 0, sizeof(Board));

    

    int len = strlen(fen);
    int rank = 7;
    int file = 0;

    
    int last = 0;
    // STEP 1: read each rank one by one to load the actual position: 
    // Note: discuss whether we should use white_occupied and black_occupied and initialize them if so.
    
        for(int i=0; i<len; i++){

            
            if(rank < 0)
                break;

            last = i+1; 
            char c = fen[i];
            int index = (8*rank) + file;
            //decrement rank whenever we find backslash (or space for the last rank)
            if(c == ' ' || c == '/'){
                file = 0;
                rank--;
                continue;
            }
    
            if(c == 'r')
                board->bR |= (1ULL << index);
            else if(c == 'n')
                board->bN |= (1ULL << index);
            else if(c == 'b')
                board->bB |= (1ULL << index);
            else if(c == 'q')
                board->bQ |= (1ULL << index);
            else if(c == 'k')
                board->bK |= (1ULL << index);
            else if(c == 'p')
                board->bP |= (1ULL << index);
            else if(c == 'R')
                board->wR |= (1ULL << index);
            else if(c == 'N')
                board->wN |= (1ULL << index);
            else if(c == 'B')
                board->wB |= (1ULL << index);
            else if(c == 'Q')
                board->wQ |= (1ULL << index);
            else if(c == 'K')
                board->wK |= (1ULL << index);
            else if(c == 'P')
                board->wP |= (1ULL << index);
            else if(isdigit(c) == 1){
                //care with this, may be 49? Indexing at 0
                int skip = c - 48;
                file += skip;
                continue;
            }
            file++;
        }
    
    //STEP 2: read whose turn it is to play:
    if(fen[last] == 'w')
        board->toMove = w;
    else if(fen[last] == 'b')
        board->toMove = b;
    else{
        printf("Couldnt get next move!\n");
        exit(1);
    }
    last++;

    //skip whitespace
    while(fen[last] == ' ')
        last++;

    //STEP 3: read castling availability:
    while(fen[last] != ' '){
        if(fen[last] == 'K'){
            board->castle_white = CAN_CASTLE_OO;
            if(fen[last+1] == 'Q'){
                board->castle_white = CAN_CASTLE;
                last++;
            }       
        }
        else if(fen[last] == 'Q')
            board->castle_white = CAN_CASTLE_OOO;
        else if(fen[last] == 'k'){
            board->castle_black = CAN_CASTLE_OO;
            if(fen[last+1] == 'q'){
                board->castle_black = CAN_CASTLE;
                last++;
            }  
        }
        else if(fen[last] == 'q')
            board->castle_black = CAN_CASTLE_OOO;
            
        last++;
    }

    //skip whitespace
    while(fen[last] == ' ')
        last++;


    //STEP4: get enpassant square
   
    if(fen[last] == '-')
        board->en_passant_square = 0;
    else{
        int en_passant_index = fen[last] - 97; 
        last++;
        if(isdigit(fen[last]) == 1)
            en_passant_index += (fen[last] - 49)*8;
        else
            fprintf(stderr, "Invalid En Passant square!\n");
        board->en_passant_square |= (1 << en_passant_index);
    }
    last++;

    //skip whitespace
    while(fen[last] == ' ')
        last++;

    //STEP5: Get halfmove counter
    int halfCounter = 0;
    while(isdigit(fen[last]) == 1){
        halfCounter = 10*halfCounter + (fen[last] - '0');
        last++;
    }
    board->halfMove = halfCounter;

    //skip whitespace
    while(fen[last] == ' ')
        last++;

    //STEP6: get fullmove counter
    int fullCounter = 0;
    while(isdigit(fen[last]) == 1){
        fullCounter = 10*fullCounter + (fen[last] - '0');
        last++;
    }

    board->fullMove = fullCounter;



    return board;
}
    








int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: <fen> <moves> <timeout>\n");
        return 1;
    }
    
    char *endptr;
    int timeout = strtol(argv[3], &endptr, 10);
    if (*endptr != '\0') {
        fprintf(stderr, "Provide a valid integer!\n");
        return 1;
    }

    char *fen = argv[1];
    Board *start = loadFEN(fen);
    printf("White King is: %llu\n", start->wR);
    printf("Halfmove counter is: %d\n", start->halfMove);
    printf("Fullmove counter is: %d\n", start->fullMove);
    printf("Tomove is: %d\n", start->toMove);
    printf("En passant square is: %llu\n", start->en_passant_square);
    

    fprintf(stdout, "%d\n", choose_move(argv[1], argv[2], timeout));
    
    return 0;
}
