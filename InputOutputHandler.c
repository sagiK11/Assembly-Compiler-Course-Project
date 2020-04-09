
#include "declarationsHeader.h"

static int address = STARTING_ADDRESS;
extern struct keyWords keys[];
extern symPtr symTableHead;
extern dataPtr dataListHead;
extern extPtr extListHead;
extern insPtr insListHead;
extern char *fileName;
extern size_t dataListLength;
extern size_t insListLength;

/*
 * Creates all the relevant files.
 */
void createFiles() {

    /*Coding the memory words according to its type*/
    writeObjectFile();

    /*Creating the ex.ps File.*/
    writeExtFile();

    /*Creating the ent.ps File.*/
    writeEntFile();
}

/*
 * Creates the object file.
 */
void writeObjectFile() {
    FILE *res_fd;
    char objectFile[] = ".ob", temp[TMP_STR_LEN];
    char tempFileName[STRING_SIZE];
    insPtr ptr = insListHead;

    /*File naming and adding extension.*/
    strcpy(tempFileName, fileName);
    strcat(tempFileName, objectFile);
    if (insListLength == 0 && dataListLength == 0) /*Nothing to code.*/
        return;

    /*Appending the machine code file*/
    if ((!(res_fd = fopen(tempFileName, WRITE_ONLY)))) {
        printErrorWithComment(err_app_file, "object file");
        exit(EXIT_FAILURE);
    }

    /*Adding sizes to file.*/
    addSizesToObjectFile(res_fd, insListLength, dataListLength);
    for (address = STARTING_ADDRESS; ptr; ptr = ptr->next) {
        sprintf(temp, "%d", address);/*Converting the address to string*/
        fputs(temp, res_fd);
        fputs(TAB, res_fd);

        /*If the current word is used with external symbol, we update its address for later use.*/
        if ((ptr->data)->ERAforAdd == 1)
            updateExtList(address);
        address++;

        fputs(ptr->strWord, res_fd);
        fputs(NEWLINE, res_fd);
    }

    /*Adding the data code to the object file.*/
    addDataCodeToFile(res_fd);
    fclose(res_fd);
}

/*
 * Creates the ent file.
 */
void writeEntFile() {
    FILE *fd;
    char entAddress[TMP_STR_LEN], tempFileName[STRING_SIZE], entFile[] = ".ent";
    size_t entSym = 0;
    symPtr ptr;

    /*File naming and adding extension.*/
    strcpy(tempFileName, fileName);
    strcat(tempFileName, entFile);

    if (!(fd = fopen(tempFileName, WRITE_ONLY))) {
        printErrorWithComment(writing_to_file, "ent file");
        exit(EXIT_FAILURE);
    }

    /*Writing to file the symbols & address.*/
    for (ptr = symTableHead; ptr; ptr = ptr->next) {
        if (ptr->isEnt  && ++entSym) {
            sprintf(entAddress, "%d", ptr->data);/*Converting the address to string*/
            fputs(ptr->symName, fd);
            fputs(TAB, fd);
            fputs(entAddress, fd);
            fputs(NEWLINE, fd);
        }
    }

    /*If we didnt have entry symbols, we remove the file.*/
    if (entSym == 0) {
        remove(tempFileName);
        return;
    }
    fclose(fd);
}

/*
 * Creates the ext file/
 */
void writeExtFile() {

    FILE *fd;
    char tempFileName[STRING_SIZE], extFile[] = ".ext", extAddress[TMP_STR_LEN];
    extPtr ptr = extListHead;

    /*File naming and adding extension.*/
    strcpy(tempFileName, fileName);
    strcat(tempFileName, extFile);
    if (extListHead == NULL)/*No extern symbols within the file.*/
        return;

    if (!(fd = fopen(tempFileName, WRITE_ONLY))) {
        printErrorWithComment(writing_to_file, "ext file");
        exit(EXIT_FAILURE);
    }

    /*Writing the symbols and its address to the File.*/
    for (; ptr; ptr = ptr->next) {
        sprintf(extAddress, "%d", ptr->address);/*Converting the address to string.*/
        fputs(ptr->symbol, fd); /*Writing the symbol.*/
        fputs(TAB, fd);
        fputs(extAddress, fd);/*Writing its address.*/
        fputs(NEWLINE, fd);
    }
    fclose(fd);
}

/*
 * Adding the sizes of the data & instructions code length to the top of the object file.
 */
void addSizesToObjectFile(FILE *res_fd, int insSize, int dataSize) {
    char temp[CMD_LENGTH];
    fputs(TAB, res_fd);

    /*Converting the address to string.*/
    sprintf(temp, "%d", insSize);
    /*Putting the size of the instructions.*/
    fputs(temp, res_fd);
    fputs(TAB, res_fd);
    /*Putting the size of the data.*/
    sprintf(temp, "%d", dataSize);
    fputs(temp, res_fd);
    fputs(NEWLINE, res_fd);
}

/*
 * Adds the data code after the instruction part code to the object file.
 */
void addDataCodeToFile(FILE *res_fd) {
    const size_t theEndOfFile = 0;
    dataPtr ptr = dataListHead;
    char temp[TMP_STR_LEN];
    if (dataListLength == 0)/*Nothing to code.*/
        return;

    /*Getting to the end of the file*/
    fseek(res_fd, theEndOfFile, SEEK_END);

    /*Writing the code into the file*/
    for (; ptr; ptr = ptr->next, address++) {
        sprintf(temp, "%d", address);/*Converting the address to string*/
        if (ptr != dataListHead)
            fputs(NEWLINE, res_fd);

        fputs(temp, res_fd);
        fputs(TAB, res_fd);
        fputs(ptr->data, res_fd);
    }
}

/*
 * Changes the type of the symbol to entry.
 */
void preWritingToFileEnt() {
    char lineCpy[LINE_SIZE];
    char tempLabel[SYM_SIZE];
    symPtr sPtr = symTableHead;
    const int entStrLen = 6;

    strcpy(lineCpy, line);

    /*Taking the label after the .entry*/
    strcpy(tempLabel, (strstr(lineCpy, GUIDING_SENTENCE_ENT) + entStrLen));
    removeSpaces(tempLabel);

    /*Validating the sym is legit.*/
    symIsIllegal(tempLabel);

    /*Changing the value of the ent sym to be true for later use.*/
    for (; sPtr; sPtr = sPtr->next) {
        if (strcmp(tempLabel, sPtr->symName) == EQUAL) {
            sPtr->isEnt = TRUE;
        }
    }
}