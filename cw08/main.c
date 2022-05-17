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
size_t matrixParamsLen = 0;
int W, H;
int M;
int matrixRows;
int **fileMatrix;
int *colsInIthRow;

void raiseError(const char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

void raisePError(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void freeMemory(void) {
    for (int i = 0; i < H; ++i) free(fileMatrix[i]);
    free(fileMatrix);
    free(colsInIthRow);
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

void cntLinesInFile(FILE *file) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, file) != -1) ++matrixRows;
    free(line);
}

void parseFile(void) {
    FILE *file = fopen(imageName, "r");
    if (file == NULL) raisePError("fopen");

    char *line = NULL;
    size_t len = 0;
    errno = 0;

    // the 1st line
    ssize_t charsRead;
    if ((charsRead = getline(&line, &len, file)) == -1) {
        if (errno != 0) raisePError("getline");
        raiseError("Early EOF.");
    }
    matrixParamsLen += charsRead;
    if (strcmp(line, "P2\n") != 0) raiseError("The first line must be 'P2'.");

    // the 2nd line
    if ((charsRead = getline(&line, &len, file)) == -1) {
        if (errno != 0) raisePError("getline");
        raiseError("Early EOF.");
    }
    matrixParamsLen += charsRead;

    char *remainder;
    W = (int) strtol(line, &remainder, 10);
    if (W <= 0) raiseError("The width of the matrix must be a positive integer.");
    if (*remainder != ' ') raiseError("The width and the height of the matrix must be seperated by a single space.");
    ++remainder;
    char *newLineRemainder;
    H = (int) strtol(remainder, &newLineRemainder, 10);
    if (H <= 0) raiseError("The height of the matrix must be a positive integer.");
    // check previous TODO
    if (m > W * H) {
        raiseError("The number of threads must not be greater than the number of elements in the matrix");
    }
    if (*newLineRemainder != '\n') raiseError("The second line should be 'W H'.");

    // the third line
    if ((charsRead = getline(&line, &len, file)) == -1) {
        if (errno != 0) raisePError("getline");
        raiseError("Early EOF.");
    }
    matrixParamsLen += charsRead;

    M = (int) strtol(line, &newLineRemainder, 10);
    if (M < 0 || M > 255) raiseError("M must be an integer between 0 and 255.");
    if (*newLineRemainder != '\n') raiseError("The third line should be 'M'.");

    // count the number of lines in the file
    long matrixBeginning = ftell(file);
    cntLinesInFile(file);
    fseek(file, matrixBeginning, SEEK_SET);

    // create the matrix
    fileMatrix = malloc(matrixRows * sizeof(int *));
    colsInIthRow = calloc(matrixRows, sizeof(int));
//    for (int i = 0; i < H; ++i) fileMatrix[i] = malloc(W * sizeof(int));

    // parse the remaining lines
    char *lineBeginning = NULL;
    int i = 0, j = 0;
    while ((charsRead = getline(&line, &len, file)) != -1) {
        lineBeginning = line;
        // we're going to need at most charsRead/2 + 1, since half of them are white characters, '\0' and '\n'
        fileMatrix[i] = malloc((charsRead / 2 + 1) * sizeof(int));
        while (1) {
            int val = (int) strtol(line, &remainder, 10);
            if (val < 0 || val > 255) {
                freeMemory();
                raiseError("All values in the matrix must be integers between 0 and 255");
            }
            printf("Val = %d, i = %d, j = %d\n", val, i, j);
            fileMatrix[i][j] = val;
            ++colsInIthRow[i];
            if (val < M) M = val;
            line = remainder;
            remainder = NULL;
            // skip white characters
            if (*line == '\n') break;
            while (isspace(*line)) ++line;
            ++j;
        }
        ++i;
        j = 0;
        line = lineBeginning;
    }
    if (lineBeginning == NULL) {
        freeMemory();
        if (errno != 0) raisePError("getline");
        else raiseError("Early EOF.");
    }

    free(lineBeginning);
    fclose(file);
}

void *getImageNegative(void *arg) {
    int *val = (int *) arg;
    switch (divMethod) {
        case NUMBERS:;
            int iStart = val[0];
            int jStart = val[1];
            int iEnd = val[2];
            int jEnd = val[3];
            int idx = val[4];
            printf("idx = %d, iStart = %d, jStart = %d, iEnd = %d, jEnd = %d\n", idx, iStart, jStart, iEnd, jEnd);
            while (iStart < matrixRows) {
                while (1) {
                    printf("idx = %d, row = %d, col = %d\n", idx, iStart, jStart);
                    fileMatrix[iStart][jStart] = 255 - fileMatrix[iStart][jStart];
                    if (iStart == iEnd && jStart == jEnd) goto end;
                    ++jStart;
                    if (colsInIthRow[iStart] <= jStart) break;
                }
                ++iStart;
                jStart = 0;
            }

        end: break;
        case BLOCK:
            break;
    }
    return NULL;
}

/* We're going to divide it evenly like this:
 * 1. get the number of pixels to be negated by each thread: negInt = (W * H) / m
 * 2. the first thread will negate pixels within the range [0, negInt - 1] := [l, r]
 * 3. the next and the others will have the intervals [l + negInt, r + negInt]
 * 4. if (W * H) % m == 0 then the number of pixels to be negated can be divided evenly between threads
 * 5. else it cannot be done, so we will assign one more task to each thread starting from the beginning
 */
void runThreads(void) {
    pthread_t threads[m];
    int indices[m][5]; // [iStart, jStart, iEnd, jEnd]

    switch (divMethod) {
        case NUMBERS:
            ;int row = 0, col = 0;
            int numsRead = 0;
            int currThread = 0;
            int threadDiv = W * H / m;
            int charsForCurrThread = threadDiv;
            indices[0][0] = 0;
            indices[0][1] = 0;
            while (row < matrixRows) {
                while (1) {
                    ++numsRead;
//                    printf("row = %d, col = %d\n", row, col);
                    if (numsRead == charsForCurrThread) {
                        charsForCurrThread += threadDiv;
                        indices[currThread][2] = row;
                        indices[currThread][3] = col;
                        indices[currThread][4] = currThread;
                        ++currThread;
                        printf("Curr: %d, m: %d\n", currThread, m);
                        if (currThread < m) {
                            if (colsInIthRow[row] <= col + 1) {
                                indices[currThread][0] = row + 1;
                                indices[currThread][1] = 0;
                            } else {
                                indices[currThread][0] = row;
                                indices[currThread][1] = col + 1;
                            }
                        }
                    }
                    ++col;
                    if (colsInIthRow[row] <= col) break;
                }
                ++row;
                col = 0;
            }
            break;
        case BLOCK:
            break;
    }
    for (int i = 0; i < m; ++i) {
        pthread_create(&threads[i], NULL, getImageNegative, indices[i]);
    }

    for (int i = 0; i < m; ++i) {
        pthread_join(threads[i], NULL);
    }
}

void saveNegatedImage(void) {
    FILE *file = fopen(imageNegName, "w");
    if (file == NULL) raisePError("fopen");

    // the matrix parameters
    char matrixParams[matrixParamsLen];
    sprintf(matrixParams, "P2\n%d %d\n%d\n", W, H, 255 - M);
    fputs(matrixParams, file);

    // the matrix
    char intToStrArr[5]; // 255 is max, so {'2', '5', '5', ' ', '\0'}
    for (int i = 0; i < matrixRows; ++i) {
        for (int j = 0; j < colsInIthRow[i] - 1; ++j) {
//            printf("i = %d, j = %d, val = %d\n", i, j, fileMatrix[i][j]);
            sprintf(intToStrArr, "%d ", fileMatrix[i][j]);
            printf("Val = %d, i = %d, j = %d\n", fileMatrix[i][j], i, j);
            fputs(intToStrArr, file);
        }
        // the last we want to treat differently as we need to append '\n' instead of ' '
        sprintf(intToStrArr, "%d\n", fileMatrix[i][colsInIthRow[i] - 1]);
        fputs(intToStrArr, file);
    }
    fclose(file);
}

int main(int argc, char *argv[]) {
    parseArgv(argc, argv);

    parseFile();

    runThreads();

    saveNegatedImage();

    freeMemory();

    return 0;
}
