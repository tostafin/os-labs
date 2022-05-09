#include "common.h"

void raisePError(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void raiseError(const char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

sem_t *getSemaphoreAddress(const char *name) {
    sem_t *semAddress = sem_open(name, O_RDWR);
    if (semAddress == SEM_FAILED) raisePError("sem_open");

    return semAddress;
}

int getSharedMemDesc(const char *name) {
    int id = shm_open(name, O_RDWR, 0600);
    if (id == -1) raisePError("shmget");

    return id;
}

char *getTimestamp(void) {
    static char timeStr[20];
    long ms; // Milliseconds
    time_t s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s = spec.tv_sec;
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds
    if (ms > 999) {
        s++;
        ms = 0;
    }

    sprintf(timeStr, "%"PRIdMAX".%03ld seconds", (intmax_t) s, ms);
    return timeStr;
}

int getRandInt(int minNum, int maxNum) { //both are inclusive
    return rand() % (maxNum + 1 - minNum) + minNum;
}

int getNumOfPizzasOnTable(int *idx, Table *table) {
    int k = 0;
    for (int i = 0; i < OVEN_AND_TABLE_SIZE; ++i) {
        if (table->place[i] != -1) {
            ++k;
            *idx = i;
        }
    }
    return k;
}