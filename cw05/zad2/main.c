#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void raiseError(char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

void raisePError(char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void readOutput(FILE *stream) {
    if (stream == NULL) raisePError("popen");
    char *line = NULL;
    size_t len = 0;
    while ((getline(&line, &len, stream)) != -1) printf("%s", line);
    printf("\n");
    free(line);
    if (pclose(stream) == -1) raisePError("pclose");
}

void sortBySender(void) {
    puts("Sorted by senders:");
    FILE *sender = popen("mail -H | sort -k3", "r");
    readOutput(sender);
}

void sortByDate(void) {
    puts("Sorted by date:");
    FILE *date = popen("mail -H", "r");
    readOutput(date);
}

void sendEMail(char *argv[]) {
    puts("Sending email");
    // echo body | mail -s subject address
    char mailCommand[strlen("echo ") + strlen(argv[3]) + strlen(" | mail -s ") + strlen(argv[2]) + 1 + strlen(argv[3])];
    strcpy(mailCommand, "echo ");
    strcat(mailCommand, argv[3]);
    strcat(mailCommand, " | mail -s ");
    strcat(mailCommand, argv[2]);
    strcat(mailCommand, " ");
    strcat(mailCommand, argv[1]);
    FILE *sendMail = popen(mailCommand, "w");
    if (sendMail == NULL) raisePError("popen");
    puts("E-mail sent");
    pclose(sendMail);
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        if (strcmp(argv[1], "nadawca") == 0) {
            sortBySender();
        } else if (strcmp(argv[1], "data") == 0) {
            sortByDate();
        } else {
            raiseError("Wrong argument passed to the function.");
        }
    } else if (argc == 4) {
        sendEMail(argv);
    } else {
        raiseError("Wrong argument passed to the function.");
    }

    return 0;
}
