#include "common.h"

typedef enum {
    DRAW,
    WIN,
    LOSE,
    NONE
} Result;

int socketFd;

void cleanSockets(void) {
    shutdown(socketFd, SHUT_RDWR);
    close(socketFd);
}

void SIGINTHandler(int sigNum) {
    cleanSockets();
    exit(EXIT_SUCCESS);
}

void parseArgv(int argc, char *argv[], char **clientName, size_t *clientNameLen, ConnectMode *connectMode,
               char **serverAddress, uint16_t *serverPort) {
    if (argc != 4) raiseError("You must pass exactly three arguments.");

    if ((*clientNameLen = strlen(argv[1])) > CLIENT_NAME_MAX_LEN)
        raiseError("Client's name length cannot be longer than 64.");
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
                raiseError(
                        "The address does not contain a character string representing a valid network address in the specified address family");
            }
            if (inet_pton_res == -1) {
                raisePError("inet_pton");
            }

            sockaddrIn.sin_addr = inAddr;
            sockaddrIn.sin_port = htons(serverPort);
            sockaddrIn.sin_family = AF_INET;
            sockaddrIn.sin_zero[0] = '\0';
            if (connect(socketFd, (struct sockaddr *) &sockaddrIn, sizeof(struct sockaddr)) == -1)
                raisePError("connect1");
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

void drawBoard(const char board[]) {
    puts("_ _ _");
    for (int i = 0; i < 9; ++i) {
        if (board[i] != '\0') {
            printf("%c", board[i]);
        } else {
            printf("_");
        }

        if (i % 3 != 2) {
            printf("|");
        } else {
            printf("\n");
        }
    }
    puts("");
}

Result checkWinner(const char *board, int move, char mySign) {
    char boardArr[3][3];
    int movesCnt = 0;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            boardArr[i][j] = board[3 * i + j];
            if (boardArr[i][j] == 'X' || boardArr[i][j] == 'O') ++movesCnt;
        }
    }
    int x = (move - 1) / 3;
    int y = (move - 1) % 3;
    int n = 3;

    //check col
    for (int i = 0; i < n; i++) {
        if (boardArr[x][i] != mySign)
            break;
        if (i == n - 1) {
            return WIN;
        }
    }

    //check row
    for (int i = 0; i < n; i++) {
        if (boardArr[i][y] != mySign)
            break;
        if (i == n - 1) {
            return WIN;
        }
    }

    //check diag
    if (x == y) {
        //we're on a diagonal
        for (int i = 0; i < n; i++) {
            if (boardArr[i][i] != mySign)
                break;
            if (i == n - 1) {
                return WIN;
            }
        }
    }

    //check anti diag
    if (x + y == n - 1) {
        for (int i = 0; i < n; i++) {
            if (boardArr[i][(n - 1) - i] != mySign)
                break;
            if (i == n - 1) {
                return WIN;
            }
        }
    }

    //check draw
    if (movesCnt == 8) {
        return DRAW;
    }

    return NONE;
}

void handleClients(char *clientName, size_t clientNameLen) {
    // sending Client's name to Server
    ssize_t nWrote;
    if ((nWrote = write(socketFd, clientName, clientNameLen)) != clientNameLen) {
        if (nWrote == -1) raisePError("write");
        raiseError("Client's name wasn't sent properly.");
    }

    // waiting for confirmation (saved)
    char response[RESPONSE_MAX_SIZE];
    read(socketFd, response, RESPONSE_MAX_SIZE);
    if (strcmp(response, "busy") == 0) {
        cleanSockets();
        raiseError("Your name is already occupied by another Client.");
    }
    if (strcmp(response, "saved") == 0) {
        puts("Server saved your name.");
        write(socketFd, "received", 9);
    }

    char mySign;
    int move = 0;
    bool gameOver = false;
    while (true) {
        read(socketFd, response, RESPONSE_MAX_SIZE);
        puts("Received a message from the server.");
        if (response[0] == 'x' || response[0] == 'o') {
            mySign = (char) toupper(response[0]);
            printf("I've been assigned %c\n", mySign);
        } else {
            if (response[0] == 'L') {
                puts("You lost!");
                break;
            } else if (response[0] == 'D') {
                puts("The game has been drawn!");
                break;
            }
            drawBoard(response);
            while (true) {
                puts("Your move (1-9):");
                scanf("%d", &move);
                if (move < 1 || move > 9) {
                    puts("Move out of range. Pick a number within a range 1-9.");
                } else if (response[move- 1] != '\0') {
                    puts("This field is already marked. Pick a different one.");
                } else {
                    break;
                }
            }
            response[move - 1] = mySign;
            switch (checkWinner(response, move, mySign)) {
                case DRAW:
                    response[0] = 'D';
                    response[1] = '\0';
                    puts("The game has been drawn!");
                    gameOver = true;
                    break;
                case WIN:
                    response[0] = 'W';
                    response[1] = '\0';
                    puts("I WON :)");
                    gameOver = true;
                    break;
                case LOSE:
                case NONE:
                    break;
            }
            write(socketFd, response, RESPONSE_MAX_SIZE);
            puts("Sent the result to the server.");
            if (gameOver) break;
        }
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
