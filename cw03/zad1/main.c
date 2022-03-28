#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void raiseError(char *message) {
    fprintf(stderr, "[ERROR]: %s", message);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if (argc != 2) raiseError("You must pass exactly one argument: number of child processes.");
    long n = strtol(argv[1], NULL, 10);
    if (n <= 0) raiseError("Incorrect input: you must pass a positive integer.");

    pid_t childPid;
    for (int i = 0; i < n; ++i) {
        if ((childPid = fork()) == 0) {
            printf("I am a child process and my pid is %d.\n", getpid());
            exit(EXIT_SUCCESS);
        } else if (childPid == -1) {
            perror("Forking error");
            abort();
        }
    }

    return 0;
}