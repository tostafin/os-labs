#include "common.h"

void raisePError(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void raiseError(const char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

int getSharedMemId(int projId) {
    key_t key = ftok(getenv("HOME"), projId);
    if (key == -1) raisePError("ftok");

    int id = shmget(key, 0, 0);
    if (id == -1) raisePError("semget");

    return id;
}