#include "common.h"

void raiseError(const char *message) {
    fprintf(stderr, "[ERROR]: %s\n", message);
    exit(EXIT_FAILURE);
}

void raisePError(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}