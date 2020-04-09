#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*Macros.*/
#define TRUE 1
#define FALSE 0
#define MIN_ARGUMENTS 2
#define LINE_SIZE 80
#define STRING_SIZE 70
#define SYM_SIZE 31
#define ACTIONS_NUM 16
#define CMD_LENGTH 10
#define TMP_STR_LEN 10
#define MEM_WORD_LEN 14
#define LINE_DEFINERS_NUM 20
#define EQUAL 0
#define STR_END '\0'
#define O_BRACKETS '('
#define C_BRACKETS ')'
#define O_BRACKETS_STR "("
#define STRING '\"'
#define COMMA ','
#define COMMA_STR ","
#define ISLABEL ':'
#define ISCOMMENT ';'
#define DOT '.'
#define SLASH '/'
#define TAB "\t"
#define NEWLINE "\n"
#define IMMEDIATE_METHOD_ID '#'
#define NUM_OF_KEYS 28
#define GUIDING_SENTENCE_STRING ".string"
#define GUIDING_SENTENCE_INT ".data"
#define GUIDING_SENTENCE_EXTERN ".extern"
#define GUIDING_SENTENCE_ENT ".ent"
#define IMMEDIATE_METHOD 0
#define DIRECT_METHOD 1
#define JUMP_METHOD 2
#define DIRECT_REGISTER 3
#define FICTIVE_ADDRESS 0
#define READ_ONLY "r"
#define WRITE_ONLY "w"
#define INT_NUM_CMD 0
#define INT_NUM_REG 1
#define INT_NUM_ADD 2
#define STARTING_ADDRESS 100
#define MEMORY_ERROR(p)\
{\
    if(!p){\
        fprintf(stderr,"Memory allocation failed. Exiting.\n");\
        fprintf(stderr,"Error in pointer: %s line: %d\n",#p,lineNumber);\
        exit(EXIT_FAILURE);\
    }\
}
#define MAKE_NEW_WORD machWord = (Word) malloc(sizeof(struct machWord));\
                      MEMORY_ERROR(machWord)\

/*For clarity.*/
typedef short boolean;
/*------------------*//*Memory word.*/
typedef struct machWord {
    unsigned int ERA:2;
    unsigned int destOp:2;
    unsigned int srcOp:2;
    unsigned int opCode:4;
    unsigned int param2:2;
    unsigned int param1:2;
    unsigned int :0;
    unsigned int ERAforReg: 2;
    unsigned int reg1 :6;
    unsigned int reg2: 6;
    unsigned int :0;
    unsigned int ERAforAdd:2;
    unsigned int address: 12;
    unsigned int :0;
    unsigned int isCmd : 1;
    unsigned int isReg : 1;
    unsigned int isAdd : 1;
} *Word;
/*------------------*//*Constants keywords.*/
typedef struct keyWords {
    char *symbol;
    unsigned int numValue;
    boolean hasSrc;
    boolean hasDst;
    boolean legitSrcMethodZro;
    boolean legitSrcMethodOne;
    boolean legitSrcMethodTwo;
    boolean legitSrcMethodThr;
    boolean legitDstMethodZro;
    boolean legitDstMethodOne;
    boolean legitDstMethodTwo;
    boolean legitDstMethodThr;
} *keyWords;
/*------------------*//*Symbol table.*/
typedef struct symbol *symPtr;
typedef struct symbol {
    char symName[SYM_SIZE];
    unsigned int data;
    boolean isExt;
    boolean isEnt;
    boolean inInsSen;
    boolean definedInFile;
    symPtr next;
} symbol;
/*------------------*//*Parameters list.*/
typedef struct paramsList *paramsPtr;
typedef struct paramsList {
    int lineNum;
    char firstParam[STRING_SIZE];
    char secondParam[STRING_SIZE];
    paramsPtr next;
} paramsList;
/*------------------*//*External symbols list.*/
typedef struct extList *extPtr;
typedef struct extList {
    char symbol[SYM_SIZE];
    int address;
    boolean updated;
    extPtr next;
} extNode;
/*------------------*//*Data list.*/
typedef struct DataList *dataPtr;
typedef struct DataList {
    char data[MEM_WORD_LEN];
    dataPtr next;
} dataNode;
/*------------------*//*Instructions list.*/
typedef struct insList *insPtr;
typedef struct insList {
    Word data;
    char strWord[MEM_WORD_LEN];
    char str[STRING_SIZE];
    insPtr next;
} insNode;
/*------------------*//*Line strings list.*/
typedef struct lineList *linePtr;
typedef struct lineList {
    char str[STRING_SIZE];
    linePtr next;
} lineNode;
/*------------------*//*Global variables.*/
boolean generalError;
size_t lineNumber;
char *line;
linePtr lineListHead;
size_t lineListLength;
/*func prototype*/
/*------------------*//*Assembler function*/
void translateFiles(int argc, char **argv);

void freeDataStructures();

void closeFile(FILE *fd);

void parseFileName();

void resetErrorFlag();

void rewindForSecondPass(FILE *pIobuf);

/*------------------*//*FirstPass function*/
void firstPass(FILE *);

void codeTheLine(int *, int *);

void codeDataLine(int *);

void codeStringLine(int *);

void externSymLine();

void calCmdLine(int *);

void analyseParameters();

void parseLine(char *line);

void incrementLinesNumber();

void arrayCoding(linePtr, char *, int *);

void oneNumberCoding(linePtr, int *);

void checkForFloatErrors(char *, char *);

void codeTwoParameters(char *);

void codeOneParameter(char *);

void incMemoryWordsByOne();

void incMemoryWordsByTwo();

void moveLineAfterSymbol(char *);

boolean labelBeforeParam();

boolean lineWithLabelDefinition();

boolean lineWithCommandDefinition();

boolean lineWithDataDefinition();

boolean lineWithStringDefinition();

boolean noParameters(char *);

boolean twoParameters(char *lineCpy);

boolean dataIsAnArray(char *);

boolean bothParametersAreRegisters(char *, char *);

linePtr movePtrToFirstParameter(linePtr, char *);

char *getLineFromFile(char *line, FILE *pIobuf);

/*------------------*//*SecondPass functions*/
void secondPass(FILE *);

void getCommand(char *);

void syncParametersToLine(paramsPtr *);

void preWritingToFileEnt();

void createFiles();

void codeCommand();

void codeParameters();

void specialActionCoding(Word *, paramsPtr);

void regActionCoding(Word *, paramsPtr);

void bothParamsAreRegistersSetting(Word *, unsigned, unsigned);

void paramIsANumberSetting(Word *, char *, int);

void paramIsALabelSetting(Word *, symPtr *);

void paramIsARegisterSetting(Word *, unsigned, boolean, boolean);

void externSymWasntUsed(FILE *);

void checkForErrorsInRegAction();

void preCode(Word);

void findOpCode(Word, char *);

void copyParameters(paramsPtr);

void codedAsFirstParamIsLabel();

void resetFlags();

void resetLineNumber();

void codedAsFirstParameterIsNumber(paramsPtr);

void codedAsFirstParameterIsRegister();

void unaryActionCoding(Word *, paramsPtr);

void firstMethodCoding(Word *, paramsPtr);

void secondMethodCoding(Word *, paramsPtr);

void findParameters(Word *);

void codeFirstParamMethod(Word *, paramsPtr, int);

void codeSecondParamMethod(Word *, paramsPtr, boolean);

void findLabelBeforeParameters(char *, char *);

boolean nothingToCode(paramsPtr, char *);

boolean specialTypeAction(char *cmd);

boolean errorsInSymTable();

boolean isImmediate(char *);

boolean paramIsLabel(char *);

boolean isRegister(char *);

boolean lineWithEntryLabel();

boolean lineWithExternLabel();

boolean codedAsBothParametersAreRegisters();

boolean codedAsSecondParameterIsLabel();

boolean codedAsSecondParameterIsNumber(paramsPtr);

boolean codedAsSecondParameterIsRegister();

boolean codingParametersSuccessful(paramsPtr);

boolean errorInLine(paramsPtr);

boolean noOperands(char *line);

boolean searchLabel(symPtr, Word, char *, char *);

/*------------------*//*Aux function*/
void removeSpaces(char *);

void printError(int);

void printErrorWithComment(int, char *);

void turnOnErrorFlag();

void printWarning(int, char *, size_t);

void createLineList();

void checkForErrors(char *, Word);

void writeObjectFile();

void writeEntFile();

void writeExtFile();

void addSizesToObjectFile(FILE *, int, int);

void addDataCodeToFile(FILE *);

void isFilesAreEqual();

void memWordToString(Word, size_t, size_t, size_t, char *, char *);

void extSymbolSetting(char *, char *, symPtr);

void symInitailzeSetting(symPtr, int *, boolean);

void insSymbolSetting(char *, symPtr);

char *decToWeirdBinary(int);

boolean symIsIllegal(char *);

boolean lineIsComment(char *);

boolean isBinaryAction(char *);

size_t getNumberOfParametersInLine();

size_t findOperandType();

boolean symIsExtern(symPtr, char *);

/*------------------*//*line analysis functions*/
void analyzeLine(int *);

void findSymbols(int *);

void findSyntaxErrors();

void bracketsAndStrError(int);

void findWrongMethodUse();

void commaError();

void searchUndefChars();

void findRedundantInfo();

void labelError();

void paramsError();

void definerError();

void extOrEntError();

void stopRtsError();

boolean symIsDefined(char *);

boolean symIsAKey(char *);

/*------------------*//*DataTypes function*/
void addToSymTable(char *, int *, boolean);

void updateSymbolTable(int *);

void updateLabelAddress(char *, int *);

void addToDataList(int, int *);

void addToInsList(Word *, char *, char *);

void addToLineList(char *);

void addToParamsList(char *, char *);

void addToExtList(char *);

void updateExtList(int);

void printSymTable();

void printParamsList();

void freeDataList();

void freeInsList();

void freeParamsList();

void freeSymTable();

void freeExtList();

void freeLineList();

/*------------------*//*Errors enums*/
enum errors {
    illegal_sym = 101,
    sym_defined_already,
    sym_is_key,
    brack_err,
    string_err,
    undef_parameter,
    writing_to_file,
    undef_symbol,
    float_err,
    immediate_wrong_use,
    not_enough_params,
    file_op_err,
    err_app_file,
    src_meth0_illegal,
    src_meth1_illegal,
    src_meth2_illegal,
    src_meth3_illegal,
    symbol_is_ent_and_ext,
    data_line_with_no_data,
    no_definer_in_line,
    dst_meth0_illegal,
    dst_meth1_illegal,
    dst_meth2_illegal,
    dst_meth3_illegal,
    missing_comma,
    comm_mis_match,
    extra_commas,
    undef_char,
    extra_ent,
    uns_ex_sym,
    redu_info,
    illegal_sym_mth,
    bef_ext,
    sym_miss_place,
    order_error,
    invalid_input,
    missing_second_param,
    missing_first_param,
    missing_both_params,
    chars_before_comment,
    missing_sym,
    illegal_operand

};
