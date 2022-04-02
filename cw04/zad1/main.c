#include "sigHelper.h"

void handleSignal(int sigNum) {
    printf("Handling signal number %d.\n", sigNum);
}

int main(int argc, char *argv[]) {
    if (argc != 2) raiseError("You must pass exactly one argument.");

    Mode mode = getMode(argv[1]);
    printf("\n\n%s:\n", argv[1]);
    switch (mode) {
        case IGNORE:
            signal(SIGUSR1, SIG_IGN);
            break;

        case HANDLER:
            signal(SIGUSR1, handleSignal);
            break;

        case MASK:
        case PENDING:
            maskSignal();
            break;
    }

    raise(SIGUSR1);
    if (mode == PENDING) checkPending();

    pid_t childPid;
    if ((childPid = fork()) == 0) {
        switch (mode) {
            case IGNORE:
                puts("\nIgnore signal in the child process:");
                raise(SIGUSR1);
                break;

            case HANDLER:
                puts("\nHandler in the child process:\n");
                raise(SIGUSR1);
                break;

            case MASK:
                puts("\nMask in the child process: ");
                raise(SIGUSR1);
                checkMask();
                break;

            case PENDING:
                puts("\nPending status in the child process:");
                checkPending();
                break;
        }

        if (mode != HANDLER) {
            if (execl("./execTest", "execTest", argv[1], NULL) == -1) raisePError("execl");
        }

    } else if (childPid == -1) {
        perror("Fork");
        abort();
    }

    wait(0);

    return 0;
}
