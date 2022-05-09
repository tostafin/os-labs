#include "common.h"

Oven *oven;
Table *table;

int N;
int M;

int semId;
int ovenId;
int tableId;
pid_t cooksPids[MAX_N];
pid_t suppliersPids[MAX_M];

void SIGINTHandler(int sigNum) {
    for (int i = 0; i < N; ++i) kill(cooksPids[i], SIGINT);
    for (int i = 0; i < M; ++i) kill(suppliersPids[i], SIGINT);
    while (wait(NULL) > 0);

    if (semctl(semId, 0, IPC_RMID) == -1) raisePError("semctl");

    shmctl(ovenId, IPC_RMID, NULL);
    shmctl(tableId, IPC_RMID, NULL);

    puts("\nPizzeria konczy prace.");
    exit(EXIT_SUCCESS);
}

void parseArgv(int argc, char *argv[]) {
    if (argc != 3) raiseError("You must pass exactly two arguments: N and M.");
    N = (int) strtol(argv[1], NULL, 10);
    if (N <= 0) raiseError("N must be a positive integer.");
    if (N >= 1<<15) raiseError("N must be less than 2^15");
    M = (int) strtol(argv[2], NULL, 10);
    if (M <= 0) raiseError("M must be a positive integer.");
    if (M >= 1<<15) raiseError("M must be less than 2^15");
}

int createSemaphores(void) {
    key_t key = ftok(getenv("HOME"), 'S');
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

    size_t size = projId == OVEN_PROJ_ID ? sizeof(Oven) : sizeof(Table);

    int id = shmget(key, size, IPC_CREAT | IPC_EXCL | 0600);
    if (id == -1) raisePError("shmget");

    return id;
}

void prepOvenAndTable(void) {
    for (int i = 0; i < OVEN_AND_TABLE_SIZE; ++i) {
        oven->place[i] = -1;
        table->place[i] = -1;
    }
}

void createCooks(void) {
    pid_t childPid;
    for (int i = 0; i < N; ++i) {
        if ((childPid = fork()) == -1) {
            raisePError("fork");
        } else if (childPid == 0) {
            cooksPids[i] = getpid();
            execl("./cook", "cook", NULL);
            raisePError("execl");
        }
    }
}

void createSuppliers(void) {
    pid_t childPid;
    for (int i = 0; i < M; ++i) {
        if ((childPid = fork()) == -1) {
            raisePError("fork");
        } else if (childPid == 0) {
            suppliersPids[i] = getpid();
            execl("./supplier", "supplier", NULL);
            raisePError("execl");
        }
    }
}

int main(int argc, char *argv[]) {
    parseArgv(argc, argv);

    semId = createSemaphores();
    ovenId = createSharedMemSeg(OVEN_PROJ_ID); //for the oven
    tableId = createSharedMemSeg(TABLE_PROJ_ID); //for the table
    oven = (Oven *) shmat(ovenId, NULL, 0);
    table = (Table *) shmat(tableId, NULL, 0);

    prepOvenAndTable();

    signal(SIGINT, SIGINTHandler);

    createCooks();
    createSuppliers();

    while (wait(NULL) > 0);

    return 0;
}
