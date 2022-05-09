#include "common.h"

Oven *oven;
Table *table;

int N;
int M;

sem_t *ovenSem;
sem_t *tableSem;
int ovenId;
int tableId;
pid_t cooksPids[MAX_N];
pid_t suppliersPids[MAX_M];

void SIGINTHandler(int sigNum) {
    for (int i = 0; i < N; ++i) kill(cooksPids[i], SIGINT);
    for (int i = 0; i < M; ++i) kill(suppliersPids[i], SIGINT);
    while (wait(NULL) > 0);

    if (sem_close(ovenSem) == -1) raisePError("sem_close");
    if (sem_close(tableSem) == -1) raisePError("sem_close");
    if (sem_unlink(OVEN_SEM_NAME) == -1) raisePError("sem_unlink");
    if (sem_unlink(TABLE_SEM_NAME) == -1) raisePError("sem_unlink");

    if (munmap((void *) oven, sizeof(Oven)) == -1) raisePError("munmap");
    if (munmap((void *) table, sizeof(Table)) == -1) raisePError("munmap");
    if (shm_unlink(OVEN_SHM_NAME) == -1) raisePError("shm_unlink");
    if (shm_unlink(TABLE_SHM_NAME) == -1) raisePError("shm_unlink");

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

sem_t *createSemaphores(const char *name) {
    sem_t *semAddress = sem_open(name, O_CREAT | O_EXCL | O_RDWR, 0600, 1);
    if (semAddress == SEM_FAILED) raisePError("sem_open");

    return semAddress;
}

int createSharedMemSeg(const char *name) {
    int id = shm_open(name, O_CREAT | O_EXCL | O_RDWR, 0600);
    if (id == -1) raisePError("shm_open");

    return id;
}

void prepOvenAndTable(void) {
    for (int i = 0; i < OVEN_AND_TABLE_SIZE; ++i) {
        oven->place[i] = -1;
        table->place[i] = -1;
    }
    oven->nextIdx = 0;
    table->nextIdx = 0;
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

    ovenSem = createSemaphores(OVEN_SEM_NAME);
    tableSem = createSemaphores(TABLE_SEM_NAME);

    ovenId = createSharedMemSeg(OVEN_SHM_NAME); //for the oven
    ftruncate(ovenId, sizeof(Oven));
    tableId = createSharedMemSeg(TABLE_SHM_NAME); //for the table
    ftruncate(tableId, sizeof(Table));
    oven = (Oven *) mmap(NULL, sizeof(Oven), PROT_READ | PROT_WRITE, MAP_SHARED, ovenId, 0);
    if (oven == MAP_FAILED) raisePError("mmap");
    table = (Table *) mmap(NULL, sizeof(Table), PROT_READ | PROT_WRITE, MAP_SHARED, tableId, 0);
    if (table == MAP_FAILED) raisePError("mmap");

    prepOvenAndTable();

    signal(SIGINT, SIGINTHandler);

    createCooks();
    createSuppliers();

    while (wait(NULL) > 0);

    return 0;
}
