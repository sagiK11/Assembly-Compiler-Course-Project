#include "declarationsHeader.h"


/*
 * Constants keywords within the language.
 */
struct keyWords keys[] =
        {                 /*The booleans refers the legit methods the cmd's can be used.*/
                {"mov",     0,  TRUE,  TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, TRUE,  FALSE, TRUE},
                {"cmp",     1,  TRUE,  TRUE,  TRUE,  TRUE,  FALSE, TRUE,  TRUE,  TRUE,  FALSE, TRUE},
                {"add",     2,  TRUE,  TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, TRUE,  FALSE, TRUE},
                {"sub",     3,  TRUE,  TRUE,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, TRUE,  FALSE, TRUE},
                {"not",     4,  FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE},
                {"clr",     5,  FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE},
                {"lea",     6,  TRUE,  TRUE,  FALSE, TRUE,  FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE},
                {"inc",     7,  FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE},
                {"dec",     8,  FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE},
                {"jmp",     9,  FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  TRUE},
                {"bne",     10, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  TRUE},
                {"red",     11, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  FALSE, TRUE},
                {"prn",     12, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  FALSE, TRUE},
                {"jsr",     13, FALSE, TRUE,  FALSE, FALSE, FALSE, FALSE, FALSE, TRUE,  TRUE,  TRUE},
                {"rts",     14, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,},
                {"stop",    15, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,},
                {".data",   0},
                {".string", 0},
                {".entry",  0},
                {".extern", 0},
                {"r0",      0},
                {"r1",      1},
                {"r2",      2},
                {"r3",      3},
                {"r4",      4},
                {"r5",      5},
                {"r6",      6},
                {"r7",      7}
        };
