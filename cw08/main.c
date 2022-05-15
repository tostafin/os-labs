#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define IMAGE_NAME_LEN 100

typedef enum divMethod {
    NUMBERS,
    BLOCK
} DivMethod;

int m = 0;
DivMethod divMethod;
char imageName[IMAGE_NAME_LEN];
char imageNegName[IMAGE_NAME_LEN];

void raiseError(const char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

void raisePError(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void freeMemory(int **fileMatrix, int H) {
    for (int i = 0; i < H; ++i) free(fileMatrix[i]);
    free(fileMatrix);
}

void parseArgv(int argc, char *argv[]) {
    if (argc != 5) {
        raiseError("You must pass exactly four argument: the number of threads, the method of division,"
                   " the name of the image to be negated, the name of the negated image.");
    }

    m = (int) strtol(argv[1], NULL, 10);
    if (m <= 0) raiseError("The number of threads must be a positive integer.");
    // TODO: check if m <= W * H after file parse

    if (strcmp(argv[2], "numbers") == 0) divMethod = NUMBERS;
    else if (strcmp(argv[2], "block") == 0) divMethod = BLOCK;
    else raiseError("The method of division must be either 'numbers' or 'block'.");

    strcpy(imageName, argv[3]);

    strcpy(imageNegName, argv[4]);
}

int **parseFile(int *W, int *H) {
    FILE *file = fopen(imageName, "r");
    if (file == NULL) raisePError("fopen");

    char *line = NULL;
    size_t len = 0;
    errno = 0;

    // the 1st line
    if (getline(&line, &len, file) == -1) {
        if (errno != 0) raisePError("getline");
        raiseError("Early EOF.");
    }
    if (strcmp(line, "P2\n") != 0) raiseError("The first line must be 'P2'.");

    // the 2nd line
    if (getline(&line, &len, file) == -1) {
        if (errno != 0) raisePError("getline");
        raiseError("Early EOF.");
    }
    char *remainder;
    *W = (int) strtol(line, &remainder, 10);
    if (*W <= 0) raiseError("The width of the matrix must be a positive integer.");
    if (*remainder != ' ') raiseError("The width and the height of the matrix must be seperated by a single space.");
    ++remainder;
    char *newLineRemainder;
    *H = (int) strtol(remainder, &newLineRemainder, 10);
    if (*H <= 0) raiseError("The height of the matrix must be a positive integer.");
    // check previous TODO
    if (m > *W * *H) {
        raiseError("The number of threads must not be greater than the number of elements in the matrix");
    }
    if (*newLineRemainder != '\n') raiseError("The second line should be 'W H'.");

    // the third line
    if (getline(&line, &len, file) == -1) {
        if (errno != 0) raisePError("getline");
        raiseError("Early EOF.");
    }
    int M = (int) strtol(line, &newLineRemainder, 10);
    if (M < 0 || M > 255) raiseError("M must be an integer between 0 and 255.");
    if (*newLineRemainder != '\n') raiseError("The third line should be 'M'.");

    // build the matrix
    int **fileMatrix = malloc(*H * sizeof(int*));
    for (int i = 0; i < *H; ++i) fileMatrix[i] = malloc(*W * sizeof(int));

    // the remaining lines (the matrix)
    int i = 0, j = 0;
    char *lineBeginning = NULL;
    while (getline(&line, &len, file) != -1) {
        lineBeginning = line;
        while (1) {
            int val = (int) strtol(line, &remainder, 10);
            if (val < 0 || val > 255) {
                freeMemory(fileMatrix, *H);
                raiseError("All values in the matrix must be integers between 0 and 255");
            }
            fileMatrix[i][j] = val;
            if (*remainder != '\n') {
                line = remainder;
                remainder = NULL;
                // skip white characters
                while (isspace(*line)) ++line;
            }
            else break;
            ++j;
        }
        ++i;
        j = 0;
        line = lineBeginning;
    }
    if (lineBeginning == NULL) {
        free(line);
        raiseError("Early EOF.");
    }
    if (errno != 0) raisePError("getline");

    free(lineBeginning);
    fclose(file);
    return fileMatrix;
}

int main(int argc, char *argv[]) {
    parseArgv(argc, argv);

    int W, H;
    int **fileMatrix = parseFile(&W, &H);

    freeMemory(fileMatrix, H);

    return 0;
}
