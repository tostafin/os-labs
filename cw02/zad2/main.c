#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifdef USE_SYS
#include <unistd.h>
#include <fcntl.h>
#endif

void raiseError(const char *message) {
    fprintf(stderr, "[ERROR]: %s", message);
    exit(EXIT_FAILURE);
}

#ifdef USE_LIB
int main(int argc, char *argv[]) {
    char *fileName = calloc(1 << 10, sizeof(char));
    if (argc != 3) raiseError("Wrong number of arguments.");
    if (strlen(argv[1]) != 1) {
        raiseError("First argument must be a single character.");
    }

    FILE *file = fopen(argv[2], "r");
    if (file == NULL) {
        perror("Cannot open the file");
        exit(EXIT_FAILURE);
    }

    char character = argv[1][0];
    int charOccurrenceCnt = 0, lineCharOccurrenceCnt = 0;
    bool occurredInCurrLine = false;
    char c[1];
    while (fread(c, 1, 1, file) == 1) {
        if (c[0] == character) {
            charOccurrenceCnt++;
            if (!occurredInCurrLine) {
                lineCharOccurrenceCnt++;
                occurredInCurrLine = true;
            }
        } else if (c[0] == '\n') {
            occurredInCurrLine = false;
        }
    }

    fprintf(stdout, "Character %c occurred %d times in %d lines.", character, charOccurrenceCnt,
            lineCharOccurrenceCnt);
    fclose(file);
    free(fileName);
    return 0;
}
#endif

#ifdef USE_SYS
int main(int argc, char *argv[]) {
    char *fileName = calloc(1 << 10, sizeof(char));
    if (argc != 3) raiseError("Wrong number of arguments.");
    if (strlen(argv[1]) != 1) {
        raiseError("First argument must be a single character.");
    }

    int file = open(argv[2], O_RDONLY);
    if (file < 0) {
        perror("Cannot open the file");
        exit(EXIT_FAILURE);
    }

    char character = argv[1][0];
    int charOccurrenceCnt = 0, lineCharOccurrenceCnt = 0;
    bool occurredInCurrLine = false;
    char c;
    while (read(file, &c, 1) == 1) {
        if (c == character) {
            charOccurrenceCnt++;
            if (!occurredInCurrLine) {
                lineCharOccurrenceCnt++;
                occurredInCurrLine = true;
            }
        } else if (c == '\n') {
            occurredInCurrLine = false;
        }
    }

    fprintf(stdout, "Character %c occurred %d times in %d lines.", character, charOccurrenceCnt,
            lineCharOccurrenceCnt);
    close(file);
    free(fileName);
    return 0;
}
#endif
