#include "declarationsHeader.h"

symPtr symTableHead;
paramsPtr paramsListHead;
dataPtr dataListHead;
insPtr insListHead;
size_t dataListLength;
size_t insListLength;
char *fileName;


int main(int argc, char *argv[]) {

    if (argc < MIN_ARGUMENTS) {
        printError(not_enough_params, "");
        exit(EXIT_FAILURE);
    }

    translateFiles(argc, argv);

    return EXIT_SUCCESS;
}

void translateFiles(int argc, char **argv) {
    FILE *fd;
    size_t fileIndex = 1;

    for (; argc > 1; --argc) {
        if ((fd = fopen(argv[fileIndex], READ_ONLY)) == NULL) {
            printError(file_op_err, argv[fileIndex]);
            exit(EXIT_FAILURE);
        }

        fileName = argv[fileIndex++];
        parseFileName(fileName);
        resetErrorFlag();
        firstPass(fd);
        rewindForSecondPass(fd);
        secondPass(fd);
        freeDataStructures();
        closeFile(fd);
    }

}

void rewindForSecondPass(FILE *fd) {
    rewind(fd);
}

void resetErrorFlag() {
    generalError = FALSE;
}

void parseFileName() {
    *strchr(fileName, DOT) = STR_END;
}

void closeFile(FILE *fd) {
    fclose(fd);
}

void freeDataStructures() {
    freeSymTable();
    freeParamsList();
    freeExtList();
    freeDataList();
    freeInsList();

}
