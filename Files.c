#include "Files.h"

#include <string.h>

#include "Errors.h"
#include "globals.h"

/* global variables, documented in "Assembler.h" */
extern char *globalCurrentFile;
extern unsigned long globalCurrentLine;


/* Checks whether all extensions given by the assembler can be added to the received file name */
int validateFileNameLength(char *fileName) {
	if (strlen(fileName) + MAX_SUFFIX_LENGTH >= MAX_FILE_NAME_LENGTH) {
		reportInputError(ERROR_TOO_LONG_FILE_NAME, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}
	return NO_ERROR;
}



FILE * openFile(const char *fileName, const char *suffix, const char *mode) {
	/* create new string with full file name to protect the original strings */
	char nameWithSuffix[MAX_FILE_NAME_LENGTH];
	strcpy(nameWithSuffix, fileName);
	strcat(nameWithSuffix, suffix);


	return fopen(nameWithSuffix, mode);
}



void removeFile(const char *fileName, const char *suffix) {
	/* create new string with full file name to protect the original strings */
	char nameWithSuffix[MAX_FILE_NAME_LENGTH];
	strcpy(nameWithSuffix, fileName);
	strcat(nameWithSuffix, suffix);

	remove(nameWithSuffix);
}


int readLineFromFile(FILE *fp, char *line) {
	if (fgets(line, MAX_LINE_LENGTH, fp) == NULL) {
		if (feof(fp)) {
			return EOF;
		}

		/* fgets returned NULL and the file hasn't reach EOF -> error */
		reportSystemError(ERROR_READ_LINE_FROM_FILE);
		return SYSTEM_ERROR;
	}

	return NO_ERROR;
}


/* The line is of the correct length if, after being read into a buffer of max length of a line,
 * a newline can be found in the buffer, or we have reached the end of the file and therefore there is no newline in the buffer. */
int validateLineLength(const char *line, FILE *file) {
	if (strrchr(line, '\n') == NULL) {
		if (feof(file) == 0) {
			reportInputError(TOO_LONG_LINE, globalCurrentFile, globalCurrentLine);
			return INPUT_ERROR;
		}
	}

	return NO_ERROR;
}




