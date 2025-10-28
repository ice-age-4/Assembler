#include "LineParsing.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "Data.h"
#include "Errors.h"
#include "Files.h"
#include "globals.h"
#include "Labels.h"
#include "Macros.h"
#include "Operations.h"
#include "utils.h"

/* global variables, documented in "Assembler.h" */
extern char *globalCurrentFile;
extern unsigned long globalCurrentLine;
extern unsigned long globalIC, globalDC;
extern const OperationRules OP_RULES[];



/* ---------- Static Functions declarations ---------- */


/**
 * Locates the line type in context of macros, and moves the pointer to the line, to point to the character following the word that determines the line type, if such a word exists
 * @param line The line whose type must be determined, by reference
 * @param macroTable pointer to the macro table
 * @param lastMacro Pointer to a pointer to lastMacro, for updating in case a use in macro is found
 * @return the type of the line, according to enum of MacroLineType
 */
static int getLineType(char **line, Table *macroTable, Macro **lastMacro);



/**
 * Checks whether the line is an empty line or a comment line, or is a line of another type
 * @param line the current line that the preAssembler parses
 * @return if empty or comment returns the corresponding line type (enum). LINE_OTHER otherwise.
 */
static MacroLineType commentOrEmptyLine(char *line);



/**
 * extracts optional label name to labelName parameter
 * and moves line pointer to point on first non-white char after optional label definition
 * without checking validation of definition.
 * @param line pointer to pointer to a string that represents the current sentence in the extended source file (amFile)
 * which is not empty or comment sentence.
 * the pointer to the line will be moved to point on first non-white char after removing (optional) label name.
 * @param labelName editable buffer in which the label name will be saved, if exist label definition in the sentence
 * @return TRUE if there's a label definition, FALSE if there's no label definition,
 * INPUT_ERROR on invalid input: no characters before colon, or no white char after colon, or only white chars after label definition.
 */
static int extractOptionalLabelName(char **line, char *labelName);



/**
 * checks what is the type of given sentence.
 * @param line pointer to pointer to a not empty sentence of assembly language, without label definition in beginning of the sentence.
 * the pointer to the line will be moved to point on first char after first word, except of operation or not recognized sentence.
 * @return the type of the sentence (see enum SentenceType)
 */
static SentenceType getSentenceType(char **line);



/**
 * Gets a sentence that represents operation, parses the operation different parts, creates and fills an operation struct,
 * adds it to the operation table, and promote globalIC according to the operation size.
 * @param line string that represents operation (known that there is a legal operation name in the beginning, followed by whitespace or '\0')
 * @param operationTable A table-type pointer to the operation table, to update a new operation.
 * @return On success, NO_ERROR.
 * On an error related to the contents of the input file, INPUT_ERROR.
 * On an error related to system operation, SYSTEM_ERROR
 */
static int parseOperation(char *line, Table *operationTable);



/**
 * Receives string expression that supposed to contain operands that are associated with a particular operation.
 * Depending on the number of operands the operation expects to receive, performs an appropriate parsing process on the operands
 * @param line a string that supposed to represent operands of a particular operation.
 * @param operation The operation to which the operands are associated
 * @return On success, NO_ERROR.
 * On an error related to the contents of the input file, INPUT_ERROR.
 */
static int parseOperands(char *line, Operation *operation);



/**
 * Receives a string expression that supposed to contain single operand, which is associated with a particular operation.
 * If the expression represents a valid operand, the function will fill in all possible relevant fields of the operand and the operation.
 * in this step, the operands fields of value, labelName, and ARE are not final.
 * The function also checks that the addressing method of the operand matches the operation, given the type of the operand and the assembly language rules.
 * @param expression a string that supposed to represent existing legal operand.
 * @param operation The operation to which the operand is associated
 * @param operand Pointer to a structure of an operand into which the details of the operand should be encoded
 * @return On success, NO_ERROR.
 * On an error related to the contents of the input file, INPUT_ERROR.
 */
static int parseSingleOperand(char *expression, Operation *operation, Operand *operand);



/**
 * Receives a pointer to a pointer to a string which contains a non-empty expression representing an operand, without white starting and ending.
 * locates the addressing method and removes the character that determines the addressing method from the string,
 * in addressing methods where such a character exists
 * @param expression a pointer to a pointer to a string which contains a non-empty expression representing an operand, without white starting and ending.
 * @return the addressing method (enum Addressing in "operations.h")
 */
static Addressing extractAddressingMethod(char **expression);



/**
 * reads and validates immediate operand definition and if valid stores it's value in the value parameter.
 * @param expression a string that represents a number, after '#'.
 * @param value pointer to a variable where to store the number read.
 * @return On success, NO_ERROR.
 * On an error related to the contents of the input file, INPUT_ERROR.
 */
static int getImmValue(char *expression, long *value);



/**
 * validates external label name and adds it to the label table.
 * @param line string that represents an extern label definition (the rest of the sentence, after ".extern" directive.)
 * @param labelTable A table-type pointer to the label table,
 * to update a new label, and prevent defining a new label with existing name of label
 * @param macroTable A table-type pointer to the macro table,
 * to prevent defining a label with existing name of a macro
 * @return On success, NO_ERROR.
 * On an error related to the contents of the input file, INPUT_ERROR.
 * On an error related to system operation, SYSTEM_ERROR
 */
static int handleExtLabelDef(char *line ,Table *labelTable, Table *macroTable);



/* ---------- End of Static Functions declarations ---------- */






/* ---------- FUNCTIONS ---------- */

/* ----- preAssembler ----- */


/* Processes a single line from the input file asFile:
 * Finds the line type, and based on the line type and the state of the macro read flag, decides what to do */
int parseLinePreAssembler(char *line, FILE *amFile, Table *macroTable, Macro **lastMacro, int *readMacro) {

	/* NOTICE: about lastMacro parameter:
	 * When adding a macro, lastMacro is updated to point to it, so that when the following lines,
	 * which constitute the content of the macro, are processed,
	 * the pointer to the macro will be easily accessible and will not disappear with each exit from the function to read a new line.

	 * Another situation in which lastMacro is updated is when a macro name is used, for printing purposes.

	 * The states are not contradictory, since it can be assumed that the input does not use a macro within another macro.
	 * Therefore, if a macro is detected, it is safe to override lastMacro, because we are necessarily not inside a macro definition. */

	int lineType;				/* an indicator for current line type in context of macros (see enum MacroLineType) */

	/* A buffer with a maximum size of a line to which the macro name in the macro definition line will be saved.
	 * The buffer size ensures that the macro name will fit properly into the buffer, because only lines of the correct size are analyzed.
	 * The macro name's length will be checked by a function that checks the macro definition's validity. */
	char macroName[MAX_LINE_LENGTH];

	/* first, get the line type (empty, comment, macro use, macro definition, macro end, or regular line) */
	lineType = getLineType(&line, macroTable, lastMacro);

	/* now lineType holds the line type and line points to next character after the first word, except of regular line.
	 * in case of using macro, the macro is saved into the last macro local variable.
	 * in case of macro definition or end, the readMacro flag is raised or unraised */
	/* in each case, the code and function called to handle the rest of the sentence, does it for the whole sentence string,
	 * meaning there's no additional work after the switch statement */
	switch (lineType) {
		case LINE_EMPTY:
		case LINE_COMMENT: {
			break;
		}
		case LINE_MACRO_USE: {
			/* check that the rest of the line is white */
			if (isWhite(line) == FALSE) {
				reportInputError(ERROR_EXTRA_TEXT_AFTER_MACRO_CALL, globalCurrentFile, globalCurrentLine);
				return INPUT_ERROR;
			}

			/* no errors, print the macro content */
			fprintMacroContent(amFile, *lastMacro);
			break;
		}

		case LINE_MACRO_DEF: {
			/* get macro name */
			skipWhite(&line);
			sscanf(line, "%s", macroName);

			if (validateMacroDef(macroName, macroTable) == INPUT_ERROR) {
				return INPUT_ERROR;
			}

			/* check that the rest of the line is white */
			line += strlen(macroName);
			if (isWhite(line) == FALSE) {
				reportInputError(ERROR_EXTRA_TEXT_AFTER_MACRO_DEF, globalCurrentFile, globalCurrentLine);
				return INPUT_ERROR;
			}

			*lastMacro = addMacroToTable(macroTable, macroName);
			if (*lastMacro == NULL) {
				reportSystemError(ERROR_MEMORY_ALLOCATION);
				return SYSTEM_ERROR;
			}

			*readMacro = TRUE;
			break;
		}

		case LINE_MACRO_END: {
			if (isWhite(line) == FALSE) {
				reportInputError(ERROR_EXTRA_TEXT_AFTER_MACRO_END, globalCurrentFile, globalCurrentLine);
				return INPUT_ERROR;
			}
				*readMacro = FALSE;
			break;
		}
		case LINE_REGULAR: {
			if (*readMacro == TRUE) {
				if (addLineToMacro(*lastMacro, line) == SYSTEM_ERROR) {
					reportSystemError(ERROR_MEMORY_ALLOCATION);
					return SYSTEM_ERROR;
				}
			}
			else {
				fputs(line, amFile);
				if (line == '\0' || line[strlen(line) - 1] != '\n') {
					fputc('\n', amFile);
				}
			}
			break;
		}
		default:
			break;
	}


	/* if we've got here, parsing current line finished successfully */
	return NO_ERROR;
}


static int getLineType(char **line, Table *macroTable, Macro **lastMacro) {
	int type;										/* holds the return value of commentOrEmptyLine to return it if the line is comment or empty */
	char firstWord[MAX_LINE_LENGTH] = "\0";			/* the size ensures that the first word in the line will fit properly into the buffer */

	/* Used to save the result of a macro search.
	 * If found, we will also update the pointer to lastMacro, so that the found macro will be accessible outside the function for printing its content.
	 * But in the case that it is not found, we do not want to overwrite the current value of lastMacro,
	 * because it may contain a pointer to a macro that is also being built in this line, and access to it is still necessary. */
	Macro *tmp = NULL;


	/* first, if the line is comment or empty line, do nothing and just return the type */
	type = commentOrEmptyLine(*line);
	if (type != LINE_OTHER)
		return type;

	/* the line is not empty or comment -> check the type according to the first word */
	/* clean starting white chars. line is not empty, so at least one non-white char stays */
	skipWhite(line);
	
	sscanf(*line, "%s", firstWord);

	/* check if there's a usage in a defined macro */
	tmp = findMacroInTable(macroTable, firstWord);
	if (tmp != NULL) {
		/* there's a usage in defined macro. */
		*lastMacro = tmp;
		*line += strlen(firstWord);
		return LINE_MACRO_USE;
	}

	if (strcmp(firstWord, "mcro") == 0) {
		*line += strlen(firstWord);
		return LINE_MACRO_DEF;
	}

	if (strcmp(firstWord, "mcroend") == 0) {
		*line += strlen(firstWord);
		return LINE_MACRO_END;
	}

	/* if we've got here, it's just a regular line */
	return LINE_REGULAR;
}


static MacroLineType commentOrEmptyLine(char *line) {
	/* comment line starts with ';', with no whitespace before it. */
	if (*line == ';') {
		return LINE_COMMENT;
	}

	/* remove white starting and endig */
	skipWhite(&line);
	removeWhiteEnding(line);
	if (*line == '\0') {
		return LINE_EMPTY;
	}

	return LINE_OTHER;
}


/* ----- First Pass ----- */


/* Processes a single line from the extended source file, the amFile:
 * Finds the sentence type, and based on it decides what to do. */
int parseLineFirstPass(char *line, Table *macroTable, Table *labelTable, Table *dataTable, Table *operationTable) {
	int status;							/* Holds the error status of a function called from within this function, for handling various error conditions */
	int labelDefFound = FALSE;			/* A flag indicating whether a label definition was detected at the beginning of the current line */
	SentenceType type;					/* for the type of the sentence (string, data, entry, extern, operation, error) */

	/* A buffer with a maximum size of a line to which the label name will be saved.
	 * The buffer size ensures that the label name will fit properly into the buffer, because only lines of the correct size are analyzed.
	 * The label name's length will be checked by a function that checks the label definition's validity. */
	char labelName[MAX_LINE_LENGTH] = "\0";

	/* notice: there are no empty and comment sentences in the amFile, they weren't copied in preAssembler stage */

	/* handle optional label */
	status = extractOptionalLabelName(&line, labelName);

	/* if status == INPUT_ERROR -> syntax error around ':' in line. stop and return. */
	if (status == INPUT_ERROR)
		return INPUT_ERROR;

	/* if status == FALSE -> no label in this line -> do nothing. */

	if (status == TRUE) {
		/* label definition found. if the definition is valid, raise the flag */
		if (validateLabelDef(labelName, labelTable, macroTable) == INPUT_ERROR) {
			return INPUT_ERROR;
		}
		labelDefFound = TRUE;
	}
	/* end of handling optional label. now line points to first non-white char in line,
	 * and if was label definition, it's name is legal, labelName holds it and labelDefFound == TRUE */


	type = getSentenceType(&line);
	/* now type holds the sentence type and line points to next character after the first word,
	 * except of operation or not recognized sentence, where line points to first non-white char in sentence
	 * (in operation it's the first char of the operation name) */

	/* in each case, the function called to handle the rest of the sentence, does it for the whole sentence string,
	 * meaning there's no additional work after the switch statement */
	switch  (type) {
		case SENTENCE_NOT_RECOGNIZED: {
			return INPUT_ERROR;
		}
		case SENTENCE_DATA_DIRECTIVE: {

			if (labelDefFound == TRUE) {
				/* add label with globalDC and other details, and check if succeeded */
				if (addLabelToTable(labelTable, labelName, globalDC, LABEL_TYPE_DATA, LABEL_LINKAGE_LOCAL) == NULL) {
					reportSystemError(ERROR_MEMORY_ALLOCATION);
					return SYSTEM_ERROR;
				}
			}
			/* insert numbers to dataTable and check for errors. */
			status = addNumbersToTable(dataTable, line);
			if (status != NO_ERROR)
				return status;
			break;
		}
		case SENTENCE_STRING_DIRECTIVE: {
			if (labelDefFound == TRUE) {
				/* add label with globalDC and other details, and check if succeeded */
				if (addLabelToTable(labelTable, labelName, globalDC, LABEL_TYPE_DATA, LABEL_LINKAGE_LOCAL) == NULL) {
					reportSystemError(ERROR_MEMORY_ALLOCATION);
					return SYSTEM_ERROR;
				}
			}
			/* insert string to dataTable and check for errors. */
			status = addStringToTable(dataTable, line);
			if (status != NO_ERROR)
				return status;
			break;
		}
		case SENTENCE_ENTRY_DIRECTIVE: {
			/* will be handled in second pass */
			break;
		}
		case SENTENCE_EXTERN_DIRECTIVE: {
			/* do nothing with label definition in this line (before extern directive) */

			/* handle extern operand in the following function */
			status = handleExtLabelDef(line, labelTable, macroTable);
			if (status != NO_ERROR)
				return status;
			break;
		}

		case SENTENCE_OPERATION: {
			if (labelDefFound == TRUE) {
				/* add label with globalIC and other details, and check if succeeded */
				if (addLabelToTable(labelTable, labelName, globalIC, LABEL_TYPE_CODE, LABEL_LINKAGE_LOCAL) == NULL) {
					reportSystemError(ERROR_MEMORY_ALLOCATION);
					return SYSTEM_ERROR;
				}
			}

			skipWhite(&line);
			/* now line points on the first char of operation name. let's parse it. */
			status = parseOperation(line, operationTable);
			if (status != NO_ERROR)
				return status;
			break;
		}
		default:
			break;
	}	/* end of switch */

	return NO_ERROR;
}


/* extracts label name if exists in begin of sentence, by searching colon in the sentence */
static int extractOptionalLabelName(char **line, char *labelName) {
	char *colon;			/* A pointer that will receive the address of the first colon in the string, if any */

	/* move line pointer to first non-white char in the sentence */
	skipWhite(line);
	/* search for first colon in sentence */
	colon = strchr(*line, ':');

	/* no colon in line -> no label definition in line. */
	if (colon == NULL) {
		return FALSE;
	}

	/* there's a label definition, but no label name */
	if (colon == *line) {
		reportInputError(ERROR_EXPECTED_LABEL_NAME_BEFORE_COLON, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}

	/* found ':'. expecting white char after it */
	if ( (*(colon + 1) != ' ') && (*(colon + 1) != '\t') ) {
		reportInputError(ERROR_EXPECTED_WHITE_CHAR_AFTER_COLON, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}

	/* extract label name */
	strncpy(labelName, *line, colon-(*line));
	labelName[colon-(*line)] = '\0';
	*line = colon+1;


	/* move line to next non-white char. */
	skipWhite(line);

	/* check that label definition is not alone in line */
	if (**line == '\n') {
		reportInputError(ERROR_ONLY_LABEL_DEF, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}

	return TRUE;
}


static SentenceType getSentenceType(char **line) {
	/* A buffer with a maximum size of a line to which the first word of the sentence will be saved.
	 * The buffer size ensures that the first word will fit properly into the buffer, because only lines of the correct size are analyzed. */
	char firstWord[MAX_LINE_LENGTH];

	/* The type of sentence is determined by the first word in the sentence. */
	skipWhite(line);
	sscanf(*line, "%s", firstWord);

	if (strcmp(firstWord, ".data") == 0) {
		*line += strlen(firstWord);
		return SENTENCE_DATA_DIRECTIVE;
	}

	if (strcmp(firstWord, ".string") == 0) {
		*line += strlen(firstWord);
		return SENTENCE_STRING_DIRECTIVE;
	}

	if (strcmp(firstWord, ".entry") == 0) {
		*line += strlen(firstWord);
		return SENTENCE_ENTRY_DIRECTIVE;
	}

	if (strcmp(firstWord, ".extern") == 0) {
		*line += strlen(firstWord);
		return SENTENCE_EXTERN_DIRECTIVE;
	}

	if (idxOfOperation(firstWord) != NOT_FOUND) {
		return SENTENCE_OPERATION;
	}

	reportInputError(ERROR_UNRECOGNIZED_OPERATION, globalCurrentFile, globalCurrentLine);
	return SENTENCE_NOT_RECOGNIZED;
}


static int parseOperation(char *line, Table *operationTable) {

	Operation *operation;			/* stores the pointer of the operation that wiil be created within this function */
	short operationIdx;				/* index of the sentence's operation in the OP_RULES array */

	/* A buffer with a maximum size of an operation name to which the name of the operation (first word of the sentence) will be saved.
	 * The buffer size ensures that the first word will fit properly into the buffer, because only lines of the correct size are analyzed. */
	char operationName[MAX_OPERATION_NAME] = "\0";

	/* When entering the function, it is known that the expression begins with a valid operation name. */
	skipWhite(&line);
	sscanf(line, "%s", operationName);
	operationIdx = idxOfOperation(operationName);

	/* create a new operation and check if succeeded */
	if ((operation = createOperation(operationIdx)) == NULL) {
		reportSystemError(ERROR_MEMORY_ALLOCATION);
		return SYSTEM_ERROR;
	}

	/* move line to point to the first char after operation name. */
	line += strlen(operationName);

	/* the rest of the line is the operands part. let's parse them */
	if (parseOperands(line, operation) == INPUT_ERROR) {
		freeOperation((void **)&operation);
		return INPUT_ERROR;
	}

	/* if we've got here -> the operation was created and filled successfully. try to save it in operation table */
	if (addOperationToTable(operationTable, operation) == SYSTEM_ERROR) {
		reportSystemError(ERROR_MEMORY_ALLOCATION);
		freeOperation((void **)&operation);
		return SYSTEM_ERROR;
	}

	/* after parsing and adding an operation, globalIC must be promoted */
	globalIC += operation->size;

	return NO_ERROR;
}


/* In the assembly language, each operation has a fixed number of operands that it should receive:
 * 0 - no operands.
 * 1 - a single operand, which is the destination operand.
 * 2 - two operands: first a source operand, then a destination operand.
 * Therefore, the number of operands that an operation is supposed to receive determines the processing required on its operands, as implemented by the function */
static int parseOperands(char *line, Operation *operation) {
	char *comma;			/* A pointer that will receive the address of the first comma in the line string, if any */

	/* the expected number of operand matches to the operation given as parameter */
	int expectedNumOfOperands = OP_RULES[operation->idx].numOfOperands;

	switch (expectedNumOfOperands) {
		case 0: {
			/* check that the operands string is empty */
			skipWhite(&line);
			removeWhiteEnding(line);
			if (*line != '\0') {
				reportInputError(ERROR_REQUIRED_NO_OPERANDS, globalCurrentFile, globalCurrentLine);
				return INPUT_ERROR;
			}
			setNotExistOperand(operation->srcOperand);
			setNotExistOperand(operation->destOperand);
			break;
		}
		case 1: {
			/* in operation with 1 operand, the relevant operand is always the dest operand. the src operand doesn't exist. */
			/* parse the single operand into destination operand */
			if (parseSingleOperand(line, operation, operation->destOperand) == INPUT_ERROR)
				return INPUT_ERROR;

			/* src operand doesn't exist */
			setNotExistOperand(operation->srcOperand);
			break;
		}
		case 2: {
			/* separate into 2 strings. each contains 1 operand */
			comma = strchr(line, ',');	/* if not found -> error: expected ',' between 2 operands */
			if (comma == NULL) {
				reportInputError(ERROR_EXPECTED_COMMA_BETWEEN_OPERANDS, globalCurrentFile, globalCurrentLine);
				return INPUT_ERROR;
			}
			*comma = '\0';
			comma++;

			/* parse the first operand into source operand */
			if (parseSingleOperand(line, operation, operation->srcOperand) == INPUT_ERROR)
				return INPUT_ERROR;

			/* parse the second operand into destination operand */
			if (parseSingleOperand(comma, operation, operation->destOperand) == INPUT_ERROR)
				return INPUT_ERROR;

			break;
		}

		default:
			break;

	} /* end of switch */

	/* parsing operand finished successfully */
	return NO_ERROR;
}


/* Main issues to be analyzed: addressing method, value, are and influence on operation size */
static int parseSingleOperand(char *expression, Operation *operation, Operand *operand) {

	/* check if expression is empty or white */
	skipWhite(&expression);
	removeWhiteEnding(expression);
	if (*expression == '\0') {
		reportInputError(ERROR_MISSING_OPERAND, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}

	/* get operand addressing method and move expression to point on first char that is relevant for operand value  */
	operand->addressing = extractAddressingMethod(&expression);

	/* validate that the operand addressing method matches the operation rules. */
	switch (operand->type) {
		case SRC_OP: {
			if (OP_RULES[operation->idx].srcAllowedAddressing[operand->addressing] == FALSE) {
				reportInputError(ERROR_INVALID_SRC_ADRS_MTHD, globalCurrentFile, globalCurrentLine);
				return INPUT_ERROR;
			}
			break;
		}
		case DST_OP: {
			if (OP_RULES[operation->idx].destAllowedAddressing[operand->addressing] == FALSE) {
				reportInputError(ERROR_INVALID_DST_ADRS_MTHD, globalCurrentFile, globalCurrentLine);
				return INPUT_ERROR;
			}
			break;
		}
	} /* end of switch */


	/* fill all possible relevant fields of operand */
	switch (operand->addressing) {

		/* all we need to know about an operand in immediate addressing is its value and its are field.
		 * we can know that right now.
		 * Also, an operand in immediate addressing occupies an additional word of memory. */
		case IMMEDIATE_ADDRESSING: {
			if (getImmValue(expression, &(operand->value.signedVal)) == INPUT_ERROR)
				return INPUT_ERROR;
			operand->are = A;
			(operation->size)++;
			break;
		}

		/* all we need to know about an operand in direct addressing is its name and its are field.
		 * we can know its name right now.
		 * the ARE field depends on the label type, and we will leave this to be filled in the second pass.
		 * Also, an operand in direct addressing occupies an additional word of memory. */
		case DIRECT_ADDRESSING: {
			strcpy(operand->labelName, expression);
			(operation->size)++;
			break;
		}

		/* all we need to know about an operand in relative addressing is its name and its are field.
		 * we can know that right now.
		 * in the second pass we will be able to validate thar the label type matches the addressing method.
		 * Also, an operand in direct addressing occupies an additional word of memory. */
		case RELATIVE_ADDRESSING: {
			strcpy(operand->labelName, expression);
			operand->are = A;
			(operation->size)++;
			break;
		}

		/* all we need to know about an operand in register addressing is its register number.
		 * we can know that right now. */
		case REGISTER_ADDRESSING: {
			operand->regNum = idxOfRegister(expression);
			break;
		}

		default:
			break;

	}	/* end of switch */

	/* parsing single operand finished successfully */
	return NO_ERROR;
}


static Addressing extractAddressingMethod(char **expression) {

	if (idxOfRegister(*expression) != NOT_FOUND)
		return REGISTER_ADDRESSING;
	if (**expression == '#') {
		(*expression)++;
		return IMMEDIATE_ADDRESSING;
	}
	if (**expression == '&') {
		(*expression)++;
		return RELATIVE_ADDRESSING;
	}
	return DIRECT_ADDRESSING;
}


static int getImmValue(char *expression, long *value) {
	char *end;
	/* check that the number starts on first char (because expression points to first char after '#') */
	if (*expression != '+' && *expression != '-' && !isdigit(*expression)) {
		reportInputError(ERROR_MISSING_NUMBER_OPERAND, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}

	/* get number */
	*value = strtol(expression, &end, 10);

	/* check if succeeded to get number */
	if (end == expression) {
		reportInputError(ERROR_INVALID_NUMBER, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}

	/* validate range of number */
	if (validateImmNumRange(*value) != NO_ERROR) {
		reportInputError(ERROR_NUMBER_OUT_OF_RANGE_IMM, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}

	return NO_ERROR;
}


static int handleExtLabelDef(char *line ,Table *labelTable, Table *macroTable) {

	/* prepare for validating label definition */
	skipWhite(&line);
	removeWhiteEnding(line);

	/* now line has only the label name, without white starting and ending */
	if (*line == '\0') {
		reportInputError(ERROR_EXPECTED_LABEL_NAME_AFTER_EXTERN, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}

	/* validate label definition */
	if (validateLabelDef(line, labelTable, macroTable) == INPUT_ERROR)
		return INPUT_ERROR;

	/* validation complete. -> add label to table and check success */
	if (addExternLabelToTable(labelTable, line) == NULL) {
		reportSystemError(ERROR_MEMORY_ALLOCATION);
		return SYSTEM_ERROR;
	}
	return NO_ERROR;
}



/* ----- Second Pass ----- */


int handleEntrySecondPass(char *line, Table *labelTable, int *createEnt) {
	/* Important distinction: Since the second pass is only executed if the first pass passed without errors,
	 * and the first pass parsed lines containing the ".entry" directive up to and including the directive word,
	 * it can be determined that if a ".entry" directive is present in a line, it is in a valid position.
	 * (if in asFile was a comment line including substring ".entry", it wasn't copied to amFile by preAssembler,
	 * so there's no need to handle this case)
	 * In addition, it is followed by some whitespace or end of string, otherwise the getSentenceType function,
	 * which relies on the first word in the sentence, would return an unrecognized sentence code.
	 * Therefore, it can be searched for directly, and then the rest of the sentence can be parsed.
	 */

	int status;					/* Holds the error status of a function called from within this function, for handling various error conditions */
	Label *label;				/* Auxiliary pointer to a label */

	/* look for ".entry" in the line */
	line = strstr(line, ".entry");
	/* If there is no entry prompt on this line, there is nothing to check and everything is OK. */
	if (line == NULL)
		return NO_ERROR;

	/* move line to point on next char after ".entry" directive.  */
	line += strlen(".entry");

	/* rest of the line is supposed to be a defined label name. find it and update its linkage */
	skipWhite(&line);
	removeWhiteEnding(line);

	/* first check if operand of entry exist in line */
	if (*line == '\0') {
		reportInputError(ERROR_EMPTY_DIRECTIVE, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}

	label = findLabelInTable(labelTable, line);
	if (label == NULL) {
		reportInputError(ERROR_LABEL_NOT_DEFINED, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}

	/* updateEntry(label) returns NO_ERROR on success, or INPUT_ERROR on failure (trying to update extern label as entry). */
	status = updateEntry(label);
	if (status == NO_ERROR) {
		*createEnt = TRUE;
	}
	return status;
}