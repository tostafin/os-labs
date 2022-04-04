#include "sigHelper.h"

int receivedSignals = 0;
volatile bool receivingSignals = true;
pid_t senderPID;

void startHandler(int signo, siginfo_t *info, void *context) {
    ++receivedSignals;
}

void endHandler(int signo, siginfo_t *info, void *context) {
    receivingSignals = false;
    senderPID = info->si_pid;
}

int main(int argc, char *argv[]) {
    if (argc != 2) raiseError("You must pass exactly one of the following: KILL, SIGQUEUE, SIGRT");
    printf("Catcher PID: %d\n", getpid());
    SigMode sigMode = getMode(argv[1]);

    int startSigNum = getStartSigNum(sigMode), endSigNum = getEndSigNum(sigMode);
    prepareSigaction(startSigNum, startHandler);
    prepareSigaction(endSigNum, endHandler);

    sigset_t mask;
    prepareMask(&mask, sigMode, startSigNum, endSigNum);

    while (receivingSignals) {
        if (sigsuspend(&mask) != -1) raisePError("sigsuspend");
    }

    sendSignals(senderPID, sigMode, receivedSignals);

    printf("Catcher received %d signals in total.\n", receivedSignals);
    return 0;
}
