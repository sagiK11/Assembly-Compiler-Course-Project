#include "declarationsHeader.h"

extern paramsPtr paramsListHead;
extern symPtr symTableHead;
extern insPtr insListHead;
extPtr extListHead;
extern struct keyWords keys[];

void preCode(Word machWord) {
    /*Putting the info we know for sure*/
    machWord->ERA = 0;
    machWord->isCmd = TRUE;
    machWord->isReg = FALSE;
    machWord->isAdd = FALSE;
}

/*
 * Codes the command.
 */
void codeCommand() {
    char cmd[CMD_LENGTH];
    paramsPtr ptr = paramsListHead;
    Word machWord = (Word) malloc(sizeof(struct machWord));
    MEMORY_ERROR(machWord)

    getCommand(cmd);
    syncParametersToLine(&ptr);

    if (nothingToCode(ptr, cmd))
        return;

    preCode(machWord);
    findOpCode(machWord, cmd);

    if (specialTypeAction(cmd))
        specialActionCoding(&machWord, ptr);
    else
        regActionCoding(&machWord, ptr);

    checkForErrors(cmd, machWord);
}


/*
 * Codes the memory word with the first method setting.
 */
void paramIsANumberSetting(Word *machWord, char *param, int notImmediate) {
    double temp;
    int compare = 0;

    (*machWord)->isAdd = TRUE;
    (*machWord)->isCmd = FALSE;
    (*machWord)->isReg = FALSE;
    temp = notImmediate == FALSE ? atoi(param + 1) : atoi(param);
    /*Testing its not a floating parameter.*/
    compare = (int) temp;
    if (temp > compare) {/*Checking if the number is a floating point data type.*/
        printError(float_err, param);
        return;
    }

    /*Sets the address to the value of the int.*/
    if (temp != 0)
        (*machWord)->address = (unsigned) temp;
    else
        printError(undef_parameter, param);
}

/*
 *Codes the memory word with the second method setting.
 */
void paramIsALabelSetting(Word *machWord, symPtr *sPtr) {
    const int relocatable = 2, ext = 1, exSymbol = -1;

    (*machWord)->isAdd = TRUE;
    (*machWord)->isCmd = FALSE;
    (*machWord)->isReg = FALSE;
    (*machWord)->ERAforAdd = relocatable;

    /*Testing if the parameter is defined as extern for later use.*/
    if ((*sPtr)->data == exSymbol) {
        (*machWord)->address = FICTIVE_ADDRESS;
        (*machWord)->ERAforAdd = ext;
        addToExtList((*sPtr)->symName);
    } else
        (*machWord)->address = ((*sPtr)->data);
}


