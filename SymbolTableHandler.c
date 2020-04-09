#include "declarationsHeader.h"

extern symPtr symTableHead;
extern struct keyWords keys[];


/*
 * Adding the passed string to the symbol table.
 */
void addToSymTable(char *newSym, int *cnt, boolean definedInFile) {
    symPtr ptr = symTableHead, newNode;
    char lineCpy[LINE_SIZE];
    static symPtr last;

    if (symIsExtern(ptr, newSym))
        return;

    strcpy(lineCpy, line);
    newNode = (symPtr) malloc(sizeof(symbol));
    MEMORY_ERROR(newNode)

    strcpy(newNode->symName, newSym);
    symInitailzeSetting(newNode, cnt, definedInFile);;
    extSymbolSetting(lineCpy, newSym, newNode);
    insSymbolSetting(lineCpy, newNode);

    if (symTableHead == NULL) {
        symTableHead = newNode;
        last = symTableHead;
    } else {
        last->next = newNode;
        last = last->next;
    }
}

boolean symIsExtern(symPtr ptr, char *newSym) {
    while (ptr != NULL) {
        if (strcmp(ptr->symName, newSym) == EQUAL) {
            if (ptr->data == -1 && !ptr->definedInFile) {
                ptr->isExt = TRUE;
                return TRUE;
            }
        }
        ptr = ptr->next;
    }
    return FALSE;
}

void insSymbolSetting(char *lineCpy, symPtr newNode) {
    int i;
    for (i = 0; i < ACTIONS_NUM; i++) {
        if (strstr(lineCpy, keys[i].symbol)) {
            newNode->inInsSen = TRUE;
        }
    }
}

void symInitailzeSetting(symPtr newNode, int *cnt, boolean definedInFile) {
    newNode->data = cnt != NULL ? (*cnt) : -1;
    newNode->definedInFile = definedInFile;
    newNode->isExt = FALSE;
    newNode->isEnt = FALSE;
    newNode->next = NULL;
}

void extSymbolSetting(char *lineCpy, char *newSym, symPtr pSymbol) {
    if (strstr(lineCpy, GUIDING_SENTENCE_EXTERN)) {
        symIsIllegal(newSym);
        pSymbol->isExt = TRUE;
    }

}

/*
 * After the first pass we update each symbol table to its correct address.
 * */
void updateSymbolTable(int *IC) {
    symPtr ptr = symTableHead;

    for (; ptr != NULL; ptr = ptr->next) {
        if (ptr->isExt != TRUE && ptr->inInsSen != TRUE)
            ptr->data += (*IC);
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
            printErrorWithComment(undef_symbol, ptr->symName);
            return TRUE;
        }
    }

    for (ptr = symTableHead; ptr; ptr = ptr->next) {
        if (ptr->isEnt && ptr->isExt)
            printErrorWithComment(symbol_is_ent_and_ext, ptr->symName);
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
               p->isEnt ? "yes" : "no");
        p = p->next;
    }
    fprintf(stdout, "----------------------------------------------------------------------\n");

}

/*
 * Frees the symbol table.
 */
void freeSymTable() {
    symPtr ptr;

    while (symTableHead != NULL) {
        ptr = symTableHead;
        symTableHead = symTableHead->next;
        free(ptr);
    }
    symTableHead = NULL;
}
