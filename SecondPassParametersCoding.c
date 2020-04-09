
#include "declarationsHeader.h"

extern paramsPtr paramsListHead;
extern symPtr symTableHead;
extern struct keyWords keys[];

boolean firstParamCoded, isParamOneReg, isParamTwoReg;
boolean isNotImmediate, isSecondParamAvailable, isBinAction;
char firstParam[SYM_SIZE], secondParam[SYM_SIZE];
char cmd[CMD_LENGTH], strWord[MEM_WORD_LEN], temp[STRING_SIZE];
unsigned int reg1Num = 0, reg2Num = 0;


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

    printErrorWithComment(undef_parameter, secondParam);
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

    if (!firstParamCoded)/*Checking if we had trouble coding parameter 1.*/
        printErrorWithComment(undef_parameter, firstParam);

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

    if (isParamTwoReg) {
        MAKE_NEW_WORD
        paramIsARegisterSetting(&machWord, reg2Num, FALSE, isBinAction);
        memWordToString(machWord, INT_NUM_ADD, 0, 0, strWord, secondParam);
        addToInsList(&machWord, strWord, secondParam);
        return TRUE;
    }
    return FALSE;
}

boolean codedAsSecondParameterIsNumber(paramsPtr ptr) {
    Word machWord;

    if (strchr((secondParam), IMMEDIATE_METHOD_ID) ||
        ((atoi(secondParam) != 0 && ++isNotImmediate))) {
        if (strcmp(cmd, "prn") != EQUAL && strcmp(cmd, "cmp") != EQUAL) {
            printErrorWithComment(dst_meth0_illegal, secondParam);
            return TRUE;
        }
        MAKE_NEW_WORD
        paramIsANumberSetting(&machWord, ptr->secondParam, isNotImmediate);
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

    if (isParamOneReg) {
        MAKE_NEW_WORD
        paramIsARegisterSetting(&machWord, reg1Num, TRUE, isBinAction);
        memWordToString(machWord, INT_NUM_ADD, 0, 0, strWord, firstParam);
        addToInsList(&machWord, strWord, firstParam);
        firstParamCoded = TRUE;
    }

}

void codedAsFirstParameterIsNumber(paramsPtr ptr) {
    Word machWord;

    if (strchr((firstParam), IMMEDIATE_METHOD_ID) ||
        ((atoi(firstParam) != 0 && ++isNotImmediate))) {
        MAKE_NEW_WORD
        paramIsANumberSetting(&machWord, ptr->firstParam, isNotImmediate);
        memWordToString(machWord, INT_NUM_ADD, 0, 0, strWord, firstParam);
        addToInsList(&machWord, strWord, firstParam);
        firstParamCoded = TRUE;
    }

}

void resetFlags() {
    firstParamCoded = FALSE;
    isParamOneReg = FALSE;
    isParamTwoReg = FALSE;
    isNotImmediate = FALSE;
    isSecondParamAvailable = FALSE;
    isBinAction = FALSE;
    reg1Num = 0;
    reg2Num = 0;
    getCommand(cmd);
    isBinAction = isBinaryAction(cmd);
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
    boolean isReg1Found = FALSE, isReg2Found = FALSE;
    isSecondParamAvailable = strlen(secondParam) == 0 ? FALSE : TRUE;

    for (i = firstRegister; i < NUM_OF_KEYS; i++) {
        if (!isReg1Found && strcmp(firstParam, keys[i].symbol) == EQUAL) {
            isParamOneReg = TRUE;
            reg1Num = keys[i].numValue;
            isReg1Found = TRUE;
            j = i;
        }
        if (!isReg2Found && isSecondParamAvailable && strcmp(secondParam, keys[i].symbol) == EQUAL) {
            isParamTwoReg = TRUE;
            reg2Num = keys[i].numValue;
            isReg2Found = TRUE;
            k = i;
        }
    }
    if (isParamOneReg && isParamTwoReg) {
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

boolean errorInLine(paramsPtr ptr) {
    return ptr == NULL ? TRUE : FALSE;
}

void copyParameters(paramsPtr ptr) {
    strcpy(firstParam, ptr->firstParam);
    strcpy(secondParam, ptr->secondParam);
}
