#ifndef UTILS_H
#define UTILS_H
#include "Labels.h"
#include "Operations.h"
#include "Table.h"


/** This file contains various functions
 * such as verification, printing, and more, that accompany the assembler operation
 */


/** The following macro define the size of the computer's memory: 2^21 == 2097152 */
#define MEMORY_SIZE (2097152)

/** The following macros define the limits of the allowed range of ASCII code so the char will be visible */
#define MIN_CHAR_DATA (32)	/* Lower limit of the allowed range of visible ASCII codes: 32 */
#define MAX_CHAR_DATA (126)	/* Upper limit of the allowed range of visible ASCII codes: 126 */

/** The following macros define the limits of the allowed range for a 24-bit signed number */
#define MIN_NUMBER_DATA (-8388608)	/* Lower limit of the allowed range for storage in memory for data: -(2^23) == -8388608 */
#define MAX_NUMBER_DATA (8388607)	/* Upper limit of the allowed range for storage in memory for data: (2^23) - 1 == 8388607 */

/** The following macros define the limits of the allowed range for a 21-bit signed number */
#define MIN_NUMBER_IMM (-1048576)	/* Lower limit of the allowed range for use as immediate operand of instruction: -(2^20) = -1048576 */
#define MAX_NUMBER_IMM (1048575)	/* Upper limit of the allowed range for use as immediate operand of instruction: (2^20) - 1 = 1048575 */

/** Represents the number of directives in the assembly language */
#define NUM_OF_DIRECTIVES (4)





/* -------- FUNCTIONS -------- */



/* ----- validation function ----- */

/* -- syntax validations -- */

/**
 * Checks if a macro name definition complies with all assembler rules (including length, duplication, valid characters).
 * @param name The name of the macro to be checked
 * @param macroTable Pointer to the table of macros held by the assembler
 * Macro table is required as parameter to this function to ensure that the macro name isn't used as a macro name.
 * @return NO_ERROR if the macro name is valid and INPUT_ERROR otherwise
 */
int validateMacroDef(char *name, Table *macroTable);


/**
 * verifies that a label name definition complies with all assembler rules (including length, duplication, valid characters).
 * @param name The name of the label to be checked
 * @param labelTable Pointer to the table of labels held by the assembler
 * @param macroTable Pointer to the table of macros held by the assembler
 * Macro and label tables are required as parameter to this function to ensure that the label name isn't used as a macro or label name.
 * @return NO_ERROR if the name is valid, INPUT_ERROR otherwise
 */
int	validateLabelDef(char *name, Table *labelTable, Table * macroTable);


/**
 * Checks, for an operand given as a label, that there is a match between the addressing method in which it is given and the type of the label.
 * The condition is that an operand in a relative addressing method can only be a local label defined in an instruction line
 * @param label Pointer to the label that is the operand
 * @param operand Pointer to entire operand
 * @return INPUT_ERROR if the operand given in relative addressing is an external label or a label defined on a data prompt line,
 *			NO_ERROR otherwise
 */
int validateLabelTypeMatchAdrsMthd(Label *label, Operand *operand);


/*  -- overflow validations -- */

/**
 * A global function that checks whether the processor's memory limit has been reached
 * @return SYSTEM_ERROR if the memory limit has been reached, NO_ERROR otherwise
 */
int validateMemoryLimit();


/**
* checks that the given char is within the allowed range.
 * the char has to be a visible char, so it's ASCII code has to be between MIN_CHAR_DATA (32) and MAX_CHAR_DATA (126).
 * @param c The character to be checked
 * @return NO_ERROR if the char is within the range, INPUT_ERROR otherwise.
 */
int validateDataCharRange(char c);


/**
 * checks that the given number is within the allowed range for storage in 24 bits (signed) of data machine word.
 * the number has to be between MIN_NUMBER_DATA (2^23) to MAX_NUMBER_DATA (2^23 - 1).
 * @param num the number to be checked
 * @return NO_ERROR if the number is within the allowed range, INPUT_ERROR otherwise.
 */
int validateDataNumRange(long num);


/**
 * checks that the given number is within the allowed range for storage in 21 bits (signed) of instruction machine word.
 * (three bits out of 24 are reserved for the ARE field of operations' machine words.)
 * the number has to be between MIN_NUMBER_IMM (2^20) to MAX_NUMBER_IMM (2^20 - 1).
 * @param num the number to be checked
 * @return NO_ERROR if the number is within the allowed range, INPUT_ERROR otherwise.
 */
int validateImmNumRange(long num);


/* ----- print functions ----- */

/* -- print files -- */



/**
 * Responsible for printing to the output file with the extension ".ob" in the desired format:
 * A header containing the total number of machine words for instructions and for data,
 * followed by the instruction image and the data image
 * @param obFile Pointer to the file to which the printing is performed (Error-free stream).
 * @param operationTable The final updated operation table (after updating all operands' details)
 * @param dataTable The data table, without updating the data addresses.
 *					The globalIC variable should hold the final value of the operation image size.
 *					it will be added to each data unit's address.
 * @return SYSTEM_ERROR if an error occurred while printing to the file, NO_ERROR otherwise.
 */
int fprintObFile(FILE *obFile, Table *operationTable, Table *dataTable);


/**
 * Responsible for printing to the output file with the extension ".ent" in the desired format:
 * All label names and addresses declared as entry labels.
 * @param entFile Pointer to the file to which the printing is performed (Error-free stream).
 * @param labelTable The final updated label table (after updating data labels address)
 * @return SYSTEM_ERROR if an error occurred while printing to the file, NO_ERROR otherwise.
 */
int fprintEntFile(FILE *entFile, Table *labelTable);


/**
 * Responsible for printing to the output file with the extension ".ext" in the desired format:
 * All uses of external labels as operands of an operation.
 * For each use - prints the label name and the address of the machine word in which the label was used.
 * @param extFile Pointer to the file to which the printing is performed (Error-free stream).
 * @param operationTable The final updated operation table (after updating all operands' details)
 * @return SYSTEM_ERROR if an error occurred while printing to the file, NO_ERROR otherwise.
 */
int fprintExtFile(FILE *extFile, Table *operationTable);



/* -- print lines -- */


/**
 * prints an address in memory to a file in 7 decimal digits with zero padding on the left.
 * @param file Pointer to a file to print to
 * @param address The address to print
 */
void fprintAddress(FILE *file, unsigned long address);


/**
 * Prints a given 24-bit machine word to a file in 6-digit hexadecimal format with leading zeros
 * @param file Pointer to a file to print to
 * @param word the machine word to print
 */
void fprintWordInHexa(FILE *file, unsigned long word);


/**
 * prints a data unit to a file: address and value. using printAddress and PrintWordInHexa
 * @param file Pointer to a file to print to
 * @param data the data unit to print
 */
void fprintDataUnit(FILE *file, void *data);


/**
 * prints label, given name and address, in format: "name address". Prints a newline at the end
 * address print format as described in fprintAddress function.
 * @param file Pointer to a file to print to
 * @param labelName the label name to print
 * @param address The address to print
 */
void fprintLabel(FILE *file, char *labelName, unsigned long address);









/* ----- clean functions ----- */

/**
 * Frees all memory allocated during the program's run. used before exiting the program for any reason.
 * @param macroTable Pointer to the macro table
 * @param labelTable Pointer to the label table
 * @param DataTable Pointer to the data table
 * @param operationTable Pointer to the operation table
 */
void freeAllMemory(Table **macroTable, Table **labelTable, Table **DataTable, Table **operationTable);


/**
 * Closes all files opened while the program was running. Used before exiting the program for any reason.
 * @param asFile Pointer to a file named the same as the input file, with the extension ".as"
 * @param amFile Pointer to a file named the same as the input file, with the extension ".am"
 * @param obFile Pointer to a file named the same as the input file, with the extension ".ob"
 * @param entFile Pointer to a file named the same as the input file, with the extension ".ent"
 * @param extFile Pointer to a file named the same as the input file, with the extension ".ext"
 * @return SYSTEM_ERROR if an error occurred while closing the files, NO_ERROR otherwise
 */
int closeAllFiles(FILE *asFile, FILE *amFile, FILE *obFile, FILE *entFile, FILE *extFile);



/* ----- other functions ----- */


/**
 * skips any leading whitespace characters (spaces or tabs) in the string.
 * @param ptr A pointer to a pointer to the string.
 *			  The function not changes the original string but modifies the pointer to point to the first non-whitespace character.
 *			  If the pointer to the string is not saved before the function is called, the white skipped part will not be accessible
 */
void skipWhite(char **ptr);


/**
 * Removes trailing whitespace (spaces, tabs or newlines) from a string.
 * @param ptr Pointer to the string.
 *			  notice: the original string is modified when calling this function.
 */
void removeWhiteEnding(char *ptr);


/**
 * Checks whether a string consists of only whitespace (spaces, tabs and newlines) characters.
 * The function does not modify the original string
 * @param str the string to check
 * @return TRUE if the string consists of only whitespace, FALSE otherwise
 */
int isWhite(char *str);

/**
 * Finds the index of the directive in the directive list
 * @param directiveName The name of the directive to search for
 * @return the index of the directive in the directive list, if found,
 *			or NOT_FOUND if not found
 */
short idxOfDirective(char *directiveName);


/**
 * finds the index of the register name in the register list.
 * @param registerName the register name to look for
 * @return index of the register name in the register list, if found. otherwise, NOT_FOUND.
 */
short idxOfRegister(char *registerName);


#endif
