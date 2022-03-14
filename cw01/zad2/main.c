#include "../zad1/library.h"
#include <ctype.h>
#include <sys/times.h>
#include <sys/time.h>

#ifdef USE_DYNAMIC_LIBRARY
#include <dlfcn.h>
#endif

enum command {
    create_table, wc_files, remove_block
};

bool isNumber(const char *string) {
    for (int i = 0; string[i] != '\0'; i++) {
        if (!isdigit(string[i])) return false;
    }
    return true;
}

int findEndOfFilesIdx(int startIdx, int argc, char *argv[]) {
    int endIdx = startIdx + 1;
    while (endIdx < argc && strcmp(argv[endIdx], "create_table") != 0 && strcmp(argv[endIdx], "remove_block") != 0 &&
           strcmp(argv[endIdx], "wc_files") != 0 && strcmp(argv[endIdx], argv[argc - 1]) != 0) {
        endIdx++;
    }
    return --endIdx;
}

double timeDifference(clock_t start, clock_t end) {
    return ((double) (end - start) / (double) sysconf(_SC_CLK_TCK));
}

void writeToReportFile(FILE *reportFile, enum command currCommand, struct timeval *realTimeStart,
                       struct timeval *realTimeEnd,
                       struct tms *tmsStart, struct tms *tmsEnd) {
    long realTimeSeconds = realTimeEnd->tv_sec - realTimeStart->tv_sec;
    long realTimeMicroseconds = realTimeEnd->tv_usec - realTimeStart->tv_usec;
    double realTime = (double) realTimeSeconds + (double) realTimeMicroseconds * 1e-6;

    double userTime = timeDifference(tmsStart->tms_cutime, tmsEnd->tms_cutime);

    double sysTime = timeDifference(tmsStart->tms_cstime, tmsEnd->tms_cstime);
    char *firstLine = calloc(15, sizeof(char));

    switch (currCommand) {
        case create_table:
            strcpy(firstLine, "create table:\n");
            break;

        case wc_files:
            strcpy(firstLine, "wc files:\n");
            break;

        case remove_block:
            strcpy(firstLine, "remove block:\n");
            break;

        default:
            raiseError("Command not found.");
            break;
    }

    fprintf(reportFile, "%s", firstLine);
    fprintf(reportFile, "Real time:    %fs\n", realTime);
    fprintf(reportFile, "User time:    %fs\n", userTime);
    fprintf(reportFile, "System time:  %fs\n\n", sysTime);

    printf("%s", firstLine);
    printf("Real time:    %fs\n", realTime);
    printf("User time:    %fs\n", userTime);
    printf("System time:  %fs\n\n", sysTime);

    free(firstLine);
}

void freeAllMemory(PointersArray *pointersArray) {
    for (int i = 0; i < pointersArray->size; i++) {
        if (pointersArray->usedFields[i]) free(pointersArray->pointersArray[i].wcResult);
    }
    free(pointersArray->pointersArray);
    free(pointersArray->usedFields);
}

#ifdef USE_DYNAMIC_LIBRARY
void raiseError(char *errorMessage) {
    fprintf(stderr, "[ERROR]: %s", errorMessage);
    exit(EXIT_FAILURE);
}
#endif

int main(int argc, char *argv[]) {
#ifdef USE_DYNAMIC_LIBRARY
    void *handle = dlopen("./liblibrary.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Can't load the dynamic library.");
        exit(EXIT_FAILURE);
    }

    void (*createTable)();
    createTable = (void (*)()) dlsym(handle, "createTable");

    MemoryBlock (*wcFiles)();
    wcFiles = (MemoryBlock (*)()) dlsym(handle, "wcFiles");

    int (*reserveMemoryBlock)();
    reserveMemoryBlock = (int (*)()) dlsym(handle, "reserveMemoryBlock");

    void (*removeBlock)();
    removeBlock = (void (*)()) dlsym(handle, "removeBlock");

    void (*raiseError)();
    raiseError = (void (*)()) dlsym(handle, "raiseError");

    if (dlerror() != NULL) raiseError("Can't load the dynamic library.");
#endif
    PointersArray pointersArray = {NULL, NULL, -1};
    FILE *reportFile = fopen(argv[argc - 1], "a");
    struct timeval realTimeStart, realTimeEnd;
    struct tms tmsStart, tmsEnd;
    if (argc < 2) raiseError("The number of arguments must be greater than 1.");
    int i = 1;
    enum command currCommand = 0;
    while (i < argc) {
        gettimeofday(&realTimeStart, 0);
        times(&tmsStart);
        if (strcmp("create_table", argv[i]) == 0) {
            if (!(isNumber(argv[i + 1]))) {
                raiseError("The argument after create_table must be an integer.");
            }
#ifdef USE_DYNAMIC_LIBRARY
            (*createTable)(&pointersArray, (int) strtol(argv[i + 1], NULL, 10));
#else
            createTable(&pointersArray, (int) strtol(argv[i + 1], NULL, 10));
#endif
            currCommand = create_table;
            i += 2;
        } else if (strcmp("wc_files", argv[i]) == 0) {
            if (pointersArray.pointersArray == NULL) raiseError("The table hasn't been created yet.");
            int startIdx = i + 1;
            int endIdx = findEndOfFilesIdx(startIdx, argc, argv);
#ifdef USE_DYNAMIC_LIBRARY
            MemoryBlock memoryBlock = (*wcFiles)(startIdx, endIdx, argv);
            (*reserveMemoryBlock)(&pointersArray, &memoryBlock);
#else
            MemoryBlock memoryBlock = wcFiles(startIdx, endIdx, argv);
            reserveMemoryBlock(&pointersArray, &memoryBlock);
#endif
            currCommand = wc_files;
            i += endIdx - startIdx + 2;
        } else if (strcmp("remove_block", argv[i]) == 0) {
            if (pointersArray.pointersArray == NULL) raiseError("The table hasn't been created yet.");

            if (!(isNumber(argv[i + 1]))) {
                raiseError("The argument after remove_block must be an integer.");
            }
            int index = (int) strtol(argv[i + 1], NULL, 10);

            if (pointersArray.size <= index) raiseError("The index is larger than the table's size.");
#ifdef USE_DYNAMIC_LIBRARY
            (*removeBlock)(&pointersArray, index);
#else
            removeBlock(&pointersArray, index);
#endif
            currCommand = remove_block;
            i += 2;
        } else if (i == argc - 1) i++;
        else raiseError("Incorrect command.");

        gettimeofday(&realTimeEnd, 0);
        times(&tmsEnd);
        writeToReportFile(reportFile, currCommand, &realTimeStart, &realTimeEnd, &tmsStart, &tmsEnd);
    }

    if (pointersArray.pointersArray != NULL) freeAllMemory(&pointersArray);
    fclose(reportFile);
#ifdef USE_DYNAMIC_LIBRARY
    dlclose(handle);
#endif
}
