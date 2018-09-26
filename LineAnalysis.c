#include "data.h"

extern symPtr symTableHead;
extern struct keyWords keys[];


/*
 * Testing the line for any kind of errors and adding symbols to the symbol table.
 */
void lineAnalysis(int* DC){
    /*Finding errors withing the lines.*/
    findSyntaxErrors();
    /*Finding errors withing the symbols & adding it into the symbol table.*/
    findSymbols(DC);
    /*Finding wrong use of methods 0,1,2,3.*/
    wrongMethodUse();
    /*Finds redundant strings.*/
    redundantInfo();


}

/*
 * Finds syntax errors and reports to the user if it finds any.
 */
void findSyntaxErrors(){
    /*Searching for brackets errors.*/
    bracketsAndStrError(O_BRACKETS);
    /*Searching for \" errors.*/
    bracketsAndStrError(STRING);
    /*Searching for comma errors.*/
    commaError();
    /*Searching for 'wondering' chars.*/
    searchUndefChars();
    /*Searching for errors related to labels.*/
    labelError();
    /*Searching for errors related to parameters.*/
    paramsError();
    /*Testing if there's no definer in the line.*/
    definerError();
    /*Searching for errors related to ent or extern sybols.*/
    extOrEntError();
    /*Searching for errors related stop and rts operators.*/
    stopRtsError();

}

/*
 * Generic function for testing miss match between opening and closing chars, like () and ""
 */
void bracketsAndStrError(int type){
    int j = 0, len, cnt = 0, closer;
    char lineCpy[LINE_SIZE], cmd[CMD_LENGTH];
    getCommand(cmd);
    len = strlen(line);
    strcpy(lineCpy,line);
    /*No command and its a .string line.*/
    if(cmd[0] == STR_END && strstr(lineCpy,GUIDING_SENTENCE_STRING) == NULL)
        return;
    switch (type){
        case O_BRACKETS : closer  = C_BRACKETS;
            break;
        case STRING : closer = STRING;
            break;
    }
    /*Testing for a miss match between brackets or  \" char, depends on the parameter 'type'.*/
    for(j = 0; j < len; j++){
        if(line[j] == type)
            ++cnt;
        else {
            if (line[j] == closer) {
                if (cnt > 0) {
                    --cnt;
                } else {
                    if (type == STRING)
                        printError(string_err, "");
                    else
                        printError(brack_err, "");
                    return;
                }
            }
        }
    }
    if( closer == STRING && (cnt % 2) == 0)
        return;
    if(cnt != 0)
        type == STRING ? printError(string_err,"") : printError(brack_err,"");
}

/*
 * Finding a symbol label withing the line, inserting it into the symbols table
 * and checking if it is already has been defined.
 */
void findSymbols(int* DC){
    int i = 0;
    linePtr ptr;
    char* temp, *temp2;

    for ( ptr = lineListHead; ptr; ptr = ptr->next, i++){
        /*Looking for the ':' char within the string.*/
        if( (strchr(ptr->str,ISLABEL)) != NULL ) {
            temp = ptr->str;
            temp2 = (strchr(temp,ISLABEL)+1);
            if(strlen(temp2) != 0){
                printError(illegal_sym,temp);
                return;
            }
            *(strchr(temp,ISLABEL)) = STR_END;

            if (symIsDefined(temp)) {
                return;
            }else if (symIsIllegal(temp)){
                return;
            }else if(symIsAKey(temp)){
                return;
            }
            else{
                addToSymTable(temp, DC, TRUE);
            }
        }
    }
}

/*
 * Testing if the Symbol that was passed as argument is already defined in the symbol Table
 */
boolean symIsDefined(char* sym){
    symPtr ptr;
    ptr = symTableHead;

    for(; ptr != NULL; ptr = ptr->next){
        if( strcmp(ptr->symName,sym) == EQUAL && ptr->definedInFile == TRUE){
            printError(sym_defined_already,sym);
        }
        if(strcmp( (ptr->symName),sym) == EQUAL)
            return TRUE;
    }
    return FALSE;
}

/*
 * Testing if the Symbol that was passed as argument is defined as a saved keyword.
 */
boolean symIsAKey(char* sym){
    int i = 0;

    for(;i < NUM_OF_KEYS; ++i){
        if( strcmp(sym, (keys[i].symbol)) == 0){
            printError(sym_is_key,sym);
            return TRUE;
        }
    }
    return FALSE;
}

/*
 * Tests if a symbol is not defined according to the syntax rules.
 */
boolean symIsIllegal(char* sym){
    char p[STRING_SIZE];
    int i = 1;
    const size_t symMaxLen = 30;
    strcpy(p,sym);

    if( (strlen(sym) > symMaxLen) || (!(isalpha(p[0]))) ) {
        printError(illegal_sym,sym);
        return TRUE;
    }

    while( p[i] ){
        if( (!isalnum(p[i])) ){
            printError(illegal_sym,sym);
            return TRUE;
        }
        ++i;
    }
    return FALSE;
}

/*
 * Tests for wrong uses of the 4 methods defined in the instructions.
 */
void wrongMethodUse(){
    char lineCpy[LINE_SIZE], *temp, cmd[CMD_LENGTH];
    strcpy(lineCpy,line);
    getCommand(cmd);

    /*No command in the line.*/
    if(cmd[0] == STR_END)
        return;

    /*Checking first parameter*/
    if( (temp = strstr(lineCpy,"#")) != NULL){
        if(isdigit(temp[1]) == FALSE && temp[1] != '-'){
            if(strchr(temp,COMMA))
                *strchr(temp,COMMA) = STR_END;
            printError(immediate_wrong_use,temp);
        }

    /*Checking second parameter (if exists)*/
    if ((temp = strstr(++temp, "#")) != NULL) {
         if (isdigit(temp[1]) == FALSE && temp[1] != '-' && temp[1] != '+') {
            if (strchr(temp, C_BRACKETS))
                *strchr(temp, C_BRACKETS) = STR_END;
            printError(immediate_wrong_use, temp);
            }
        }
    }

}

/*
 * Tests for any comma error within the file.
 */
void commaError(){
    int i = 0, cntCommas = 0, cntDigist = 0;
    char lineCpy[LINE_SIZE], cmd[CMD_LENGTH];
    strcpy(lineCpy,line);

    getCommand(cmd);

    /*if the first char in cmd is null, its means we have no command in the line.*/
    /*Testing for a missing comma between two parameteres.*/
    if( cmd[0] != STR_END && isBinaryAction(cmd) && strchr(lineCpy , COMMA) == NULL){
        printError(missing_comma,"");
        return;
    }

    /*Its a cmd but its not binary action and we have a comma without ()*/
    if( cmd[0] != STR_END && !isBinaryAction(cmd) && strchr(lineCpy , COMMA) != NULL){
        if(strchr(lineCpy,O_BRACKETS) == NULL) {
            printError(extra_commas, "");
            return;
        }
    }

    /*Counting commas for later use.*/
    while(lineCpy[i++]){
        if(lineCpy[i] == COMMA)
            ++cntCommas;
    }

    /*If its not a .data sentence, there's no shouldn't be more than 1 comma*/
    if(cntCommas > 1 && strstr(lineCpy , GUIDING_SENTENCE_INT) == NULL){
        printError(extra_commas,"");
        return;
    }

    /*One comma with no brackets is ok.*/
    if(strchr(lineCpy,O_BRACKETS) != NULL && cntCommas  == 1)
        return;;

    /*Counting numbers for later use.*/
    i = 0;
    while(lineCpy[i++]){
        if(isdigit(lineCpy[i])) {
            cntDigist++;
            if(i > 0 && (isdigit(lineCpy[i-1]) || lineCpy[i-1] == DOT))/*Number counted twice..*/
                cntDigist--;
        }
    }

    if(strstr(lineCpy,GUIDING_SENTENCE_INT) != NULL && cntCommas > 0 && cntCommas+1 < cntDigist){
        printError(missing_comma,"");
        return;;
    }
    /*The right relation between commas and ints is: digit num = commas+1*/
    if( strstr(lineCpy,GUIDING_SENTENCE_INT) != NULL && cntCommas > 0 && cntCommas+1 != cntDigist)
        printError(extra_commas,"");

}

/*
 * Searching for 'wondering' chars within the code file.
 */
void searchUndefChars(){
    char lineCpy[LINE_SIZE] ,temp[2];
    size_t i = 0;
    boolean IN_STRING = FALSE;
    strcpy(lineCpy,line);

    /*Searching for illegal char wondering in the code.*/
    for( i = 0; lineCpy[i]; i++){
        if( isspace(lineCpy[i]))
            continue;

        /*Skipping chars within the string.*/
        if( lineCpy[i] == '\"' && IN_STRING == FALSE){
            IN_STRING = TRUE;
        }else if(lineCpy[i] == '\"' && IN_STRING == TRUE){
            IN_STRING = FALSE;
        }

        if(IN_STRING)
            continue;

        /*Searching for chars not matching to those below.*/
        if(  (!isalnum(lineCpy[i])) && lineCpy[i] != COMMA && lineCpy[i] != O_BRACKETS
                                    && lineCpy[i] != C_BRACKETS && lineCpy[i] != '.'
                                    && lineCpy[i] != '\"' && lineCpy[i] != '#'
                                    && lineCpy[i] != ISLABEL && lineCpy[i] != '-'
                                    && lineCpy[i] != '+' && lineCpy[i] != ISCOMMENT){
            strcpy(temp, (lineCpy + i));
            removeSpaces( temp );
            printError(undef_char, temp);

        }
    }
}

/*
 * Tests for any unnecessary information within the line.
 */
void redundantInfo(){
    char lineCpy[LINE_SIZE], cmd[CMD_LENGTH];
    int i;
    linePtr ptr, prev;

    strcpy(lineCpy,line);
    getCommand(cmd);

    if(strstr(lineCpy,GUIDING_SENTENCE_ENT)  || strstr(lineCpy,GUIDING_SENTENCE_EXTERN)){
        if( lineListLength > 2){
            for(ptr = lineListHead,i = 0; ptr; ptr = ptr->next, i++) {
                if (strstr(ptr->str, GUIDING_SENTENCE_EXTERN) ||
                    strstr(ptr->str, GUIDING_SENTENCE_ENT)) {
                    if (i == 0) /*After*/
                        printError( redu_info, ((ptr->next)->next)->str);
                    else /*Before*/
                        printError( redu_info, prev->str);
                }
                prev = ptr;
            }
        }
    }

    for(ptr = lineListHead,i = 0; cmd[0] != STR_END && ptr; ptr = ptr->next,i++){
        if( strchr(ptr->str,C_BRACKETS) && i+1 != lineListLength)
            printError(redu_info,(ptr->next)->str);
    }
}

/*
 * Tests for label error.
 */
void labelError(){
    char lineCpy[LINE_SIZE],cmd[CMD_LENGTH];
    linePtr ptr;
    size_t i = 0, j = 0;

    strcpy(lineCpy,line);
    getCommand(cmd);

    if(lineListHead == NULL)
        return;

    if(strchr(lineCpy,ISLABEL)){
        strcpy(lineCpy , lineListHead->str);

        /*Testing for strings before the label definition.*/
        if(strchr(lineCpy , ISLABEL) == NULL){
            printError(sym_miss_place,"");
        }

        if(cmd[0] == STR_END)
            return;

         /*Testing for incorrect order: label->cmd->operands.*/
        for(i = 0, ptr = lineListHead; i != 1 && ptr; ptr = ptr->next, i++)
            ;

        for(j = 0; ptr && j < LINE_DEFINERS_NUM ; j++){
            if(strstr(ptr->str, keys[j].symbol) != NULL)
                return;
        }
        printError(order_error,"");
    }
}

/*
 * Tests for parameters error.
 */
void paramsError(){
    char lineCpy[LINE_SIZE], cmd[CMD_LENGTH], *firstParam;
    size_t i,  lastChar, offset = 2;
    linePtr ptr;

    strcpy(lineCpy,line);
    getCommand(cmd);

    if(cmd[0] == STR_END) /*No params*/
    	return;

    if(strchr(lineCpy,COMMA)) {

        /*Missing both params.*/
        for (ptr = lineListHead; ptr->next; ptr = ptr->next)
            ;
        for (i = 0; i < ACTIONS_NUM - 2; i++) {
            if (strstr(ptr->str, keys[i].symbol))
                printError(missing_both_params, "");
        }

        /*Missing second param.*/
        removeSpaces(lineCpy);
        lastChar = strlen(lineCpy) - 1;
        if (lineCpy[lastChar] == COMMA)
            printError(missing_second_param, "");

        /*Missing first param.*/
        firstParam = strtok(lineCpy, COMMA_STR);
        removeSpaces(firstParam);
        if(strchr(firstParam, ISLABEL))/*Getting rid of the symbol.*/
            firstParam = strchr(firstParam,ISLABEL) + 1;

        for (i = 0; i < ACTIONS_NUM - offset; i++) {
            if (strcmp(firstParam, keys[i].symbol) == EQUAL)
                printError(missing_first_param, "");
        }
    }
}

/*
 * Tests for missing difiner - .data/.string/command within the line.
 */
void definerError(){
    char lineCpy[LINE_SIZE];
    size_t i;
    strcpy(lineCpy, line);

    for(i = 0; i < LINE_DEFINERS_NUM; i++){
        if(strstr(lineCpy,keys[i].symbol))
            return;
    }

    printError(no_definer_in_line,"");
}

void extOrEntError(){
	char lineCpy[LINE_SIZE];
	strcpy(lineCpy , line);

	if(strstr(lineCpy, GUIDING_SENTENCE_EXTERN) == NULL &&
								 strstr(lineCpy,GUIDING_SENTENCE_ENT) == NULL){
		return;
	}

	if(lineListLength < 2)
		printError(missing_sym, "");
}

void stopRtsError(){
	char lineCpy[LINE_SIZE];
	linePtr ptr;
	strcpy(lineCpy , line);

	if(strstr(lineCpy, "stop") == NULL && strstr(lineCpy, "rts") == NULL)
		return;

	for( ptr = lineListHead; ptr; ptr = ptr->next){
		if ( (strcmp(ptr->str , "stop") == EQUAL) || (strcmp( ptr->str , "rts") == EQUAL) ){
			if(ptr->next == NULL)
				return;
			printError(illegal_operand, ptr->str);		
		
		}
	}	



}


