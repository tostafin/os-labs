#ifndef COMMON_H
#define COMMON_H

#define OVEN_AND_TABLE_SIZE 5

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
};

typedef struct oven {
    int place[OVEN_AND_TABLE_SIZE];
    int nextIdx;
} Oven;

typedef struct table {
    int place[OVEN_AND_TABLE_SIZE];
    int nextIdx;
} Table;

void raisePError(const char *message);
void raiseError(const char *message);

#endif //COMMON_H
