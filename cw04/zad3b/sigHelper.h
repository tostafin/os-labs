#ifndef SIGHELPER_H
#define SIGHELPER_H
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

typedef enum sig_mode {
    KILL,
    SIGQUEUE,
    SIGRT
} SigMode;

void raiseError(char *message);
void raisePError(char *message);
SigMode getMode(char *mode);
void prepareSigaction(int sigNum, void (*handler)(int, siginfo_t*, void*));
int getStartSigNum(SigMode sigMode);
int getEndSigNum(SigMode sigMode);
void prepareMask(sigset_t *mask, int startSigNum, int stopSigNum);
void blockAllSignals(void);
void sendSignals(pid_t pid, SigMode sigMode, int numOfSignals, sigset_t *mask, bool confirmingSignals);
void sendConfirmation(pid_t pid, SigMode sigMode, sigset_t *mask);
#endif //SIGHELPER_H
