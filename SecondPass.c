#include "data.h"

extern  paramsPtr paramsListHead;
extern symPtr symTableHead;
extern insPtr insListHead;
extPtr extListHead;
extern struct keyWords keys[];


/*
 * Takes one line and codes it.
 */
void secondPass(FILE* fd){

    if( generalError )
        return;

    /*Working with a ptr to the current line.*/
    line = (char *) malloc(sizeof(char) * LINE_SIZE);
    MEMORY_ERROR(line)
    lineCnt = 0;

    /*Taking one line at a time and coding it*/
    while (fgets(line , LINE_SIZE , fd) && ++lineCnt){
        if (isComment(line))
            continue;
        *strchr(line, '\n') = STR_END;    
        /*Coding the line & making the ent file*/
        if (lineWithCommandDefinition()) {
            codeCommand();
            codeParameters();
        } else if (lineWithEntryLabel())
            preWritingToFileEnt();

    }
    /*Finds if an extern symbol was not used during the run.*/
    externSymWasntUsed(fd);

    /*If all was successful we create the files.*/
    if(errorsInSymTable() == FALSE && generalError == FALSE)
        createFiles();
    free(line);

}

/*
 * Codes the command.
 */
void codeCommand(){

    char cmd[CMD_LENGTH];
    int i = 0;
    paramsPtr ptr = paramsListHead;
    Word machWord = (Word) malloc(sizeof(struct machWord));
    MEMORY_ERROR(machWord)

    getCommand(cmd);
    syncParametersToLine(&ptr);

    /*We had a problem in this line*/
    if(ptr == NULL && strcmp(cmd , "stop") != EQUAL && strcmp(cmd , "rts") != EQUAL)
        return;

    /*Putting the info we know for sure*/
    machWord->ERA = 0;
    machWord->isCmd = TRUE;
    machWord->isReg = FALSE;
    machWord->isAdd = FALSE;

    /*Finding OpCode */
    for (i = 0; i < ACTIONS_NUM; i++)
        if( strcmp(cmd, keys[i].symbol) == 0 )
            machWord->opCode = keys[i].numValue;

    /*Finding if its a special type of action*/
    if(strcmp(cmd, "jmp") == EQUAL || strcmp(cmd, "bne") == EQUAL || strcmp(cmd, "jsr") == EQUAL)
        specialActionCoding(&machWord,ptr);
    else /*Its not a special action*/
        regActionCoding(&machWord,ptr);

    /*Testing for miss use of the 4 methods.*/
    checkForErrors(cmd , machWord);
}

/*
 * Codes the parameters.
 */
void codeParameters() {

    char cmd[CMD_LENGTH], strWord[MEM_WORD_LEN];
    int i = 0, j, k;
    const unsigned int firstRegister = 20;
    unsigned int reg1Num = 0, reg2Num = 0;
    boolean firstParamCoded = FALSE, paramOneIsReg = FALSE, paramTwoIsReg = FALSE;
    boolean notImmediate = FALSE, hasSecondParam = FALSE, binaryAction = FALSE;
    boolean havntFoundReg1 = TRUE, havntFoundReg2 = TRUE;
    paramsPtr ptr = paramsListHead;
    symPtr sPtr = symTableHead;
    Word machWord;
    char firstParam[SYM_SIZE], secondParam[SYM_SIZE],temp[STRING_SIZE];

    size_t paramsNum = getNumberOfParmsInLine();
    getCommand(cmd);

    /*No parameters.*/
    if (strcmp(cmd, "stop") == EQUAL)
        return;

    binaryAction = isBinaryAction(cmd);
    syncParametersToLine(&ptr);

    if(ptr == NULL) /*We had a problem in this line.*/
        return;

    /*Working with local strings.*/
    strcpy(firstParam,ptr->firstParam);
    strcpy(secondParam,ptr->secondParam);
    hasSecondParam = strlen(secondParam) == 0 ? FALSE : TRUE;

    /*The parameters are both registers.*/
    for (i = firstRegister; i < NUM_OF_KEYS; i++) {
        if (havntFoundReg1 &&strcmp(firstParam, keys[i].symbol) == EQUAL) {
            paramOneIsReg = TRUE;
            reg1Num = keys[i].numValue;
            havntFoundReg1 = FALSE;
            j = i;
        }
        if (havntFoundReg2 &&hasSecondParam && strcmp(secondParam, keys[i].symbol) == EQUAL) {
            paramTwoIsReg = TRUE;
            reg2Num = keys[i].numValue;
            havntFoundReg2 = FALSE;
            k = i;
        }
    }
    if (paramOneIsReg && paramTwoIsReg) {
        MAKE_NEW_WORD
        bothParamsAreRegistersSetting(&machWord, reg1Num, reg2Num);
        strcpy(temp,keys[j].symbol);
        strcat(temp,keys[k].symbol);
        memWordToString(machWord,INT_NUM_REG,machWord->reg1,machWord->reg2,strWord,temp);
        addToInsList(&machWord,strWord,temp);
        return;
    }

    /*The first parameter is a label*/
    sPtr = symTableHead;
    while (sPtr && !firstParamCoded) {
        if (strcmp(firstParam, sPtr->symName) == EQUAL) {
            MAKE_NEW_WORD
            paramIsALabelSetting(&machWord, &sPtr);
            memWordToString(machWord,INT_NUM_ADD,0,0,strWord,firstParam);
            addToInsList(&machWord,strWord,firstParam);
            firstParamCoded = TRUE;
        }
        sPtr = sPtr->next;
    }

    /*The first parameter is a number*/
    if (strchr((firstParam), IMMEDIATE_METHOD_ID) || ((atoi(firstParam) != 0 && ++notImmediate))){
        MAKE_NEW_WORD
        paramIsANumberSetting(&machWord,ptr->firstParam,notImmediate);
        memWordToString(machWord,INT_NUM_ADD,0,0,strWord,firstParam);
        addToInsList(&machWord,strWord,firstParam);
        firstParamCoded = TRUE;
    }
    getCommand(cmd);

    /*The first parameter is a a register*/
    if (paramOneIsReg) {
        MAKE_NEW_WORD
        paramIsARegisterSetting(&machWord,reg1Num,TRUE,binaryAction);
        memWordToString(machWord,INT_NUM_ADD,0,0,strWord,firstParam);
        addToInsList(&machWord,strWord,firstParam);
        firstParamCoded = TRUE;
    }

    if(paramsNum == 1) /*No need to further check*/
        return;

    if(firstParamCoded == FALSE)/*Checking if we had trouble coding parameter 1.*/
        printError(undef_parameter,firstParam);

     /*The second parameter is a label*/
    for(sPtr = symTableHead; sPtr; sPtr= sPtr->next){
        if (strcmp(secondParam, sPtr->symName) == EQUAL) {
            MAKE_NEW_WORD
            paramIsALabelSetting(&machWord,&sPtr);
            memWordToString(machWord,INT_NUM_ADD,0,0,strWord,secondParam);
            addToInsList(&machWord,strWord,secondParam);
            return;
        }
    }

    /*The second parameter is a number*/
    if(strchr((secondParam), IMMEDIATE_METHOD_ID)||((atoi(secondParam) != 0 && ++notImmediate))){
        if(strcmp(cmd,"prn") != EQUAL && strcmp(cmd, "cmp") != EQUAL){
            printError(dst_meth0_illegal,secondParam);
            return;
        }
        MAKE_NEW_WORD
        paramIsANumberSetting(&machWord,ptr->secondParam,notImmediate);
        memWordToString(machWord,INT_NUM_ADD,0,0,strWord,secondParam);
        addToInsList(&machWord,strWord,secondParam);
        return;
    }

    /*The second parameter is a a register*/
    if (paramTwoIsReg) {
        MAKE_NEW_WORD
        paramIsARegisterSetting(&machWord,reg2Num, FALSE,binaryAction);
        memWordToString(machWord,INT_NUM_ADD,0,0,strWord,secondParam);
        addToInsList(&machWord,strWord,secondParam);
        return;
    }
    

    /*If we got here this means parameter 2 is undefined*/
    printError(undef_parameter,secondParam);
}

/*
 * Copies the command within the line to the pass argument.
 */
void getCommand(char* cmd){
    int i = 0;
    char lineCpy[LINE_SIZE];
    strcpy(lineCpy,line);
    /*Searching and coping the cmd to the passed argument.*/
    for(;i < ACTIONS_NUM;i++){
        if(strstr( lineCpy,keys[i].symbol ) ){
            strcpy( cmd , keys[i].symbol );
            return;
        }

    }
    /*Notifying to the caller there's no cmd in the line.*/
    cmd[0] = STR_END;
}

/*
 * Changes the type of the symbol to entry.
 */
void preWritingToFileEnt(){
    char lineCpy[LINE_SIZE];
    char tempLabel[SYM_SIZE];
    symPtr sPtr = symTableHead;
    const int entStrLen = 6;

    strcpy(lineCpy,line);

    /*Taking the label after the .entry*/
    strcpy(tempLabel,(strstr(lineCpy,GUIDING_SENTENCE_ENT) + entStrLen));
    removeSpaces(tempLabel);

    /*Validating the sym is legit.*/
    symIsIllegal(tempLabel);

    /*Changing the value of the ent sym to be true for later use.*/
    for(;sPtr ;sPtr = sPtr->next){
        if(strcmp(tempLabel,sPtr->symName) == EQUAL) {
            sPtr->isEnt = TRUE;
        }
    }
}

/*
 *Sync's the paramters from the params list to the current line.
 */
void syncParametersToLine(paramsPtr* ptr){

    while((*ptr) != NULL){
        if((*ptr)->lineNum == lineCnt)
            return;
        (*ptr) = (*ptr)->next;
    }
}

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
 * Codes the command with the setting of a special command (jmp/bne/jsr).
 */
void specialActionCoding(Word* machWord,paramsPtr ptr) {
    char *temp, lineCpy[LINE_SIZE],cmd[CMD_LENGTH], strWord[MEM_WORD_LEN];
    const unsigned int relocatable = 2, ext = 1;
    const int exSymbol = -1, cmdLength = 3;
    symPtr sPtr = symTableHead;
    boolean unaryAction = FALSE;
    Word machWord2;
    size_t paramsNum = 0, operandType;
    strcpy(lineCpy, line);
    getCommand(cmd);
    paramsNum = getNumberOfParmsInLine(); /*It returns only 1 or 2*/

    if (strlen(ptr->secondParam) == 0)
        unaryAction = TRUE;

    /*All of the actions has no src*/
    (*machWord)->srcOp = IMMEDIATE_METHOD;

    /*Finding if we have parameters*/
    paramsNum = strchr(lineCpy, O_BRACKETS) == NULL ? 0 : paramsNum;
    operandType = findOperandType();
    if (operandType == DIRECT_METHOD) {
        (*machWord)->destOp = DIRECT_METHOD;
    } else if (operandType == JUMP_METHOD) {
        (*machWord)->destOp = JUMP_METHOD;
    } else if (operandType == DIRECT_REGISTER){
        (*machWord)->destOp = DIRECT_REGISTER;
    }

    /*Finding parameters methods*/
    /*first method*/
    if (isImmediate(ptr->firstParam) || atoi(ptr->firstParam))
        (*machWord)->param1 = IMMEDIATE_METHOD;
    else if (paramIsLabel(ptr->firstParam) && paramsNum != 0)
        (*machWord)->param1 = DIRECT_METHOD;
    else if (isRegister(ptr->firstParam) && paramsNum != 0)
        (*machWord)->param1 = DIRECT_REGISTER;
    else if (paramsNum == 0)
        (*machWord)->param1 = IMMEDIATE_METHOD;
    /*second method*/
    if (unaryAction == FALSE) {/*Checking that the line has a second parameter*/
        if (isImmediate(ptr->secondParam) || atoi(ptr->secondParam))
            (*machWord)->param2 = IMMEDIATE_METHOD;
        else if (paramIsLabel(ptr->secondParam))
            (*machWord)->param2 = DIRECT_METHOD;
        else if (isRegister(ptr->secondParam))
            (*machWord)->param2 = DIRECT_REGISTER;
    }
    memWordToString((*machWord),INT_NUM_CMD,0,0,strWord,cmd);
    addToInsList(machWord,strWord,cmd);

    /*Find if the command has a label before the parameters*/
    strcpy(lineCpy,line);
    if(strchr(lineCpy,O_BRACKETS) != NULL){
        temp = (char *) malloc(sizeof(char) * SYM_SIZE);
        MEMORY_ERROR(temp)
        if ((temp = strstr(lineCpy, "jmp")) != NULL)
            temp += cmdLength;
        else if ((temp = strstr(lineCpy, "bne")) != NULL)
            temp += cmdLength;
        else if ((temp = strstr(lineCpy, "jsr")) != NULL)
            temp += cmdLength;
        temp = strtok(temp, O_BRACKETS_STR);
        removeSpaces( temp );
        machWord2 = (Word) malloc(sizeof(struct machWord));
        MEMORY_ERROR(machWord2)
        while( sPtr ) {/*Searching the label inside the Symbol Table*/
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
                memWordToString(machWord2, INT_NUM_ADD, 0, 0, strWord,temp);
                addToInsList(&machWord2,strWord,temp);
                return;
            }
            sPtr = sPtr->next;
        }

        /*If we got here this means the symbol is undefined.*/
        printError(undef_symbol,temp);
    }
}

/*
 * Codes the command with a regular setting (not jmp/bne/jsr).
 */
void regActionCoding(Word* machWord, paramsPtr ptr){
    boolean unaryAction = FALSE;
    char cmd[CMD_LENGTH], strWord[MEM_WORD_LEN];

    getCommand(cmd);
    checkForErrorsInRegAction();

    if(strstr(line,"stop") != NULL || strstr(line,"rts") != NULL){/*No operands*/
        (*machWord)->srcOp = 0;
        (*machWord)->destOp = 0;
    }else {

        if (isBinaryAction(cmd) == FALSE)
            unaryAction = TRUE;
        if(unaryAction == FALSE && strlen(ptr->secondParam) == 0)
            printError(missing_comma, "");

        /*Unary action, coding only the destination.*/
        if (unaryAction) {
        	(*machWord)->srcOp = 0; /*No src operand*/
            if (isImmediate(ptr->firstParam))
                (*machWord)->destOp = IMMEDIATE_METHOD;
            else if (paramIsLabel(ptr->firstParam))
                (*machWord)->destOp = DIRECT_METHOD;
            else if (isRegister(ptr->firstParam))
                (*machWord)->destOp = DIRECT_REGISTER;

            /*Its not an unary action, coding both.*/
        } else {

            /*First method.*/
            if (isImmediate(ptr->firstParam) || atoi(ptr->firstParam))
                (*machWord)->srcOp = IMMEDIATE_METHOD;
            else if (paramIsLabel(ptr->firstParam) )
                (*machWord)->srcOp = DIRECT_METHOD;
            else if (isRegister(ptr->firstParam))
                (*machWord)->srcOp = DIRECT_REGISTER;

            /*Second method.*/
            if (isImmediate(ptr->secondParam) || atoi(ptr->secondParam))
                (*machWord)->destOp = IMMEDIATE_METHOD;
            else if (paramIsLabel(ptr->secondParam) )
                (*machWord)->destOp = DIRECT_METHOD;
            else if (isRegister(ptr->secondParam)) {
                (*machWord)->destOp = DIRECT_REGISTER;
            }

        }

    }
    /*its not special action*/
    (*machWord)->param1 = 0;
    (*machWord)->param2 = 0;

    /*Adding the new word to the instructions list.*/
    memWordToString((*machWord), INT_NUM_CMD, 0, 0, strWord, cmd);
    addToInsList(machWord, strWord, cmd);
}

/*
 *Codes the memory word with setting of two registers.
 */
void bothParamsAreRegistersSetting(Word* machWord, unsigned reg1Num, unsigned reg2Num){
    (*machWord)->reg1 = reg1Num;
    (*machWord)->reg2 = reg2Num;
    (*machWord)->isReg = TRUE;
    (*machWord)->isCmd = FALSE;
    (*machWord)->isAdd = FALSE;
}

/*
 * Codes the memory word with the first method setting.
 */
void paramIsANumberSetting(Word* machWord, char* param, int notImmediate){
    double temp;
    int compare = 0;

    (*machWord)->isAdd = TRUE;
    (*machWord)->isCmd = FALSE;
    (*machWord)->isReg = FALSE;
    temp = notImmediate == FALSE ? atoi(param + 1) : atoi(param);
    /*Testing its not a floating parameter.*/
    compare = (int) temp;
    if(temp > compare) {/*Checking if the number is a floating point data type.*/
        printError(float_err, param);
        return;
    }

    /*Sets the address to the value of the int.*/
    if(temp != 0)
        (*machWord)->address = (unsigned)temp;
    else
        printError(undef_parameter,param);
}

/*
 *Codes the memory word with the second method setting.
 */
void paramIsALabelSetting(Word* machWord, symPtr* sPtr){
    const int relocatable = 2, ext = 1 ,exSymbol = -1;

    (*machWord)->isAdd = TRUE;
    (*machWord)->isCmd = FALSE;
    (*machWord)->isReg = FALSE;
    (*machWord)->ERAforAdd = relocatable;

    /*Testing if the parameter is defined as extern for later use.*/
    if((*sPtr)->data == exSymbol) {
        (*machWord)->address = FICTIVE_ADDRESS;
        (*machWord)->ERAforAdd = ext;
        addToExtList((*sPtr)->symName);
    }
    else
        (*machWord)->address = ((*sPtr)->data);
}

/*
 *Codes the memory word with the 4th method setting.
 */
void paramIsARegisterSetting(Word* machWord, unsigned regNum,boolean regIsFirstParam,boolean bin){
    unsigned int offset = 6;

    (*machWord)->isAdd = TRUE;
    (*machWord)->isCmd = FALSE;
    (*machWord)->isReg = FALSE;
    (*machWord)->ERAforAdd = FALSE;
    (*machWord)->ERA = FALSE;

    /*Decides how to put the value of the register according to its place*/
    if(regIsFirstParam && bin == TRUE)
        ((*machWord)->address = (regNum) << offset);
    else
        (*machWord)->address = regNum;

}

/*
 * Tests if extern symbols weren't used over the file.
 */
void externSymWasntUsed(FILE* fd){
    char tempLine[LINE_SIZE], *tempSym;
    extPtr ptr = extListHead;
    boolean found = FALSE;
    const size_t extLength = 7;
    size_t lineCntCpy = 0;

    rewind(fd);

    while(fgets(tempLine,LINE_SIZE,fd) && ++lineCntCpy){
        if(isComment(tempLine))
            continue;
        found = FALSE;

        if((tempSym = strstr(tempLine, GUIDING_SENTENCE_EXTERN)) != NULL){
            tempSym += extLength;
            removeSpaces(tempSym);
            removeSpaces(tempLine);
            for(ptr = extListHead; ptr &&!found; ptr = ptr->next){
                if(strcmp(ptr->symbol, tempSym) == EQUAL)
                    found = TRUE;
            }
            if(ptr == NULL && found == FALSE)
                printWarning(uns_ex_sym, tempSym, lineCntCpy);
        }
    }

}

/*
 * Tests if there are errors in a regular action code.
 */
void checkForErrorsInRegAction(){
    char lineCpy[LINE_SIZE], *beforeParentheses;
    size_t i = 0;
    symPtr ptr;
    strcpy(lineCpy,line);

    if(strchr(lineCpy, O_BRACKETS) == NULL)
        return;

    beforeParentheses = strtok(lineCpy, O_BRACKETS_STR);
    for(i = 0; i < ACTIONS_NUM; i++)
        if(strstr(lineCpy, keys[i].symbol) != NULL){
            beforeParentheses = strstr(beforeParentheses, keys[i].symbol);
            break;
    }

    for(ptr = symTableHead; ptr; ptr = ptr->next){
        if(strstr(beforeParentheses, ptr->symName)){
            printError(illegal_sym_mth, ptr->symName);
        }
    }
}

/*
 * Returns true if the operand is with immediate method use.
 * Else, returns false.
 */
boolean isImmediate(char* param){
    char paramCpy[LINE_SIZE];
    strcpy(paramCpy,param);

    /*Searching for '#' in the line.*/
    if(strchr(paramCpy,IMMEDIATE_METHOD_ID))
        return TRUE;
    return FALSE;

}

/*
 *Returns true if the operand is with the 1st method use.
 * Else, returns false.
 */
boolean paramIsLabel(char* param){
    symPtr p = symTableHead;

    /*Testing if the param is a symbol*/
    while(p != NULL){
        if(strcmp(p->symName, param) == EQUAL)
            return TRUE;
        p = p->next;
    }
    return FALSE;
}

/*
 * Returns true if the operand is with the 4th method use.
 * Else, returns false.
 */
boolean isRegister(char* param){
    int i, firstReg = 20;
    /*Testing if the parameter is a register*/
    for(i = firstReg; i <NUM_OF_KEYS;i++)
        if(strcmp(keys[i].symbol, param) == EQUAL)
            return TRUE;
    return FALSE;

}

/*
 * Returns true if there's an entry label within the line.
 * Else, returns false.
 */
boolean lineWithEntryLabel(){
    char lineCpy[LINE_SIZE];
    strcpy(lineCpy,line);

    /*Searching for .entry within the line.*/
    if(strstr(lineCpy, GUIDING_SENTENCE_ENT))
        return TRUE;
    return FALSE;
}

/*
 * Returns true if there's an extern label within the line.
 * Else, returns false.
 */
boolean lineWithExternLabel(){
    char lineCpy[LINE_SIZE];
    strcpy(lineCpy,line);

    /*Searching for .extern within the line.*/
    if(strstr(lineCpy, GUIDING_SENTENCE_EXTERN))
        return TRUE;
    return FALSE;
}

