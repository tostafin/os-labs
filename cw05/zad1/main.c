#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

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
        token = strtok(NULL, " | ");
        if (*commandsCnt == numOfArgs - 1) {
            numOfArgs *= 2;
            commandsOrder = realloc(commandsOrder, numOfArgs * sizeof(*commandsOrder));
        }
    }

    free(line);
    return commandsOrder;
}

char **splitCommandsInOrder(int *splitCommandsCnt, char **splitComponents, int *commandOrder, int commandsCnt) {
    int numOfCommands = 10;
    char **commands = malloc(numOfCommands * sizeof(*commands));
    char *token, *strCpy;

    for (int i = 0; i < commandsCnt; ++i) {
        strCpy = strdup(splitComponents[commandOrder[i]]);
        token = strsep(&strCpy, "|");
        while (token != NULL) {
            commands[*splitCommandsCnt] = malloc(strlen(token) * sizeof(*commands));
            commands[(*splitCommandsCnt)++] = token;
            token = strsep(&strCpy, "|");
            if (*splitCommandsCnt == numOfCommands - 1) {
                numOfCommands *= 2;
                commands = realloc(commands, numOfCommands * sizeof(*commands));
            }
        }
    }

    return commands;
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
    printf("%s\n", splitCommands[0]);
    printf("%s\n", splitCommands[1]);
    printf("%s\n", splitCommands[2]);

    int i;
    for (i = 1; i < compsCnt; ++i) free(splitComponents[i]);
    free(splitComponents);
    free(commandOrder);
//    for (i = 0; i < splitCommandsCnt; ++i) free(splitCommands[i]);
//    free(splitCommands);
    fclose(file);
    return 0;
}
