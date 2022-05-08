#include "common.h"

void raisePError(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void raiseError(const char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}
