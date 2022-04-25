#include "common.h"

void raiseError(char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

void raisePError(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void removeQueue(int queueId) {
    if (msgctl(queueId, IPC_RMID, NULL) == -1) raisePError("msgctl");
}

MsgQueue *createMsgQueue(char provider) {
    MsgQueue *msgQueue = malloc(sizeof(*msgQueue));
    key_t queueKey;
    if (provider == 's') queueKey = ftok(getenv("HOME"), KEY_GEN_NUM);
    else if (provider == 'c') queueKey = ftok(getenv("HOME"), getpid());
    else raiseError("Wrong provider: must be either 's' (server) or 'c' (client).");
    msgQueue->key = queueKey;
    msgQueue->provider = provider;

    if (queueKey == -1) raisePError("ftok");
    msgQueue->id = msgget(queueKey, IPC_CREAT | 0666 | IPC_EXCL);
    if (msgQueue->id == -1) raisePError("msgget");

    return msgQueue;
}

void sendMsgToQueue(int queueId, msgbuf *msgBuf) {
    msgsnd(queueId, msgBuf, MAX_MSG_SIZE, 0);
}

int getQueueId(key_t key) {
    key_t queueKey;
    if (key == -1) {
        if ((queueKey = ftok(getenv("HOME"), KEY_GEN_NUM)) == -1) {
            raisePError("ftok");
        }
    } else {
        queueKey = key;
    }
    int id = msgget(queueKey, 0);
    if (id == -1) raisePError("msgget");
    return id;
}