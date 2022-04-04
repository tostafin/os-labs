#include "sigHelper.h"

int receivedSignals = 0;
volatile bool receivingSignals = true;
pid_t senderPID;
SigMode sigMode;
sigset_t mask;

void startHandler(int signo, siginfo_t *info, void *context) {
    if (sigMode == SIGQUEUE) printf("Received signal number %d\n", info->si_value.sival_int);

    senderPID = info->si_pid;
    sendConfirmation(senderPID, sigMode, &mask);
    ++receivedSignals;
}

void endHandler(int signo, siginfo_t *info, void *context) {
    receivingSignals = false;
    senderPID = info->si_pid;
}

int main(int argc, char *argv[]) {
    if (argc != 2) raiseError("You must pass exactly one of the following: KILL, SIGQUEUE, SIGRT");
    printf("Catcher PID: %d\n", getpid());
    sigMode = getMode(argv[1]);

    int startSigNum = getStartSigNum(sigMode), endSigNum = getEndSigNum(sigMode);
    prepareSigaction(startSigNum, startHandler);
    prepareSigaction(endSigNum, endHandler);

    prepareMask(&mask, startSigNum, endSigNum);
    blockAllSignals();

    while (receivingSignals) {
        if (sigsuspend(&mask) != -1) raisePError("sigsuspend");
    }

    prepareMask(&mask, startSigNum, endSigNum);
    sendSignals(senderPID, sigMode, receivedSignals, &mask, false);

    printf("Catcher received %d signals in total.\n", receivedSignals);
    return 0;
}
