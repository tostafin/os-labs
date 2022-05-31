#include "common.h"

#define MAX_NUM_OF_CLIENTS 16

int networkSocketFd, localSocketFd;

pthread_mutex_t clientsMutex = PTHREAD_MUTEX_INITIALIZER;
typedef enum {
    WAIT = -2,
    WAIT_SENT = -1
} Opponent;

typedef struct client {
    char names[CLIENT_NAME_MAX_LEN];
    int fd;
    ConnectMode connectMode;
    Opponent opponent;
} Client;
Client clients[MAX_NUM_OF_CLIENTS];

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
    if ((networkSocketFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1) raisePError("socket");
    struct sockaddr_in sockaddrIn;
    struct in_addr inAddr = {INADDR_ANY};
    sockaddrIn.sin_addr = inAddr;
    sockaddrIn.sin_port = htons(portNum);
    sockaddrIn.sin_family = AF_INET;
    sockaddrIn.sin_zero[0] = '\0';
    if (bind(networkSocketFd, (struct sockaddr *) &sockaddrIn, sizeof(struct sockaddr)) == -1) raisePError("bind");

    // local socket
    if ((localSocketFd = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1) raisePError("socket");
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socketPath);
    unlink(socketPath);
    if (bind(localSocketFd, (struct sockaddr *) &addr, sizeof(struct sockaddr)) == -1) raisePError("bind");

    for (int i = 0; i < MAX_NUM_OF_CLIENTS; ++i) {
        clients[i].opponent = WAIT;
    }
    errno = 0;
}

void closeConnectionWithClient(int fd) {
    shutdown(fd, SHUT_RDWR);
    close(fd);
}

void *listenFunc(void *arg) {
    listen(networkSocketFd, MAX_NUM_OF_CLIENTS / 2);
    listen(localSocketFd, MAX_NUM_OF_CLIENTS / 2);

    ConnectMode connectMode = -1;
    int clientFd;
    char clientName[CLIENT_NAME_MAX_LEN];
    while (true) {
        // TODO: use epoll/poll/select
        if ((clientFd = accept(networkSocketFd, NULL, NULL)) != -1) connectMode = NETWORK;
        else if ((clientFd = accept(localSocketFd, NULL, NULL)) != -1) connectMode = LOCAL;

        if (connectMode != -1) {
            read(clientFd, clientName, CLIENT_NAME_MAX_LEN);
            if ((errno = pthread_mutex_lock(&clientsMutex)) != 0) raisePError("pthread_mutex_lock");
            for (int i = 0; i < MAX_NUM_OF_CLIENTS; ++i) {
                if (strcmp(clients[i].names, clientName) == 0) {
                    printf("Client named %s tried to connect, but there's already Client with that name connected.", clientName);
                    write(clientFd, "busy", 5);
                    closeConnectionWithClient(clientFd);
                    goto continueWhile;
                }
            }
            for (int i = 0; i < MAX_NUM_OF_CLIENTS; ++i) {
                if (clients[i].names[0] == '\0') {
                    strcpy(clients[i].names, clientName);
                    clients[i].fd = clientFd;
                    clients[i].connectMode = connectMode;
                    break;
                }
            }
            if ((errno = pthread_mutex_unlock(&clientsMutex)) != 0) raisePError("pthread_mutex_unlock");
            printf("Client named %s saved.\n", clientName);
            write(clientFd, "saved", 6);
        }
        continueWhile:
        connectMode = -1;
    }
}

void *gameFunc(void *arg) {
    while (true) {
        if ((errno = pthread_mutex_lock(&clientsMutex)) != 0) raisePError("pthread_mutex_lock");
        for (int i = 0; i < MAX_NUM_OF_CLIENTS; ++i) {
            if (clients[i].names[0] != '\0') {
                if (clients[i].opponent == WAIT) {
                    write(clients[i].fd, "wait", 5);
                    clients[i].opponent = WAIT_SENT;
                }
            }
        }
        if ((errno = pthread_mutex_unlock(&clientsMutex)) != 0) raisePError("pthread_mutex_unlock");
    }

    return NULL;
}

void *pingFunc(void *arg) {
    return NULL;
}

void handleServer(void) {
    pthread_t listenThread, gameThread, pingThread;

    pthread_create(&listenThread, NULL, listenFunc, NULL);
    pthread_create(&gameThread, NULL, gameFunc, NULL);
    pthread_create(&pingThread, NULL, pingFunc, NULL);

    pthread_join(listenThread, NULL);
    pthread_join(gameThread, NULL);
    pthread_join(pingThread, NULL);
}

int main(int argc, char *argv[]) {
    uint16_t portNum;
    char *socketPath;
    parseArgv(argc, argv, &portNum, &socketPath);

    initSockets(portNum, socketPath);

    signal(SIGINT, SIGINTHandler);

    handleServer();

    cleanSockets();

    return 0;
}
