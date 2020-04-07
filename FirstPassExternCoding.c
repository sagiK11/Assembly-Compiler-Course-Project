#include "declarationsHeader.h"

/*
 * Adding extern symbols to the symbol table.
 */
void externSymLine() {
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