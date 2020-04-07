
#include "declarationsHeader.h"

extern struct keyWords keys[];
extern symPtr symTableHead;


/*
 * The func takes as input a string and returns it without any white chars.
 */
void removeSpaces(char *sym) {
    int cnt = 0, i = 0;
    for (; sym[i]; i++) {
        if (!isspace(sym[i])) {
            sym[cnt++] = sym[i];
        }
    }
    sym[cnt] = STR_END;
}

/*
 * Returns the passed argument as a 14 long string in weird binary code.
 */
char *decToWeirdBinary(int num) {
    int i;
    unsigned int mask = 1;
    char *res = (char *) malloc(sizeof(char) * (MEM_WORD_LEN));
    MEMORY_ERROR(res)
    res[MEM_WORD_LEN] = STR_END;/*Putting the null char.*/

    /*Takes the num and makes from it a 15 length long binary string.*/
    for (i = MEM_WORD_LEN - 1; i >= 0; --i, mask <<= 1) {
        if ((unsigned) num & mask)
            res[i] = SLASH;
        else
            res[i] = DOT;
    }
    return res;
}

/*
 * Tests for logic errors withing the command.
 */
void checkForErrors(char *cmd, Word machWord) {
    short i;
    for (i = 0; i < ACTIONS_NUM; i++) {
        if (strcmp(keys[i].symbol, cmd) == EQUAL)
            break;
    }
    /*Looking for miss use of the methods - this block tests srcOp..*/
    if (keys[i].legitSrcMethodZro == FALSE &&
        machWord->srcOp == IMMEDIATE_METHOD && keys[i].hasSrc == TRUE)
        printError(src_meth0_illegal, cmd);
    else if (keys[i].legitSrcMethodOne == FALSE && machWord->srcOp == DIRECT_METHOD)
        printError(src_meth1_illegal, cmd);
    else if (keys[i].legitSrcMethodTwo == FALSE && machWord->srcOp == JUMP_METHOD)
        printError(src_meth2_illegal, "sagi");
    else if (keys[i].legitSrcMethodThr == FALSE && machWord->srcOp == DIRECT_REGISTER)
        printError(src_meth3_illegal, cmd);

    /*This block tests DestOp part of the command.*/
    if (keys[i].legitDstMethodZro == FALSE && machWord->destOp == 0 && keys[i].hasDst == TRUE)
        printError(dst_meth0_illegal, cmd);
    else if (keys[i].legitDstMethodOne == FALSE && machWord->destOp == DIRECT_METHOD)
        printError(dst_meth1_illegal, cmd);
    else if (keys[i].legitDstMethodTwo == FALSE && machWord->destOp == JUMP_METHOD)
        printError(dst_meth2_illegal, cmd);
    else if (keys[i].legitDstMethodThr == FALSE && machWord->destOp == DIRECT_REGISTER)
        printError(dst_meth3_illegal, cmd);

}

/*
 * Copies the command within the line to the pass argument.
 */
void getCommand(char *cmd) {
    int i = 0;
    char lineCpy[LINE_SIZE];
    strcpy(lineCpy, line);
    /*Searching and coping the cmd to the passed argument.*/
    for (; i < ACTIONS_NUM; i++) {
        if (strstr(lineCpy, keys[i].symbol)) {
            strcpy(cmd, keys[i].symbol);
            return;
        }

    }
    /*Notifying to the caller there's no cmd in the line.*/
    cmd[0] = STR_END;
}

/*
 * Returns the number of parameters within the line.
 */
size_t getNumberOfParametersInLine() {
    const size_t ONE_PARAMETER = 1, TWO_PARAMETERS = 2;
    char lineCpy[LINE_SIZE];
    strcpy(lineCpy, line);
    /*Returns number of parameters in the line, returns only 1 or 2.*/
    if (strchr(lineCpy, COMMA) != NULL)
        return TWO_PARAMETERS;
    return strchr(lineCpy, COMMA) == NULL ? ONE_PARAMETER : TWO_PARAMETERS;
}

/*
 * Returns the type of the operand.
 */
size_t findOperandType() {
    char lineCpy[LINE_SIZE], *temp;
    short i = 0, firstRegister = 20, cmdLength = 3;
    symPtr ptr;
    strcpy(lineCpy, line);
    temp = (char *) malloc(sizeof(char) * LINE_SIZE);

    /*These cmds doesnt have operands*/
    if (strstr(lineCpy, "stop") != NULL || strstr(lineCpy, "rts") != NULL) {
        free(temp);
        return 0;
    }

    /*Getting to the line from the start of the cmd.*/
    for (i = 0; i < ACTIONS_NUM; i++) {
        if ((temp = strstr(lineCpy, keys[i].symbol)) != NULL)
            break;
    }

    /*Getting pass the cmd to the operands.*/
    temp += cmdLength;
    /*Getting rid of white chars.*/
    removeSpaces(temp);
    /*If label and parenthesis its jump*/
    if (temp[0] != O_BRACKETS && strchr(temp, O_BRACKETS) != NULL)
        return JUMP_METHOD;

    /*If no parameters and str == register its 3*/
    if (strchr(temp, O_BRACKETS) == NULL)
        for (i = firstRegister; i < NUM_OF_KEYS; i++)
            if (strcmp(temp, keys[i].symbol) == EQUAL)
                return DIRECT_REGISTER;

    /* Else it has to be a label*/
    for (ptr = symTableHead; ptr; ptr = ptr->next)
        if (strcmp(temp, ptr->symName) == EQUAL)
            return DIRECT_METHOD;

    /*If we got here it means the parameter is neither of the above.*/
    printError(undef_parameter, temp);
    return 0;
}

/*
 * Returns true if the line is a comment line, else returns false.
 */
boolean lineIsComment(char *line) {
    char lineCopy[STRING_SIZE];
    size_t i = 0;
    strcpy(lineCopy, line);

    /*Searching for ';' char within the line.*/
    if (strchr(lineCopy, ISCOMMENT))
        return TRUE;
    for (i = 0; lineCopy[i]; i++) {
        if (isspace(lineCopy[i]) == FALSE)
            break;
    }

    /*Blank line.*/
    if (i == strlen(lineCopy))
        return TRUE;
    return FALSE;
}

/*
 * Fills the lineList list with each string within the current line.
 */
void fillLineList() {
    int offset = 0;
    const int ok = 1;
    char str[LINE_SIZE];
    char *lineCopy = (char *) malloc(sizeof(char) * LINE_SIZE);
    MEMORY_ERROR(lineCopy)

    /*Zeroing the size and working on a copy of the line*/
    lineListLength = 0;
    strcpy(lineCopy, line);

    /*Extracting each string from the line*/
    while (sscanf(lineCopy, "%s%n", str, &offset) == ok) {
        lineCopy += offset;
        addToLineList(str);
    }
}

/*
 * Returns True if the command is a binary command, else, returns false.
 */
boolean isBinaryAction(char *cmd) {

    /*The following cmd's are binary commands.*/
    if (strcmp(cmd, "mov") == EQUAL || strcmp(cmd, "cmp") == EQUAL)
        return TRUE;
    if (strcmp(cmd, "add") == EQUAL || strcmp(cmd, "sub") == EQUAL)
        return TRUE;
    if (strcmp(cmd, "lea") == EQUAL)
        return TRUE;
    return FALSE;
}

/*
 * Takes a string as argument and fills it with the appropriate weird binary code.
 */
void
memWordToString(Word word, size_t intNum, size_t reg1, size_t reg2, char *tempWord, char *str) {
    unsigned int *machWord = (unsigned int *) word, mask;
    const size_t bothRegister = 1, maskForReg = 32, maskForAddAndCmd = 8192, twoReg = 2, last = 14;
    /*32 = 0..0100000, 8192 = 0..010000000000000 in binary.*/
    size_t i, j = 0;

    if (generalError)
        return;
    if (intNum == bothRegister) {

        for (i = 0; i < twoReg; i++) {
            for (mask = maskForReg; mask; mask >>= 1, ++j) {
                if ((unsigned) reg1 & mask) {
                    tempWord[j] = SLASH;
                } else {
                    tempWord[j] = DOT;
                }
            }
            /*Moving to the second register.*/
            reg1 = reg2;
        }

        /*the ERA is absolute for register.*/
        tempWord[j] = DOT;
        tempWord[++j] = DOT;

    } else {

        /*Finds the binary form of the word and puts it into the buffer.*/
        for (mask = maskForAddAndCmd; mask; mask >>= 1, ++j) {
            if ((*(machWord + intNum)) & mask) {
                tempWord[j] = SLASH;
            } else {
                tempWord[j] = DOT;
            }
        }
    }

    /*Making sure there's the null char is the string.*/
    tempWord[last] = STR_END;
}

/*
 * Returns true if the operand is with immediate method use.
 * Else, returns false.
 */
boolean isImmediate(char *param) {
    char paramCpy[LINE_SIZE];
    strcpy(paramCpy, param);

    /*Searching for '#' in the line.*/
    if (strchr(paramCpy, IMMEDIATE_METHOD_ID))
        return TRUE;
    return FALSE;

}


