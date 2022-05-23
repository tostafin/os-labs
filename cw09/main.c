#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

#define PRESENTS_TO_DELIVER 3
#define NUM_OF_REINDEER 9
#define REINDEER_REQUIRED 9
#define NUM_OF_ELVES 10
#define ELVES_REQUIRED 3

struct drand48_data randData;
double randResult = 0;

pthread_t santaThread;
pthread_t reindeerThreads[NUM_OF_REINDEER];


pthread_mutex_t isSantaAwakeMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t isSantaAwakeCond = PTHREAD_COND_INITIALIZER;
bool isSantaAwake = false;


pthread_mutex_t reindeerCntMutex = PTHREAD_MUTEX_INITIALIZER;
short reindeerCnt = 0;

pthread_mutex_t reindeerDeliverPresentsMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reindeerDeliverPresentsCond = PTHREAD_COND_INITIALIZER;
bool reindeerDeliverPresents = false;

pthread_mutex_t reindeerReturnedMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reindeerReturnedCond = PTHREAD_COND_INITIALIZER;
bool reindeerReturned[NUM_OF_REINDEER] = {false};


void raiseError(const char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

void raisePError(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

int getRandomInt(int min, int max) {
    drand48_r(&randData, &randResult);
    return (int) (randResult * (max - min + 1)) + min;
}

void *santaFunc(void *arg) {
    for (int i = 0; i < PRESENTS_TO_DELIVER; ++i) {
        isSantaAwake = false;
        if ((errno = pthread_mutex_lock(&isSantaAwakeMutex)) != 0) raisePError("pthread_mutex_lock");
        while (!isSantaAwake) {
            pthread_cond_wait(&isSantaAwakeCond, &isSantaAwakeMutex);
        }
        if ((errno = pthread_mutex_unlock(&isSantaAwakeMutex)) != 0) raisePError("pthread_mutex_lock");

        // when Santa wakes up, he should notify other reindeer to deliver presents with him if there are 9 waiting
        puts("Mikolaj: budze sie");
        if ((errno = pthread_mutex_lock(&reindeerDeliverPresentsMutex)) != 0) raisePError("pthread_mutex_lock");
        reindeerDeliverPresents = true;
        if ((errno = pthread_mutex_unlock(&reindeerDeliverPresentsMutex)) != 0) raisePError("pthread_mutex_lock");
        pthread_cond_broadcast(&reindeerDeliverPresentsCond);

        // zero the number of waiting reindeer
        if ((errno = pthread_mutex_lock(&reindeerCntMutex)) != 0) raisePError("pthread_mutex_lock");
        reindeerCnt = 0;
        if ((errno = pthread_mutex_unlock(&reindeerCntMutex)) != 0) raisePError("pthread_mutex_lock");

        // delivering gifts
        puts("Mikolaj: dostarczam zabawki");
        sleep(getRandomInt(2, 4));
        if ((errno = pthread_mutex_lock(&reindeerReturnedMutex)) != 0) raisePError("pthread_mutex_lock");
        for (int j = 0; j < NUM_OF_REINDEER; ++j) {
            reindeerReturned[j] = true;
        }
        pthread_cond_broadcast(&reindeerReturnedCond);
        reindeerDeliverPresents = false;
        if ((errno = pthread_mutex_unlock(&reindeerReturnedMutex)) != 0) raisePError("pthread_mutex_lock");
        puts("Mikolaj: zasypiam");
    }

    for (int i = 0; i < NUM_OF_REINDEER; ++i) {
        pthread_cancel(reindeerThreads[i]);
    }

    return NULL;
}

void *reindeerFunc(void *arg) {
    int idx;
    while (true) {
        // sleep for 5-10 seconds
        sleep(getRandomInt(2, 5)); //TODO: change to 5 to 10

        // wait for 9 reindeer
        if ((errno = pthread_mutex_lock(&reindeerCntMutex)) != 0) raisePError("pthread_mutex_lock");
        idx = reindeerCnt;
        ++reindeerCnt;
        printf("Renifer: czeka %d reniferow na Mikolaja, %lu\n", reindeerCnt, pthread_self());

        // if that is the reindeer no 9, wake up santa
        if (reindeerCnt == REINDEER_REQUIRED) {
            printf("Renifer: wybudzam Mikolaja, %lu\n", pthread_self());
            if ((errno = pthread_mutex_lock(&isSantaAwakeMutex)) != 0) raisePError("pthread_mutex_lock");
            isSantaAwake = true;
            pthread_cond_signal(&isSantaAwakeCond);
            if ((errno = pthread_mutex_unlock(&isSantaAwakeMutex)) != 0) raisePError("pthread_mutex_unlock");
        }
        if ((errno = pthread_mutex_unlock(&reindeerCntMutex)) != 0) raisePError("pthread_mutex_unlock");

        // other 8 reindeer wait for the last one to come
        if ((errno = pthread_mutex_lock(&reindeerDeliverPresentsMutex)) != 0) raisePError("pthread_mutex_lock");
        while (!reindeerDeliverPresents) {
            pthread_cond_wait(&reindeerDeliverPresentsCond, &reindeerDeliverPresentsMutex);
        }
        if ((errno = pthread_mutex_unlock(&reindeerDeliverPresentsMutex)) != 0) raisePError("pthread_mutex_unlock");

        // returning from gift giving with Santa
        if ((errno = pthread_mutex_lock(&reindeerReturnedMutex)) != 0) raisePError("pthread_mutex_lock");
        while (!reindeerReturned[idx]) {
            pthread_cond_wait(&reindeerReturnedCond, &reindeerReturnedMutex);
        }
        reindeerReturned[idx] = false; // for the next iteration
        if ((errno = pthread_mutex_unlock(&reindeerReturnedMutex)) != 0) raisePError("pthread_mutex_unlock");

    }

    return NULL;
}

void runThreads(void) {
    srand48_r(time(NULL), &randData);

    pthread_create(&santaThread, NULL, santaFunc, NULL);
    for (int i = 0; i < NUM_OF_REINDEER; ++i) {
        pthread_create(&reindeerThreads[i], NULL, reindeerFunc, NULL);
    }

    pthread_join(santaThread, NULL);
    for (int i = 0; i < NUM_OF_REINDEER; ++i) {
        pthread_join(reindeerThreads[i], NULL);
    }
}

int main(void) {
    runThreads();

    return 0;
}