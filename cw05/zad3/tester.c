#include "common.h"

// main originalPath copyPath rowNum
int main(int argc, char *argv[]) {
    if (argc != 4) raiseError("You must pass exactly three arguments.");
    int rowNum = (int) strtol(argv[3], NULL, 10);
    if (rowNum <= 0) raiseError("You must pass a positive integer as a row number");

    FILE *originalFile = fopen(argv[1], "r");
    if (originalFile == NULL) raisePError("fopen");
    FILE *copyFile = fopen(argv[2], "r");
    if (copyFile == NULL) raisePError("fopen");

    char *originalLine = NULL;
    size_t len;
    getline(&originalLine, &len, originalFile);

    char *copyLine = NULL;
    getline(&copyLine, &len, copyFile);
    int i = 1;
    while (i != rowNum) {
        getline(&copyLine, &len, copyFile);
        ++i;
    }

    if (strcmp(originalLine, copyLine) == 0) printf("Line %d OK.\n", rowNum);
    else printf("Line %d not matching the original file!\n", rowNum);

    fclose(originalFile);
    fclose(copyFile);
    free(originalLine);
    free(copyLine);
    return 0;
}
