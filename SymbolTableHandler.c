#include "declarationsHeader.h"

extern symPtr symTableHead;
extern struct keyWords keys[];

/*
 * Adding the passed string to the symbol table.
 */
void addToSymTable(char *newSym, int *CNT, boolean definedInFile) {
    symPtr p = symTableHead, newNode;
    char lineCpy[LINE_SIZE];
    static symPtr last;
    int i;

    /*The new symbol is extern.*/
    while (p != NULL) {
        if (strcmp(p->symName, newSym) == EQUAL) {
            if (p->data == -1 && p->definedInFile == FALSE) {
                p->isExt = TRUE;
                return;
            }
        }
        p = p->next;
    }
    strcpy(lineCpy, line);
    newNode = (symPtr) malloc(sizeof(symbol));
    MEMORY_ERROR(newNode)

    strcpy(newNode->symName, newSym);
    /*This means its a label that hasn't been defined yet.*/
    newNode->data = CNT != NULL ? (*CNT) : -1;
    newNode->definedInFile = definedInFile;
    newNode->isExt = FALSE;
    newNode->isEnt = FALSE;
    newNode->next = NULL;

    /* testing if the symbol is extern.*/
    if (strstr(lineCpy, GUIDING_SENTENCE_EXTERN)) {
        symIsIllegal(newSym); /*Testing for legit.*/
        newNode->isExt = TRUE;
    }

    /*Testing if the symbol is labeled with instruction sentence.*/
    for (i = 0; i < ACTIONS_NUM; i++) {
        if (strstr(lineCpy, keys[i].symbol)) {
            newNode->inInsSen = TRUE;
        }
    }
    /*List is empty.*/
    if (symTableHead == NULL) {
        symTableHead = newNode;
        last = symTableHead;

    } else { /*List is not empty.*/
        last->next = newNode;
        last = last->next;
    }
}

/*
 * After the first pass we update each symbol table to its correct address.
 * */
void updateSymbolTable(int *IC) {
    symPtr p = symTableHead;

    for (; p != NULL; p = p->next) {
        if (p->isExt != TRUE && p->inInsSen != TRUE)
            p->data += (*IC);
    }
}

/*
 * Searching for errors withing the symbol table after the second pass.
 */
boolean errorsInSymTable() {
    symPtr ptr = symTableHead;
    const short ext = -1;


    for (; ptr; ptr = ptr->next) {
        if (ptr->data == ext && ptr->isExt != TRUE) {
            printError(undef_symbol, ptr->symName);
            return TRUE;
        }
    }

    for (ptr = symTableHead; ptr; ptr = ptr->next) {
        if (ptr->isEnt == TRUE && ptr->isExt == TRUE)
            printError(symbol_is_ent_and_ext, ptr->symName);
    }
    return FALSE;
}

/*
 * Updates lables that are being used during a command line.
 */
void updateLabelAddress(char *sym, int *CNT) {
    symPtr p = symTableHead;

    /*Relates only to labels in a instruction sentence.*/
    /*After we know the right address we update it in the Symbol Table.*/
    while (p != NULL) {
        if (strcmp(p->symName, sym) == EQUAL) {
            p->data = *CNT;
        }
        p = p->next;
    }
}

/*
 * Debugging func, prints the symbol table to the screen.
 */
void printSymTable() {
    symPtr p = symTableHead;

    fprintf(stdout, "\n--------------------------------------------------------------------\n");
    fprintf(stdout, "Symbol\t| Address\t| isExternal\t| isInActionSen\t| isEnt\t\t|\n");
    while (p) {
        printf("'%s'\t| %d\t\t| %s\t\t| %s\t\t| %s\t\t|\n",
               p->symName, p->data, p->isExt == 1 ? "yes" : "no", p->inInsSen == 1 ? "yes" : "no",
               p->isEnt == TRUE ? "yes" : "no");
        p = p->next;
    }
    fprintf(stdout, "----------------------------------------------------------------------\n");

}

/*
 * Frees the symbol table.
 */
void freeSymTable() {
    symPtr p;

    while (symTableHead != NULL) {
        p = symTableHead;
        symTableHead = symTableHead->next;
        free(p);
    }

    symTableHead = NULL;
}
