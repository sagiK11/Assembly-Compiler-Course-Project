#include "declarationsHeader.h"

extern dataPtr dataListHead;
extern size_t dataListLength;

/*
 * Adds the pass num to the data list and updating DC.
 */
void addToDataList(int num, int *DC) {
    static dataPtr last;
    dataPtr newNode = (dataPtr) malloc(sizeof(dataNode));
    MEMORY_ERROR(newNode)

    strcpy(newNode->data, decToWeirdBinary(num));
    newNode->next = NULL;

    if (dataListHead == NULL) {
        dataListHead = newNode;
        last = dataListHead;
    } else {
        last->next = newNode;
        last = last->next;
    }

    /*Updating the length and DC.*/
    dataListLength++;
    (*DC)++;
}

/*
 * Frees the data list.
 */
void freeDataList() {
    dataPtr ptr;

    while (dataListHead != NULL) {
        ptr = dataListHead;
        dataListHead = dataListHead->next;
        free(ptr);
    }
    dataListLength = 0;
}