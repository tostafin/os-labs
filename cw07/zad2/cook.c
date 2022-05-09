#include "common.h"

Oven *oven;
Table *table;

void SIGINTHandler(int sigNum) {
    if (oven != NULL) {
        if (munmap(oven, sizeof(Oven)) == -1) raisePError("munmap");
    }
    if (table != NULL) {
        if (munmap(table, sizeof(Table)) == -1) raisePError("munmap");
    }
    exit(EXIT_SUCCESS);
}

void setupIPC(sem_t **ovenSem, sem_t **tableSem) {
    *ovenSem = getSemaphoreAddress(OVEN_SEM_NAME);
    *tableSem = getSemaphoreAddress(TABLE_SEM_NAME);

    oven = (Oven *) mmap(NULL, sizeof(Oven), PROT_READ | PROT_WRITE, MAP_SHARED, getSharedMemDesc(OVEN_SHM_NAME), 0);
    if (oven == MAP_FAILED) raisePError("mmap");
    table = (Table *) mmap(NULL, sizeof(Table), PROT_READ | PROT_WRITE, MAP_SHARED, getSharedMemDesc(TABLE_SHM_NAME), 0);
    if (table == MAP_FAILED) raisePError("mmap");
}

int getNumOfPizzasInOven(void) {
    int m = 0;
    for (int i = 0; i < OVEN_AND_TABLE_SIZE; ++i) {
        if (oven->place[i] != -1) ++m;
    }
    return m;
}

int cookPizza(pid_t pid, sem_t *ovenSem, int n) {
    int currIdx = -1;
    if (sem_wait(ovenSem) == -1) raisePError("sem_wait");

    if (oven->place[oven->nextIdx] == -1) {
        oven->place[oven->nextIdx] = n;
        currIdx = oven->nextIdx;
        oven->nextIdx = (oven->nextIdx + 1) % OVEN_AND_TABLE_SIZE;
        int m = getNumOfPizzasInOven();

        printf("(%d %s) Dodalem pizze: %d. Liczba pizz w piecu: %d.\n", pid, getTimestamp(), n, m);

        if (sem_post(ovenSem) == -1) raisePError("sem_wait");

    } else {
        if (sem_post(ovenSem) == -1) raisePError("sem_wait");
    }

    return currIdx;
}

bool putPizzaOnTable(pid_t pid, sem_t *ovenSem, sem_t *tableSem, int n, int currIdx) {
    if (sem_wait(tableSem) == -1) raisePError("sem_wait");

    if (sem_wait(ovenSem) == -1) raisePError("sem_wait");

    if (table->place[table->nextIdx] == -1) {
        oven->place[currIdx] = -1;

        table->place[table->nextIdx] = n;
        table->nextIdx = (table->nextIdx + 1) % OVEN_AND_TABLE_SIZE;

        int m = getNumOfPizzasInOven();
        int tmp;
        int k = getNumOfPizzasOnTable(&tmp, table);

        printf("(%d %s) Wyjmuje pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n", pid, getTimestamp(),
               n, m, k);

        if (sem_post(ovenSem) == -1) raisePError("sem_wait");

        if (sem_post(tableSem) == -1) raisePError("sem_wait");

        return true;
    }
    if (sem_post(ovenSem) == -1) raisePError("sem_wait");

    if (sem_post(tableSem) == -1) raisePError("sem_wait");

    return false;
}

void handleCooking(sem_t *ovenSem, sem_t *tableSem) {
    pid_t pid = getpid();
    printf("Kucharz %d zaczyna pichcic.\n", pid);
    srand(pid);
    int currIdx;
    while (true) {
        int n = getRandInt(0, 9);
        printf("(%d %s) Przygotowuje pizze: %d.\n", pid, getTimestamp(), n);
        sleep(getRandInt(1, 2));
        while ((currIdx = cookPizza(pid, ovenSem, n)) == -1);
        sleep(getRandInt(4, 5));
        while (putPizzaOnTable(pid, ovenSem, tableSem, n, currIdx) == false);
    }
}

int main(void) {
    sem_t *ovenSem, *tableSem;
    setupIPC(&ovenSem, &tableSem);

    signal(SIGINT, SIGINTHandler);

    handleCooking(ovenSem, tableSem);

    return 0;
}
