#include "declarationsHeader.h"

size_t memoryWords;




/*
 * Calculating the number of memory words the line will take.
 */
void calCmdLine(int *IC) {
    analyseParameters();
    memoryWords += labelBeforeParam()  ? 1 : 0;

    if (lineWithLabelDefinition())
        updateLabelAddress(lineListHead->str, IC);

    incMemoryWordsByOne();
    (*IC) += memoryWords;
}


/*
 * Counting the number of parameters in the line & inserting them into a list
 */
void analyseParameters() {
    char lineCpy[LINE_SIZE], cmd[CMD_LENGTH], tmp[LINE_SIZE];
    linePtr ptr = lineListHead;

    strcpy(lineCpy, line);
    getCommand(cmd);

    if (noParameters(cmd))
        return;
    ptr = movePtrToFirstParameter(ptr, cmd);

    moveLineAfterSymbol(lineCpy);

    /*tmp pointing to the line since the first string after the command.*/
    strcpy(tmp, strstr(lineCpy, ptr->str));
    strcpy(lineCpy, tmp);
    removeSpaces(lineCpy);

    if (twoParameters(lineCpy)) {
        codeTwoParameters(lineCpy);
    } else {
        codeOneParameter(lineCpy);
    }

}

void moveLineAfterSymbol(char* lineCpy) {
    char lineCpy2[LINE_SIZE];
    if (strchr(lineCpy, ISLABEL) != NULL) {/*Moving the lineCpy after the symbol.*/
        strcpy(lineCpy2, line);
        strcpy(lineCpy, (strchr(lineCpy2, ISLABEL)));
    }

}

void codeOneParameter(char *lineCpy) {
    char firstParam[STRING_SIZE];

    strcpy(firstParam, lineCpy);
    addToParamsList(firstParam, NULL);
    checkForFloatErrors(firstParam, NULL);
    incMemoryWordsByOne();
}




void codeTwoParameters(char *lineCpy) {
    char firstParam[STRING_SIZE], secondParam[STRING_SIZE];
    size_t i = 0, j = 0;
    incMemoryWordsByTwo();

    if (strchr(lineCpy, O_BRACKETS) != NULL) {/* Line has brackets.*/
        strcpy(lineCpy, strchr(lineCpy, O_BRACKETS) + 1);
        while ((firstParam[i] = lineCpy[i]) != COMMA)/*Getting first param.*/
            i++;
        firstParam[i++] = STR_END;
        while ((secondParam[j++] = lineCpy[i]) != C_BRACKETS)/*Getting second param.*/
            i++;
        secondParam[--j] = STR_END;
        addToParamsList(firstParam, secondParam);

    } else {/* Just parameters.*/

        strcpy(firstParam, strtok(lineCpy, COMMA_STR));
        strcpy(secondParam, strtok(NULL, COMMA_STR));
        addToParamsList(firstParam, secondParam);
    }

    memoryWords += bothParametersAreRegisters(firstParam, secondParam) ? -1
                                                                       : 0;/*Two reg's takes one word.*/
    checkForFloatErrors(firstParam, secondParam);
}

void incMemoryWordsByOne() {
    memoryWords++;
}

void incMemoryWordsByTwo() {
    memoryWords += 2;
}

linePtr movePtrToFirstParameter(linePtr ptr, char *cmd) {
    for (ptr = lineListHead; ptr; ptr = ptr->next) {
        if (strstr(ptr->str, cmd)) {
            ptr = ptr->next;
            return ptr;
        }
    }
}


boolean noParameters(char *cmd) {
    return (strcmp(cmd, "stop") == EQUAL || strcmp(cmd, "rts") == EQUAL);
}

boolean twoParameters(char *lineCpy) {
    return strchr(lineCpy, COMMA) != NULL;
}
