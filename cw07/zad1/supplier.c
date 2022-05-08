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

int main(void) {
    signal(SIGINT, SIGINTHandler);

    oven = shmat(getSharedMemId(OVEN_PROJ_ID), NULL, 0600);
    table = shmat(getSharedMemId(TABLE_PROJ_ID), NULL, 0600);

    while (1);

    return 0;
}
