#include "common.h"

Table *table;

void SIGINTHandler(int sigNum) {
    if (table != NULL) {
        if (munmap(table, sizeof(Table)) == -1) raisePError("munmap");
    }
    exit(EXIT_SUCCESS);
}

void deliverPizza(pid_t pid, sem_t *tableSem) {
    if (sem_wait(tableSem) == -1) raisePError("sem_wait");

    int pizzaIdx = -1;
    int k = getNumOfPizzasOnTable(&pizzaIdx, table);
    if (k != 0) {
        int n = table->place[pizzaIdx];
        --k;
        printf("(%d %s) Pobieram pizze: %d. Liczba pizz na stole: %d.\n", pid, getTimestamp(), n, k);
        table->place[pizzaIdx] = -1;

        if (sem_post(tableSem) == -1) raisePError("sem_wait");

        sleep(getRandInt(4, 5));

        printf("(%d %s) Dostarczam pizze: %d.\n", pid, getTimestamp(), n);

        sleep(getRandInt(4, 5));
    } else {
        if (sem_post(tableSem) == -1) raisePError("sem_wait");
    }
}

void handleDelivering(sem_t *tableSem) {
    pid_t pid = getpid();
    printf("Dostawca %d zaczyna rozwozic.\n", pid);
    while (true) deliverPizza(pid, tableSem);
}

int main(void) {
    signal(SIGINT, SIGINTHandler);

    sem_t *tableSem = getSemaphoreAddress(TABLE_SEM_NAME);

    table = (Table *) mmap(NULL, sizeof(Table), PROT_READ | PROT_WRITE, MAP_SHARED, getSharedMemDesc(TABLE_SHM_NAME), 0);
    if (table == MAP_FAILED) raisePError("mmap");

    handleDelivering(tableSem);

    return 0;
}
