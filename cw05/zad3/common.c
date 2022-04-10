#include "common.h"

void raiseError(char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

void raisePError(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}
