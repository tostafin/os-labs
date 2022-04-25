#ifndef CLIENT_COMMON_H
#define CLIENT_COMMON_H

#define KEY_GEN_NUM 'A'
#define MTEXT_MAX_LEN 100
#define MAX_CLIENTS 10

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

typedef struct msgQueue {
    int id;
    key_t key;
    char provider;
} MsgQueue;

typedef enum msgType {
    STOP = 1,
    LIST,
    TO_ALL,
    TO_ONE,
    INIT
} MsgType;

typedef struct msgbuf {
    long mtype;
    char mtext[MTEXT_MAX_LEN];
    int id;
} msgbuf;

#define MAX_MSG_SIZE (sizeof(msgbuf) - sizeof(long))

void raiseError(char *message);
void raisePError(char *message);
void removeQueue(int queueId);
MsgQueue *createMsgQueue(char provider);
void sendMsgToQueue(int queueId, msgbuf *msgBuf);
int getQueueId(key_t key);

#endif //CLIENT_COMMON_H
