#include "declarationsHeader.h"

/*
 * Adds the passed string to the line list.
 */
void addToLineList(char *newStr) {
    static linePtr last;
    linePtr newNode = (linePtr) malloc(sizeof(lineNode));
    MEMORY_ERROR(newNode)

    strcpy(newNode->str, newStr);
    newNode->next = NULL;

    if (lineListHead == NULL) {
        lineListHead = newNode;
        last = lineListHead;
    } else {
        last->next = newNode;
        last = last->next;
    }

    lineListLength++;

}

/*
 * Frees the line list.
 */
void freeLineList() {
    linePtr temp;

    while (lineListHead) {
        temp = lineListHead;
        lineListHead = lineListHead->next;
        free(temp);
    }

    lineListLength = 0;
}

