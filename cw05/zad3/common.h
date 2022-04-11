#ifndef CONSUMER_COMMON_H
#define CONSUMER_COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
void raiseError(char *message);
void raisePError(char *message);
#endif //CONSUMER_COMMON_H
