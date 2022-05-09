#ifndef COMMON_H
#define COMMON_H

#define OVEN_AND_TABLE_SIZE 5
//#define OVEN_PROJ_ID 'O'
//#define TABLE_PROJ_ID 'T'
#define MAX_N (1<<15) - 1 //max value of any semaphore
#define MAX_M (1<<15) - 1 //max value of any semaphore
#define OVEN_SEM_NAME "/posixOvenSemName"
#define TABLE_SEM_NAME "/posixTableSemName"
#define OVEN_SHM_NAME "/posixOvenShmName"
#define TABLE_SHM_NAME "/posixTableShmName"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <assert.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

typedef struct oven {
    int place[OVEN_AND_TABLE_SIZE];
    int nextIdx;
} Oven;

typedef struct table {
    int place[OVEN_AND_TABLE_SIZE];
    int nextIdx;
} Table;

enum places {
    OVEN,
    TABLE
};

void raisePError(const char *message);
void raiseError(const char *message);
sem_t *getSemaphoreAddress(const char *name);
int getSharedMemDesc(const char *name);
char *getTimestamp(void);
int getRandInt(int minNum, int maxNum);
int getNumOfPizzasOnTable(int *idx, Table *table);

#endif //COMMON_H
