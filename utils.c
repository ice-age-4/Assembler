#include "utils.h"

#include <ctype.h>
#include <string.h>

#include "Assembler.h"
#include "Data.h"
#include "Errors.h"
#include "Files.h"
#include "globals.h"
#include "Labels.h"
#include "Macros.h"

/* global variables, documented in "Assembler.h" */
extern char *globalCurrentFile;
extern unsigned long globalCurrentLine;
extern unsigned long globalIC, globalDC;

/* A constant static array of cpu's register names */
static const char *REGS[NUM_OF_REGISTERS] = { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7" };

/* A constant static array of assembly language directives names */
static const char *DIRECTIVES[NUM_OF_DIRECTIVES] = { "data", "string", "entry", "extern"};

/**
 * Verify that name is not an assembly reserved word (operations, directives, registers)
 * @param name The name to be verified against the reserved words.
 * @return NO_ERROR if the name is not reserved by assembly language, INPUT_ERROR otherwise
 */
static int validateNotReservedWord(char *name);


/**
 * Verifies that a macro name complies with all assembly language syntax rules:
 * Starts with an underscore or an alphabetic letter, and all characters are underscores, alphabetic letters, or digits.
 * macro name length is limited by MAX_MACRO_NAME (31).
 * @param name the name to be checked
 * @return NO_ERROR if syntax is valid, INPUT_ERROR otherwise.
 */
static int validateMacroNameSyntax(char *name);


/**
 * Verifies that a label name complies with all assembly language syntax rules:
 * Starts with an alphabetic letter, and all characters are alphabetic letters or digits.
 * label name length is limited by MAX_LABEL_NAME (31).
 * @param name the name to be checked
 * @return NO_ERROR if syntax is valid, INPUT_ERROR otherwise.
 */
static int validateLabelNameSyntax(char *name);


/* linear search in all arrays of reserved words */
int validateNotReservedWord(char *name) {
	int status = NO_ERROR;				/* Holds error status during function */

	if (idxOfOperation(name) != NOT_FOUND)
		status = INPUT_ERROR;
	if (idxOfRegister(name) != NOT_FOUND)
		status = INPUT_ERROR;
	if (idxOfDirective(name) != NOT_FOUND)
		status = INPUT_ERROR;

	/* compare also with macro definition reserved words */
	if ((strcmp(name, "mcro") == 0) || (strcmp(name, "mcroend") == 0))
		status = INPUT_ERROR;

	if (status == INPUT_ERROR)
		reportInputError(ERROR_RESERVED_WORD, globalCurrentFile, globalCurrentLine);

	return status;
}


/* checks: not reserved word, not used yet as macro name and valid syntax */
int validateMacroDef(char *name, Table *macroTable) {

	/* checks that the name is not a reserved word */
	if (validateNotReservedWord(name) == INPUT_ERROR)
		return INPUT_ERROR;

	/* checks that there's no macro with this name */
	if (findMacroInTable(macroTable, name) != NULL) {
		reportInputError(ERROR_MACRO_NAME_ALREADY_USED, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}

	/* checks that name syntax is correct */
	if (validateMacroNameSyntax(name) == INPUT_ERROR) {
		return INPUT_ERROR;
		/* the function called prints appropriate errors messages. */
	}

	return NO_ERROR;
}


/* checks: not reserved word, not used yet as macro or label name and valid syntax */
int validateLabelDef(char *name, Table *labelTable, Table *macroTable) {

	/* checks that the name is not a reserved word */
	if (validateNotReservedWord(name) == INPUT_ERROR)
		return INPUT_ERROR;

	/* checks that there's no label with this name */
	if (findLabelInTable(labelTable, name) != NULL) {
		reportInputError(ERROR_LABEL_NAME_ALREADY_USED_AS_LABEL, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}

	/* checks that there's no macro with this name */
	if (findMacroInTable(macroTable, name) != NULL) {
		reportInputError(ERROR_LABEL_NAME_ALREADY_USED_AS_MACRO, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}

	/* checks that name syntax is correct */
	if (validateLabelNameSyntax(name) == INPUT_ERROR) {
		return INPUT_ERROR;
		/* the function called prints appropriate errors messages. */
	}

	return NO_ERROR;
}


/* The total memory space for both code and data images cannot exceed the memory limit. */
int validateMemoryLimit() {
	if (globalIC + globalDC <= MEMORY_SIZE) {
		return NO_ERROR;
	}
	return SYSTEM_ERROR;
}


/* The allowed storage range is defined by the printable character range in the ASCII table. */
int validateDataCharRange(char c) {
	if (c >= MIN_CHAR_DATA && c <= MAX_CHAR_DATA) {
		return NO_ERROR;
	}
	return INPUT_ERROR;
}


/* The allowed storage range is defined by the number of bits allocated to store the number. */
int validateDataNumRange(long num) {
	if (num >= MIN_NUMBER_DATA && num <= MAX_NUMBER_DATA) {
		return NO_ERROR;
	}
	return INPUT_ERROR;
}


/* The allowed storage range is defined by the number of bits allocated to store the number. */
int validateImmNumRange(long num) {
	if (num >= MIN_NUMBER_IMM && num <= MAX_NUMBER_IMM) {
		return NO_ERROR;
	}
	return INPUT_ERROR;
}


/* relative addressing and external or data label don't work together */
int validateLabelTypeMatchAdrsMthd(Label *label, Operand *operand) {
	if (operand->addressing == RELATIVE_ADDRESSING) {
		if (isExtern(label) == TRUE || isData(label) == TRUE)
			return INPUT_ERROR;
	}
	return NO_ERROR;
}


/* easy to understand */
int fprintObFile(FILE *obFile, Table *operationTable, Table *dataTable) {
	/* print header */
	fprintf(obFile, "%7ld %ld\n", globalIC-IC_INIT_VALUE, globalDC);
	/* Calls a function that prints the instruction image in the desired format */
	fprintOperationTable(obFile, operationTable);
	/* Calls a function that prints the data image in the desired format */
	fprintDataTable(obFile, dataTable);

	/* check if an error occurred while printing to the file
	 * When calling the function, the file was error-free */
	if (ferror(obFile)) {
		reportSystemError(ERROR_WRITE_TO_FILE);
		return SYSTEM_ERROR;
	}
	return NO_ERROR;
}


/* easy to understand */
int fprintEntFile(FILE *entFile, Table *labelTable) {
	/* Calls a function that prints from the label table the labels declared as entry in the desired format */
	fprintTable(entFile, labelTable, fprintEntryLabel);

	/* check if an error occurred while printing to the file.
	 * When calling the function, the file was error-free */
	if (ferror(entFile)) {
		reportSystemError(ERROR_WRITE_TO_FILE);
		return SYSTEM_ERROR;
	}
	return NO_ERROR;
}


/* easy to understand */
int fprintExtFile(FILE *extFile, Table *operationTable) {
	/* Calls a function that prints from the instruction image, the uses of external labels as operands of operations, in the desired format */
	fprintTable(extFile, operationTable, fprintExtUsagesInOperation);

	/* check if an error occurred while printing to the file.
	 * When calling the function, the file was error-free */
	if (ferror(extFile)) {
		reportSystemError(ERROR_WRITE_TO_FILE);
		return SYSTEM_ERROR;
	}
	return NO_ERROR;
}


void fprintDataUnit(FILE *file, void *data) {
	Data *tmp = (Data *)data;

	fprintAddress(file, tmp->dc + globalIC);
	fprintf(file, " ");
	fprintWordInHexa(file, tmp->value);
	fprintf(file, "\n");
}

/* Prints an address (of a label or a word in memory) in a specified format */
void fprintAddress(FILE *file, unsigned long address) {
	fprintf(file, "%07lu", address);
}


/* Prints label name and address in a specified format for ".ent" and ".ext" files */
void fprintLabel(FILE *file, char *labelName, unsigned long address) {
	fprintf(file, "%s ", labelName);
	fprintAddress(file, address);
	fprintf(file, "\n");
}


/* Prints a given 24-bit machine word to a file in a specified format */
void fprintWordInHexa(FILE *file, unsigned long word) {
	/* we do (word & 0xFFFFFF) to delete any lit bits on the left of the first 24 bits, which are irrelevant.
	 * (they may be lit due to sign extension). */
	fprintf(file, "%06lx", word & 0xFFFFFF);
}


/* static function. prototype and documentation above in this file */
static int validateMacroNameSyntax(char *name) {
	int status = NO_ERROR;			/* Holds error status during function */
	char *tmp = name;				/* Helper pointer for double-scanning the string */

	/* check name is not empty */
	if (*tmp == '\0') {
		reportInputError(ERROR_REQUIRED_MACRO_NAME, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}


	/* checks valid characters - details in documentation */
	if (!(isalpha(*tmp) || *tmp == '_'))
		status = INPUT_ERROR;
	for (tmp+=1; *tmp != '\0'; tmp++ ) {
		if (!(isalnum(*tmp) || *tmp == '_'))
			status = INPUT_ERROR;
	}
	if (status == INPUT_ERROR) {
		reportInputError(ERROR_INVALID_MACRO_NAME_SYNTAX, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}

	/* checks the limit on maximum length of macro name */
	if (strlen(name) > MAX_MACRO_NAME) {
		reportInputError(ERROR_MACRO_NAME_TOO_LONG, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}

	return NO_ERROR;
}


/* static function. prototype and documentation above in this file */
static int validateLabelNameSyntax(char *name) {
	int status = NO_ERROR;			/* Holds the error status during the function  */
	char *tmp = name;				/* Helper pointer for double-scanning the string */

	/* check name is not empty */
	if (*tmp == '\0') {
		reportInputError(ERROR_REQUIRED_LABEL_NAME, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}

	/* checks valid characters */
	if (!isalpha(*tmp))
		status = INPUT_ERROR;
	for (tmp+=1; *tmp != '\0'; tmp++ ) {
		if (!isalnum(*tmp))
			status = INPUT_ERROR;
	}
	if (status == INPUT_ERROR) {
		reportInputError(ERROR_INVALID_LABEL_NAME_SYNTAX, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}

	/* checks the limit on maximum length of label name */
	if (strlen(name) > MAX_LABEL_NAME) {
		reportInputError(ERROR_LABEL_NAME_TOO_LONG, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}

	return NO_ERROR;
}


/* During assembly execution, any memory allocated is retained within the tables.
 * Freeing the tables using helper functions ensures that all allocated memory is freed. */
void freeAllMemory(Table **macroTable, Table **labelTable, Table **dataTable, Table **operationTable) {
	if (*macroTable != NULL)
		freeMacroTable(macroTable);
	if (*labelTable != NULL)
		freeLabelTable(labelTable);
	if (*dataTable != NULL)
		freeDataTable(dataTable);
	if (*operationTable != NULL)
		freeOperationTable(operationTable);
}


/* During assembly execution, no additional files were opened other than those passed as parameters to the function.
 * Closes the files carefully while checking for errors. */
int closeAllFiles(FILE *asFile, FILE *amFile, FILE *obFile, FILE *entFile, FILE *extFile) {
	if (asFile) {
		if (fclose(asFile) != 0) {
			reportSystemError(ERROR_CLOSING_FILE);
			return SYSTEM_ERROR;
		}
	}
	if (amFile) {
		if (fclose(amFile) != 0) {
			reportSystemError(ERROR_CLOSING_FILE);
			return SYSTEM_ERROR;
		}
	}
	if (obFile) {
		if (fclose(obFile) != 0) {
			reportSystemError(ERROR_CLOSING_FILE);
			return SYSTEM_ERROR;
		}
	}
	if (entFile) {
		if (fclose(entFile) != 0) {
			reportSystemError(ERROR_CLOSING_FILE);
			return SYSTEM_ERROR;
		}
	}
	if (extFile) {
		if (fclose(extFile) != 0) {
			reportSystemError(ERROR_CLOSING_FILE);
			return SYSTEM_ERROR;
		}
	}

	return NO_ERROR;
}


/* Easy to understand */
void skipWhite(char **ptr) {
	while (**ptr == ' ' || **ptr == '\t')
		(*ptr)++;
}


/* Removes consecutive whitespace characters from the end of the string. modifies original string */
void removeWhiteEnding(char *ptr) {
	char *end;		/* pointer that runs backwards from the end of the string */

	if (*ptr == '\0')
		return;

	/* set end to point to last char in string and skip whitespaces backwards */
	end = ptr + strlen(ptr) - 1;
	while (end != ptr && (*end == ' ' || *end == '\t' || *end == '\n'))
		end--;

	/* end points to last char in string that is not white, or to the first char in totally white string.
	 * put '\0' next to it  */
	*(end+1) = '\0';

	/* cover the case of totally white string */
	if (*end == ' ' || *end == '\t' || *end == '\n')
		*end = '\0';
}


/* uses removeWhiteEnding to determine if the string is totally white. protects original string */
int isWhite(char *str) {
	/* move string to another place in memory, to protect original string */
	char line[MAX_LINE_LENGTH];
	strcpy(line, str);
	str = line;

	removeWhiteEnding(str);
	if (*str == '\0')
		return TRUE;
	return FALSE;
}

/* linear search in DIRECTIVE array */
short idxOfDirective(char *directiveName) {
	short i;

	for (i = 0; i < NUM_OF_DIRECTIVES; i++) {
		if (strcmp(directiveName, DIRECTIVES[i]) == 0)
			return i;
	}
	return NOT_FOUND;
}

/* linear search in REGS array */
short idxOfRegister(char *registerName) {
	short i;

	for (i = 0; i < NUM_OF_REGISTERS; i++) {
		if (strcmp(registerName, REGS[i]) == 0)
			return i;
	}
	return NOT_FOUND;
}