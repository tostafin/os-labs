#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <linux/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include <arpa/inet.h>

#define CLIENT_NAME_MAX_LEN 64

int networkSocketFd, localSocketFd;

typedef enum connectMode {
    NETWORK,
    LOCAL
} ConnectMode;

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

void parseArgv(int argc, char *argv[], char **clientName, ConnectMode *connectMode, char **serverAddress, uint16_t *serverPort) {
    if (argc != 4) raiseError("You must pass exactly three arguments.");

    if (strlen(argv[1]) > CLIENT_NAME_MAX_LEN) raiseError("Client's name length cannot be longer than 64.");
    *clientName = argv[1];

    if (strcmp(argv[2], "network") == 0) *connectMode = NETWORK;
    else if (strcmp(argv[2], "local") == 0) *connectMode = LOCAL;
    else raiseError("The connect mode must be either 'network' or 'local'.");

    char *portPtr;
    if ((portPtr = strchr(argv[3], ':')) == NULL) raiseError("Wrong server address passed.");
    *portPtr = '\0';
    *serverAddress = argv[3];
    *serverPort = (uint16_t) strtol(portPtr + 1, NULL, 10);
}

void connectToSocket(ConnectMode connectMode, char *serverAddress, uint16_t serverPort) {
    switch (connectMode) {
        case NETWORK:
            if ((networkSocketFd = socket(AF_INET, SOCK_STREAM, 0)) == -1) raisePError("socket");
            struct sockaddr_in sockaddrIn;
            struct in_addr inAddr;
            int inet_pton_res = inet_pton(AF_INET, serverAddress, &inAddr);
            if (inet_pton_res == 0) {
                raiseError("The address does not contain a character string representing a valid network address in the specified address family");
            }
            if (inet_pton_res == -1) {
                raisePError("inet_pton");
            }
            sockaddrIn.sin_addr = inAddr;
            sockaddrIn.sin_port = htons(serverPort);
            sockaddrIn.sin_family = AF_INET;
            sockaddrIn.sin_zero[0] = '\0';

            if (connect(networkSocketFd, (struct sockaddr *) &sockaddrIn, sizeof(struct sockaddr)) == -1) {
                perror("connect");
                exit(EXIT_FAILURE);
            }
        case LOCAL:
            if ((localSocketFd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) raisePError("socket");
            struct sockaddr_un addr;
            addr.sun_family = AF_UNIX;
            strcpy(addr.sun_path, serverAddress);
            if (bind(localSocketFd, (struct sockaddr *) &addr, sizeof(struct sockaddr)) == -1) raisePError("bind");
    }
}

int main(int argc, char *argv[]) {
    ConnectMode connectMode;
    char *clientName, *serverAddress;
    u_int16_t serverPort;
    parseArgv(argc, argv, &clientName, &connectMode, &serverAddress, &serverPort);

    connectToSocket(connectMode, serverAddress, serverPort);

    signal(SIGINT, SIGINTHandler);

    cleanSockets();

    return 0;
}
