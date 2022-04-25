#include "server.h"

MsgQueue *msgQueue;
pid_t clients[MAX_CLIENTS][3]; // idx - ClientId, [0] - ClientKey, [1] - ClientQueueId, [2] - ClientPID
volatile int numOfClients = 0;
msgbuf msgBuf;

void cleanAtExit(void) {
    removeQueue(msgQueue->id);
    free(msgQueue);
}

void handleInit(key_t clientQueueKey) {
    if (numOfClients >= MAX_CLIENTS) {
        puts("Too many clients connected to the server already.");
    } else {
        puts("Client received.");
        int clientId;
        printf("Client's PID: %d. Queue key: %d\n", msgBuf.id, clientQueueKey);
        for (clientId = 0; clientId < MAX_CLIENTS; ++clientId) {
            if (clients[clientId][0] == -1) break;
        }

        ++numOfClients;
        clients[clientId][0] = clientQueueKey;
        clients[clientId][1] = getQueueId(clientQueueKey);
        clients[clientId][2] = msgBuf.id;

        msgBuf.mtype = INIT;
        char clientIdStr[10];
        sprintf(clientIdStr, "%d", clientId);
        strcpy(msgBuf.mtext, clientIdStr);
        msgBuf.id = clientId;

        sendMsgToQueue(clients[clientId][1], &msgBuf);
        puts("Server sent the INIT communicate back to Client.");
    }
}

void receiveStopFromClient(int clientId) {
    puts("STOP received by Server.");
    clients[clientId][0] = -1;
    --numOfClients;
}

void writeToFile(FILE *file) {
    time_t myTime = time(NULL);
    char *timeStr = ctime(&myTime);
    fputs(timeStr, file);
    char clientIdStr[5];
    sprintf(clientIdStr, "%d\n", msgBuf.id);
    fputs(clientIdStr, file);
    fputs(msgBuf.mtext, file);
    fputs("\n\n", file);
}

void receiveCommunicates(void) {
    FILE *file = fopen("serverInfo.txt", "w");
    key_t clientQueueKey;
    while (true) {
        if (msgrcv(msgQueue->id, &msgBuf, MAX_MSG_SIZE, -5, 0) == -1) {
            cleanAtExit();
            raisePError("msgrcv");
        } else {
            clientQueueKey = (key_t) strtol(msgBuf.mtext, NULL, 10);
            writeToFile(file);
            switch (msgBuf.mtype) {
                case STOP:
                    receiveStopFromClient(msgBuf.id);
                    sendMsgToQueue(getQueueId(clientQueueKey), &msgBuf);
                    if (numOfClients == 0) return; // breaks the while loop
                    else break;
                case LIST:
                    break;
                case TO_ALL:
                    break;
                case TO_ONE:
                    break;
                case INIT:
                    handleInit(clientQueueKey);
                    break;
            }
        }
    }
}

void clearClientsArr() {
    for (int i = 0; i < MAX_CLIENTS; ++i) clients[i][0] = -1; // the unique key is never -1
}

void handleStop(void) {
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i][0] != -1) {
            msgBuf.mtype = STOP;
            msgBuf.id = i;
            sendMsgToQueue(clients[i][1], &msgBuf);
            if (msgrcv(clients[i][1], &msgBuf, MAX_MSG_SIZE, STOP, 0) == -1) {
                if (errno == EIDRM) --numOfClients;
                else raisePError("msgrcv");
            }
        }
    }
    if (numOfClients == 0) return;
    raiseError("Number of clients isn't zero yet.");
}

void SIGINTHandler(int sigNum) {
    puts("\nSIGINT sent by Server.");
    handleStop();
    exit(EXIT_SUCCESS);
}

int main(void) {
    puts("Server running...");
    clearClientsArr();
    printf("My PID: %d\n", getpid());
    msgQueue = createMsgQueue('s');
    puts("Server's queue created.");
    atexit(cleanAtExit);
    signal(SIGINT, SIGINTHandler);

    puts("Server ready for handling communicates.\n");
    receiveCommunicates();

    return 0;
}
