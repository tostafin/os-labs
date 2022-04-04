#include "sigHelper.h"

int signalsToSend;
int receivedSignals = 0;
volatile bool sendingSignals = true;
SigMode sigMode;

void startHandler(int signo, siginfo_t *info, void *context) {
    if (sigMode == SIGQUEUE) printf("Received signal number %d\n", info->si_value.sival_int);
    ++receivedSignals;
}

void endHandler(int signo, siginfo_t *info, void *context) {
    sendingSignals = false;
}

int main(int argc, char *argv[]) {
    if (argc != 4) raiseError("You must pass exactly three arguments.");
    pid_t catcherPid = (pid_t) strtol(argv[1], NULL, 10);
    if (catcherPid <= 0) raiseError("You must pass a correct catcher PID.");
    signalsToSend = (int) strtol(argv[2], NULL, 10);
    if (signalsToSend <= 0) raiseError("You must pass a correct number of processes to be sent.");

    sigMode = getMode(argv[3]);
    int startSigNum = getStartSigNum(sigMode), endSigNum = getEndSigNum(sigMode);

    prepareSigaction(startSigNum, startHandler);
    prepareSigaction(endSigNum, endHandler);

    sendSignals(catcherPid, sigMode, signalsToSend);

    sigset_t mask;
    prepareMask(&mask, sigMode, startSigNum, endSigNum);

    while (sendingSignals) {
        if (sigsuspend(&mask) != -1) raisePError("sigsuspend");
    }

    printf("Sender received %d signals out of %d sent to catcher (circa %.2f%%).\n", receivedSignals,
           signalsToSend, ((double) receivedSignals / (double) signalsToSend) * 100);
    return 0;
}