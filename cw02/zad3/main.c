#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#ifdef USE_NFTW
#include <ftw.h>
#endif

int regularFiles = 0, directories = 0, characterDevices = 0, blockDevices = 0, fifoNamedPipes = 0, symbolicLinks = 0,
        sockets = 0;

void raiseError(const char *message) {
    fprintf(stderr, "[ERROR]: %s", message);
    exit(EXIT_FAILURE);
}

void printFileDetails(const char *absPath, const struct stat *buf) {
    char fileType[18];
    __mode_t statFileType = buf->st_mode;
    if (S_ISREG(statFileType)) {
        strcpy(fileType, "regular file");
        regularFiles++;
    } else if (S_ISDIR(statFileType)) {
        strcpy(fileType, "directory");
        directories++;
    } else if (S_ISCHR(statFileType)) {
        strcpy(fileType, "character device");
        characterDevices++;
    } else if (S_ISBLK(statFileType)) {
        strcpy(fileType, "block device");
        blockDevices++;
    } else if (S_ISFIFO(statFileType)) {
        strcpy(fileType, "fifo (named pipe)");
        fifoNamedPipes++;
    } else if (S_ISLNK(statFileType)) {
        strcpy(fileType, "symbolic link");
        symbolicLinks++;
    } else if (S_ISSOCK(statFileType)) {
        strcpy(fileType, "socket");
        sockets++;
    }

    symbolicLinks += (int) buf->st_nlink - 1;


    printf("absolute path: %s, no of links: %lu, file type: %s, size in bytes: %ld, last access time: %s,"
           " last modified time: %s\n\n", absPath, buf->st_nlink, fileType, buf->st_size, ctime(&buf->st_atime),
           ctime(&buf->st_mtime)
    );
}

#ifndef USE_NFTW
void traverseDir(char *path) {
    DIR *dir = opendir(path);

    if (dir == NULL) {
        perror("Cannot open the directory");
        exit(EXIT_FAILURE);
    }

    char *absPath = calloc(1 << 10, sizeof(char));
    struct dirent *file;
    while ((file = readdir(dir)) != NULL) {
        strcpy(absPath, path);
        strcat(absPath, "/");
        strcat(absPath, file->d_name);

        struct stat buf;

        if (lstat(absPath, &buf) < 0) raiseError("Cannot use lstat on the file.");

        if (S_ISDIR(buf.st_mode)) {
            if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) continue;
            traverseDir(absPath);
        }

        printFileDetails(absPath, &buf);
    }

    free(absPath);
    closedir(dir);
}
#else
int nftwfunc(const char *filename, const struct stat *statptr, int fileflags, struct FTW *pftw) {
    printFileDetails(filename, statptr);
    return 0;
}
#endif

int main(int argc, char *argv[]) {
    if (argc != 2) raiseError("You must input exactly one argument.");
    char *absPath = calloc(1 << 10, sizeof(char));
    realpath(argv[1], absPath);
#ifndef USE_NFTW
    traverseDir(absPath);
#else
    nftw(absPath, nftwfunc, 1, FTW_PHYS);
    directories--;
#endif
    printf("***SUMMARY***\n");
    printf("regular files: %d, directories: %d, character devices: %d, block devices: %d, fifo (named pipes): %d,"
           " symbolic links: %d, sockets: %d\n\n",
           regularFiles, directories, characterDevices, blockDevices, fifoNamedPipes, symbolicLinks, sockets);
    free(absPath);
    return 0;
}
