#include "data.h"
extern symPtr symTableHead;
extern  paramsPtr paramsListHead;
extern extPtr extListHead;
extern insPtr insListHead;
extern dataPtr dataListHead;
extern size_t insListLength;
extern size_t dataListLength;


/*
 * Constants keywords within the language.
 */
struct keyWords keys[]=
        {                 /*The booleans refers the legit methods the cmd's can be used.*/
                {"mov",0,  TRUE,TRUE,      TRUE,TRUE,FALSE,TRUE,      FALSE,TRUE,FALSE,TRUE},
                {"cmp",1,  TRUE,TRUE,      TRUE,TRUE,FALSE,TRUE,      TRUE,TRUE,FALSE,TRUE },
                {"add",2,  TRUE,TRUE,      TRUE,TRUE,FALSE,TRUE,      FALSE,TRUE,FALSE,TRUE},
                {"sub",3,  TRUE,TRUE,      TRUE,TRUE,FALSE,TRUE,      FALSE,TRUE,FALSE,TRUE},
                {"not",4,  FALSE,TRUE,     FALSE,FALSE,FALSE,FALSE,   FALSE,TRUE,FALSE,TRUE},
                {"clr",5,  FALSE,TRUE,     FALSE,FALSE,FALSE,FALSE,   FALSE,TRUE,FALSE,TRUE},
                {"lea",6,  TRUE,TRUE,      FALSE,TRUE,FALSE,FALSE,    FALSE,TRUE,FALSE,TRUE},
                {"inc",7,  FALSE,TRUE,     FALSE,FALSE,FALSE,FALSE,   FALSE,TRUE,FALSE,TRUE},
                {"dec",8,  FALSE,TRUE,     FALSE,FALSE,FALSE,FALSE,   FALSE,TRUE,FALSE,TRUE},
                {"jmp",9,  FALSE,TRUE,     FALSE,FALSE,FALSE,FALSE,   FALSE,TRUE,TRUE,TRUE},
                {"bne",10, FALSE,TRUE,     FALSE,FALSE,FALSE,FALSE,   FALSE,TRUE,TRUE,TRUE},
                {"red",11, FALSE,TRUE,     FALSE,FALSE,FALSE,FALSE,   FALSE,TRUE,FALSE,TRUE},
                {"prn",12, FALSE,TRUE,     FALSE,FALSE,FALSE,FALSE,   TRUE,TRUE,FALSE,TRUE},
                {"jsr",13, FALSE,TRUE,     FALSE,FALSE,FALSE,FALSE,   FALSE,TRUE,TRUE,TRUE},
                {"rts",14, FALSE,FALSE,    FALSE,FALSE,FALSE,FALSE,   FALSE,FALSE,FALSE,FALSE,},
                {"stop",15,FALSE,FALSE,    FALSE,FALSE,FALSE,FALSE,   FALSE,FALSE,FALSE,FALSE,},
                {".data",0},
                {".string",0},
                {".entry",0},
                {".extern",0},
                {"r0",0},
                {"r1",1},
                {"r2",2},
                {"r3",3},
                {"r4",4},
                {"r5",5},
                {"r6",6},
                {"r7",7}
        };

/*
 * Adding the passed string to the symbol table.
 */
void addToSymTable(char* newSym,int* CNT,boolean definedInFile){
    symPtr p = symTableHead, newNode;
    char lineCpy[LINE_SIZE];
    static symPtr last;
    int i;

    /*The new symbol is extern.*/
    while(p != NULL){
        if(strcmp(p->symName, newSym) == EQUAL) {
            if (p->data == -1 && p->definedInFile == FALSE){
                p->isExt = TRUE;
                return;
            }
        }
        p = p->next;
    }
    strcpy(lineCpy,line);
    newNode = (symPtr) malloc(sizeof(symbol));
    MEMORY_ERROR(newNode)

    strcpy(newNode->symName,newSym);
    /*This means its a label that hasn't been defined yet.*/
    newNode->data = CNT != NULL ? (*CNT) : -1;
    newNode->definedInFile = definedInFile;
    newNode->isExt = FALSE;
    newNode->isEnt = FALSE;
    newNode->next = NULL;

    /* testing if the symbol is extern.*/
    if(strstr(lineCpy,GUIDING_SENTENCE_EXTERN)) {
        symIsIllegal(newSym); /*Testing for legit.*/
        newNode->isExt = TRUE;
    }

    /*Testing if the symbol is labeled with instruction sentence.*/
    for(i = 0; i < ACTIONS_NUM;i++){
        if(strstr(lineCpy,keys[i].symbol)) {
            newNode->inInsSen = TRUE;
        }
    }
     /*List is empty.*/
    if(symTableHead == NULL) {
        symTableHead = newNode;
        last = symTableHead;

    }else{ /*List is not empty.*/
        last->next = newNode;
        last = last->next;
    }
}

/*
 * After the first pass we update each symbol table to its correct address.
 * */
void updateSymbolTable(int* IC){
    symPtr p = symTableHead;

    for(;p != NULL; p = p->next){
        if(p->isExt != TRUE && p->inInsSen != TRUE)
            p->data += (*IC);
    }
}

/*
 * Searching for errors withing the symbol table after the second pass.
 */
boolean errorsInSymTable(){
    symPtr ptr = symTableHead;
    const short ext = -1;


    for(; ptr; ptr = ptr->next){
        if(ptr->data == ext && ptr->isExt != TRUE) {
            printError(undef_symbol, ptr->symName);
            return TRUE;
        }
    }

    for(ptr = symTableHead; ptr; ptr = ptr->next){
        if(ptr->isEnt == TRUE && ptr->isExt == TRUE)
            printError(symbol_is_ent_and_ext,ptr->symName);
    }
    return FALSE;
}

/*
 * Debugging func, prints the symbol table to the screen.
 */
void printSymTable(){
    symPtr p = symTableHead;

    fprintf(stdout,"\n--------------------------------------------------------------------\n");
    fprintf(stdout,"Symbol\t| Address\t| isExternal\t| isInActionSen\t| isEnt\t\t|\n");
    while(p){
        printf("'%s'\t| %d\t\t| %s\t\t| %s\t\t| %s\t\t|\n",
               p->symName,p->data, p->isExt == 1 ? "yes":"no",p->inInsSen == 1 ? "yes":"no",
                        p->isEnt == TRUE ? "yes":"no");
        p = p->next;
    }
    fprintf(stdout,"----------------------------------------------------------------------\n");

}

/*
 * Updates lables that are being used during a command line.
 */
void updateLabelAddress(char* sym,int* CNT){
    symPtr p = symTableHead;

    /*Relates only to labels in a instruction sentence.*/
    /*After we know the right address we update it in the Symbol Table.*/
    while(p != NULL){
        if(strcmp(p->symName,sym) == EQUAL ) {
            p->data = *CNT;
        }
        p = p->next;
    }
}

/*
 * Adds the passed strings to the parameters list.
 */
void addToParamsList(char* firstParm,char* secondParm){
    static paramsPtr last;
    paramsPtr newNode = (paramsPtr) malloc(sizeof(paramsList));
    MEMORY_ERROR(newNode)

    newNode->lineNum = lineCnt;
    newNode->next = NULL;

     /*First parameter.*/
    if(firstParm != NULL)
        strcpy(newNode->firstParam,firstParm);
    else
        strcpy(newNode->firstParam,"");

     /*Second parameter.*/
    if(secondParm != NULL)
        strcpy(newNode->secondParam,secondParm);
    else
        strcpy(newNode->secondParam,"");


    if(paramsListHead == NULL){
        paramsListHead = newNode;
        last = paramsListHead;
    }else{
        last->next = newNode;
        last = newNode;
    }
}

/*
 * Adds the pass num to the data list and updating DC.
 */
void addToDataList(int num,int* DC){
    static dataPtr last;
    dataPtr newNode = (dataPtr) malloc(sizeof(dataNode));
    MEMORY_ERROR(newNode)

    strcpy(newNode->data, decToWeirdBinary(num));
    newNode->next = NULL;

    if(dataListHead == NULL){
        dataListHead = newNode;
        last = dataListHead;
    }else{
        last->next = newNode;
        last = last->next;
    }

    /*Updating the length and DC.*/
    dataListLength++;
    (*DC)++;
}

/*
 * Adds the passed memory word to the instructions list.
 */
void addToInsList(Word* newWord,char* strWord,char* str){
    static insPtr last;
    insPtr  newNode = (insPtr) malloc(sizeof(insNode));
    MEMORY_ERROR(newNode)

    /*Updating the length of the list.*/
    insListLength++;

    newNode->data = (*newWord);
    strcpy(newNode->str,str);
    strcpy(newNode->strWord,strWord);
    newNode->next = NULL;


    if(insListHead == NULL){
        insListHead = newNode;
        last = insListHead;
    }else{
        last->next = newNode;
        last = last->next;
    }
}

/*
 * Add the passed symbol to the extern list.
 */
void addToExtList(char* sym){

    static extPtr last;
    extPtr newExtSym = (extPtr) malloc(sizeof(extNode));
    MEMORY_ERROR(newExtSym)

    strcpy(newExtSym->symbol,sym);
    newExtSym->updated = FALSE;
    newExtSym->next = NULL;


    if(extListHead == NULL){
        extListHead = newExtSym;
        last = extListHead;
    }
    else{
        last->next = newExtSym;
        last = last->next;
    }
}

/*
 * Adds the passed string to the line list.
 */
void addToLineList(char* newStr){
    static linePtr last;
    linePtr newNode = (linePtr) malloc(sizeof(lineNode));
    MEMORY_ERROR(newNode)

    strcpy(newNode->str , newStr);
    newNode->next = NULL;

    if(lineListHead == NULL){
        lineListHead = newNode;
        last = lineListHead;
    }else{
        last->next = newNode;
        last = last->next;
    }

    lineListLength++;

}

/*
 * Updates the address's of the extern symbols.
 */
void updateExtList(int address){
    extPtr exPtr;
    for(exPtr = extListHead ; exPtr ; exPtr = exPtr->next){

        if (exPtr->updated == FALSE) {
            exPtr->address = address;
            exPtr->updated = TRUE;
            return;
        }

    }
}
/*
 * Frees the symbol table.
 */

void freeSymTable(){
    symPtr p;

    while(symTableHead != NULL){
        p = symTableHead;
        symTableHead = symTableHead->next;
        free(p);
    }

    symTableHead = NULL;
}

/*
 * Frees the extern list.
 */
void freeExtList(){
    extPtr p;

    while(extListHead != NULL){
        p = extListHead;
        extListHead = extListHead->next;
        free(p);
    }
}

/*
 *Debugging function for printing to the screen the parameters.
 */
void printParamsList(){
    paramsPtr p = paramsListHead;

    while(p != NULL){
        puts("**************************");
        printf("line = %d\nFirst Parameter: '%s'\nSecond Parameter: '%s'\n",
                                         p->lineNum,p->firstParam,p->secondParam);
        p =p->next;
        puts("**************************");
    }
}

/*
 * Frees the parameters list.
 */
void freeParamsList(){
    paramsPtr p;

    while(paramsListHead != NULL){
        p = paramsListHead;
        paramsListHead = paramsListHead->next;
        free(p);
    }
}

/*
 * Frees the data list.
 */
void freeDataList(){
    dataPtr ptr;

    while(dataListHead != NULL){
        ptr = dataListHead;
        dataListHead = dataListHead->next;
        free(ptr);
    }
    dataListLength = 0;
}

/*
 * Frees the instructions list.
 */
void freeInsList(){
    insPtr ptr;
    Word p;

    while(insListHead != NULL){
        ptr = insListHead;

         /*Freeing the word.*/
        p = ptr->data;
        free(p);
        insListHead = insListHead->next;

         /*Freeing the node itself.*/
        free(ptr);
    }
    insListLength = 0;
}

/*
 * Frees the line list.
 */
void freeLineList(){
    linePtr temp;

    while(lineListHead){
        temp = lineListHead;
        lineListHead = lineListHead->next;
        free(temp);
    }

    lineListLength = 0;
}

