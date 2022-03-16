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

    char c;
    long offset = 0;
    bool wasAnythingWritten = false;
    while ((c = (char) fgetc(file1)) != EOF) {
        offset++;
        if (!isspace(c)) {
            fseek(file1, -offset, SEEK_CUR);
            offset = 0;
            if (wasAnythingWritten) {
                fputc('\n', file2);
            } else {
                wasAnythingWritten = true;
            }

            c = (char) fgetc(file1);
            while (c != EOF && c != '\n') {
                fputc(c, file2);
                c = (char) fgetc(file1);
            }
        }

        if (c == '\n') {
            offset = 0;
        }
    }

    fclose(file1), fclose(file2);
    return 0;
}
