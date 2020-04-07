
#include "declarationsHeader.h"

extern symPtr symTableHead;

/*
 * Codes the command with the setting of a special command (jmp/bne/jsr).
 */
void specialActionCoding(Word *machWord, paramsPtr ptr) {
    char lineCpy[LINE_SIZE], cmd[CMD_LENGTH], strWord[MEM_WORD_LEN];
    boolean unaryAction = FALSE;
    size_t paramsNum = 0;
    strcpy(lineCpy, line);
    getCommand(cmd);
    paramsNum = strchr(lineCpy, O_BRACKETS) == NULL ? 0 : getNumberOfParametersInLine();

    if (strlen(ptr->secondParam) == 0)
        unaryAction = TRUE;

    /*All of the actions has no src*/
    (*machWord)->srcOp = IMMEDIATE_METHOD;

    findParameters(machWord);
    codeFirstParamMethod(machWord, ptr, paramsNum);
    codeSecondParamMethod(machWord, ptr, unaryAction);

    memWordToString((*machWord), INT_NUM_CMD, 0, 0, strWord, cmd);
    addToInsList(machWord, strWord, cmd);

    strcpy(lineCpy, line);
    findLabelBeforeParameters(lineCpy, strWord);

}

void findLabelBeforeParameters(char *lineCpy, char *strWord) {
    char *temp;
    Word machWord2;
    symPtr sPtr = symTableHead;
    const int cmdLength = 3;
    if (strchr(lineCpy, O_BRACKETS) != NULL) {
        temp = (char *) malloc(sizeof(char) * SYM_SIZE);
        MEMORY_ERROR(temp)
        if ((temp = strstr(lineCpy, "jmp")) != NULL)
            temp += cmdLength;
        else if ((temp = strstr(lineCpy, "bne")) != NULL)
            temp += cmdLength;
        else if ((temp = strstr(lineCpy, "jsr")) != NULL)
            temp += cmdLength;
        temp = strtok(temp, O_BRACKETS_STR);
        removeSpaces(temp);
        machWord2 = (Word) malloc(sizeof(struct machWord));
        MEMORY_ERROR(machWord2)
        if (searchLabel(sPtr, machWord2, temp, strWord))
            return;

        /*If we got here this means the symbol is undefined.*/
        printError(undef_symbol, temp);
    }

}

boolean searchLabel(symPtr sPtr, Word machWord2, char *temp, char *strWord) {
    const unsigned int relocatable = 2;
    const int exSymbol = -1, ext = 1;
    while (sPtr) {
        if (strcmp(temp, sPtr->symName) == EQUAL) {
            machWord2->ERAforAdd = relocatable;
            if (sPtr->data == exSymbol) {
                machWord2->address = FICTIVE_ADDRESS;
                machWord2->ERAforAdd = ext;
                addToExtList(sPtr->symName);
            } else
                machWord2->address = (sPtr->data);

            machWord2->isAdd = TRUE;
            machWord2->isCmd = FALSE;
            machWord2->isReg = FALSE;
            memWordToString(machWord2, INT_NUM_ADD, 0, 0, strWord, temp);
            addToInsList(&machWord2, strWord, temp);
            return TRUE;
        }
        sPtr = sPtr->next;
    }
    return FALSE;

}

void codeSecondParamMethod(Word *machWord, paramsPtr ptr, boolean unaryAction) {
    if (unaryAction == FALSE) {/*Checking that the line has a second parameter*/
        if (isImmediate(ptr->secondParam) || atoi(ptr->secondParam))
            (*machWord)->param2 = IMMEDIATE_METHOD;
        else if (paramIsLabel(ptr->secondParam))
            (*machWord)->param2 = DIRECT_METHOD;
        else if (isRegister(ptr->secondParam))
            (*machWord)->param2 = DIRECT_REGISTER;
    }

}

void codeFirstParamMethod(Word *machWord, paramsPtr ptr, int paramsNum) {
    if (isImmediate(ptr->firstParam) || atoi(ptr->firstParam))
        (*machWord)->param1 = IMMEDIATE_METHOD;
    else if (paramIsLabel(ptr->firstParam) && paramsNum != 0)
        (*machWord)->param1 = DIRECT_METHOD;
    else if (isRegister(ptr->firstParam) && paramsNum != 0)
        (*machWord)->param1 = DIRECT_REGISTER;
    else if (paramsNum == 0)
        (*machWord)->param1 = IMMEDIATE_METHOD;

}

void findParameters(Word *machWord) {
    size_t operandType;

    operandType = findOperandType();
    if (operandType == DIRECT_METHOD) {
        (*machWord)->destOp = DIRECT_METHOD;
    } else if (operandType == JUMP_METHOD) {
        (*machWord)->destOp = JUMP_METHOD;
    } else if (operandType == DIRECT_REGISTER) {
        (*machWord)->destOp = DIRECT_REGISTER;
    }

}
