#include "declarationsHeader.h"

/*
 * Codes the numbers and puts its memory words into the data list.
 */
void codeDataLine(int *DC) {
    char lineCpy[LINE_SIZE];
    linePtr ptr = lineListHead;
    strcpy(lineCpy, line);

    if (dataIsAnArray(lineCpy)) {
        arrayCoding(ptr, lineCpy, DC);
    } else {/*Its not an array,but one number, inserting it.*/
        oneNumberCoding(ptr, DC);
    }
}

void arrayCoding(linePtr ptr, char *lineCpy, int *DC) {
    char temp[STRING_SIZE], *dataVar;
    size_t i = 1, firstInt, label = 3, noLabel = 2;

    /*Checking if there's a label before the data.*/
    firstInt = strchr(lineCpy, ISLABEL) ? label : noLabel;
    /*starting from the third string to avoid non numbers words.*/
    for (ptr = lineListHead; ptr && i != firstInt; ptr = ptr->next, i++);

    if (!(atoi(ptr->str)))
        printError(order_error, "");

    for (; ptr; ptr = ptr->next) {

        /*Taking the ints and putting into the data list.*/
        strcpy(temp, ptr->str);
        dataVar = strtok(temp, COMMA_STR);
        while (dataVar != NULL) {
            if (strchr(dataVar, DOT) != NULL)
                printError(float_err, dataVar);
            else
                addToDataList(atoi(dataVar), DC);
            dataVar = strtok(NULL, COMMA_STR);
        }
    }
}

void oneNumberCoding(linePtr ptr, int *DC) {
    /*Getting to the first number.*/
    for (ptr = lineListHead; ptr && (!atoi(ptr->str)); ptr = ptr->next);
    if (ptr == NULL)
        printError(data_line_with_no_data, "");
    else if (strchr(ptr->str, DOT))
        printError(float_err, ptr->str);
    else
        addToDataList(atoi(ptr->str), DC);

}


boolean dataIsAnArray(char *lineCpy) {
    return strchr(lineCpy, COMMA) != NULL;
}
