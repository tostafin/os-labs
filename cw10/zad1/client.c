#include "common.h"

int socketFd;

void cleanSockets(void) {
    shutdown(socketFd, SHUT_RDWR);
    close(socketFd);
}

void SIGINTHandler(int sigNum) {
    cleanSockets();
    exit(EXIT_SUCCESS);
}

void parseArgv(int argc, char *argv[], char **clientName, size_t *clientNameLen, ConnectMode *connectMode, char **serverAddress, uint16_t *serverPort) {
    if (argc != 4) raiseError("You must pass exactly three arguments.");

    if ((*clientNameLen = strlen(argv[1])) > CLIENT_NAME_MAX_LEN) raiseError("Client's name length cannot be longer than 64.");
    *clientName = argv[1];

    if (strcmp(argv[2], "network") == 0) *connectMode = NETWORK;
    else if (strcmp(argv[2], "local") == 0) *connectMode = LOCAL;
    else raiseError("The connect mode must be either 'network' or 'local'.");
    *serverAddress = argv[3];

    if (*connectMode == NETWORK) {
        char *portPtr;
        if ((portPtr = strchr(argv[3], ':')) == NULL) raiseError("Wrong server address passed.");
        *portPtr = '\0';
        *serverPort = (uint16_t) strtol(portPtr + 1, NULL, 10);
    }
}

void connectToSocket(ConnectMode connectMode, char *serverAddress, uint16_t serverPort) {
    switch (connectMode) {
        case NETWORK:
            if ((socketFd = socket(AF_INET, SOCK_STREAM, 0)) == -1) raisePError("socket");

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
            if (connect(socketFd, (struct sockaddr *) &sockaddrIn, sizeof(struct sockaddr)) == -1) raisePError("connect1");
            break;
        case LOCAL:
            if ((socketFd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) raisePError("socket");

            struct sockaddr_un addr;
            addr.sun_family = AF_UNIX;
            strcpy(addr.sun_path, serverAddress);
            if (connect(socketFd, (struct sockaddr *) &addr, sizeof(struct sockaddr)) == -1) raisePError("connect2");
            break;
    }
}

void handleClients(char *clientName, size_t clientNameLen) {
    // sending Client's name to Server
    ssize_t nWrote;
    if ((nWrote = write(socketFd, clientName, clientNameLen)) != clientNameLen) {
        if (nWrote == -1) raisePError("write");
        raiseError("Client's name wasn't sent properly.");
    }

    // waiting for confirmation (saved)
    char response[6];
    read(socketFd, response, 6);
    if (strcmp(response, "busy") == 0) {
        cleanSockets();
        raiseError("Your name is already occupied by another Client.");
    }
    if (strcmp(response, "saved") == 0) {
        puts("Server saved your name.");
    }

    while (true) {
    }

}

int main(int argc, char *argv[]) {
    ConnectMode connectMode;
    char *clientName, *serverAddress;
    size_t clientNameLen;
    u_int16_t serverPort;
    parseArgv(argc, argv, &clientName, &clientNameLen, &connectMode, &serverAddress, &serverPort);

    printf("Hello, %s!\n", clientName);

    connectToSocket(connectMode, serverAddress, serverPort);

    signal(SIGINT, SIGINTHandler);

    handleClients(clientName, clientNameLen);

    cleanSockets();

    return 0;
}
