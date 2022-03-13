#ifndef LAB1_LIBRARY_H
#define LAB1_LIBRARY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/times.h>
#include <unistd.h>
#include <sys/time.h>

typedef struct MemoryBlock {
    char *wcResult;
} MemoryBlock;

typedef struct PointersArray {
    MemoryBlock *pointersArray;
    bool *usedFields;
    int size;
} PointersArray;

void createTable(PointersArray *pointersArray, int size);
MemoryBlock wcFiles(int startIdx, int endIdx, char *argv[]);
int reserveMemoryBlock(PointersArray *pointersArray, MemoryBlock *memoryBlock);
void removeBlock(PointersArray *pointersArray, int index);
void raiseError(char *errorMessage);

#endif //LAB1_LIBRARY_H
