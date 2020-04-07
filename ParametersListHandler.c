#include "declarationsHeader.h"

extern paramsPtr paramsListHead;

/*
 * Adds the passed strings to the parameters list.
 */
void addToParamsList(char *firstParm, char *secondParm) {
    static paramsPtr last;
    paramsPtr newNode = (paramsPtr) malloc(sizeof(paramsList));
    MEMORY_ERROR(newNode)

    newNode->lineNum = lineCnt;
    newNode->next = NULL;

    /*First parameter.*/
    if (firstParm != NULL)
        strcpy(newNode->firstParam, firstParm);
    else
        strcpy(newNode->firstParam, "");

    /*Second parameter.*/
    if (secondParm != NULL)
        strcpy(newNode->secondParam, secondParm);
    else
        strcpy(newNode->secondParam, "");


    if (paramsListHead == NULL) {
        paramsListHead = newNode;
        last = paramsListHead;
    } else {
        last->next = newNode;
        last = newNode;
    }
}

/*
 *Debugging function for printing to the screen the parameters.
 */
void printParamsList() {
    paramsPtr p = paramsListHead;

    while (p != NULL) {
        puts("**************************");
        printf("line = %d\nFirst Parameter: '%s'\nSecond Parameter: '%s'\n",
               p->lineNum, p->firstParam, p->secondParam);
        p = p->next;
        puts("**************************");
    }
}

/*
 * Frees the parameters list.
 */
void freeParamsList() {
    paramsPtr p;

    while (paramsListHead != NULL) {
        p = paramsListHead;
        paramsListHead = paramsListHead->next;
        free(p);
    }
}
