#include "declarationsHeader.h"

extern struct keyWords keys[];
boolean sentenceWithLabel;
boolean sentenceWithCommand;
size_t memoryWords;

/*
 * Takes one line from the file and runs tests over it and gathering info for the second pass.
 */
void firstPass(FILE *fd) {
    int IC = STARTING_ADDRESS, DC = 0;
    lineCnt = 0;
    line = (char *) malloc(sizeof(char) * LINE_SIZE);
    MEMORY_ERROR(line)

    while (fgets(line, LINE_SIZE, fd) && ++lineCnt) { /*Taking one line at a time.*/
        *strchr(line, '\n') = STR_END;

        if (lineIsComment(line))
            continue;

        fillLineList();
        analyzeLine(&DC);
        if (generalError == FALSE)
            codeTheLine(&IC, &DC);
        freeLineList();
    }
    updateSymbolTable(&IC);
    free(line);
}

/*
 * Codes the current line according to the type of the line.
 */
void codeTheLine(int *IC, int *DC) {
    resetMemoryWordsLength();
    getLineType();

    if (lineWithDataDefinition())/*.data line - array or an int.*/
        codeDataLine(DC);
    else if (lineWithStringDefinition())/*Its a line with a string.*/
        codeStringLine(DC);
    else if (lineWithExternLabel()) /*Its a line with extern label*/
        externSymLine();
    else if (sentenceWithCommand)/*Command line- calculating the number of words it'll take*/
        calCmdLine(IC);
}

void resetMemoryWordsLength() {
    memoryWords = 0;
}

void getLineType() {
    sentenceWithLabel = lineWithLabelDefinition();
    sentenceWithCommand = lineWithCommandDefinition();
}

boolean bothParametersAreRegisters(char *firstParam, char *secondParam) {
    return isRegister(firstParam) && isRegister(secondParam);
}


void checkForFloatErrors(char *firstParam, char *secondParam) {
    if (firstParam != NULL && strchr(firstParam, DOT))
        printError(float_err, firstParam);
    if (secondParam != NULL && strchr(secondParam, DOT))
        printError(float_err, secondParam);
}


/*
* Returns true if there a label before the parameters.
* Else, returns false.
*/
boolean labelBeforeParam() {
    char lineCpy[LINE_SIZE], cmd[CMD_LENGTH];

    strcpy(lineCpy, line);
    getCommand(cmd);

    /*Testing if the line has a label before the parameters*/
    if (strcmp(cmd, "jmp") != EQUAL && strcmp(cmd, "bne") != EQUAL && strcmp(cmd, "jsr") != EQUAL)
        return FALSE;
    if (strchr(lineCpy, O_BRACKETS) == NULL)
        return FALSE;
    return TRUE;

}

/*
* Returns true if there's a label definition withing the line.
* Else, returns false.
*/
boolean lineWithLabelDefinition() {
    char lineCpy[LINE_SIZE];
    strcpy(lineCpy, line);

    /*Searching for ':' in the line.*/
    if (strchr(lineCpy, ISLABEL))
        return TRUE;
    return FALSE;
}

/*
* Returns true if there's a data definition withing the line.
* Else, returns false.
*/
boolean lineWithDataDefinition() {
    char lineCpy[LINE_SIZE];
    strcpy(lineCpy, line);

    //*Searching for .data in the line.*/
    if (strstr((lineCpy), GUIDING_SENTENCE_INT))
        return TRUE;
    return FALSE;
}

/*
* Returns true if there's a string definition withing the line.
* Else, returns false.
*/
boolean lineWithStringDefinition() {
    char lineCpy[LINE_SIZE];
    strcpy(lineCpy, line);

    /*Searching for .string in the line.*/
    if (strstr((lineCpy), GUIDING_SENTENCE_STRING))
        return TRUE;
    return FALSE;
}

/*
* Returns true if there's a command definition withing the line.
* Else, returns false.
*/
boolean lineWithCommandDefinition() {
    char lineCpy[LINE_SIZE];
    int i = 0;
    strcpy(lineCpy, line);

    /*Searching for a sub string matching the our commands.*/
    for (i = 0; i < ACTIONS_NUM; i++) {
        if (strstr(lineCpy, (keys[i].symbol))) {
            return TRUE;
        }
    }
    return FALSE;
}
