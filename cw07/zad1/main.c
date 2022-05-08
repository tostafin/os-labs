#include "common.h"

int semId;
int ovenId;
int tableId;

Oven *oven;
Table *table;

void SIGINTHandler(int sigNum) {
    if (semctl(semId, 0, IPC_RMID) == -1) raisePError("semctl");

    if (oven != NULL) {
        if (shmdt(oven) == -1) raisePError("shmdt");
    }
    if (table != NULL) {
        if (shmdt(table) == -1) raisePError("shmdt");
    }

    shmctl(ovenId, IPC_RMID, NULL);
    shmctl(tableId, IPC_RMID, NULL);

    puts("\nPizzeria konczy prace.");
    exit(EXIT_SUCCESS);
}

void parseArgv(int argc, char *argv[], int *N, int *M) {
    if (argc != 3) raiseError("You must pass exactly two arguments: N and M.");
    *N = (int) strtol(argv[1], NULL, 10);
    if (*N <= 0) raiseError("N must be a positive integer.");
    *M = (int) strtol(argv[2], NULL, 10);
    if (*M <= 0) raiseError("M must be a positive integer.");
}

int createSemaphores(void) {
    key_t key = ftok(getenv("HOME"), getpid());
    if (key == -1) raisePError("ftok");

    int id = semget(key, 2, IPC_CREAT | IPC_EXCL | 0600);
    if (id == -1) raisePError("semget");

    union semun arg = {1};
    if (semctl(id, 0, SETVAL, arg) == -1) raisePError("semctl");
    if (semctl(id, 1, SETVAL, arg) == -1) raisePError("semctl");

    return id;
}

int createSharedMemSeg(int projId) {
    key_t key = ftok(getenv("HOME"), projId);
    if (key == -1) raisePError("ftok");

    int id = shmget(key, sizeof(Oven), IPC_CREAT | IPC_EXCL | 0600);
    if (id == -1) raisePError("shmget");

    return id;
}

int main(int argc, char *argv[]) {
    int N, M;
    parseArgv(argc, argv, &N, &M);

    semId = createSemaphores();
    ovenId = createSharedMemSeg('O'); //for the oven
    tableId = createSharedMemSeg('T'); //for the table

    signal(SIGINT, SIGINTHandler);

    return 0;
}
