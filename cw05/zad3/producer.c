#include "common.h"

// producer pipePath i filePath N
int main(int argc, char *argv[]) {
    if (argc != 5) raiseError("You must pass exactly 4 arguments.");
    size_t N = (size_t) strtol(argv[4], NULL, 10);
    if (N <= 0) raiseError("The last argument must be a positive integer.");

    int fifo = open(argv[1], O_WRONLY);
    if (fifo == -1) raisePError("open");

    FILE *file = fopen(argv[3], "r");
    if (file == NULL) raisePError("fopen");
    char buff[N + 10]; // the first few positions for i, then \n, then the actual content and '\0' at the end
    strcpy(buff, argv[2]);
    size_t iLen = strlen(argv[2]);
    char *content = &buff[iLen]; // first index after i
    *content = '\n';
    ++content;
    size_t len;
    srand(time(NULL));
    while ((len = fread(content, 1, N, file)) > 0) {
        buff[iLen + 1 + len] = '\0';
        sleep(rand() % 2 + 1);
        write(fifo, buff, N + 10);
        content = &buff[iLen] + 1;
    }
    close(fifo);
    fclose(file);
    return 0;
}
