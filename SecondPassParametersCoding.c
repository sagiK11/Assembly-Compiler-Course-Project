
#include "declarationsHeader.h"

extern paramsPtr paramsListHead;
extern symPtr symTableHead;
extern struct keyWords keys[];

boolean firstParamCoded = FALSE, paramOneIsReg = FALSE, paramTwoIsReg = FALSE;
boolean notImmediate = FALSE, hasSecondParam = FALSE, binaryAction = FALSE;
boolean havntFoundReg1 = TRUE, havntFoundReg2 = TRUE;
char firstParam[SYM_SIZE], secondParam[SYM_SIZE];
char cmd[CMD_LENGTH], strWord[MEM_WORD_LEN], temp[STRING_SIZE];
unsigned int reg1Num = 0, reg2Num = 0;

/*
 * Codes the parameters.
 */
void codeParameters() {
    paramsPtr ptr = paramsListHead;
    resetFlags();

    if (noParameters(cmd))
        return;

    syncParametersToLine(&ptr);
    if (errorInLine(ptr))
        return;

    copyParameters(ptr);

    if (codingParametersSuccessful(ptr))
        return;

    printError(undef_parameter, secondParam);
}

boolean errorInLine(paramsPtr ptr) {
    return ptr == NULL ? TRUE : FALSE;
}

boolean codingParametersSuccessful(paramsPtr ptr) {
    size_t paramsNum = getNumberOfParametersInLine();

    if (codedAsBothParametersAreRegisters())
        return TRUE;

    codedAsFirstParamIsLabel();
    codedAsFirstParameterIsNumber(ptr);
    codedAsFirstParameterIsRegister();

    if (paramsNum == 1) /*No need to further check*/
        return TRUE;

    if (firstParamCoded == FALSE)/*Checking if we had trouble coding parameter 1.*/
        printError(undef_parameter, firstParam);

    if (codedAsSecondParameterIsLabel())
        return TRUE;

    if (codedAsSecondParameterIsNumber(ptr))
        return TRUE;

    if (codedAsSecondParameterIsRegister())
        return TRUE;
    return FALSE;
}

boolean codedAsSecondParameterIsRegister() {
    Word machWord;

    if (paramTwoIsReg) {
        MAKE_NEW_WORD
        paramIsARegisterSetting(&machWord, reg2Num, FALSE, binaryAction);
        memWordToString(machWord, INT_NUM_ADD, 0, 0, strWord, secondParam);
        addToInsList(&machWord, strWord, secondParam);
        return TRUE;
    }
    return FALSE;
}

boolean codedAsSecondParameterIsNumber(paramsPtr ptr) {
    Word machWord;

    if (strchr((secondParam), IMMEDIATE_METHOD_ID) ||
        ((atoi(secondParam) != 0 && ++notImmediate))) {
        if (strcmp(cmd, "prn") != EQUAL && strcmp(cmd, "cmp") != EQUAL) {
            printError(dst_meth0_illegal, secondParam);
            return TRUE;
        }
        MAKE_NEW_WORD
        paramIsANumberSetting(&machWord, ptr->secondParam, notImmediate);
        memWordToString(machWord, INT_NUM_ADD, 0, 0, strWord, secondParam);
        addToInsList(&machWord, strWord, secondParam);
        return TRUE;
    }
    return FALSE;
}

boolean codedAsSecondParameterIsLabel() {
    symPtr sPtr;
    Word machWord;

    for (sPtr = symTableHead; sPtr; sPtr = sPtr->next) {
        if (strcmp(secondParam, sPtr->symName) == EQUAL) {
            MAKE_NEW_WORD
            paramIsALabelSetting(&machWord, &sPtr);
            memWordToString(machWord, INT_NUM_ADD, 0, 0, strWord, secondParam);
            addToInsList(&machWord, strWord, secondParam);
            return TRUE;
        }
    }
    return FALSE;

}

void codedAsFirstParameterIsRegister() {
    Word machWord;

    if (paramOneIsReg) {
        MAKE_NEW_WORD
        paramIsARegisterSetting(&machWord, reg1Num, TRUE, binaryAction);
        memWordToString(machWord, INT_NUM_ADD, 0, 0, strWord, firstParam);
        addToInsList(&machWord, strWord, firstParam);
        firstParamCoded = TRUE;
    }

}

void codedAsFirstParameterIsNumber(paramsPtr ptr) {
    Word machWord;

    if (strchr((firstParam), IMMEDIATE_METHOD_ID) ||
        ((atoi(firstParam) != 0 && ++notImmediate))) {
        MAKE_NEW_WORD
        paramIsANumberSetting(&machWord, ptr->firstParam, notImmediate);
        memWordToString(machWord, INT_NUM_ADD, 0, 0, strWord, firstParam);
        addToInsList(&machWord, strWord, firstParam);
        firstParamCoded = TRUE;
    }

}

void resetFlags() {
    firstParamCoded = FALSE;
    paramOneIsReg = FALSE;
    paramTwoIsReg = FALSE;
    notImmediate = FALSE;
    hasSecondParam = FALSE;
    binaryAction = FALSE;
    havntFoundReg1 = TRUE;
    havntFoundReg2 = TRUE;
    reg1Num = 0;
    reg2Num = 0;
    getCommand(cmd);
    binaryAction = isBinaryAction(cmd);
}

void codedAsFirstParamIsLabel() {
    symPtr sPtr = symTableHead;
    Word machWord;

    while (sPtr && !firstParamCoded) {
        if (strcmp(firstParam, sPtr->symName) == EQUAL) {
            MAKE_NEW_WORD
            paramIsALabelSetting(&machWord, &sPtr);
            memWordToString(machWord, INT_NUM_ADD, 0, 0, strWord, firstParam);
            addToInsList(&machWord, strWord, firstParam);
            firstParamCoded = TRUE;
        }
        sPtr = sPtr->next;
    }


}

boolean codedAsBothParametersAreRegisters() {
    int i = 0, j, k;
    const unsigned int firstRegister = 20;
    Word machWord;
    hasSecondParam = strlen(secondParam) == 0 ? FALSE : TRUE;

    for (i = firstRegister; i < NUM_OF_KEYS; i++) {
        if (havntFoundReg1 && strcmp(firstParam, keys[i].symbol) == EQUAL) {
            paramOneIsReg = TRUE;
            reg1Num = keys[i].numValue;
            havntFoundReg1 = FALSE;
            j = i;
        }
        if (havntFoundReg2 && hasSecondParam && strcmp(secondParam, keys[i].symbol) == EQUAL) {
            paramTwoIsReg = TRUE;
            reg2Num = keys[i].numValue;
            havntFoundReg2 = FALSE;
            k = i;
        }
    }
    if (paramOneIsReg && paramTwoIsReg) {
        MAKE_NEW_WORD
        bothParamsAreRegistersSetting(&machWord, reg1Num, reg2Num);
        strcpy(temp, keys[j].symbol);
        strcat(temp, keys[k].symbol);
        memWordToString(machWord, INT_NUM_REG, machWord->reg1, machWord->reg2, strWord, temp);
        addToInsList(&machWord, strWord, temp);
        return TRUE;
    }
    return FALSE;

}

void copyParameters(paramsPtr ptr) {
    strcpy(firstParam, ptr->firstParam);
    strcpy(secondParam, ptr->secondParam);
}
