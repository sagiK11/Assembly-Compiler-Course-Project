#include "declarationsHeader.h"

extern paramsPtr paramsListHead;
extern symPtr symTableHead;
extern insPtr insListHead;
extPtr extListHead;
extern struct keyWords keys[];

/*
 * Takes one line and codes it.
 */
void secondPass(FILE *fd) {

    if (generalError)
        return;

    /*Working with a ptr to the current line.*/
    line = (char *) malloc(sizeof(char) * LINE_SIZE);
    MEMORY_ERROR(line)
    resetLineNumber();

    while (getLineFromFile(line, fd)) {/*Taking one line at a time and coding it*/
        parseLine(line);
        incrementLinesNumber();

        if (lineIsComment(line))
            continue;

        if (lineWithCommandDefinition()) {
            codeCommand();
            codeParameters();
        } else if (lineWithEntryLabel())
            preWritingToFileEnt();
    }

    /*Finds if an extern symbol was not used during the run.*/
    externSymWasntUsed(fd);

    if (!errorsInSymTable() && !generalError)
        createFiles();
    free(line);
}



boolean specialTypeAction(char *cmd) {
    return strcmp(cmd, "jmp") == EQUAL || strcmp(cmd, "bne") == EQUAL ||
           strcmp(cmd, "jsr") == EQUAL;
}

void findOpCode(Word machWord, char *cmd) {
    int i;
    for (i = 0; i < ACTIONS_NUM; i++)
        if (strcmp(cmd, keys[i].symbol) == 0)
            machWord->opCode = keys[i].numValue;

}


boolean nothingToCode(paramsPtr ptr, char *cmd) {
    return ptr == NULL && strcmp(cmd, "stop") != EQUAL && strcmp(cmd, "rts") != EQUAL;
}

/*
 *Sync's the parameters from the params list to the current line.
 */
void syncParametersToLine(paramsPtr *ptr) {
    while ((*ptr) != NULL) {
        if ((*ptr)->lineNum == lineNumber)
            return;
        (*ptr) = (*ptr)->next;
    }
}

/*
 * Tests if extern symbols weren't used over the file.
 */
void externSymWasntUsed(FILE *fd) {
    char tempLine[LINE_SIZE], *tempSym;
    extPtr ptr = extListHead;
    boolean isFound = FALSE;
    const size_t extLength = 7;
    size_t lineNum = 0;

    rewind(fd);

    while (getLineFromFile(tempLine, fd)) {
        ++lineNum;
        if (lineIsComment(tempLine))
            continue;

        isFound = FALSE;
        if ((tempSym = strstr(tempLine, GUIDING_SENTENCE_EXTERN)) != NULL) {
            tempSym += extLength;
            removeSpaces(tempSym);
            removeSpaces(tempLine);
            for (ptr = extListHead; ptr && !isFound; ptr = ptr->next) {
                if (strcmp(ptr->symbol, tempSym) == EQUAL)
                    isFound = TRUE;
            }
            if (ptr == NULL && !isFound)
                printWarning(uns_ex_sym, tempSym, lineNum);
        }
    }

}

/*
 * Tests if there are errors in a regular action code.
 */
void checkForErrorsInRegAction() {
    char lineCpy[LINE_SIZE], *beforeParentheses;
    size_t i = 0;
    symPtr ptr;
    strcpy(lineCpy, line);

    if (strchr(lineCpy, O_BRACKETS) == NULL)
        return;

    beforeParentheses = strtok(lineCpy, O_BRACKETS_STR);
    for (i = 0; i < ACTIONS_NUM; i++)
        if (strstr(lineCpy, keys[i].symbol) != NULL) {
            beforeParentheses = strstr(beforeParentheses, keys[i].symbol);
            break;
        }

    for (ptr = symTableHead; ptr; ptr = ptr->next) {
        if (strstr(beforeParentheses, ptr->symName)) {
            printErrorWithComment(illegal_sym_mth, ptr->symName);
        }
    }
}

/*
 *Returns true if the operand is with the 1st method use.
 * Else, returns false.
 */
boolean paramIsLabel(char *param) {
    symPtr ptr = symTableHead;

    /*Testing if the param is a symbol*/
    while (ptr != NULL) {
        if (strcmp(ptr->symName, param) == EQUAL)
            return TRUE;
        ptr = ptr->next;
    }
    return FALSE;
}

/*
 * Returns true if the operand is with the 4th method use.
 * Else, returns false.
 */
boolean isRegister(char *param) {
    int i, firstReg = 20;
    /*Testing if the parameter is a register*/
    for (i = firstReg; i < NUM_OF_KEYS; i++)
        if (strcmp(keys[i].symbol, param) == EQUAL)
            return TRUE;
    return FALSE;

}

/*
 * Returns true if there's an entry label within the line.
 * Else, returns false.
 */
boolean lineWithEntryLabel() {
    char lineCpy[LINE_SIZE];
    strcpy(lineCpy, line);

    /*Searching for .entry within the line.*/
    if (strstr(lineCpy, GUIDING_SENTENCE_ENT))
        return TRUE;
    return FALSE;
}

/*
 * Returns true if there's an extern label within the line.
 * Else, returns false.
 */
boolean lineWithExternLabel() {
    char lineCpy[LINE_SIZE];
    strcpy(lineCpy, line);

    /*Searching for .extern within the line.*/
    if (strstr(lineCpy, GUIDING_SENTENCE_EXTERN))
        return TRUE;
    return FALSE;
}

void resetLineNumber() {
    lineNumber = 0;
}


