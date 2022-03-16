#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#ifdef USE_SYS
#include <fcntl.h>
#include <unistd.h>
#endif

void raiseError(const char *message) {
    fprintf(stderr, "[ERROR]: %s", message);
    exit(EXIT_FAILURE);
}

#ifdef USE_LIB
int main(int argc, char *argv[]) {
    char *fileName1 = calloc(1 << 10, sizeof(char)), *fileName2 = calloc(1 << 10, sizeof(char));
    if (argc == 1) {
        fprintf(stdout, "Input the first file name:\n");
        fscanf(stdin, "%s", fileName1);
        fprintf(stdout, "Input the second file name:\n");
        fscanf(stdin, "%s", fileName2);
    } else if (argc == 3) {
        strcpy(fileName1, argv[1]);
        strcpy(fileName2, argv[2]);
    } else raiseError("Wrong number of arguments.");

    FILE *file1 = fopen(fileName1, "r");
    FILE *file2 = fopen(fileName2, "w");
    if (file1 == NULL || file2 == NULL) perror("Can't open files");
    free(fileName1), free(fileName2);

    char c[1];
    long offset = 0;
    bool wasAnythingWritten = false;
    while (fread(c, 1, 1, file1) == 1) {
        offset++;
        if (!isspace(c[0])) {
            fseek(file1, -offset, SEEK_CUR);
            offset = 0;
            if (wasAnythingWritten) {
                char newLine[1];
                newLine[0] = '\n';
                fwrite(newLine, 1, 1, file2);
            } else {
                wasAnythingWritten = true;
            }

            size_t charRead = fread(c, 1, 1, file1);
            while (charRead == 1 && c[0] != '\n') {
                fwrite(c, 1, 1, file2);
                charRead = fread(c, 1, 1, file1);
            }
        }

        if (c[0] == '\n') {
            offset = 0;
        }
    }

    fclose(file1), fclose(file2);
    return 0;
}
#endif

#ifdef USE_SYS
int main(int argc, char *argv[]) {
    char *fileName1 = calloc(1 << 5, sizeof(char)), *fileName2 = calloc(1 << 5, sizeof(char));
    if (argc == 1) {
        printf("Input the first file path:\n");
        scanf("%s", fileName1);
        printf("Input the second file path:\n");
        scanf("%s", fileName2);
    } else if (argc == 3) {
        strcpy(fileName1, argv[1]);
        strcpy(fileName2, argv[2]);
    } else raiseError("Wrong input: expected no arguments or two paths to files.");

    int file1 = open(fileName1, O_RDONLY);
    int file2 = open(fileName2, O_WRONLY | O_TRUNC);
    if (file1 < 0 || file2 < 0) perror("Can't open the files");
    free(fileName1), free(fileName2);

    char c;
    long offset = 0;
    bool wasAnythingWritten = false;
    while (read(file1, &c, 1) == 1) {
        offset++;
        if (!isspace(c)) {
            lseek(file1, -offset, SEEK_CUR);
            offset = 0;
            if (wasAnythingWritten) {
                char newLine = '\n';
                write(file2, &newLine, 1);
            } else {
                wasAnythingWritten = true;
            }

            long charRead = read(file1, &c, 1);
            while (charRead == 1 && c != '\n') {
                write(file2, &c, 1);
                charRead = read(file1, &c, 1);
            }
        }

        if (c == '\n') {
            offset = 0;
        }
    }

    close(file1), close(file2);
    return 0;
}
#endif
