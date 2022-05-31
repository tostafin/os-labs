#ifndef ZAD1_COMMON_H
#define ZAD1_COMMON_H

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
#include <stdbool.h>
#include <errno.h>

#define CLIENT_NAME_MAX_LEN 64

typedef enum connectMode {
    NETWORK,
    LOCAL
} ConnectMode;

void raiseError(const char *message);
void raisePError(const char *message);

#endif //ZAD1_COMMON_H
