#include <stdio.h>
#include <stdlib.h>

int choose_move(char * fen, char * moves, int timeout) {
    (void)fen; (void)moves; (void)timeout;
    return 0;
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

    fprintf(stdout, "%d\n", choose_move(argv[1], argv[2], timeout));

    return 0;
}
