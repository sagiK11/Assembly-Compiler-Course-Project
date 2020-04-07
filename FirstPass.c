#include "data.h"

extern struct keyWords keys[];
extern dataPtr dataListHead;
boolean sentenceWithLabel;
boolean sentenceWithCommand;
size_t memWords;

/*
 * Takes one line from the file and runs tests over it and gathering info for the second pass.
 */
void firstPass(FILE* fd) {
    int IC = STARTING_ADDRESS, DC = 0;
    lineCnt = 0;
    line = (char *) malloc(sizeof(char) * LINE_SIZE);
    MEMORY_ERROR(line)

    /*Taking one line at a time.*/
    while (fgets(line, LINE_SIZE, fd) && ++lineCnt){
    	*strchr(line, '\n') = STR_END;
        if (isComment(line))
            continue;

        /*Storing all the strings in line into a list.*/
        fillLineList();
        /*Running tests over the line.*/
        lineAnalysis( &DC );
        /*If no error, doing the pre-coding.*/
        if(generalError == FALSE)
            codeTheLine( &IC, &DC );
        /*Freeing the strings in the array.*/
        freeLineList();
    }
    /*Updating the address's of the appropriate symbols.*/
    updateSymbolTable( &IC );
    /*Freeing the pointer for the line.*/
    free( line );
}

/*
 * Codes the current line according to the type of the line.
 */
void codeTheLine(int* IC,int* DC) {

    /*Zeroing the length of the memory words.*/
    memWords = 0;

    /*Finding with what line are we dealing.*/
    sentenceWithLabel = lineWithLabelDefinition();
    sentenceWithCommand = lineWithCommandDefinition();

    /*Deciding how to act according to the type of the line.*/
    if (lineWithDataDefinition())/*.data line - array or an int.*/
        codeDataLine(DC);
    else if (lineWithStringDefinition())/*Its a line with a string.*/
        codeStringLine(DC);
    else if (lineWithExternLabel()) /*Its a line with extern label*/
        externSymLine();
    else if (sentenceWithCommand)/*Command line- calculating the number of words it'll take*/
        calCmdLine(IC);    
}

/*
 * Codes the numbers and puts its memory words into the data list.
 */
void codeDataLine(int* DC){
    char lineCpy[LINE_SIZE], temp[STRING_SIZE], *dataVar;
    linePtr ptr = lineListHead;
    size_t i = 1,firstInt;
    strcpy(lineCpy,line);

    /*Checking if theres a label before the data.*/
    if(strchr(lineCpy,ISLABEL))
        firstInt = 3;
    else
        firstInt = 2;
    if(strchr(lineCpy,COMMA)) {/*Its an array.*/

        /*starting from the 3 third string to avoid non numbers words.*/
        for(ptr = lineListHead;ptr && i != firstInt; ptr = ptr->next,i++);

        if(! (atoi(ptr->str)))
            printError(order_error,"");

        for (; ptr; ptr = ptr->next) {

            /*Taking the ints and putting into the data list.*/
            strcpy(temp,ptr->str);
            dataVar = strtok(temp,COMMA_STR);
            while(dataVar != NULL){
                if(strchr(dataVar , DOT) != NULL)
                    printError(float_err,dataVar);
                else
                    addToDataList(atoi(dataVar),DC);
                dataVar = strtok(NULL,COMMA_STR);

            }
        }
    }
    else{/*Its not an array,but one number, inserting it.*/

        /*Getting to the first number.*/
        for(ptr = lineListHead; ptr && (!atoi(ptr->str)); ptr = ptr->next)
            ;
        if(ptr == NULL)
            printError(data_line_with_no_data,"");
        else if(strchr(ptr->str , DOT))
            printError(float_err , ptr->str);
        else
            addToDataList(atoi(ptr->str) , DC);
    }
}

/*
 * Codes the ascii chars and puts it memory words into the data list.
 */
void codeStringLine(int* DC){
    char lineCpy[LINE_SIZE];
    size_t i = 1;
    strcpy(lineCpy,line);

    /*Taking each char in the str and putting its ascii code to dataList.*/
    /*taking only the string part from the line*/
    strcpy(lineCpy,strchr(lineCpy,STRING));

    /*marking the end of the string.*/
    *strrchr(lineCpy,STRING) = STR_END;

    /*Inserting each char into the data array as ascii code*/
    while(lineCpy[i])
        addToDataList( (int)lineCpy[i++] , DC);
    addToDataList(STR_END,DC);
}

/*
 * Adding extern symbols to the symbol table.
 */
void externSymLine(){
    linePtr ptr;

    /*Adding the extern symbol to the sym table.*/
    for (ptr = lineListHead; ptr; ptr = ptr->next)
        if (strstr(ptr->str, GUIDING_SENTENCE_EXTERN)) {
            addToSymTable((ptr->next)->str, NULL, FALSE);
            if (ptr != lineListHead)
                printWarning(bef_ext, lineListHead->str, 0);
            break;
        }
}

/*
 * Calculating the number of memory words the line will take.
 */
void calCmdLine(int* IC){
    boolean symBeforeParams = FALSE;

    /*Getting the number of parameters & adding to parameters list.*/
    analyseParameters();

    /*Searching if there's a label before the parameters*/
    symBeforeParams = labelBeforeParam();

    /*Incrementing to consider the label.*/
    memWords += symBeforeParams == TRUE ? 1 : 0;

    /*Updating the address of the label to this line address*/
    if (sentenceWithLabel )
        updateLabelAddress(lineListHead->str, IC);

    /*incrementing memWords to consider the command itself*/
    ++memWords;

    /*Updating IC*/
    (*IC) += memWords;
}

/*
 * Counting the number of parameters in the line & inserting them into a list
 */
void analyseParameters() {
    char lineCpy[LINE_SIZE], cmd[CMD_LENGTH], lineCpy2[LINE_SIZE];
    char firstParam[STRING_SIZE], secondParam[STRING_SIZE], tmp[LINE_SIZE];
    size_t i = 0, j = 0;
    linePtr ptr = lineListHead;

    strcpy(lineCpy, line);
    getCommand(cmd);

    if (strcmp(cmd, "stop") == EQUAL || strcmp(cmd, "rts") == EQUAL)/*No params.*/
        return;

    for (ptr = lineListHead; ptr; ptr = ptr->next) {
        if (strstr(ptr->str, cmd)) {
            ptr = ptr->next;
            break;
        }
    }

    if (ptr == NULL){
        printError(missing_first_param, "");
        return;
    }

    if( strchr(lineCpy , ISLABEL) != NULL) {/*Moving the lineCpy after the symbol.*/
    	strcpy(lineCpy2, line);
    	strcpy(lineCpy , (strchr(lineCpy2 , ISLABEL)));
    }

	/*tmp pointing to the line since the first string after the command.*/
    strcpy(tmp, strstr(lineCpy, ptr->str));	
    strcpy(lineCpy , tmp);
    removeSpaces(lineCpy);

    if(strchr(lineCpy , COMMA)){ /*Two parameters.*/
        memWords += 2;

        if(strchr(lineCpy , O_BRACKETS) != NULL ) {/* Line has brackets.*/
            strcpy(lineCpy , strchr(lineCpy,O_BRACKETS)+1);
            while( (firstParam[i] = lineCpy[i]) != COMMA)/*Getting first param.*/
                i++;  
            firstParam[i++] = STR_END;
            while( (secondParam[j++] = lineCpy[i]) != C_BRACKETS)/*Getting second param.*/
                i++;
            secondParam[--j] = STR_END;
            addToParamsList(firstParam  , secondParam);

        }
        else{/* Just parameters.*/

            strcpy(firstParam , strtok(lineCpy, COMMA_STR));
            strcpy(secondParam , strtok(NULL,COMMA_STR));
            addToParamsList(firstParam,secondParam);

        }
        if(isRegister(firstParam) && isRegister(secondParam)) /*Two reg's takes one word.*/
            memWords--;

        //Error checking
        if (strchr(firstParam, DOT))
            printError(float_err, firstParam);
        if (strchr(secondParam, DOT))
            printError(float_err, secondParam);

    }else{

        strcpy(firstParam, lineCpy);
        addToParamsList(firstParam, NULL);
        memWords++;
        if(strchr(firstParam , DOT))
            printError(float_err,firstParam);

    }

}

/*
* Returns true if there a label before the parameters.
* Else, returns false.
*/
boolean labelBeforeParam(){
    char lineCpy[LINE_SIZE], cmd[CMD_LENGTH];

    strcpy(lineCpy,line);
    getCommand(cmd);

    /*Testing if the line has a label before the parameters*/
    if(strcmp(cmd,"jmp") != EQUAL && strcmp(cmd,"bne") != EQUAL && strcmp(cmd,"jsr") != EQUAL)
        return FALSE;
    if(strchr(lineCpy,O_BRACKETS) == NULL)
        return FALSE;
    return TRUE;

}

/*
* Returns true if there's a label definition withing the line.
* Else, returns false.
*/
boolean lineWithLabelDefinition(){
    char lineCpy[LINE_SIZE];
    strcpy(lineCpy,line);

    /*Searching for ':' in the line.*/
    if (strchr(lineCpy,ISLABEL))
        return TRUE;
    return FALSE;
}

/*
* Returns true if there's a data definition withing the line.
* Else, returns false.
*/
boolean lineWithDataDefinition(){
    char lineCpy[LINE_SIZE];
    strcpy(lineCpy,line);

    //*Searching for .data in the line.*/
    if(strstr((lineCpy),GUIDING_SENTENCE_INT))
        return TRUE;
    return FALSE;
}

/*
* Returns true if there's a string definition withing the line.
* Else, returns false.
*/
boolean lineWithStringDefinition(){
    char lineCpy[LINE_SIZE];
    strcpy(lineCpy,line);

    /*Searching for .string in the line.*/
    if(strstr((lineCpy),GUIDING_SENTENCE_STRING))
        return TRUE;
    return FALSE;
}

/*
* Returns true if there's a command definition withing the line.
* Else, returns false.
*/
boolean lineWithCommandDefinition(){
    char lineCpy[LINE_SIZE];
    int i = 0;
    strcpy(lineCpy,line);

    /*Searching for a sub string matching the our commands.*/
    for(i = 0; i < ACTIONS_NUM;i++) {
        if (strstr(lineCpy, (keys[i].symbol))) {
            return TRUE;
        }
    }
    return FALSE;
}
