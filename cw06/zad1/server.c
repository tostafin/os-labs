#include "server.h"

MsgQueue *msgQueue;
pid_t clients[MAX_CLIENTS][2]; // idx - ClientId, [0] - ClientKey, [1] - ClientQueueId
int numOfClients = 0;
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

        msgBuf.mtype = INIT;
        char clientIdStr[10];
        sprintf(clientIdStr, "%d", clientId);
        strcpy(msgBuf.mtext, clientIdStr);
        msgBuf.id = clientId;

        sendMsgToQueue(clients[clientId][1], &msgBuf);
        puts("Server sent the INIT communicate back to Client.");
    }
}

void handleStop(int clientId) {
    puts("STOP received by Server.");
    clients[clientId][0] = -1;
    --numOfClients;
}

void handleCommunicates(void) {
    key_t clientQueueKey;
    while (true) {
        if (msgrcv(msgQueue->id, &msgBuf, MAX_MSG_SIZE, -5, 0) == -1) {
            cleanAtExit();
            raisePError("msgrcv");
        } else {
            clientQueueKey = (key_t) strtol(msgBuf.mtext, NULL, 10);
            switch (msgBuf.mtype) {
                case STOP:
                    handleStop(msgBuf.id);
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

int main(void) {
    puts("Server running...");
    clearClientsArr();
    printf("My PID: %d\n", getpid());
    msgQueue = createMsgQueue('s');
    puts("Server's queue created.");
    atexit(cleanAtExit);

    puts("Server ready for handling communicates.\n");
    handleCommunicates();

    return 0;
}
