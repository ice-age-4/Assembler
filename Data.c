#include "Data.h"

#include <stdlib.h>
#include <string.h>

#include "Errors.h"
#include "globals.h"
#include "utils.h"

/* global variables, documented in "Assembler.h" */
extern unsigned long globalIC;
extern unsigned long globalDC;
extern char *globalCurrentFile;
extern unsigned long globalCurrentLine;


/**
 * creates a new data unit with address (dc) and value.
 * @param address The address of the data in the memory image.
 * @param value The value that represents the data, up to 24 bits
 * @return On success, returns the pointer to the Data unit. On failure of memory allocation, returns NULL.
 */
static Data * createData(unsigned long address, unsigned long value);


/* static function. prototype and documentation above in this file */
static Data * createData(unsigned long address, unsigned long value) {
	Data * data = malloc(sizeof(Data));

	if (data != NULL) {
		data->dc = address;
		data->value = value;
	}
	return data;
}



Data * addSingleDataToTable(Table *dataTable, unsigned long dc, unsigned long value) {
	Data *data = createData(dc, value);

	/* check if memory allocation for the new data unit succeeded */
	if (data == NULL) {
		return NULL;
	}
	/* memory allocation of data unit succeeded -> add the new data unit to the data table */
	if (addToTable(dataTable, (void *) data) == SYSTEM_ERROR) {
		free(data);
		return NULL;
	}
	return data;
}

void fprintDataTable(FILE *file, Table *dataTable) {
	fprintTable(file, dataTable, fprintDataUnit);
}




int addNumbersToTable(Table *dataTable, char *numbers) {
	char *p = numbers;
	char *end;
	long num;

	/* ignore white starting and ending */
	skipWhite(&p);
	removeWhiteEnding(p);

	/* empty directive. required arguments */
	if (*p == '\0') {
		reportInputError(ERROR_EMPTY_DIRECTIVE, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}

	/* comma before first number */
	if (*p == ',') {
		reportInputError(ERROR_COMMA_BEFORE_FIRST_NUMBER, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}

	/* loop for all numbers in string */
	while (*p != '\0') {

		/* get next number */
		num = strtol(p, &end, 10);

		/* check if succeeded to get number */
		if (end == p) {
			reportInputError(ERROR_INVALID_NUMBER, globalCurrentFile, globalCurrentLine);
			return INPUT_ERROR;
		}

		/* validate range of number */
		if (validateDataNumRange(num) == INPUT_ERROR) {
			reportInputError(ERROR_NUMBER_OUT_OF_RANGE_DATA, globalCurrentFile, globalCurrentLine);
			return INPUT_ERROR;
		}

		/* all parts of validation completed. add number to table and promote globalDC */
		if (addSingleDataToTable(dataTable, globalDC, num) == NULL) {
			reportSystemError(ERROR_MEMORY_ALLOCATION);
			return SYSTEM_ERROR;
		}
		globalDC++;

		/* reach next non-white char */
		p = end;
		skipWhite(&p);

		/* first non-white char after number is '\0' -> finish. */
		if (*p == '\0') {
			return NO_ERROR;
		}

		/* this is the first non-white char after number -> it has to be ','. */
		if (*p != ',') {
			reportInputError(ERROR_EXPECTED_COMMA_BETWEEN_NUMBERS, globalCurrentFile, globalCurrentLine);
			return INPUT_ERROR;
		}

		/* the first non-white char after number was ',' -> skip it. */
		p++;
		/* reach next non-white char */
		skipWhite(&p);

		/* end of string after ',' -> error */
		if (*p == '\0') {
			reportInputError(ERROR_COMMA_AFTER_LAST_NUMBER, globalCurrentFile, globalCurrentLine);
			return INPUT_ERROR;
		}

		/* the string of numbers continues, repeat the loop to handle next number. */
	}

	return NO_ERROR;
}


int addStringToTable(Table *dataTable, char *dataLine) {
	/* ignore white chars in begin and end of data line. */
	skipWhite(&dataLine);
	removeWhiteEnding(dataLine);

	/* empty directive. required arguments */
	if (*dataLine == '\0') {
		reportInputError(ERROR_EMPTY_DIRECTIVE, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}

	/* validate that the string is legal (starting and ending with '"'). */
	if ((dataLine[0] != '"') || (dataLine[strlen(dataLine) - 1] != '"')) {
		reportInputError(ERROR_INVALID_STRING, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}

	/* checks that the string is not of 1 char like this: " */
	if (strlen(dataLine) == 1) {
		reportInputError(ERROR_INVALID_STRING, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}

	/* promoting data to point to first char in the string that should be entered into the data table */
	dataLine++;

	/* if we've got here, the string is valid. -> for each char in string - validae range, enter to data table and check if succeeded. */
	/* inserting all characters in the string except the terminating character '"', which precedes '\0', to the data table */
	while (*(dataLine+1) != '\0') {
		/* validate range of char */
		if (validateDataCharRange(*dataLine) == INPUT_ERROR) {
			reportInputError(ERROR_CHAR_OUT_OF_RANGE_DATA, globalCurrentFile, globalCurrentLine);
			return INPUT_ERROR;
		}

		/* if not succeeded to add data to table, stop and return an error value. */
		if (addSingleDataToTable(dataTable, globalDC, *dataLine) == NULL) {
			reportSystemError(ERROR_MEMORY_ALLOCATION);
			return SYSTEM_ERROR;
		}
		globalDC++;
		dataLine++;
	}

	/* inserting '\0' as a terminating character to the data table and check if succeeded */
	if (addSingleDataToTable(dataTable, globalDC, '\0') == NULL) {
		reportSystemError(ERROR_MEMORY_ALLOCATION);
		return SYSTEM_ERROR;
	}
	globalDC++;
	return NO_ERROR;
}

void freeDataTable(Table **dataTable) {
	freeTable(dataTable, freeDataUnit);
}

void freeDataUnit(void **data) {
	Data **tmp = (Data **)data;
	if (*tmp != NULL) {
		free(*tmp);
		*tmp = NULL;
	}
}


