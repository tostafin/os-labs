#include "sigHelper.h"

void raiseError(char *message) {
    printf("[ERROR]: %s\n", message);
    exit(EXIT_FAILURE);
}

void raisePError(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

SigMode getMode(char *mode) {
    if (strcmp(mode, "KILL") == 0) return KILL;
    if (strcmp(mode, "SIGQUEUE") == 0) return SIGQUEUE;
    if (strcmp(mode, "SIGRT") == 0) return SIGRT;

    raiseError("Wrong argument passed.");
    exit(EXIT_FAILURE);
}

void prepareSigaction(int sigNum, void (*handler)(int, siginfo_t*, void*)) {
    struct sigaction act;
    act.sa_sigaction = handler;
    act.sa_flags = SA_SIGINFO;
    if (sigemptyset(&act.sa_mask)) raisePError("sigemptyset");
    if (sigaction(sigNum, &act, NULL)) raisePError("sigaction");
}

int getStartSigNum(SigMode sigMode) {
    switch (sigMode) {
        case KILL:
        case SIGQUEUE:
            return SIGUSR1;

        case SIGRT:
            return SIGRTMIN;

        default:
            return -1;
    }
}

int getEndSigNum(SigMode sigMode) {
    switch (sigMode) {
        case KILL:
        case SIGQUEUE:
            return SIGUSR2;

        case SIGRT:
            return SIGRTMIN + 1;

        default:
            return -1;
    }
}

void prepareMask(sigset_t *mask, int startSigNum, int stopSigNum) {
    if (sigfillset(mask) == -1) raisePError("sigfillset");
    if (sigdelset(mask, startSigNum) == -1) raisePError("sigdelset");
    if (sigdelset(mask, stopSigNum) == -1) raisePError("sigdelset");
    if (sigprocmask(SIG_BLOCK, mask, NULL) == -1) raisePError("sigprocmask");
}

void sendSignals(pid_t pid, SigMode sigMode, int numOfSignals) {
    switch (sigMode) {
        case KILL:
            for (int i = 0; i < numOfSignals; ++i) {
                if (kill(pid, SIGUSR1) == -1) raisePError("kill");
            }
            if (kill(pid, SIGUSR2) == -1) raisePError("kill");
            break;

        case SIGQUEUE:
            ;sigval_t sigNo;
            for (int i = 1; i <= numOfSignals; ++i) {
                sigNo.sival_int = i;
                if (sigqueue(pid, SIGUSR1, sigNo) == -1) raisePError("sigqueue");
            }
            sigNo.sival_int = numOfSignals;
            if (sigqueue(pid, SIGUSR2, sigNo) == -1) raisePError("sigqueue");
            break;

        case SIGRT:
            for (int i = 0; i < numOfSignals; ++i) {
                if (kill(pid, SIGRTMIN) == -1) raisePError("kill");
            }
            if (kill(pid, SIGRTMIN + 1) == -1) raisePError("kill");
            break;

        default:
            raiseError("Wrong signal mode.");
    }
}
