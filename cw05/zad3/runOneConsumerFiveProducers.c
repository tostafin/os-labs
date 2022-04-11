#include "common.h"

int main() {
    remove("pipe");
    mkfifo("pipe", 0666);
    pid_t childPid;

    if ((childPid = fork()) == 0) {
        execl("./consumer", "consumer", "./pipe", "./files/writeFile.txt", "5", NULL);
        raisePError("execl");
    } else if (childPid == -1) {
        raisePError("fork");
    }

    for (int i = 1; i <= 5; ++i) {
        if ((childPid = fork()) == 0) {
            char rowNum[2];
            sprintf(rowNum, "%d", i);
            char filePath[30] = "./files/N5/file";
            strcat(filePath, rowNum);
            strcat(filePath, ".txt");
            execl("./producer", "producer", "./pipe", rowNum, filePath, "5", NULL);
            raisePError("execl");
        } else if (childPid == -1) {
            raisePError("fork");
        }
    }
    
    while (wait(NULL) > 0);
    
    return 0;
}
