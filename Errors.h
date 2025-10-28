<<<<<<< HEAD
#ifndef ERRORS_H
#define ERRORS_H


/**
 * represents unique error codes used throughout the assembler for reporting errors.
 *
 * each value corresponds to a specific error situation. The messages
 * associated with each error code are defined in "Errors.c".
 */
typedef enum ErrorCode {
	NO_ERROR,
	ERROR_NO_INPUT,
	ERROR_TOO_LONG_FILE_NAME,
	ERROR_CREATE_FILE,
	ERROR_OPEN_FILE,
	ERROR_CLOSING_FILE,
	ERROR_READ_LINE_FROM_FILE,
	ERROR_MEMORY_ALLOCATION,
	ERROR_WRITE_TO_FILE,
	ERROR_USING_FILE,
	ERROR_REACHED_MEMORY_LIMIT,
	TOO_LONG_LINE,
	ERROR_EXPECTED_LABEL_NAME_BEFORE_COLON,
	ERROR_EXPECTED_WHITE_CHAR_AFTER_COLON,
	ERROR_ONLY_LABEL_DEF,
	ERROR_EXPECTED_LABEL_NAME_AFTER_EXTERN,
	ERROR_REQUIRED_NO_OPERANDS,
	ERROR_MISSING_OPERAND,
	ERROR_INVALID_SRC_ADRS_MTHD,
	ERROR_INVALID_DST_ADRS_MTHD,
	ERROR_MISSING_NUMBER_OPERAND,
	ERROR_NUMBER_OUT_OF_RANGE_IMM,
	ERROR_EXPECTED_COMMA_BETWEEN_OPERANDS,
	ERROR_LABEL_NOT_DEFINED,
	ERROR_INVALID_LABEL_TYPE,
	ERROR_RESERVED_WORD,
	ERROR_MACRO_NAME_ALREADY_USED,
	ERROR_REQUIRED_MACRO_NAME,
	ERROR_INVALID_MACRO_NAME_SYNTAX,
	ERROR_MACRO_NAME_TOO_LONG,
	ERROR_EXTRA_TEXT_AFTER_MACRO_CALL,
	ERROR_EXTRA_TEXT_AFTER_MACRO_DEF,
	ERROR_EXTRA_TEXT_AFTER_MACRO_END,
	ERROR_LABEL_NAME_TOO_LONG,
	ERROR_LABEL_NAME_ALREADY_USED_AS_LABEL,
	ERROR_LABEL_NAME_ALREADY_USED_AS_MACRO,
	ERROR_REQUIRED_LABEL_NAME,
	ERROR_INVALID_LABEL_NAME_SYNTAX,
	ERROR_UPDATE_ENTRY_FOR_EXTERN_LABEL,
	ERROR_EMPTY_DIRECTIVE,
	ERROR_COMMA_BEFORE_FIRST_NUMBER,
	ERROR_COMMA_AFTER_LAST_NUMBER,
	ERROR_INVALID_NUMBER,
	ERROR_NUMBER_OUT_OF_RANGE_DATA,
	ERROR_EXPECTED_COMMA_BETWEEN_NUMBERS,
	ERROR_INVALID_STRING,
	ERROR_CHAR_OUT_OF_RANGE_DATA,
	ERROR_UNRECOGNIZED_OPERATION
	} ErrorCode;


/**
 * represents an error, including a unique code and appropriate message.
 */
typedef struct Error {
	int num;
	char* message;
} Error;


/**
 * Prints a system error message according to the error encountered.
 * For example, memory allocation and file operation failures
 * @param error Error code to print
 */
void reportSystemError(ErrorCode error);

/**
 * Prints an input error message depending on the error encountered.
 * For example, syntax and overflow errors.
 * each error is related to a specific file and line
 * @param error Error code to print
 * @param fileName The name of the file in which the error occurred
 * @param lineNum The line number where the error occurred
 */
void reportInputError(ErrorCode error, char *fileName, unsigned int lineNum);

=======
#ifndef ERRORS_H
#define ERRORS_H


/**
 * represents unique error codes used throughout the assembler for reporting errors.
 *
 * each value corresponds to a specific error situation. The messages
 * associated with each error code are defined in "Errors.c".
 */
typedef enum ErrorCode {
	NO_ERROR,
	ERROR_NO_INPUT,
	ERROR_TOO_LONG_FILE_NAME,
	ERROR_CREATE_FILE,
	ERROR_OPEN_FILE,
	ERROR_CLOSING_FILE,
	ERROR_READ_LINE_FROM_FILE,
	ERROR_MEMORY_ALLOCATION,
	ERROR_WRITE_TO_FILE,
	ERROR_USING_FILE,
	ERROR_REACHED_MEMORY_LIMIT,
	TOO_LONG_LINE,
	ERROR_EXPECTED_LABEL_NAME_BEFORE_COLON,
	ERROR_EXPECTED_WHITE_CHAR_AFTER_COLON,
	ERROR_ONLY_LABEL_DEF,
	ERROR_EXPECTED_LABEL_NAME_AFTER_EXTERN,
	ERROR_REQUIRED_NO_OPERANDS,
	ERROR_MISSING_OPERAND,
	ERROR_INVALID_SRC_ADRS_MTHD,
	ERROR_INVALID_DST_ADRS_MTHD,
	ERROR_MISSING_NUMBER_OPERAND,
	ERROR_NUMBER_OUT_OF_RANGE_IMM,
	ERROR_EXPECTED_COMMA_BETWEEN_OPERANDS,
	ERROR_LABEL_NOT_DEFINED,
	ERROR_INVALID_LABEL_TYPE,
	ERROR_RESERVED_WORD,
	ERROR_MACRO_NAME_ALREADY_USED,
	ERROR_REQUIRED_MACRO_NAME,
	ERROR_INVALID_MACRO_NAME_SYNTAX,
	ERROR_MACRO_NAME_TOO_LONG,
	ERROR_EXTRA_TEXT_AFTER_MACRO_CALL,
	ERROR_EXTRA_TEXT_AFTER_MACRO_DEF,
	ERROR_EXTRA_TEXT_AFTER_MACRO_END,
	ERROR_LABEL_NAME_TOO_LONG,
	ERROR_LABEL_NAME_ALREADY_USED_AS_LABEL,
	ERROR_LABEL_NAME_ALREADY_USED_AS_MACRO,
	ERROR_REQUIRED_LABEL_NAME,
	ERROR_INVALID_LABEL_NAME_SYNTAX,
	ERROR_UPDATE_ENTRY_FOR_EXTERN_LABEL,
	ERROR_EMPTY_DIRECTIVE,
	ERROR_COMMA_BEFORE_FIRST_NUMBER,
	ERROR_COMMA_AFTER_LAST_NUMBER,
	ERROR_INVALID_NUMBER,
	ERROR_NUMBER_OUT_OF_RANGE_DATA,
	ERROR_EXPECTED_COMMA_BETWEEN_NUMBERS,
	ERROR_INVALID_STRING,
	ERROR_CHAR_OUT_OF_RANGE_DATA,
	ERROR_UNRECOGNIZED_OPERATION
	} ErrorCode;


/**
 * represents an error, including a unique code and appropriate message.
 */
typedef struct Error {
	int num;
	char* message;
} Error;


/**
 * Prints a system error message according to the error encountered.
 * For example, memory allocation and file operation failures
 * @param error Error code to print
 */
void reportSystemError(ErrorCode error);

/**
 * Prints an input error message depending on the error encountered.
 * For example, syntax and overflow errors.
 * each error is related to a specific file and line
 * @param error Error code to print
 * @param fileName The name of the file in which the error occurred
 * @param lineNum The line number where the error occurred
 */
void reportInputError(ErrorCode error, char *fileName, unsigned int lineNum);

>>>>>>> db1e6bedf286cc1a18e377ce301b3cfe5f147529
#endif