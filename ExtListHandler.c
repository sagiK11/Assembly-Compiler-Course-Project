#include "declarationsHeader.h"

extern extPtr extListHead;

/*
 * Add the passed symbol to the extern list.
 */
void addToExtList(char *sym) {

    static extPtr last;
    extPtr newExtSym = (extPtr) malloc(sizeof(extNode));
    MEMORY_ERROR(newExtSym)

    strcpy(newExtSym->symbol, sym);
    newExtSym->updated = FALSE;
    newExtSym->next = NULL;


    if (extListHead == NULL) {
        extListHead = newExtSym;
        last = extListHead;
    } else {
        last->next = newExtSym;
        last = last->next;
    }
}

/*
 * Updates the address's of the extern symbols.
 */
void updateExtList(int address) {
    extPtr exPtr;
    for (exPtr = extListHead; exPtr; exPtr = exPtr->next) {

        if (!exPtr->updated) {
            exPtr->address = address;
            exPtr->updated = TRUE;
            return;
        }

    }
}

/*
 * Frees the extern list.
 */
void freeExtList() {
    extPtr p;

    while (extListHead != NULL) {
        p = extListHead;
        extListHead = extListHead->next;
        free(p);
    }
}


