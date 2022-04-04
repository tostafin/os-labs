#include "sigHelper.h"

int signalsToSend;
int receivedSignals = 0;
volatile bool receivingSignals = true;
SigMode sigMode;

void startHandler(int signo, siginfo_t *info, void *context) {
    puts("Sender received a confirmation from catcher.");
    ++receivedSignals;
}

void endHandler(int signo, siginfo_t *info, void *context) {
    receivingSignals = false;
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

    sigset_t mask;
    prepareMask(&mask, startSigNum, endSigNum);
    blockAllSignals();

    sendSignals(catcherPid, sigMode, signalsToSend, &mask, true);

    prepareMask(&mask, startSigNum, endSigNum);
    while (receivingSignals) {
        if (sigsuspend(&mask) != -1) raisePError("sigsuspend");
    }

    printf("Sender received %d signals out of %d sent to catcher (circa %.2f%%).\n", receivedSignals - signalsToSend,
           signalsToSend, ((double) (receivedSignals - signalsToSend) / (double) signalsToSend) * 100);
    return 0;
}