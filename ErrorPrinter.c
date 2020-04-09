
#include "declarationsHeader.h"


/*
 * Prints to the stderr the error and its line number.
 **/
void printErrorWithComment(int error, char *str) {

    turnOnErrorFlag();
    fprintf(stderr, "Error: ");
    switch (error) {
        case illegal_sym :
            fprintf(stderr, "Illegal symbol: '%s' in line %d.\n", str, lineNumber);
            break;
        case sym_defined_already:
            fprintf(stderr, "Symbol: '%s' in line %d is defined already.\n", str, lineNumber);
            break;
        case sym_is_key :
            fprintf(stderr, "Symbol: '%s' in line %d is defined as a saved key.\n", str, lineNumber);
            break;
        case undef_parameter :
            fprintf(stderr, "Undefined parameter: '%s' in line %d\n", str, lineNumber);
            break;
        case writing_to_file :
            fprintf(stderr, "Error writing to file: %s\n", str);
            break;
        case undef_symbol :
            fprintf(stderr, "Undefined symbol: '%s' in line %d\n", str, lineNumber);
            break;
        case float_err :
            fprintf(stderr, "Assembler does not support floating point numbers - %s\n", str);
            break;
        case immediate_wrong_use :
            fprintf(stderr, "Wrong use of Immediate method - '%s' in line %d\n", str, lineNumber);
            break;
        case err_app_file :
            fprintf(stderr, "Error appending the file: '%s'\n.", str);
            break;
        case src_meth0_illegal:
            fprintf(stderr, "Illegal use of src method 0 in cmd:'%s' in line: %d\n", str,
                    lineNumber);
            break;
        case src_meth1_illegal:
            fprintf(stderr, "Illegal use of src method 1 in cmd:'%s' in line: %d\n", str,
                    lineNumber);
            break;
        case src_meth2_illegal:
            fprintf(stderr, "Illegal use of src method 2 in cmd:'%s' in line: %d\n", str,
                    lineNumber);
            break;
        case src_meth3_illegal:
            fprintf(stderr, "Illegal use of src method 3 in cmd:'%s' in line: %d\n", str,
                    lineNumber);
            break;
        case dst_meth0_illegal:
            fprintf(stderr, "Illegal use of dest method 0 in cmd:'%s' in line: %d\n", str,
                    lineNumber);
            break;
        case dst_meth1_illegal:
            fprintf(stderr, "Illegal use of dest method 1 in cmd:'%s' in line: %d\n", str,
                    lineNumber);
            break;
        case dst_meth2_illegal:
            fprintf(stderr, "Illegal use of dest method 2 in cmd:'%s' in line: %d\n", str,
                    lineNumber);
            break;
        case dst_meth3_illegal:
            fprintf(stderr, "Illegal use of dest method 3 in cmd:'%s' in line: %d\n", str,
                    lineNumber);
            break;
        case undef_char :
            fprintf(stderr, "Undefined char: '%s' in line: %d\n", str, lineNumber);
            break;
        case redu_info:
            fprintf(stderr, "Redundant info: '%s' in line: %d\n", str, lineNumber);
            break;
        case illegal_sym_mth:
            fprintf(stderr, "Illegal label: '%s' before paramters in line: %d\n", str, lineNumber);
            break;
        case symbol_is_ent_and_ext:
            fprintf(stderr, "Symbol is defined both extern and entry: '%s'\n", str);
            break;
        case illegal_operand:
            fprintf(stderr, "Illegal operand afer '%s' command in line: %d\n", str, lineNumber);
            break;
        default:
            fprintf(stderr, "error in line %d", lineNumber);
    }
}

void printError(int error) {

    turnOnErrorFlag();
    fprintf(stderr, "Error: ");
    switch (error) {
        case brack_err :
            fprintf(stderr, "Brackets miss match in line %d\n", lineNumber);
            break;
        case string_err :
            fprintf(stderr, "String ' \"\" ' missmatch in line %d\n", lineNumber);
            break;
        case not_enough_params :
            fprintf(stderr, "Too few parameters.\n.");
            break;
        case file_op_err :
            fprintf(stderr, "Error opening file.\n.");
            break;
        case missing_comma:
            fprintf(stderr, "Missing a comma in line: %d\n", lineNumber);
            break;
        case comm_mis_match :
            fprintf(stderr, "Comma miss match in line: %d\n", lineNumber);
            break;
        case extra_commas :
            fprintf(stderr, "Too many commas in line: %d\n", lineNumber);
            break;
        case extra_ent:
            fprintf(stderr, "Extra text after entry label in line: %d\n", lineNumber);
            break;
        case sym_miss_place :
            fprintf(stderr, "Symbol definition is not the first statment in line: %d\n", lineNumber);
            break;
        case order_error:
            fprintf(stderr, "The order of operators in line: %d is incorrect.\n", lineNumber);
            break;
        case missing_both_params:
            fprintf(stderr, "Missing both parameters in line: %d\n", lineNumber);
            break;
        case missing_first_param:
            fprintf(stderr, "Missing first parameter in line: %d\n", lineNumber);
            break;
        case missing_second_param:
            fprintf(stderr, "Missing second parameter in line: %d\n", lineNumber);
            break;
        case data_line_with_no_data:
            fprintf(stderr, "No data was found in line: %d\n", lineNumber);
            break;
        case no_definer_in_line:
            fprintf(stderr, "Missing a definer (.data/.string/cmd) in line: %d\n", lineNumber);
            break;
        case missing_sym:
            fprintf(stderr, "Missing symbol in line: %d\n", lineNumber);
            break;
        default:
            fprintf(stderr, "error in line %d", lineNumber);

    }
}

/*
 * Prints to the stderr the warning and its line number.
 */
void printWarning(int error, char *str, size_t lineCntCpy) {
    fprintf(stderr, "Warning: ");
    switch (error) {
        case uns_ex_sym:
            fprintf(stderr, "Unused ext symbol: '%s' in line: %d\n", str, lineCntCpy);
            break;
        case bef_ext:
            fprintf(stderr, "String:'%s' before extern symbol in line: %d\n", str, lineNumber);
            break;
        case invalid_input:
            fprintf(stderr, "No code was written, invalid input in file: '%s'\n", str);
            break;
        case chars_before_comment:
            fprintf(stderr, "Chars before a comment identifier in line: %d\n", lineNumber);
            break;
        default:
            fprintf(stderr, "General Warning.\n");
    }
}

void turnOnErrorFlag(){
    generalError = TRUE;
}
