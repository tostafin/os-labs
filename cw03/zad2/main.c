#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

void raiseError(const char *message) {
    fprintf(stderr, "[ERROR]: %s", message);
    exit(EXIT_FAILURE);
}

double func(double x) {
    return 4 / (x * x + 1);
}

void savePartialCalculation(int n, pid_t *child_pid) {
    int interval[2] = {0, 1};
    double dx = (double) (interval[1] - interval[0]) / n;
    int i = 1;
    while (*child_pid != 0 && n--) {
        *child_pid = fork();
        double tempVal = func(interval[0] + i * dx) * dx;
        char fileName[100];
        sprintf(fileName, "w%d.txt", i);
        ++i;
        FILE *file = fopen(fileName, "w");
        if (file == NULL) {
            perror("Can't open a temp file");
            exit(EXIT_FAILURE);
        }

        fprintf(file, "%lf", tempVal);
        fclose(file);
    }
}

void collectFinalResult(int n, const pid_t *child_pid) {
    if (*child_pid != 0) {
        while (wait(NULL) > 0);

        double res = 0.0;
        if (waitpid(-1, NULL, WNOHANG) == -1) {
            for (int i = 1; i <= n; ++i) {
                char fileName[100];
                sprintf(fileName, "w%d.txt", i);
                FILE *file = fopen(fileName, "r");
                if (file == NULL) {
                    printf("%s", fileName);
                    perror("Can't open a temp file");
                    exit(EXIT_FAILURE);
                }

                double tmpVal;
                fscanf(file, "%lf", &tmpVal);
                res += tmpVal;
                fclose(file);
            }
        } else {
            raiseError("The child processes weren't terminated correctly.");
        }
        printf("%lf", res);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3)
        raiseError("You must pass exactly two arguments: the width of each rectangle and number of"
                   " child processes.");
    double width = strtod(argv[1], NULL);
    int n = (int) strtol(argv[2], NULL, 10);
    if (width <= 0 || n <= 0) raiseError("Wrong arguments passed.");

    pid_t child_pid;
    savePartialCalculation(n, &child_pid);
    collectFinalResult(n, &child_pid);

    return 0;
}
