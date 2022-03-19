#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void raiseError(const char *message) {
    fprintf(stderr, "[ERROR]: %s", message);
    exit(EXIT_FAILURE);
}

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
    int maxLineLength = 256;
    char line[maxLineLength];
    int charOccurrenceCnt = 0, lineCharOccurrenceCnt = 0;
    bool occurredInCurrLine = false;
    while (fgets(line, maxLineLength, file) != NULL) {
        for (int i = 0; line[i] != '\0'; ++i) {
            if (line[i] == character) {
                charOccurrenceCnt++;
                if (!occurredInCurrLine) {
                    lineCharOccurrenceCnt++;
                    occurredInCurrLine = true;
                }
            }
        }
        occurredInCurrLine = false;
    }

    fprintf(stdout, "Character %c occurred %d times in %d lines.", character, charOccurrenceCnt,
            lineCharOccurrenceCnt);
    fclose(file);
    free(fileName);
    return 0;
}

