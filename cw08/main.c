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
int W, H;
int M;
int **fileMatrix;

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

void parseFile(void) {
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
    if (getline(&line, &len, file) == -1) {
        if (errno != 0) raisePError("getline");
        raiseError("Early EOF.");
    }
    M = (int) strtol(line, &newLineRemainder, 10);
    if (M < 0 || M > 255) raiseError("M must be an integer between 0 and 255.");
    if (*newLineRemainder != '\n') raiseError("The third line should be 'M'.");

    // the matrix from the file
    fileMatrix = malloc(H * sizeof(int*));
    for (int i = 0; i < H; ++i) fileMatrix[i] = malloc(W * sizeof(int));

    // the remaining lines (the matrix)
    int i = 0, j = 0;
    char *lineBeginning = NULL;
    while (getline(&line, &len, file) != -1) {
        lineBeginning = line;
        while (1) {
            int val = (int) strtol(line, &remainder, 10);
            if (val < 0 || val > 255) {
                freeMemory();
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
}

void *getImageNegative(void *arg) {
    puts("AAA");
    int *val = (int *) arg;
    int l = val[0];
    int r = val[1];
    int iStart = l / W;
    int iEnd = r / W;
    int jStart = l % W;
    int jEnd = r % W;

    for (int i = iStart; i <= iEnd; ++i) {
        if (i == iStart) {
            if (i == iEnd) {
                for (int j = jStart; j < jEnd; ++j) {
                    fileMatrix[i][j] = 255 - fileMatrix[i][j];
                }
            } else {
                for (int j = jStart; j < W; ++j) {
                    fileMatrix[i][j] = 255 - fileMatrix[i][j];
                }
            }
        } else {
            if (i == iEnd) {
                for (int j = 0; j < jEnd; ++j) {
                    fileMatrix[i][j] = 255 - fileMatrix[i][j];
                }
            } else {
                for (int j = 0; j < W; ++j) {
                    fileMatrix[i][j] = 255 - fileMatrix[i][j];
                }
            }
        }
    }

    int remainingPixel = val[2];
    if (remainingPixel != -1) {
        fileMatrix[remainingPixel / W][remainingPixel % W] = 255 - fileMatrix[remainingPixel / W][remainingPixel % W];
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

    int negInt = (W * H) / m;
    int remainder = (W * H) % m;
    int negInts[m][3]; // the third value is for the remaining pixel (if -1 then there is nothing to negate)

    negInts[0][0] = 0;
    negInts[0][1] = negInt - 1;

    for (int i = 1; i < m; ++i) {
        negInts[i][0] = negInts[i - 1][0] + negInt;
        negInts[i][1] = negInts[i - 1][1] + negInt;
    }

    // filling the remaining pixels
    for (int i = 0; i < remainder; ++i) {
        negInts[i][2] = negInts[m - 1][1] + i + 1;
    }

    for (int i = remainder; i < m; ++i) {
        negInts[i][2] = -1;
    }

    for (int i = 0; i < m; ++i) {
        pthread_create(&threads[i], NULL, getImageNegative, negInts[i]);
    }

    for (int i = 0; i < m; ++i) {
        pthread_join(threads[i], NULL);
    }
}

void saveNegatedImage(void) {
    FILE *file = fopen(imageNegName, "w");
    if (file == NULL) raisePError("fopen");

    // the matrix parameters
    char matrixParams[100];
    sprintf(matrixParams, "P2\n%d %d\n%d\n", W, H, M);
    fputs(matrixParams, file);

    // the matrix
    for (int i = 0; i < H; ++i) {
        char intToStrArr[5]; // 255 is max, so {'2', '5', '5', ' ', '\0'}
        for (int j = 0; j < W - 1; ++j) {
            sprintf(intToStrArr, "%d ", fileMatrix[i][j]);
            fputs(intToStrArr, file);
        }
        // the last we want to treat differently as we need to append '\n' instead of ' '
        if (i != H - 1) sprintf(intToStrArr, "%d\n", fileMatrix[i][W - 1]);
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
