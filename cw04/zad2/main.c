#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <stdbool.h>

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
        // no exit here since we're exiting in the parent process
    } else {
        puts("SIGCHLD not working!");
        exit(EXIT_FAILURE);
    }
}

void SIGUSR1Handler(int signo, siginfo_t *info, void *context) {
    printf("Signal number: %d, PID: %d\n", signo, getpid());
    if (info->si_code == SI_QUEUE) {
        printf("SIGUSR1 working! Integer value passed by sigqueue function: %d\n", info->si_value.sival_int);
        exit(EXIT_SUCCESS);
    } else {
        puts("SIGUSR1 not working!");
        exit(EXIT_FAILURE);
    }
}

int SA_NODEFERCnt = 0;
bool cntReachMax = false;
void SA_NODEFERHandler(int signo) {
    printf("Signal number: %d, PID: %d\n", signo, getpid());
    printf("SA_NODEFERCnt = %d at the handler start\n", SA_NODEFERCnt);
    ++SA_NODEFERCnt;
    if (SA_NODEFERCnt <= 4) {
        raise(signo);
        if (SA_NODEFERCnt == 4) cntReachMax = true;
    }

    --SA_NODEFERCnt;
    printf("SA_NODEFERCnt = %d at the handler end\n", SA_NODEFERCnt);
}

void SA_RESETHANDHandler(int signo) {
    printf("Handler: SA_RESETHAND used on signal number %d\n", signo);
}

void prepareSigaction(int sigNum, void (*handler), int saFlags) {
    struct sigaction act;
    act.sa_sigaction = handler;
    act.sa_flags = saFlags;
    if (sigemptyset(&act.sa_mask)) raisePError("sigemptyset");
    if (sigaction(sigNum, &act, NULL)) raisePError("sigaction");
}

void testSA_SIGINFO(void) {
    puts("Testing flag SA_SIGINFO:");
    pid_t childPid;
    if ((childPid = fork()) == 0) {
        puts("Testing with SIGFPE:");
        prepareSigaction(SIGFPE, SIGFPEHandler, SA_SIGINFO);
        puts("Trying to do a 7 / 0 division");
        int divisor = 0;
        printf("%d", 7 / divisor);
    } else if (childPid == -1) {
        raisePError("fork");
    }
    if (wait(NULL) == -1) raisePError("wait");

    if ((childPid = fork()) == 0) {
        puts("\nTesting with SIGUSR1");
        prepareSigaction(SIGUSR1, SIGUSR1Handler, SA_SIGINFO);
        sigval_t value = {8};
        sigqueue(getpid(), SIGUSR1, value);
    } else if (childPid == -1) {
        raisePError("fork");
    }
    if (wait(NULL) == -1) raisePError("wait");

    puts("\nTesting with SIGCHLD:");
    prepareSigaction(SIGCHLD, SIGCHLDHandler, SA_SIGINFO);
    if ((childPid = fork()) == 0) {
        puts("Exiting from the child with exit status 7");
        exit(7);
    } else if (childPid == -1) {
        raisePError("fork");
    }
    if (wait(NULL) == -1) raisePError("wait");
    puts("SA_SIGINFO working!");
}

void testSA_NODEFER(void) {
    puts("\n\nTesting SA_NODEFER");
    puts("Expecting the counter to first rise to 4 and then go back to 0. Without the SA_NODEFER flag the counter "
         "would equal at most 1.");
    prepareSigaction(SIGUSR2, SA_NODEFERHandler, SA_NODEFER);
    raise(SIGUSR2);
    if (cntReachMax) puts("SA_NODEFER working!");
}

void testSA_RESETHAND(void) {
    puts("\n\nTesting SA_RESETHAND");
    prepareSigaction(SIGCHLD, SA_RESETHANDHandler, SA_RESETHAND);
    puts("Resetting SIGCHLD to default");
    raise(SIGCHLD);
    puts("Raising SIGCHLD to get the default behavior: ignore");
    raise(SIGCHLD);
    puts("SA_RESETHAND working!");
}

int main() {
    testSA_SIGINFO();
    testSA_NODEFER();
    testSA_RESETHAND();

    return 0;
}
