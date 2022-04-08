#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void raiseError(char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

void raisePError(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

char **getComponents(FILE *file, int *compsCnt) {
    int numOfComps = 10;
    char **comps = malloc(numOfComps * sizeof(*comps));
    char *line = NULL;
    size_t len = 0;
    getline(&line, &len, file);
    if (strcmp(line, "# Definicja skladnikow\n") != 0) raiseError("Wrong file structure.");

    while (getline(&line, &len, file) != -1 && strcmp(line, "\n") != 0) {
        char *command = strchr(line, '=') + 2; // +2 to remove '=' and space
        size_t commandLen = strlen(command);
        command[commandLen - 1] = '\0'; // removing '\n'
        comps[*compsCnt] = malloc((commandLen - 1) * sizeof(*comps));
        strcpy(comps[(*compsCnt)++], command);
        if (*compsCnt == numOfComps) {
            numOfComps *= 2;
            comps = realloc(comps, numOfComps * sizeof(*comps));
        }
    }
    free(line);
    return comps;
}

int *getCommandsOrder(FILE *file, int *commandsCnt) {
    char *line = NULL;
    size_t len;
    getline(&line, &len, file);
    if (strcmp(line, "# Wykonanie potoku\n") != 0) raiseError("Wrong file structure.");
    getline(&line, &len, file);

    int numOfArgs = 10;
    int *commandsOrder =  malloc(numOfArgs * sizeof(*commandsOrder));
    char *token = strtok(line, " | ");
    while (token != NULL) {
        token += 8; // "skladnik" has 8 letters
        commandsOrder[(*commandsCnt)++] = (int) strtol(token, NULL, 10);
        token = strtok(NULL, " |");
        if (*commandsCnt == numOfArgs - 1) {
            numOfArgs *= 2;
            commandsOrder = realloc(commandsOrder, numOfArgs * sizeof(*commandsOrder));
        }
    }

    free(line);
    return commandsOrder;
}

char **splitCommandsInOrder(int *splitCommandsCnt, char **splitComponents, const int *commandOrder, int commandsCnt) {
    int numOfCommands = 10;
    char **commands = malloc(numOfCommands * sizeof(*commands));
    char *strStart, *strEnd;

    for (int i = 0; i < commandsCnt; ++i) {
        strStart = strEnd = (splitComponents[commandOrder[i]]);
        while (*strEnd != '\0') {
            while (*strEnd != '|' && *strEnd != '\0') ++strEnd; // searching for pipe
            if (*(strEnd - 1) == ' ') *(strEnd - 1) = '\0'; // space before the pipe is the end of a command
            commands[*splitCommandsCnt] = malloc(strlen(strStart) * sizeof(*commands));
            strcpy(commands[(*splitCommandsCnt)++], strStart);
            if (*(strEnd - 1) == '\0') {
                *(strEnd - 1) = ' '; // bringing the space back
                strEnd += 2; // jumping to the next command
            }

            strStart = strEnd;
            if (*splitCommandsCnt == numOfCommands - 1) {
                numOfCommands *= 2;
                commands = realloc(commands, numOfCommands * sizeof(*commands));
            }
        }
    }

    return commands;
}

void closePipes(int numOfDescriptors, int fd[numOfDescriptors][2]) {
    for (int i = 0; i < numOfDescriptors; ++i) {
        if (close(fd[i][0]) == -1) raisePError("close");
        if (close(fd[i][1]) == -1) raisePError("close");
    }
}

void getCommandsAndArguments(char *commandsAndArguments[5], char *command) {
    int i = 0;
    char *token = strtok(command, " ");

    while (token != NULL) {
        commandsAndArguments[i] = token;
        ++i;
        token = strtok(NULL, " ");
    }
    commandsAndArguments[i] = NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) raiseError("You must pass exactly one argument: path to a text file.");

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) raisePError("file");

    int compsCnt = 1;
    char **splitComponents = getComponents(file, &compsCnt);

    int commandsCnt = 0;
    int *commandOrder = getCommandsOrder(file, &commandsCnt);

    int splitCommandsCnt = 0;
    char **splitCommands = splitCommandsInOrder(&splitCommandsCnt, splitComponents, commandOrder, commandsCnt);

    int numOfDescriptors = splitCommandsCnt - 1;
    int fd[numOfDescriptors - 1][2];
    int i;
    for (i = 0; i < splitCommandsCnt - 1; ++i) {
        if (pipe(fd[i]) == -1) raisePError("pipe");
    }

    pid_t childPid;
    int commandIdx = 0;
    if ((childPid = fork()) == 0) {
        dup2(fd[0][1], STDOUT_FILENO);
        closePipes(numOfDescriptors, fd);
        char *commandsAndArguments[5];
        getCommandsAndArguments(commandsAndArguments, splitCommands[commandIdx]);
        ++commandIdx;

        execvp(commandsAndArguments[0], commandsAndArguments);
    } else if (childPid == -1) {
        raisePError("fork");
    }

    for (i = 1; i < compsCnt; ++i) free(splitComponents[i]);
    free(splitComponents);
    free(commandOrder);
    for (i = 0; i < splitCommandsCnt; ++i) free(splitCommands[i]);
    free(splitCommands);
    fclose(file);
    return 0;
}

