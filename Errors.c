#include "Errors.h"

#include <stdio.h>


Error errors[] = {
	{NO_ERROR, ""},
	{ERROR_NO_INPUT, "at least 1 file name is required"},
	{ERROR_TOO_LONG_FILE_NAME, "too long file name"},
	{ERROR_CREATE_FILE, "failed to create file"},
	{ERROR_OPEN_FILE, "failed to open file, please check file name."},
	{ERROR_CLOSING_FILE, "failed to close file"},
	{ERROR_READ_LINE_FROM_FILE, "failed to read from file"},
	{ERROR_MEMORY_ALLOCATION,"failed to allocate memory"},
	{ERROR_WRITE_TO_FILE, "failed to write to file"},
	{ERROR_USING_FILE, "error while using file"},
	{ERROR_REACHED_MEMORY_LIMIT, "memory is full"},
	{TOO_LONG_LINE, "line is too long"},
	{ERROR_EXPECTED_LABEL_NAME_BEFORE_COLON, "before ':' expected label name"},
	{ERROR_EXPECTED_WHITE_CHAR_AFTER_COLON, "after ':' expected white character"},
	{ERROR_ONLY_LABEL_DEF, "after label definition expected a sentence"},
	{ERROR_EXPECTED_LABEL_NAME_AFTER_EXTERN, "after .extern expected extern label name"},
	{ERROR_REQUIRED_NO_OPERANDS, "the operation in this line requires no operands"},
	{ERROR_MISSING_OPERAND, "operand is missing"},
	{ERROR_INVALID_SRC_ADRS_MTHD, "invalid addressing method of source operand for this operation"},
	{ERROR_INVALID_DST_ADRS_MTHD, "invalid addressing method of destination operand for this operation"},
	{ERROR_MISSING_NUMBER_OPERAND, "missing number right after '#'"},
	{ERROR_NUMBER_OUT_OF_RANGE_IMM, "number enterd is out of range. expected: signed 21-bits."},
	{ERROR_EXPECTED_COMMA_BETWEEN_OPERANDS, "comma is expected between 2 operands"},
	{ERROR_LABEL_NOT_DEFINED, "label used as operand is not defined"},
	{ERROR_INVALID_LABEL_TYPE, "invalid label type. in relative addressing method, local code label is required."},
	{ERROR_RESERVED_WORD, "Not allowed to use a reserved word of the assembly"},
	{ERROR_MACRO_NAME_ALREADY_USED, "A macro with similar name already exists"},
	{ERROR_REQUIRED_MACRO_NAME, "macro name is required in definition"},
	{ERROR_INVALID_MACRO_NAME_SYNTAX, "macro name syntax is invalid"},
	{ERROR_MACRO_NAME_TOO_LONG, "macro name is too long"},
	{ERROR_EXTRA_TEXT_AFTER_MACRO_CALL, "extra text after macro call"},
	{ERROR_EXTRA_TEXT_AFTER_MACRO_DEF, "extra text after macro definition"},
	{ERROR_EXTRA_TEXT_AFTER_MACRO_END, "extra text after macro end"},
	{ERROR_LABEL_NAME_TOO_LONG, "label name is too long"},
	{ERROR_LABEL_NAME_ALREADY_USED_AS_LABEL, "A label with similar name already exists"},
	{ERROR_LABEL_NAME_ALREADY_USED_AS_MACRO, "A macro with similar name already exists"},
	{ERROR_REQUIRED_LABEL_NAME, "label name is required in definition"},
	{ERROR_INVALID_LABEL_NAME_SYNTAX, "label name syntax is invalid"},
	{ERROR_UPDATE_ENTRY_FOR_EXTERN_LABEL, "unable to update extern label to entry"},
	{ERROR_EMPTY_DIRECTIVE, "empty directive is forbidden"},
	{ERROR_COMMA_BEFORE_FIRST_NUMBER, "comma before first number is forbidden"},
	{ERROR_COMMA_AFTER_LAST_NUMBER, "comma after last number is forbidden"},
	{ERROR_INVALID_NUMBER, "invalid number writing. expected: optional + or - and then digits"},
	{ERROR_NUMBER_OUT_OF_RANGE_DATA, "number enterd is out of range. expected: signed 24-bits."},
	{ERROR_EXPECTED_COMMA_BETWEEN_NUMBERS, "comma is expected between numbers"},
	{ERROR_INVALID_STRING, "invalid string. expected: string between \"\""},
	{ERROR_CHAR_OUT_OF_RANGE_DATA, "invisible char. legal ASCII codes: 32-126"},
	{ERROR_UNRECOGNIZED_OPERATION, "unrecognized operation"}
};


void reportSystemError(ErrorCode error) {
	printf("System error: %s\n", errors[error].message);
}

void reportInputError(ErrorCode error, char *fileName, unsigned int lineNum) {
	printf("Input error in file %s, line %d: %s\n", fileName, lineNum, errors[error].message);
}