#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>

void raiseError(const char *message) {
    fprintf(stderr, "[ERROR]: %s", message);
    exit(EXIT_FAILURE);
}

char *getRelPath(char *path) {
    int pathLen = (int) strlen(path);
    char *relPath = calloc(pathLen, sizeof(char));
    int i = pathLen;
    int idx = 0;
    while (path[--i] != '/') {
        relPath[idx++] = path[i];
    }
    relPath[idx++] = '/';

    for (i = 0; i < idx / 2; ++i) {
        char tmp = relPath[idx - 1 - i];
        relPath[idx - 1 - i] = relPath[i];
        relPath[i] = tmp;
    }

    return relPath;
}

bool isTextFileAndHasString(char *path, char *searchedStr) {
    int bufLength = 1000;
    char fileCommand[bufLength];
    strcpy(fileCommand, "file ");
    strcat(fileCommand, path);
    strcat(fileCommand, " > tmp.txt");
    system(fileCommand);

    FILE *file = fopen("tmp.txt", "r");

    char buf[bufLength];
    if (fgets(buf, bufLength, file)) {
        const char *isTextFile = strstr(buf, "text");
        if (isTextFile) {
            fclose(file);
            file = fopen(path, "r");
            while (fgets(buf, bufLength, file)) {
                const char *hasSubstr = strstr(buf, searchedStr);
                if (hasSubstr) return true;
            }
            fclose(file);
        }
    }

    return false;
}

void traverseDir(char *relPath, char *absPath, int depth, char *searchedStr) {
    if (depth < 0) return;

    DIR *dir = opendir(absPath);
    if (dir == NULL) {
        perror("Cannot open the directory");
        exit(EXIT_FAILURE);
    }

    printf("Searching in %s by a process with pid %d\n", relPath, getpid());

    char nextRelPath[1000];
    char nextAbsPath[1000];
    struct dirent *file;
    while ((file = readdir(dir)) != NULL) {
        strcpy(nextRelPath, relPath);
        strcpy(nextAbsPath, absPath);
        strcat(nextRelPath, "/");
        strcat(nextAbsPath, "/");
        strcat(nextRelPath, file->d_name);
        strcat(nextAbsPath, file->d_name);

        struct stat buf;

        if (stat(nextAbsPath, &buf) < 0) raiseError("Cannot use lstat on the file.");

        if (S_ISDIR(buf.st_mode)) {
            if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) continue;
            pid_t childPid;
            if ((childPid = fork()) == 0) {
                traverseDir(nextRelPath, nextAbsPath, depth - 1, searchedStr);
                exit(EXIT_SUCCESS);
            } else if (childPid == -1) {
                raiseError("fork");
                abort();
            } else {
                waitpid(childPid, NULL, 0);
            }
        } else if (S_ISREG(buf.st_mode) && isTextFileAndHasString(nextAbsPath, searchedStr)) {
            printf("Found %s\n", file->d_name);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 4) raiseError("You must input exactly one argument.");
    int maxDepth = (int) strtol(argv[3], NULL, 10);
    if (maxDepth <= 0) raiseError("You must pass a positive integer as a last argument.");

    char absPath[1000];
    realpath(argv[1], absPath);
    char *relPath = getRelPath(argv[1]);

    traverseDir(relPath, absPath, maxDepth, argv[2]);

    system("rm -f tmp.txt");
    free(relPath);
    return 0;
}
