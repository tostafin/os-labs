#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <linux/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>

int networkSocketFd, localSocketFd;

void raiseError(const char *message) {
    fprintf(stderr, "[ERROR]: %s\n", message);
    exit(EXIT_FAILURE);
}

void raisePError(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void cleanSockets(void) {
    shutdown(networkSocketFd, SHUT_RDWR);
    close(networkSocketFd);
    shutdown(localSocketFd, SHUT_RDWR);
    close(localSocketFd);
}

void SIGINTHandler(int sigNum) {
    cleanSockets();
    exit(EXIT_SUCCESS);
}

void parseArgv(int argc, char *argv[], uint16_t *portNum, char **socketPath) {
    if (argc != 3) raiseError("You must pass exactly two arguments.");

    *portNum = (uint16_t) strtol(argv[1], NULL, 10);
    if (*portNum < 32768 || *portNum > 60999) raiseError("The port must be an integer between 32768 and 60999.");

    if (strlen(argv[2]) > UNIX_PATH_MAX) raiseError("The socket path length cannot be longer than 108.");
    *socketPath = argv[2];
}

void initSockets(uint16_t portNum, char *socketPath) {
    // network socket
    if ((networkSocketFd = socket(AF_INET, SOCK_STREAM, 0)) == -1) raisePError("socket");
    struct sockaddr_in sockaddrIn;
    struct in_addr inAddr = {INADDR_ANY};
    sockaddrIn.sin_addr = inAddr;
    sockaddrIn.sin_port = htons(portNum);
    sockaddrIn.sin_family = AF_INET;
    sockaddrIn.sin_zero[0] = '\0';
    if (bind(networkSocketFd, (struct sockaddr *) &sockaddrIn, sizeof(struct sockaddr)) == -1) raisePError("bind");

    // local socket
    if ((localSocketFd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) raisePError("socket");
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socketPath);
    unlink(socketPath);
    if (bind(localSocketFd, (struct sockaddr *) &addr, sizeof(struct sockaddr)) == -1) raisePError("bind");
}

int main(int argc, char *argv[]) {
    uint16_t portNum;
    char *socketPath;
    parseArgv(argc, argv, &portNum, &socketPath);

    initSockets(portNum, socketPath);

    signal(SIGINT, SIGINTHandler);

    cleanSockets();

    return 0;
}
