#include "declarationsHeader.h"

extern insPtr insListHead;
extern size_t insListLength;

/*
 * Adds the passed memory word to the instructions list.
 */
void addToInsList(Word *newWord, char *strWord, char *str) {
    static insPtr last;
    insPtr newNode = (insPtr) malloc(sizeof(insNode));
    MEMORY_ERROR(newNode)

    /*Updating the length of the list.*/
    insListLength++;

    newNode->data = (*newWord);
    strcpy(newNode->str, str);
    strcpy(newNode->strWord, strWord);
    newNode->next = NULL;


    if (insListHead == NULL) {
        insListHead = newNode;
        last = insListHead;
    } else {
        last->next = newNode;
        last = last->next;
    }
}

/*
 * Frees the instructions list.
 */
void freeInsList() {
    insPtr ptr;
    Word p;

    while (insListHead != NULL) {
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

