#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

void raiseError(char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

void raisePError(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

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
    int numOfProducers = 2;
    char lines[numOfProducers][1000];
    for (int j = 0; j < numOfProducers; ++j) lines[j][0] = '\0';
    while ((len = read(fifo, buff, N + 10)) > 0) {
        buff[len] = '\n';
        buff[len + 1] = '\0';
        char *content;
        i = (int) strtol(buff, &content, 10);
        ++content;
        if (lines[i - 1][0] == '\0') strcpy(lines[i - 1], content);
        else strcat(lines[i - 1], content);
    }
    close(fifo);

    FILE *file = fopen(argv[2], "w");
    if (file == NULL) raisePError("fopen");
    for (int j = 0; j < numOfProducers; ++j) fwrite(lines[j], 1, strlen(lines[j]), file);

    fclose(file);
    return 0;
}
