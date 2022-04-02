#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>

void raisePError(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void SIGFPEHandler(int signo, siginfo_t *info, void *context) {
    printf("Signal number: %d, PID: %d\n", signo, getpid());
    if (info->si_code == FPE_INTDIV) {
        puts("SIGFPE working!");
        exit(EXIT_SUCCESS);
    } else {
        puts("SIGFPE not working!");
        exit(EXIT_FAILURE);
    }

}

void SIGCHLDHandler(int signo, siginfo_t *info, void *context) {
    printf("Signal number: %d, PID: %d\n", signo, getpid());
    if (info->si_code == CLD_EXITED) {
        printf("SIGCHLD working! Exit status of the child: %d\n", info->si_status);
        exit(EXIT_SUCCESS);
    } else {
        puts("SIGCHLD not working!");
        exit(EXIT_FAILURE);
    }
}

void prepareSigaction(int sigNum, void (*handler), int saFlags) {
    struct sigaction act;
    act.sa_sigaction = handler;
    act.sa_flags = saFlags;
    sigemptyset(&act.sa_mask);
    sigaction(sigNum, &act, NULL);
}

int main() {
    puts("Testing flag SA_SIGINFO:");
    pid_t childPid;
    if ((childPid = fork()) == 0) {
        puts("Testing SIGFPE:");
        prepareSigaction(SIGFPE, SIGFPEHandler, SA_SIGINFO);
        puts("Trying to do a 7 / 0 division");
        int divisor = 0;
        printf("%d", 7 / divisor);
    } else if (childPid == -1) {
        raisePError("fork");
    }
    wait(NULL);

    puts("\nTesting SIGCHLD:");
    prepareSigaction(SIGCHLD, SIGCHLDHandler, SA_SIGINFO);
    if ((childPid = fork()) == 0) {
        puts("Exiting from the child with exit status 7");
        exit(7);
    } else if (childPid == -1) {
        raisePError("fork");
    }
    wait(NULL);


    return 0;
}
