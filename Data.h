#ifndef DATA_H
#define DATA_H
#include "Table.h"

/** This file implements functions that handle everything related to data table.
 * "Data" refers to the data that the assembler stores in memory as a result of the ".data" or ".string" directive.
 * The "data" table is the data structure (see "Table.h") in which the assembler manages the data image of a program.
 *
 * In general, it's user's responsibility to pass a not NULL pointer to a table that contains only data units.
 */

/**
 * represents a single data unit
 * which takes one machine word in the Data image.
 */
typedef struct {
	/**
	 * The address of the data in the data image.
	 * When printing the output file, the size of the instruction image will be added to this value
	 * to calculate the final address of the data in the machine translation of the program.
	 */
	unsigned long dc;
	unsigned long value;		/** The value of the data */
} Data;


/* -------- FUNCTIONS -------- */


/**
 * Allocates memory for a new data unit and adds it to the data table.
 * Doesn't check the validity of the data value.
 * @param dataTable the data table
 * @param dc The address of the data in the data image.
 * @param value the value of the data
 * @return on success, the new data unit's pointer. on failure of memory allocation, NULL.
 */
Data * addSingleDataToTable(Table *dataTable, unsigned long dc, unsigned long value);



/**
 * prints to a file the whole data table
 * @param file the file to print to
 * @param dataTable pointer to the data table
 */
void fprintDataTable (FILE *file, Table *dataTable);




/**
 * gets an editable string that represents numbers (delimited by ',') to insert into the data table,
 * and inserts number by number into the table, while validating the input.
 * @param dataTable pointer to the data table
 * @param numbers editable string that represents integers
 * @return On success, NO_ERROR.
 * On an error related to the contents of the input file, INPUT_ERROR.
 * On an error related to system operation, SYSTEM_ERROR
 */
int addNumbersToTable(Table *dataTable, char *numbers);



/**
 * gets a string of chars to insert into the data table,
 * and inserts char by char into the table, with terminating '\0',
 * while validating the input.
 * @param dataTable pointer to the data table
 * @param dataLine editable string that represents string
 * @return On success, NO_ERROR.
 * On an error related to the contents of the input file, INPUT_ERROR.
 * On an error related to system operation, SYSTEM_ERROR
 */
int addStringToTable(Table *dataTable, char *dataLine);



/**
 * frees all memory allocated for a given data table.
 * @param dataTable pointer to the data table
 */
void freeDataTable(Table **dataTable);



/**
 * releases the memory allocated for single data unit
 * @param data pointer to a data unit.
 */
void freeDataUnit(void **data);




#endif
