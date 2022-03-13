#include "library.h"

void createTable(PointersArray *pointersArray, int size) {
    if (size <= 0) raiseError("Size on an array must be a non-negative integer.");
    if (pointersArray->pointersArray != NULL) raiseError("The table has already been created.");

    pointersArray->pointersArray = (MemoryBlock*) calloc(size, sizeof(MemoryBlock));
    pointersArray->usedFields = calloc(size, sizeof(bool));
    for (int i = 0; i < size; i++) pointersArray->usedFields[i] = false;
    pointersArray->size = size;
}

MemoryBlock wcFiles(int startIdx, int endIdx, char *argv[]) {
    char *command = calloc(1 << 10, sizeof(char));
    int i;

    strcpy(command, "wc");
    for (i = 0; i < endIdx - startIdx + 1; i++) {
        strcat(command, " ");
        strcat(command, argv[startIdx + i]);
    }
    strcat(command, " > tmp.txt");

    system(command);
    free(command);

    FILE *tmpFile = fopen("tmp.txt", "r");
    if (tmpFile == NULL) raiseError("Can't open the temp file.");

    fseek(tmpFile, 0, SEEK_END);
    long fileSize = ftell(tmpFile);

    char c;
    i = 0;
    MemoryBlock memoryBlock = {(char *) calloc(fileSize + 1, sizeof(char))};
    while ((c = (char) fgetc(tmpFile)) != EOF) memoryBlock.wcResult[i++] = c;

    memoryBlock.wcResult[i] = '\0';
    fclose(tmpFile);

    return memoryBlock;
}

int reserveMemoryBlock(PointersArray *pointersArray, MemoryBlock *memoryBlock) {
    int index = 0;
    while (index < pointersArray->size && pointersArray->usedFields[index]) index++;
    if (index == pointersArray->size) raiseError("There is no left space in the array.");
    pointersArray->pointersArray[index] = *memoryBlock;
    pointersArray->usedFields[index] = true;
    return index;
}

void removeBlock(PointersArray *pointersArray, int index) {
    if (!pointersArray->usedFields[index]) raiseError("Cannot remove a block that was never reserved.");
    pointersArray->usedFields[index] = false;
    free(pointersArray->pointersArray[index].wcResult);
}

void raiseError(char *errorMessage) {
    fprintf(stderr, "[ERROR]: %s", errorMessage);
    exit(EXIT_FAILURE);
}
