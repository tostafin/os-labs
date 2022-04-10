#include "common.h"

// consumer pipePath filePath N
int main(int argc, char *argv[]) {
    if (argc != 4) raiseError("You must pass exactly 4 arguments.");
    size_t N = (size_t) strtol(argv[3], NULL, 10);
    if (N <= 0) raiseError("The last argument must be a positive integer.");

    int fifo = open(argv[1], O_RDONLY);
    if (fifo == -1) raisePError("open");

    char buff[N + 10];
    size_t len;
    int i;
    int numOfProducers = 0;
    int maxNumOfProducers = 100;
    char lines[maxNumOfProducers][1000];
    for (int j = 0; j < maxNumOfProducers; ++j) lines[j][0] = '\n';
    while ((len = read(fifo, buff, N + 10)) > 0) {
        buff[len] = '\n';
        buff[len + 1] = '\0';
        char *content;
        i = (int) strtol(buff, &content, 10);
        ++content;
        if (lines[i - 1][0] == '\n') {
            strcpy(lines[i - 1], content);
            ++numOfProducers;
        }
        else strcat(lines[i - 1], content);
    }
    close(fifo);

    FILE *file = fopen(argv[2], "w");
    if (file == NULL) raisePError("fopen");
    i = 0;
    while (numOfProducers > 0) {
        if (lines[i][0] != '\n') --numOfProducers;
        fwrite(lines[i], 1, strlen(lines[i]), file);
        ++i;
    }

    fclose(file);
    return 0;
}
