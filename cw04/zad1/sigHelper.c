#include "sigHelper.h"

void raiseError(char *message) {
    fprintf(stderr, "%s\n", message);
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
}

void maskSignal(void) {
    sigset_t sigset;
    if (sigemptyset(&sigset) == -1) raisePError("sigemptyset");
    if (sigaddset(&sigset, SIGUSR1) == -1) raisePError("sigaddset");
    if (sigprocmask(SIG_BLOCK, &sigset, NULL) == -1) raisePError("sigprocmask");
}

void checkMask(void) {
    sigset_t sigset;
    if (sigprocmask(0, NULL, &sigset) == -1) raisePError("sigprocmask");
    int inherited;
    if ((inherited = sigismember(&sigset, SIGUSR1)) == -1) raisePError("sigismember");

    printf("Mask was%s inherited by the child process.\n", !inherited ? " not" : "");
}

void checkPending(void) {
    sigset_t sigset;
    if (sigpending(&sigset) == -1) raisePError("sigpending");
    int pending = sigismember(&sigset, SIGUSR1);
    if (pending == -1) raisePError("sigismember");

    printf("Status of the signal: %s\n", pending ? "pending" : "not pending");
}
