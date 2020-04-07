#include "declarationsHeader.h"


/*
 * Codes the command with a regular setting (not jmp/bne/jsr).
 */
void regActionCoding(Word *machWord, paramsPtr ptr) {
    boolean unaryAction = FALSE;
    char cmd[CMD_LENGTH], strWord[MEM_WORD_LEN];

    getCommand(cmd);
    checkForErrorsInRegAction();

    if (noOperands(line)) {
        (*machWord)->srcOp = 0;
        (*machWord)->destOp = 0;
    } else {
        if (isBinaryAction(cmd) == FALSE)
            unaryAction = TRUE;
        if (unaryAction == FALSE && strlen(ptr->secondParam) == 0)
            printError(missing_comma, "");

        if (unaryAction) {   /*Unary action, coding only the destination.*/
            unaryActionCoding(machWord, ptr);

        } else { /*Its not an unary action, coding both.*/
            firstMethodCoding(machWord, ptr);
            secondMethodCoding(machWord, ptr);
        }
    }
    /*its not special action*/
    (*machWord)->param1 = 0;
    (*machWord)->param2 = 0;

    /*Adding the new word to the instructions list.*/
    memWordToString((*machWord), INT_NUM_CMD, 0, 0, strWord, cmd);
    addToInsList(machWord, strWord, cmd);
}

void secondMethodCoding(Word *machWord, paramsPtr ptr) {
    if (isImmediate(ptr->secondParam) || atoi(ptr->secondParam))
        (*machWord)->destOp = IMMEDIATE_METHOD;
    else if (paramIsLabel(ptr->secondParam))
        (*machWord)->destOp = DIRECT_METHOD;
    else if (isRegister(ptr->secondParam)) {
        (*machWord)->destOp = DIRECT_REGISTER;
    }

}

void firstMethodCoding(Word *machWord, paramsPtr ptr) {
    if (isImmediate(ptr->firstParam) || atoi(ptr->firstParam))
        (*machWord)->srcOp = IMMEDIATE_METHOD;
    else if (paramIsLabel(ptr->firstParam))
        (*machWord)->srcOp = DIRECT_METHOD;
    else if (isRegister(ptr->firstParam))
        (*machWord)->srcOp = DIRECT_REGISTER;

}

void unaryActionCoding(Word *machWord, paramsPtr ptr) {
    (*machWord)->srcOp = 0; /*No src operand*/
    if (isImmediate(ptr->firstParam))
        (*machWord)->destOp = IMMEDIATE_METHOD;
    else if (paramIsLabel(ptr->firstParam))
        (*machWord)->destOp = DIRECT_METHOD;
    else if (isRegister(ptr->firstParam))
        (*machWord)->destOp = DIRECT_REGISTER;

}

boolean noOperands(char *line) {
    return strstr(line, "stop") != NULL || strstr(line, "rts") != NULL;
}

/*
 *Codes the memory word with setting of two registers.
 */
void bothParamsAreRegistersSetting(Word *machWord, unsigned reg1Num, unsigned reg2Num) {
    (*machWord)->reg1 = reg1Num;
    (*machWord)->reg2 = reg2Num;
    (*machWord)->isReg = TRUE;
    (*machWord)->isCmd = FALSE;
    (*machWord)->isAdd = FALSE;
}

/*
 *Codes the memory word with the 4th method setting.
 */
void
paramIsARegisterSetting(Word *machWord, unsigned regNum, boolean regIsFirstParam, boolean bin) {
    unsigned int offset = 6;

    (*machWord)->isAdd = TRUE;
    (*machWord)->isCmd = FALSE;
    (*machWord)->isReg = FALSE;
    (*machWord)->ERAforAdd = FALSE;
    (*machWord)->ERA = FALSE;

    /*Decides how to put the value of the register according to its place*/
    if (regIsFirstParam && bin == TRUE)
        ((*machWord)->address = (regNum) << offset);
    else
        (*machWord)->address = regNum;

}

