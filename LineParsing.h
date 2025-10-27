#ifndef LINEPARSING_H
#define LINEPARSING_H
#include "Macros.h"
#include "Operations.h"
#include "Table.h"

/**
 * This file packages line analysis functions for each of the assembler stages.
 * Each stage runs its corresponding function on each line of the file it reads,
 * and the function parses the line and performs whatever is necessary for execution in the context of that stage.
 */


/**
 * Represents different types of lines in the context of macros
 */
typedef enum {
	LINE_EMPTY,				/* line that contains only white chars (space, tab and newline) */
	LINE_COMMENT,			/* line that starts with ';' */
	LINE_OTHER,				/* not comment and not empty. can be one of the next types */
	LINE_MACRO_DEF,			/* line with new macro definition */
	LINE_MACRO_END,			/* line that indicates on end of macro definition */
	LINE_MACRO_USE,			/* line that uses a macro */
	LINE_REGULAR			/* just a regular line - line that is not one of the types above */
} MacroLineType;


/**
 * Represents different types of sentences in the context of assembly language statements - operations and directives
 */
typedef enum {
	/* The empty sentence and the comment sentence are redundant here, because this enum is used during the first pass,
	 * and there are no such sentences there because the preAssembler did not copy them to the extended source file (amFile) */
	SENTENCE_OPERATION,					/* sentence with operation name */
	SENTENCE_DATA_DIRECTIVE,			/* sentence with ".data" directive */
	SENTENCE_STRING_DIRECTIVE,			/* sentence with ".string" directive */
	SENTENCE_ENTRY_DIRECTIVE,			/* sentence with ".entry" directive */
	SENTENCE_EXTERN_DIRECTIVE,			/* sentence with ".extern" directive */
	SENTENCE_NOT_RECOGNIZED				/* unrecognized sentence */
} SentenceType;



/* ---------- FUNCTIONS ---------- */

/* ----- preAssembler ----- */
/**
 * Processes a single line from the input file asFile: from the analysis of the line type to the execution of the required operations from it.
 * @param line The line that should be parsed from the source file (input), after passing a length check. must be editable.
 * @param amFile The file to which the line or corresponding macro line(s) should be printed after processing
 * @param macroTable A table-type pointer to the macro table,
 * to update a new macro, add a line to a macro, search for and print a macro when needed and prevent defining a new macro with existing name of macro
 * @param lastMacro A pointer by reference to the last macro that was added.
 * This value is only updated and used by this function. It should not be changed outside the function
 * @param readMacro A flag, by reference, that indicates whether this line is inside a macro definition or not.
 * The flag will change when a macro definition starts or ends, by this function only. It should not be changed outside the function
 * @return On success, NO_ERROR.
 * On an error related to the contents of the input file, INPUT_ERROR.
 * On an error related to system operation, SYSTEM_ERROR
 */
int parseLinePreAssembler(char *line, FILE *amFile, Table *macroTable, Macro **lastMacro, int *readMacro);



/* ----- First Pass ----- */


/**
 * parses the line for the first pass purposes, reports errors, and updates relevant tables.
 * also, updates globalIC or globalDC according to the input line.
 * @param line a line that was read from the extended source file (amFile), to parse.
 * @param macroTable A table-type pointer to the macro table,
 * to prevent defining a label with existing name of a macro
 * @param labelTable A table-type pointer to the label table,
 * to update a new label, and prevent defining a new label with existing name of label
 * @param dataTable A table-type pointer to the data table, to update new data items.
 * @param operationTable A table-type pointer to the operation table, to update a new operation.
 * @return On success, NO_ERROR.
 * On an error related to the contents of the input file, INPUT_ERROR.
 * On an error related to system operation, SYSTEM_ERROR
 */
int parseLineFirstPass(char *line, Table *macroTable, Table *labelTable, Table *dataTable, Table *operationTable);



/* ----- Second Pass ----- */

/**
 * Parses line from the extended source file (amFile) in the second pass:
 * identifies entry directives, and updates the symbol table.
 * @param line a line that was read from the extended source file (amFile), to parse for entry directives.
 * @param labelTable A table-type pointer to the label table,
 * to update an existing label as entry linkage label.
 * @param createEnt A pointer to flag that indicates whether it is necessary to create an "ent" file or not.
 * The flag is passed by reference and updated in case a valid entry declaration is detected.
 * @return On success, NO_ERROR.
 * On an error related to the contents of the input file, INPUT_ERROR.
 */
int handleEntrySecondPass(char *line, Table *labelTable, int *createEnt);



#endif
