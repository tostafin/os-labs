#include <fcntl.h>
#include "client.h"

MsgQueue *msgQueue;
int serverQueueId;
int myId;
msgbuf msgBuf;
volatile bool stop = false;

void cleanAtExit(void) {
    removeQueue(msgQueue->id);
    free(msgQueue);
}

void sendInit(void) {
    char keyStr[MTEXT_MAX_LEN];
    sprintf(keyStr, "%d", msgQueue->key);
    msgBuf.mtype = INIT;
    strcpy(msgBuf.mtext, keyStr);
    msgBuf.id = getpid();
    sendMsgToQueue(serverQueueId, &msgBuf);
}

void handleInit(void) {
    if (msgrcv(msgQueue->id, &msgBuf, MAX_MSG_SIZE, -5, 0) == -1) {
        cleanAtExit();
        raisePError("msgrcv");
    } else {
        if (msgBuf.mtype != INIT) raiseError("Client didn't receive the INIT communicate.");
        myId = msgBuf.id;
    }
}

MsgType getMsgType(char *communicate) {
    if (strcmp(communicate, "STOP") == 0) return STOP;
    if (strcmp(communicate, "LIST") == 0) return LIST;
    if (strcmp(communicate, "2ALL") == 0) return TO_ALL;
    if (strcmp(communicate, "2ONE") == 0) return TO_ONE;
    if (strcmp(communicate, "INIT") == 0) return INIT;
    return 0;
}

void handleStop() {
    msgBuf.mtype = STOP;
    strcpy(msgBuf.mtext, "STOP");
    msgBuf.id = myId;
    sendMsgToQueue(serverQueueId, &msgBuf);
}

void SIGINTHandler(int sigNum) {
    puts("\nSIGINT sent by Client.");
    handleStop();
    exit(EXIT_SUCCESS);
}

void sendCommunicates(char communicate[]) {
    signal(SIGINT, SIGINTHandler);

    scanf("%s", communicate);
    char *token = strtok(communicate, " "); // TODO: split later

    if (token != NULL) {
        MsgType msgType = getMsgType(token);

        switch (msgType) {
            case STOP:
                handleStop();
                stop = true;
                return;
            case LIST:
                break;
            case TO_ALL:
                break;
            case TO_ONE:
                break;
            case INIT:
                puts("Client already sent the INIT communicate.");
                break;
        }
    }
}

void receiveCommunicates() {
    if (msgrcv(msgQueue->id, &msgBuf, MAX_MSG_SIZE, -5, IPC_NOWAIT) == -1) {
        if (errno != ENOMSG) {
            cleanAtExit();
            raisePError("msgrcv");
        }
    } else {
        switch (msgBuf.mtype) {
            case STOP:
                puts("Client received the STOP communicate from Server.");
                handleStop();
                stop = true;
                return;
            case LIST:
                break;
            case TO_ALL:
                break;
            case TO_ONE:
                break;
            case INIT:
                puts("Client already received the INIT communicate.");
                break;
        }
    }
}

int main(void) {
    fcntl(STDIN_FILENO, F_SETFL, FNDELAY);
    puts("Client running...");
    printf("My PID: %d\n", getpid());
    msgQueue = createMsgQueue('c');
    puts("Client's queue created.");
    atexit(cleanAtExit);
    serverQueueId = getQueueId(-1);

    sendInit();
    puts("The INIT communicate sent to Server.");

    handleInit();
    puts("Client received the INIT communicate.");

    char communicate[100];
    communicate[0] = '\0';
    while (!stop) {
        receiveCommunicates();
        sendCommunicates(communicate);
    }

    return 0;
}
