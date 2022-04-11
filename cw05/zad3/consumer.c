#include "common.h"

void replaceLine(const char *fileName, int lineNum, char *contentLine) {
    static int numOfProducers = 0;
    FILE *writeFile = fopen(fileName, "r+");
    if (writeFile == NULL) raisePError("fopen");
    flock(fileno(writeFile), LOCK_EX);
    char fileArr[100][10000];
    for (int i = 0; i < 10; ++i) fileArr[i][0] = '\n';
    char *line = NULL;
    size_t len;
    int i = 0;
    while (getline(&line, &len, writeFile) != -1) {
        strcpy(fileArr[i], line);
        ++i;
    }
    if (freopen(fileName, "w", writeFile) == NULL) raisePError("freopen");

    if (fileArr[lineNum - 1][0] == '\n') {
        strcpy(fileArr[lineNum - 1], contentLine);
        ++numOfProducers;
    } else {
        fileArr[lineNum - 1][strlen(fileArr[lineNum - 1]) - 1] = '\0'; // remove the newline at the end
        strcat(fileArr[lineNum - 1], contentLine);
    }

    int tmp = numOfProducers;
    i = 0;
    while (tmp > 0) {
        if (fileArr[i][0] != '\n') --tmp;
        fwrite(fileArr[i], 1, strlen(fileArr[i]), writeFile);
        ++i;
    }

    flock(fileno(writeFile), LOCK_UN);
    fclose(writeFile);
    free(line);
}

// consumer pipePath filePath N
int main(int argc, char *argv[]) {
    if (argc != 4) raiseError("You must pass exactly 4 arguments.");
    size_t N = (size_t) strtol(argv[3], NULL, 10);
    if (N <= 0) raiseError("The last argument must be a positive integer.");

    int fifo = open(argv[1], O_RDONLY);
    if (fifo == -1) raisePError("open");

    char buff[N + 10];
    int i;

    while (read(fifo, buff, N + 10) > 0) {
        char *content;
        i = (int) strtol(buff, &content, 10);
        ++content;
        *(content + N) = '\n';
        *(content + N + 1) = '\0';
        replaceLine(argv[2], i, content);
    }
    close(fifo);
    return 0;
}

