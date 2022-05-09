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

void deliverPizza(pid_t pid, int semId, struct sembuf *sb) {
    sb->sem_num = TABLE;
    sb->sem_op = -1;
    semop(semId, sb, 1);

    int pizzaIdx = -1;
    int k = getNumOfPizzasOnTable(&pizzaIdx, table);
    if (k != 0) {
        int n = table->place[pizzaIdx];
        --k;
        printf("(%d %s) Pobieram pizze: %d. Liczba pizz na stole: %d.\n", pid, getTimestamp(), n, k);
        table->place[pizzaIdx] = -1;

        sb->sem_num = TABLE;
        sb->sem_op = 1;
        semop(semId, sb, 1);

        sleep(getRandInt(4, 5));

        printf("(%d %s) Dostarczam pizze: %d.\n", pid, getTimestamp(), n);

        sleep(getRandInt(4, 5));
    } else {
        sb->sem_num = TABLE;
        sb->sem_op = 1;
        semop(semId, sb, 1);
    }
}

void handleDelivering(int semId) {
    pid_t pid = getpid();
    printf("Dostawca %d zaczyna rozwozic.\n", pid);
    struct sembuf sb;
    while (true) deliverPizza(pid, semId, &sb);
}

int main(void) {
    signal(SIGINT, SIGINTHandler);

    int semId = getSemaphoreId();
    oven = (Oven *) shmat(getSharedMemId(OVEN_PROJ_ID), NULL, 0600);
    if (oven == (void *) -1) raisePError("shmat");
    table = (Table *) shmat(getSharedMemId(TABLE_PROJ_ID), NULL, 0600);
    if (table == (void *) -1) raisePError("shmat");

    handleDelivering(semId);

    return 0;
}
