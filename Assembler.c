#include "data.h"

symPtr symTableHead;
paramsPtr paramsListHead;
dataPtr dataListHead;
insPtr insListHead;
size_t dataListLength;
size_t insListLength;
char* fileName;


int main(int argc, char* argv[]){
    
    FILE* fd;
    size_t fileIndex = 1;

    if (argc < MIN_ARGUMENTS) {
        printError(not_enough_params, "");
        exit( EXIT_FAILURE );
    }

    TRANSLATE_ALL_FILES
        if ((fd = fopen(argv[fileIndex] , READ_ONLY)) == NULL) {
            printError(file_op_err , argv[fileIndex]);
            exit( EXIT_FAILURE );
        }

        fileName = argv[fileIndex++];
        *strchr( fileName , DOT) = STR_END;

        /*Restarting the error flag.*/
        generalError = FALSE;

        /*First-pass making the DataList & calculating length of the memory words.*/
        firstPass(fd);

        /*Returning the fd to the beginning of the file for the second pass.*/
        rewind(fd);

        /*Second-pass coding the rest of the code & creating the files.*/
        secondPass(fd);

        /*Freeing data structures I used  & closing the file.*/
        freeSymTable();
        freeParamsList();
        freeExtList();
        freeDataList();
        freeInsList();
        fclose(fd);

        /*For translating several.*/
        --argc;
    }

    return EXIT_SUCCESS;
}
