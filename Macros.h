#ifndef MACROS_H
#define MACROS_H

#include "Table.h"

/** This file implements functions that handle everything related to macro table.
 * The file manage the macros in a table data structure (see "table.h")
 * In general, it's user's responsibility to pass a not NULL pointer to a table that contains only macros.
 */


/** The maximum name length allowed for a macro name in assembly language */
#define MAX_MACRO_NAME 31



/** represents a single macro.
 */
typedef struct Macro {
 char name[MAX_MACRO_NAME];      /** the name of the macro */
 Table * content;                /** table that contains the lines of the macro */
} Macro;


/* -------- FUNCTIONS -------- */



/** Allocates memory for a new macro and adds it to the macro table.
 * Doesn't check the validity of the macro definition.
 * @param macroTable the macro table
 * @param macroName the new macro's name after validation.
* @return on success, the new macro's pointer. on failure of memory allocation, NULL.
 */
Macro * addMacroToTable(Table *macroTable, char *macroName);


/**
 * adds a new line to a specific macro's content
 * @param macro the macro to append to it a new line
 * @param line a line that should be appended to the macro
 * @return on success,  NO_ERROR. on failure of memory allocation, SYSTEM_ERROR.
 */
int addLineToMacro(Macro * macro, char *line);



/**
 * finds a macro by name in a macro table
 * @param macroTable pointer to the macro table
 * @param name macro name to search for
 * @return pointer to the macro if found. otherwise NULL
 */
Macro * findMacroInTable(Table *macroTable, char *name);



/**
 * prints to a file content of given macro.
 * @param file The file to which the printing is performed
 * @param macro The macro whose contents you want to print
 */
void fprintMacroContent(FILE * file, Macro * macro);






/**
 * frees all memory allocated for a given macro table
 * @param macroTable pointer to the macro table
 */
void freeMacroTable(Table **macroTable);



/**
 * compares a specific macro name with a name given as a string
 * @param macro a macro to compare its name
 * @param name a string to compare
 * @return TRUE on match, FALSE on mismatch
 */
int compareMacro(void *macro, void *name);



/**
 * writes to a file a single line. if the line doesn't end with newline, this function will add it.
 * @param file the file to print to
 * @param line the line to print
 */
void fprintLine(FILE * file, void *line);



/**
 * releases the memory allocated for single macro
 * @param macro pointer to a macro.
 */
void freeMacro(void **macro);


/**
 * releases the memory allocated for single line (in macro content)
 * @param line pointer to the line
 */
void freeLine(void **line);

#endif
