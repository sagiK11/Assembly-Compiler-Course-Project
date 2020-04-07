#include "declarationsHeader.h"

/*
 * Codes the ascii chars and puts it memory words into the data list.
 */
void codeStringLine(int *DC) {
    char lineCpy[LINE_SIZE];
    size_t i = 1;
    strcpy(lineCpy, line);

    /*Taking each char in the string and putting its ascii code to dataList.*/
    /*taking only the string part from the line*/
    strcpy(lineCpy, strchr(lineCpy, STRING));

    /*marking the end of the string.*/
    *strrchr(lineCpy, STRING) = STR_END;

    /*Inserting each char into the data array as ascii code*/
    while (lineCpy[i])
        addToDataList((int) lineCpy[i++], DC);
    addToDataList(STR_END, DC);
}