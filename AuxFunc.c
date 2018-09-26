
#include "data.h"

static int address = STARTING_ADDRESS;
extern struct keyWords keys[];
extern symPtr symTableHead;
extern dataPtr dataListHead;
extern extPtr extListHead;
extern insPtr insListHead;
extern char* fileName;
extern size_t dataListLength;
extern size_t insListLength;


/*
 * The func takes as input a string and returns it without any white chars.
 */
void removeSpaces(char* sym) {
    int cnt = 0, i = 0;
    for(;sym[i]; i++){
        if( ! isspace(sym[i]) ){
            sym[cnt++] = sym[i];
        }
    }
    sym[cnt] = STR_END;
}

/*
 * Prints to the stderr the error and its line number.
 **/
void printError(int error,char* str){

    /*Changing the error flag.*/
    generalError = TRUE;

    /*Printing section.*/
    fprintf(stderr, "Error: ");
    switch (error)
    {
        case illegal_sym :
            fprintf(stderr,"Illegal symbol: '%s' in line %d.\n",str,lineCnt);
            break;
        case sym_defined_already:
            fprintf(stderr,"Symbol: '%s' in line %d is defined already.\n",str,lineCnt);
            break;
        case sym_is_key :
            fprintf(stderr,"Symbol: '%s' in line %d is defined as a saved key.\n",str,lineCnt);
            break;
        case brack_err :
            fprintf(stderr,"Brackets miss match in line %d\n",lineCnt);
            break;
        case string_err :
            fprintf(stderr,"String ' \"\" ' missmatch in line %d\n",lineCnt);
            break;
        case undef_parameter :
            fprintf(stderr,"Undefined parameter: '%s' in line %d\n",str,lineCnt);
            break;
        case writing_to_file :
            fprintf(stderr,"Error writing to file: %s\n",str);
            break;
        case undef_symbol :
            fprintf(stderr,"Undefined symbol: '%s' in line %d\n",str,lineCnt);
            break;
        case float_err :
            fprintf(stderr,"Assembler does not support floating point numbers - %s\n",str);
            break;
        case immediate_wrong_use :
            fprintf(stderr,"Wrong use of Immediate method - '%s' in line %d\n",str,lineCnt);
            break;
        case not_enough_params :
            fprintf(stderr,"Too few parameters.\n.");
            break;
        case file_op_err :
            fprintf(stderr,"Error opening file.\n.");
            break;
        case err_app_file :
            fprintf(stderr,"Error appending the file: '%s'\n.",str);
            break;
        case src_meth0_illegal:
            fprintf(stderr,"Illegal use of src method 0 in cmd:'%s' in line: %d\n",str,lineCnt);
            break;
        case src_meth1_illegal:
            fprintf(stderr,"Illegal use of src method 1 in cmd:'%s' in line: %d\n",str,lineCnt);
            break;
        case src_meth2_illegal:
            fprintf(stderr,"Illegal use of src method 2 in cmd:'%s' in line: %d\n",str,lineCnt);
            break;
        case src_meth3_illegal:
            fprintf(stderr,"Illegal use of src method 3 in cmd:'%s' in line: %d\n",str,lineCnt);
            break;
        case dst_meth0_illegal:
            fprintf(stderr,"Illegal use of dest method 0 in cmd:'%s' in line: %d\n",str,lineCnt);
            break;
        case dst_meth1_illegal:
            fprintf(stderr,"Illegal use of dest method 1 in cmd:'%s' in line: %d\n",str,lineCnt);
            break;
        case dst_meth2_illegal:
            fprintf(stderr,"Illegal use of dest method 2 in cmd:'%s' in line: %d\n",str,lineCnt);
            break;
        case dst_meth3_illegal:
            fprintf(stderr,"Illegal use of dest method 3 in cmd:'%s' in line: %d\n",str,lineCnt);
            break;
        case missing_comma:
            fprintf(stderr,"Missing a comma in line: %d\n",lineCnt);
            break;
        case comm_mis_match :
            fprintf(stderr,"Comma miss match in line: %d\n",lineCnt);
            break;
        case extra_commas :
            fprintf(stderr,"Too many commas in line: %d\n",lineCnt);
            break;
        case undef_char :
            fprintf(stderr,"Undefined char: '%s' in line: %d\n",str,lineCnt);
            break;
        case extra_ent:
            fprintf(stderr,"Extra text after entry label in line: %d\n",lineCnt);
            break;
        case redu_info:
            fprintf(stderr,"Redundant info: '%s' in line: %d\n",str,lineCnt);
            break;
        case illegal_sym_mth:
            fprintf(stderr,"Illegal label: '%s' before paramters in line: %d\n",str,lineCnt);
            break;
        case sym_miss_place :
            fprintf(stderr,"Symbol definition is not the first statment in line: %d\n",lineCnt);
            break;
        case order_error:
            fprintf(stderr,"The order of operators in line: %d is incorrect.\n",lineCnt);
            break;
        case missing_both_params:
            fprintf(stderr,"Missing both parameters in line: %d\n",lineCnt);
            break;
        case missing_first_param:
            fprintf(stderr,"Missing first parameter in line: %d\n",lineCnt);
            break;
        case missing_second_param:
            fprintf(stderr,"Missing second parameter in line: %d\n",lineCnt);
            break;
        case symbol_is_ent_and_ext:
            fprintf(stderr,"Symbol is defined both extern and entry: '%s'\n",str);
            break;
        case data_line_with_no_data:
            fprintf(stderr,"No data was found in line: %d\n",lineCnt);
            break;
        case no_definer_in_line:
            fprintf(stderr,"Missing a definer (.data/.string/cmd) in line: %d\n",lineCnt);
            break;
        case missing_sym:
        	fprintf(stderr,"Missing symbol in line: %d\n",lineCnt);
        	break; 
        case illegal_operand:
        	fprintf(stderr,"Illegal operand afer '%s' command in line: %d\n",str,lineCnt);
        	break; 	   
        default:  fprintf(stderr,"error in line %d",lineCnt);
    }
}

/*
 * Prints to the stderr the warning and its line number.
 */
void printWarning(int error, char* str,size_t lineCntCpy){
    fprintf(stderr,"Warning: ");
    switch(error){
        case uns_ex_sym:
            fprintf(stderr,"Unused ext symbol: '%s' in line: %d\n",str,lineCntCpy);
            break;
        case bef_ext:
            fprintf(stderr,"String:'%s' before extern symbol in line: %d\n",str,lineCnt);
            break;
        case invalid_input:
            fprintf(stderr,"No code was written, invalid input in file: '%s'\n",str);
            break;
        case chars_before_comment:
            fprintf(stderr,"Chars before a comment identifier in line: %d\n",lineCnt);
            break;
        default:fprintf(stderr,"General Warning.\n");
    }
}

/*
 * Returns the passed argument as a 14 long string in weird binary code.
 */
char* decToWeirdBinary(int num){
    int i;
    unsigned int mask = 1;
    char *res = (char*) malloc(sizeof(char) * (MEM_WORD_LEN));
    MEMORY_ERROR(res)
    res[MEM_WORD_LEN] = STR_END;/*Putting the null char.*/

    /*Takes the num and makes from it a 15 length long binary string.*/
    for(i = MEM_WORD_LEN - 1; i >= 0; --i, mask <<= 1) {
        if ((unsigned)num & mask)
            res[i] = SLASH;
        else
            res[i] = DOT;
    }
    return res;
}

/*
 * Tests for logic errors withing the command.
 */
void checkForErrors(char* cmd, Word machWord){
    short i;
    for(i = 0; i < ACTIONS_NUM; i++){
        if(strcmp(keys[i].symbol,cmd) == EQUAL)
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
 * Creates the object file.
 */
void writeObjectFile(){

    FILE* res_fd;
    char objectFile[] = ".ob" , temp[TMP_STR_LEN];
    char tempFileName[STRING_SIZE];
    insPtr ptr = insListHead;

    /*File naming and adding extension.*/
    strcpy( tempFileName , fileName);
    strcat( tempFileName , objectFile);
    if(insListLength == 0 && dataListLength == 0) /*Nothing to code.*/
        return;

    /*Appending the machine code file*/
    if( (!(res_fd = fopen(tempFileName , WRITE_ONLY)))){
        printError(err_app_file , "object file");
        exit(EXIT_FAILURE);
    }

    /*Adding sizes to file.*/
    addSizesToObjectFile(res_fd,insListLength,dataListLength);
    for(address = STARTING_ADDRESS; ptr ; ptr = ptr->next) {
        sprintf(temp, "%d", address);/*Converting the address to string*/
        fputs(temp, res_fd);
        fputs(TAB, res_fd);

        /*If the current word is used with external symbol, we update its address for later use.*/
       if ((ptr->data)->ERAforAdd == 1)
            updateExtList(address);
        address++;

        fputs(ptr->strWord, res_fd);
        fputs(NEWLINE, res_fd);
    }

    /*Adding the data code to the object file.*/
    addDataCodeToFile(res_fd);
    fclose(res_fd);
}

/*
 * Creates the ent file.
 */
void writeEntFile(){
    FILE* fd;
    char entAddress[TMP_STR_LEN] , tempFileName[STRING_SIZE] , entFile[] = ".ent";
    size_t entSym = 0;
    symPtr ptr;

    /*File naming and adding extension.*/
    strcpy( tempFileName , fileName);
    strcat( tempFileName , entFile);

    if (!(fd = fopen(tempFileName , WRITE_ONLY))) {
        printError(writing_to_file , "ent file");
        exit(EXIT_FAILURE);
    }

    /*Writing to file the symbols & address.*/
    for(ptr = symTableHead; ptr; ptr = ptr->next) {
        if(ptr->isEnt == TRUE && ++entSym) {
            sprintf(entAddress, "%d", ptr->data);/*Converting the address to string*/
            fputs(ptr->symName, fd);
            fputs(TAB, fd);
            fputs(entAddress, fd);
            fputs(NEWLINE, fd);
        }
    }

    /*If we didnt have entry symbols, we remove the file.*/
    if(entSym == 0){
        remove(tempFileName);
        return;
    }
    fclose(fd);
}

/*
 * Creates the ext file/
 */
void writeExtFile(){

    FILE* fd;
    char tempFileName[STRING_SIZE] , extFile[] = ".ext" , extAddress[TMP_STR_LEN];
    extPtr ptr = extListHead;

    /*File naming and adding extension.*/
    strcpy( tempFileName , fileName);
    strcat( tempFileName , extFile);
    if( extListHead == NULL)/*No extern symbols within the file.*/
        return;

    if (!(fd = fopen(tempFileName, WRITE_ONLY))) {
        printError(writing_to_file, "ext file");
        exit(EXIT_FAILURE);
    }

    /*Writing the symbols and its address to the File.*/
    for(; ptr; ptr = ptr->next){
        sprintf(extAddress , "%d" ,ptr->address);/*Converting the address to string.*/
        fputs(ptr->symbol , fd); /*Writing the symbol.*/
        fputs(TAB , fd);
        fputs(extAddress , fd);/*Writing its address.*/
        fputs(NEWLINE , fd);
    }
    fclose(fd);
}

/*
 * Adding the sizes of the data & instructions code length to the top of the object file.
 */
void addSizesToObjectFile(FILE* res_fd,int insSize,int dataSize){
    char temp[CMD_LENGTH];
    fputs(TAB, res_fd);

    /*Converting the address to string.*/
    sprintf(temp,"%d",insSize);
    /*Putting the size of the instructions.*/
    fputs(temp, res_fd);
    fputs(TAB, res_fd);
    /*Putting the size of the data.*/
    sprintf(temp,"%d",dataSize);
    fputs(temp, res_fd);
    fputs(NEWLINE, res_fd);
}

/*
 * Adds the data code after the instruction part code to the object file.
 */
void addDataCodeToFile(FILE* res_fd){
    const size_t theEndOfFile = 0;
    dataPtr ptr = dataListHead;
    char temp[TMP_STR_LEN];
    if(dataListLength == 0)/*Nothing to code.*/
        return;

    /*Getting to the end of the file*/
    fseek(res_fd,theEndOfFile , SEEK_END);

    /*Writing the code into the file*/
    for(; ptr ; ptr = ptr->next , address++) {
        sprintf(temp,"%d",address);/*Converting the address to string*/
        if( ptr != dataListHead)
        	fputs(NEWLINE,res_fd);

        fputs(temp,res_fd);
        fputs(TAB,res_fd);
        fputs(ptr->data , res_fd);
    }
}

/*
 * Returns the number of parameters within the line.
 */
size_t getNumberOfParmsInLine(){
    char lineCpy[LINE_SIZE];
    strcpy(lineCpy , line);
    /*Returns number of parameters in the line, returns only 1 or 2.*/
    if(strchr(lineCpy , COMMA) != NULL)
        return 2;
    return strchr(lineCpy , COMMA) == NULL ? 1 : 2;
}

/*
 * Returns the type of the operand.
 */
size_t findOperandType(){
    char lineCpy[LINE_SIZE], *temp;
    short i = 0, firstRegister = 20, cmdLength = 3;
    symPtr ptr;
    strcpy(lineCpy,line);
    temp = (char*) malloc(sizeof(char) * LINE_SIZE);

    /*These cmds doesnt have operands*/
    if(strstr(lineCpy,"stop") != NULL || strstr(lineCpy,"rts") != NULL){
        free(temp);
        return 0;
    }

    /*Getting to the line from the start of the cmd.*/
    for(i = 0; i < ACTIONS_NUM; i++){
        if((temp = strstr(lineCpy,keys[i].symbol)) != NULL)
            break;
    }

    /*Getting pass the cmd to the operands.*/
    temp += cmdLength;
    /*Getting rid of white chars.*/
    removeSpaces(temp);
    /*If label and parenthesis its jump*/
    if(temp[0] != O_BRACKETS && strchr(temp,O_BRACKETS) != NULL)
        return JUMP_METHOD;

    /*If no parameters and str == register its 3*/
    if(strchr(temp,O_BRACKETS) == NULL)
        for(i = firstRegister; i < NUM_OF_KEYS; i++)
            if(strcmp(temp , keys[i].symbol) == EQUAL)
                return DIRECT_REGISTER;

    /* Else it has to be a label*/
    for(ptr = symTableHead; ptr ;ptr = ptr->next)
        if(strcmp(temp , ptr->symName) == EQUAL)
            return DIRECT_METHOD;

    /*If we got here it means the parameter is neither of the above.*/
    printError(undef_parameter , temp);
    return 0;
}

/*
 * Returns true if the line is a comment line, else returns false.
 */
boolean isComment(char* line){
    char lineCopy[STRING_SIZE];
    size_t i = 0;
    strcpy(lineCopy,line);

    /*Searching for ';' char within the line.*/
    if( strchr(lineCopy , ISCOMMENT))
        return TRUE;
    for( i = 0; lineCopy[i]; i++){
    	if( isspace( lineCopy[i] ) == FALSE)
    		break;
    }

    /*Blank line.*/
    if( i == strlen(lineCopy) )
    	return TRUE;    
    return FALSE;
}

/*
 * Fills the lineList list with each string within the current line.
 */
void fillLineList(){
    int offset = 0;
    const int ok = 1;
    char str[LINE_SIZE];
    char* lineCopy = (char*) malloc(sizeof(char)*LINE_SIZE);
    MEMORY_ERROR(lineCopy)

    /*Zeroing the size and working on a copy of the line*/
    lineListLength = 0;
    strcpy(lineCopy,line);

    /*Extracting each string from the line*/
    while(sscanf(lineCopy, "%s%n", str, &offset) == ok){
        lineCopy += offset;
        addToLineList(str);
    }
}

/*
 * Returns True if the command is a binary command, else, returns false.
 */
boolean isBinaryAction(char* cmd){

    /*The following cmd's are binary commands.*/
    if(strcmp(cmd,"mov") == EQUAL || strcmp(cmd,"cmp") == EQUAL)
        return TRUE;
    if(strcmp(cmd,"add") == EQUAL || strcmp(cmd,"sub") == EQUAL)
        return TRUE;
    if(strcmp(cmd,"lea") == EQUAL)
        return TRUE;
    return FALSE;
}

/*
 * Takes a string as argument and fills it with the appropriate weird binary code.
 */
void memWordToString(Word word,size_t intNum,size_t reg1, size_t reg2,char* tempWord,char* str){
    unsigned int *machWord = (unsigned int*) word, mask;
    const size_t bothRegister = 1, maskForReg = 32, maskForAddAndCmd = 8192, twoReg =2, last = 14;
    /*32 = 0..0100000, 8192 = 0..010000000000000 in binary.*/
    size_t i,j = 0;

    if(generalError)
        return;
    if(intNum == bothRegister){

        for(i = 0; i < twoReg; i++) {
            for (mask = maskForReg; mask; mask >>= 1 , ++j) {
                if ((unsigned)reg1 & mask) {
                    tempWord[j] = SLASH;
                }
                else {
                    tempWord[j] = DOT;
                }
            }
             /*Moving to the second register.*/
            reg1 = reg2;
        }

        /*the ERA is absolute for register.*/
        tempWord[j] = DOT;
        tempWord[++j] = DOT;

    }else {

        /*Finds the binary form of the word and puts it into the buffer.*/
        for(mask = maskForAddAndCmd;mask; mask >>= 1, ++j) {
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


