#include "Assembler.h"

#include "Errors.h"
#include "Files.h"
#include "globals.h"
#include "Labels.h"
#include "LineParsing.h"
#include "Macros.h"
#include "Table.h"
#include "utils.h"


/* global variables */
char *globalCurrentFile;			/** global string for name of current file processed by the assembler */
unsigned long globalCurrentLine;	/** global counter for number of current line processed by the assembler */
unsigned long globalIC;				/** global counter for the machine lines required for the instruction image */
unsigned long globalDC;				/** global counter for the machine lines required for the data image */


int assemble(char * fileName) {
	/* Pointers to all the files and all the tables that the assembler processes.
	 * These pointers are declared here to allow joint work on them between the subfunctions that the assembler executes,
	 * by passing by reference to the subfunctions */
	FILE *asFile =NULL, *amFile = NULL, * obFile = NULL, *entFile = NULL, *extFile = NULL;
	Table *macroTable = NULL, *labelTable = NULL, *dataTable = NULL, *operationTable = NULL;

	int status;			/* Holds error status during this function */

	/* initialize global variables for assembler work on current file. */
	initGlobalVars(fileName);

	/* validate the limit on maximum file length */
	if (validateFileNameLength(fileName) == INPUT_ERROR) {
		return INPUT_ERROR;		/* file name is too long */
	}


	/* ---------- PreAssembler ---------- */

	/* Run the preAssembler with access to all the tables and files that it processes */
	status = preAssembler(&asFile, &amFile, &macroTable);
	if (status != NO_ERROR) {
		/* if preAssembler returned error, need to remove file with suffix ".am" */
		removeFile(fileName, ".am");
		freeAllMemory(&macroTable, &labelTable, &dataTable, &operationTable);
		if (closeAllFiles(asFile, amFile, obFile, entFile, extFile) == SYSTEM_ERROR)
			return SYSTEM_ERROR;
		return status;
	}

	/* pre assembler finished successfully. -> continue to first pass */




	/* ---------- First Pass ---------- */

	/* Run the first pass with access to all the tables and files that it processes */
	status = firstPass(&amFile, &macroTable, &labelTable, &dataTable, &operationTable);
	if (status != NO_ERROR) {
		freeAllMemory(&macroTable, &labelTable, &dataTable, &operationTable);
		if (closeAllFiles(asFile, amFile, obFile, entFile, extFile) == SYSTEM_ERROR)
			return SYSTEM_ERROR;
		return status;
	}

	/* first pass finished successfully. -> free macro table and continue to second pass */
	freeMacroTable(&macroTable);


	/* ---------- Second Pass ---------- */

	/* Run the second pass with access to all the tables and files that it processes */
	status = secondPass(&amFile, &obFile, &entFile, &extFile, &labelTable, &dataTable, &operationTable);
	if (status != NO_ERROR) {
		/* if secondPass returned error, need to remove all secondPass output files. */
		removeFile(fileName, ".ob");
		removeFile(fileName, ".ent");
		removeFile(fileName, ".ext");
		freeAllMemory(&macroTable, &labelTable, &dataTable, &operationTable);
		if (closeAllFiles(asFile, amFile, obFile, entFile, extFile) == SYSTEM_ERROR)
			return SYSTEM_ERROR;
		return status;
	}


	/* assembler work on current file has done successfully! free all memory allocated, close all files and return */
	freeAllMemory(&macroTable, &labelTable, &dataTable, &operationTable);
	if (closeAllFiles(asFile, amFile, obFile, entFile, extFile) == SYSTEM_ERROR)
		return SYSTEM_ERROR;
	return NO_ERROR;
}



int preAssembler(FILE **asFile, FILE **amFile, Table **macroTable) {
	/* A buffer with a maximum line size to which the lines read from the asFile (input file) are saved. */
	char line[MAX_LINE_LENGTH] = "\0";

	/* A pointer to the last macro that was added/used.
	 * The parseLinePreAssembler function is the only one that updates and uses this value.
	 * To allow it to "remember" the last macro that was added/used, the variable is declared here,
	 * and passed to it by reference. The rest of the explanation is within the function */
	Macro *lastMacro = NULL;

	/* A flag that indicates whether the next line is inside a macro definition or not.
	 * The parseLinePreAssembler function is the only one that updates and uses this value.
	 * To allow it to "remember" the state of the flag, the variable is declared here,
	 * and passed to it by reference. The rest of the explanation inside the function */
	int readMacro = FALSE;

	int error = NO_ERROR;				/* Holds error status during this function */
	int status;							/* Holds the error status of a function called from within this function, for handling various error conditions */


	/* if file doesn't open - assembler can't work -> quit */
	if ((*asFile = openFile(globalCurrentFile, ".as", "r")) == NULL) {
		reportInputError(ERROR_OPEN_FILE, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;		/* file not exist in path */
	}

	/* if file doesn't open - assembler can't work. -> quit. */
	if ((*amFile = openFile(globalCurrentFile, ".am", "w+")) == NULL) {
		reportSystemError(ERROR_CREATE_FILE);
		return SYSTEM_ERROR;		/* failed to create file for writing */
	}

	/* try to create a table for macros. */
	if ((*macroTable = createTable()) == NULL) {
		reportSystemError(ERROR_MEMORY_ALLOCATION);
		return SYSTEM_ERROR;	/* failed to allocate memory for macro table */
	}


	/*  --- Scan input ".as" file line by line for macros --- */

	while (1) {
		globalCurrentLine++;

		status = readLineFromFile(*asFile, line);
		if (status == SYSTEM_ERROR)
			return SYSTEM_ERROR;
		if (status == EOF) {
			/* reached end of file -> preAssembler finished. return the error flag */
			break;
		}

		/* else, line read successfully */
		if (validateLineLength(line, *asFile) == INPUT_ERROR) {
			/* line is too long -> update that was error and move to next line */
			error = INPUT_ERROR;
			while (fgetc(*asFile) != '\n') {}

			/* don't parse a too long line */
			continue;
		}

		/* line length is ok. let's parse it in macros context */
		status = parseLinePreAssembler(line, *amFile, *macroTable, &lastMacro, &readMacro);
		if (status == SYSTEM_ERROR) {
			return SYSTEM_ERROR;
		}
		/* else, on NO_ERROR or INPUT_ERROR, we just continue to next line, and update if was any error */
		if (status == INPUT_ERROR) {
			error = INPUT_ERROR;
		}


	}	/* finished preAssembler pass on asFile */

	/* check if an error occurred while printing to the file.
	 * The file was created in this program and was only written to, so a general error check indicates write errors. */
	if (ferror(*amFile)) {
		reportSystemError(ERROR_WRITE_TO_FILE);
		return SYSTEM_ERROR;
	}


	/* preAssembler finished. return the error status */
	return error;

}



int firstPass(FILE **amFile, Table **macroTable, Table **labelTable, Table **dataTable, Table **operationTable) {
	/* A buffer with a maximum line size to which the lines read from the amFile (extended source file) are saved. */
	char line[MAX_LINE_LENGTH];

	int status;						/* Holds the error status of a function called from within this function, for handling various error conditions */
	int error = NO_ERROR;			/* Holds error status during this function */


	/* create all required tables for first pass */
	/* try to create a table for labels. */
	if ((*labelTable = createTable()) == NULL) {
		reportSystemError(ERROR_MEMORY_ALLOCATION);
		return SYSTEM_ERROR;	/* failed to allocate memory for label table */
	}
	/* try to create a table for data. */
	if ((*dataTable = createTable()) == NULL) {
		reportSystemError(ERROR_MEMORY_ALLOCATION);
		return SYSTEM_ERROR;	/* failed to allocate memory for data table */
	}
	/* try to create a table for operations. */
	if ((*operationTable = createTable()) == NULL) {
		reportSystemError(ERROR_MEMORY_ALLOCATION);
		return SYSTEM_ERROR;	/* failed to allocate memory for operations table */
	}

	/* start reading the amFile from beginning */
	if (fseek(*amFile, 0, SEEK_SET) != 0) {
		reportSystemError(ERROR_USING_FILE);
		return SYSTEM_ERROR;
	}
	globalCurrentLine = 0;


	/* --- Scan and parse extended amFile line by line for operations, data and labels --- */
	while (1) {
		globalCurrentLine++;

		/* read line */
		status = readLineFromFile(*amFile, line);
		if (status == SYSTEM_ERROR)
			return SYSTEM_ERROR;
		if (status == EOF) {
			/* reached end of file -> first pass finished. return the error flag */
			break;
		}

		/* no need to check line length here, because all lines' length checked in preAssembler stage,
		 * And this stage only started if there were no errors in the preAssembler step.  */


		/* line read successfully. let's parse it */
		status = parseLineFirstPass(line, *macroTable, *labelTable, *dataTable, *operationTable);
		if (status == SYSTEM_ERROR) {
			return SYSTEM_ERROR;
		}
		/* else, on NO_ERROR or INPUT_ERROR, we just continue on, and update if was any error */
		if (status == INPUT_ERROR) {
			error = INPUT_ERROR;
		}

		/* After analyzing a line, we will check whether the space occupied by the program so far in the computer's memory still meets the memory limitations. */
		if (validateMemoryLimit() == SYSTEM_ERROR) {
			reportSystemError(ERROR_REACHED_MEMORY_LIMIT);
			return SYSTEM_ERROR;
		}

	}	/* finished first pass on amFile */

	/* when final globalIC value of current file is known, it's time to update the data labels' addresses.
	 * notice: addresses of data units in the data table will be updated when printed*/
	updateDataLabelsAddress(*labelTable, globalIC);


	/* first pass finished! return the error status */
	return error;
}


int secondPass(FILE **amFile, FILE **obFile, FILE **entFile, FILE **extFile, Table **labelTable, Table **dataTable, Table **
               operationTable) {
	/* A buffer with a maximum line size to which the lines read from the amFile (extended source file) are saved. */
	char line[MAX_LINE_LENGTH];

	/* A flag that indicates whether it is necessary to create an "ent" file Or not.
	 * The flag is passed to a subfunction that updates it in case a valid entry declaration is detected.
	 * To allow the subfunction to update the flag state for this function as well, the variable is declared here, and passed to it by reference. */
	int createEnt = FALSE;

	/* A flag that indicates whether it is necessary to create an "ext" file Or not.
	 * The flag is passed to a subfunction that updates it in case an external label is detected as an operand of an operation.
	 * To allow the subfunction to update the flag state for this function as well, the variable is declared here, and passed to it by reference. */
	int createExt = FALSE;

	int status;						/* Holds the error status of a function called from within this function, for handling various error conditions */
	int error = NO_ERROR;			/* Holds error status during this function */



	/* ----- update operation table ----- */

	/* This is the time to update the operation table with the details of all operands that are specified using labels,
	 * and in the process, check whether an external label is used as an operand of an instruction. */
	updateOperationTable(*operationTable, *labelTable, &error, &createExt);


	/* ----- update entry declarations in label table ----- */

	/* we have all labels information updated from first pass, except entry declarations. */

	/* start reading amFile from beginning */
	if (fseek(*amFile, 0, SEEK_SET) != 0) {
		reportSystemError(ERROR_USING_FILE);
		return SYSTEM_ERROR;
	}
	globalCurrentLine = 0;

	/*  --- Scan extended amFile line by line for entry declarations --- */
	while (1) {
		globalCurrentLine++;

		/* read line */
		status = readLineFromFile(*amFile, line);
		if (status == SYSTEM_ERROR)
			return SYSTEM_ERROR;
		if (status == EOF) {
			/* reached end of file -> second pass on file finished. */
			break;
		}

		/* else, line read successfully */

		/* Since the second pass is only executed if the preAssembler and first pass passed without errors,
		 * it's not necessary to check again the line length */

		/* line length is ok. let's parse it for the second pass (search for entry declaration) */
		if (handleEntrySecondPass(line, *labelTable, &createEnt) == INPUT_ERROR) {
			/* update that an error has occured */
			error = INPUT_ERROR;
		}

		/* continue to next line */
	}	/* finished second pass on amFile */



	/* ----- output ----- */

	/* if there are no errors, create output files and fill them */

	if (error != NO_ERROR) {
		return error;
	}

	/* create all output files */

	/* if file doesn't open - assembler can't work. -> quit. */
	if ((*obFile = openFile(globalCurrentFile, ".ob", "w+")) == NULL) {
		reportSystemError(ERROR_CREATE_FILE);
		return SYSTEM_ERROR;		/* failed to create file for writing */
	}

	if (createEnt == TRUE) {
		/* if file doesn't open - assembler can't work. -> quit. */
		if ((*entFile = openFile(globalCurrentFile, ".ent", "w+")) == NULL) {
			reportSystemError(ERROR_CREATE_FILE);
			return SYSTEM_ERROR;		/* failed to create file for writing */
		}
	}

	if (createExt == TRUE) {
		/* if file doesn't open - assembler can't work. -> quit. */
		if ((*extFile = openFile(globalCurrentFile, ".ext", "w+")) == NULL) {
			reportSystemError(ERROR_CREATE_FILE);
			return SYSTEM_ERROR;		/* failed to create file for writing */
		}
	}

	/* fill output files */
	if (fprintObFile(*obFile, *operationTable, *dataTable) == SYSTEM_ERROR) {
		reportSystemError(ERROR_WRITE_TO_FILE);
		return SYSTEM_ERROR;
	}

	if (*entFile) {
		if (fprintEntFile(*entFile, *labelTable) == SYSTEM_ERROR) {
			reportSystemError(ERROR_WRITE_TO_FILE);
			return SYSTEM_ERROR;
		}
	}

	if (*extFile) {
		if (fprintExtFile(*extFile, *operationTable) == SYSTEM_ERROR) {
			reportSystemError(ERROR_WRITE_TO_FILE);
			return SYSTEM_ERROR;
		}
	}

	/* second pass finished successfully! */
	return NO_ERROR;
}


/* When starting to process a file, you must update the name of the current file being processed by the assembler,
 * the line number in the file should be initialized to 0, and globalIC and globalDC to their initial values */
void initGlobalVars(char *fileName) {
	globalCurrentFile = fileName;
	globalCurrentLine = 0;
	globalIC = IC_INIT_VALUE;
	globalDC = DC_INIT_VALUE;
}


