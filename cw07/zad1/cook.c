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

void setupIPC(int *semId) {
    *semId = getSemaphoreId();
    oven = (Oven *) shmat(getSharedMemId(OVEN_PROJ_ID), NULL, 0600);
    if (oven == (void *) -1) raisePError("shmat");
    table = (Table *) shmat(getSharedMemId(TABLE_PROJ_ID), NULL, 0600);
    if (table == (void *) -1) raisePError("shmat");
}

int getNumOfPizzasInOven(void) {
    int m = 0;
    for (int i = 0; i < OVEN_AND_TABLE_SIZE; ++i) {
        if (oven->place[i] != -1) ++m;
    }
    return m;
}

int cookPizza(pid_t pid, int semId, struct sembuf *sb, int n) {
    int currIdx = -1;
    sb->sem_num = OVEN;
    sb->sem_op = -1;
    if (semop(semId, sb, 1) == -1) raisePError("semop");

    if (oven->place[oven->nextIdx] == -1) {
        oven->place[oven->nextIdx] = n;
        currIdx = oven->nextIdx;
        oven->nextIdx = (oven->nextIdx + 1) % OVEN_AND_TABLE_SIZE;
        int m = getNumOfPizzasInOven();

        printf("(%d %s) Dodalem pizze: %d. Liczba pizz w piecu: %d.\n", pid, getTimestamp(), n, m);

        sb->sem_num = OVEN;
        sb->sem_op = 1;
        if (semop(semId, sb, 1) == -1) raisePError("semop");

    } else {
        sb->sem_num = OVEN;
        sb->sem_op = 1;
        if (semop(semId, sb, 1) == -1) raisePError("semop");
    }

    return currIdx;
}

bool putPizzaOnTable(pid_t pid, int semId, struct sembuf *sb, int n, int currIdx) {
    sb->sem_num = TABLE;
    sb->sem_op = -1;
    if (semop(semId, sb, 1) == -1) raisePError("semop");

    sb->sem_num = OVEN;
    sb->sem_op = -1;
    if (semop(semId, sb, 1) == -1) raisePError("semop");

    if (table->place[table->nextIdx] == -1) {
        oven->place[currIdx] = -1;

        table->place[table->nextIdx] = n;
        table->nextIdx = (table->nextIdx + 1) % OVEN_AND_TABLE_SIZE;

        int m = getNumOfPizzasInOven();
        int tmp;
        int k = getNumOfPizzasOnTable(&tmp, table);

        printf("(%d %s) Wyjmuje pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n", pid, getTimestamp(),
               n, m,
               k);

        sb->sem_num = OVEN;
        sb->sem_op = 1;
        if (semop(semId, sb, 1) == -1) raisePError("semop");

        sb->sem_num = TABLE;
        sb->sem_op = 1;
        if (semop(semId, sb, 1) == -1) raisePError("semop");

        return true;
    }
    sb->sem_num = OVEN;
    sb->sem_op = 1;
    if (semop(semId, sb, 1) == -1) raisePError("semop");

    sb->sem_num = TABLE;
    sb->sem_op = 1;
    if (semop(semId, sb, 1) == -1) raisePError("semop");

    return false;
}

void handleCooking(int semId) {
    pid_t pid = getpid();
    printf("Kucharz %d zaczyna pichcic.\n", pid);
    srand(pid);
    struct sembuf sb;
    sb.sem_flg = 0;
    int currIdx;
    while (true) {
        int n = getRandInt(0, 9);
        printf("(%d %s) Przygotowuje pizze: %d.\n", pid, getTimestamp(), n);
        sleep(getRandInt(1, 2));
        while ((currIdx = cookPizza(pid, semId, &sb, n)) == -1);
        sleep(getRandInt(4, 5));
        while (putPizzaOnTable(pid, semId, &sb, n, currIdx) == false);
    }
}

int main(void) {
    int semId;
    setupIPC(&semId);

    signal(SIGINT, SIGINTHandler);

    handleCooking(semId);

    return 0;
}
