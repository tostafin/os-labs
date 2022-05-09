#include "common.h"

Oven *oven;
Table *table;

void SIGINTHandler(int sigNum) {
    if (oven != NULL) {
        if (shmdt(oven) == -1) raisePError("shmdt");
    }
    if (table != NULL) {
        if (shmdt(table) == -1) raisePError("shmdt");
    }
    exit(EXIT_SUCCESS);
}

int getNumOfPizzasInOven(void) {
    int m = 0;
    for (int i = 0; i < OVEN_AND_TABLE_SIZE; ++i) {
        if (oven->place[i] != -1) ++m;
    }
    return m;
}

void cookPizza(pid_t pid, int semId, struct sembuf *sb) {
    int n = getRandInt(0, 10);
    sleep(getRandInt(1, 3));
    printf("(%d %s) Przygotowuje pizze: %d.\n", pid, getTimestamp(), n);

    sb->sem_num = OVEN;
    sb->sem_op = -1;
    if (semop(semId, sb, 1) == -1) raisePError("semop");

    oven->place[oven->nextIdx] = n;
    int currIdx = oven->nextIdx;
    oven->nextIdx = (oven->nextIdx + 1) % OVEN_AND_TABLE_SIZE;
    int m = getNumOfPizzasInOven();

    printf("(%d %s) Dodalem pizze: %d. Liczba pizz w piecu: %d.\n", pid, getTimestamp(), n, m);

    sb->sem_num = OVEN;
    sb->sem_op = 1;
    if (semop(semId, sb, 1) == -1) raisePError("semop");

    sleep(getRandInt(4, 6));

    sb->sem_num = TABLE;
    sb->sem_op = -1;
    if (semop(semId, sb, 1) == -1) raisePError("semop");

    sb->sem_num = OVEN;
    sb->sem_op = -1;
    if (semop(semId, sb, 1) == -1) raisePError("semop");

    oven->place[currIdx] = -1;

    table->place[table->nextIdx] = n;
    table->nextIdx = (table->nextIdx + 1) % OVEN_AND_TABLE_SIZE;

    m = getNumOfPizzasInOven();
    int tmp;
    int k = getNumOfPizzasOnTable(&tmp, table);

    printf("(%d %s) Wyjmuje pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n", pid, getTimestamp(), n, m,
           k);

    sb->sem_num = OVEN;
    sb->sem_op = 1;
    if (semop(semId, sb, 1) == -1) raisePError("semop");

    sb->sem_num = TABLE;
    sb->sem_op = 1;
    if (semop(semId, sb, 1) == -1) raisePError("semop");
}

void handleCooking(int semId) {
    pid_t pid = getpid();
    printf("Kucharz %d zaczyna pichcic.\n", pid);
    srand(time(NULL));
    struct sembuf sb;
    while (true) cookPizza(pid, semId, &sb);
}

int main(void) {
    signal(SIGINT, SIGINTHandler);

    int semId = getSemaphoreId();
    oven = (Oven *) shmat(getSharedMemId(OVEN_PROJ_ID), NULL, 0600);
    table = (Table *) shmat(getSharedMemId(TABLE_PROJ_ID), NULL, 0600);

    handleCooking(semId);

    return 0;
}
