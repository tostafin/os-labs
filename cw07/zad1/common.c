#include "common.h"

void raisePError(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void raiseError(const char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

int getSemaphoreId(void) {
    key_t key = ftok(getenv("HOME"), 'S');
    if (key == -1) raisePError("ftok");

    int id = semget(key, 0, 0);
    if (id == -1) raisePError("semget");

    return id;
}

int getSharedMemId(int projId) {
    key_t key = ftok(getenv("HOME"), projId);
    if (key == -1) raisePError("ftok");

    int id = shmget(key, 0, 0);
    if (id == -1) raisePError("shmget");

    return id;
}

char *getTimestamp(void) {
    static char timeStr[20];
    long ms; // Milliseconds
    time_t s;  // Seconds
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);

    s = spec.tv_sec;
    ms = round(spec.tv_nsec / 1.0e6); // Convert nanoseconds to milliseconds
    if (ms > 999) {
        s++;
        ms = 0;
    }

    sprintf(timeStr, "%"PRIdMAX".%03ld seconds", (intmax_t) s, ms);
    return timeStr;

//    printf("%"PRIdMAX".%03ld seconds\n",
//           (intmax_t) s, ms);
}

int getRandInt(int left, int right) { //right is exclusive
    /* Returns an integer in the range [left, right).
 *
 * Uses rand(), and so is affected-by/affects the same seed.
 */
    if ((right - 1) == RAND_MAX) {
        return rand();
    } else {
        // Supporting larger values for n would requires an even more
        // elaborate implementation that combines multiple calls to rand()
        assert(right <= RAND_MAX);

        // Chop off all the values that would cause skew...
        int end = RAND_MAX / right; // truncate skew
        assert(end > 0);
        end *= right;

        // ... and ignore results from rand() that fall above that limit.
        // (Worst case the loop condition should succeed 50% of the time,
        // so we can expect to bail out of this loop pretty quickly.)
        int r;
        while ((r = rand()) >= end);

        return (r % right) + left;
    }
}

int getNumOfPizzasOnTable(int *idx, Table *table) {
    int k = 0;
    for (int i = 0; i < OVEN_AND_TABLE_SIZE; ++i) {
        if (table->place[i] != -1) {
            ++k;
            *idx = i;
        }
    }
    return k;
}