#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

typedef enum mode {
    IGNORE,
    HANDLER,
    MASK,
    PENDING
} Mode;

void raiseError(char *message) {
    fprintf(stderr, "%s", message);
    exit(EXIT_FAILURE);
}

void raisePError(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

Mode getMode(char *mode) {
    if (strcmp(mode, "ignore") == 0) return IGNORE;
    if (strcmp(mode, "handler") == 0) return HANDLER;
    if (strcmp(mode, "mask") == 0) return MASK;
    if (strcmp(mode, "pending") == 0) return PENDING;

    raiseError("The passed argument must be one of the following: ignore, handler, mask, pending");
    exit(EXIT_FAILURE);  // TODO: remove
}

void handleSignal(int sigNum) {
    printf("Handling signal number %d.\n", sigNum);
}

void maskSignal(void) {
    sigset_t sigset;
    if (sigemptyset(&sigset) == -1) raisePError("sigemptyset");
    if (sigaddset(&sigset, SIGUSR1) == -1) raisePError("sigaddset");
    if (sigprocmask(SIG_BLOCK, &sigset, NULL) == -1) raisePError("sigprocmask");
}

void checkPending(void) {
    sigset_t sigset;
    if (sigpending(&sigset) == -1) raisePError("sigpending");
    int pending = sigismember(&sigset, SIGUSR1);
    if (pending == -1) raisePError("sigismember");

    printf("Status of the signal: %s\n", pending ? "pending" : "not pending");
}

void checkMask(void) {
    sigset_t sigset;
    if (sigprocmask(0, NULL, &sigset) == -1) raisePError("sigprocmask");
    int inherited;
    if ((inherited = sigismember(&sigset, SIGUSR1)) == -1) raisePError("sigismember");

    printf("Mask was%s inherited by the child process.", !inherited ? " not" : "");
}

int main(int argc, char *argv[]) {
    if (argc != 2) raiseError("You must pass exactly one argument.");

    Mode mode = getMode(argv[1]);
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
            raise(SIGUSR1);
            break;
    }

    raise(SIGUSR1);
    if (mode == PENDING) checkPending();

    pid_t childPid;
    if ((childPid = fork()) == 0) {
        if (mode != PENDING) raise(SIGUSR1);
        else checkPending();
        if (mode == MASK) checkMask();
    } else if (childPid == -1) {
        perror("Fork");
        abort();
    }

    return 0;
}

