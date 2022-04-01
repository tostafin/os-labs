#ifndef ZAD1_SIGHELPER_H
#define ZAD1_SIGHELPER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>

typedef enum mode {
    IGNORE,
    HANDLER,
    MASK,
    PENDING
} Mode;

void raiseError(char *message);
void raisePError(char *message);
Mode getMode(char *mode);
void maskSignal(void);
void checkMask(void);
void checkPending(void);

#endif //ZAD1_SIGHELPER_H
