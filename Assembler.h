<<<<<<< HEAD
#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#include <stdio.h>

#include "Table.h"

/** The address from which the program will start to be written into memory. This is the starting address of globalIC */
#define IC_INIT_VALUE (100)
/** the starting address of globalDC.
 * After the operation image encoding is complete, we'll can update all the values used in globalDC by adding the value of globalIC
 */
#define DC_INIT_VALUE (0)


/**
 * Runs the three stages of the assembler on a single file, and monitors the error status.
 * @param fileName A string that represents the name of the file on which the assembler should be run
 * @return On success, NO_ERROR.
 * On an error related to the contents of the input file, INPUT_ERROR.
 * On an error related to system operation, SYSTEM_ERROR
 */
int assemble(char * fileName);


/**
 * Copy the source file (input) to an extended source file (preAssembler's output),
 * while deploying the macros and storing them in the macro table.
 * @param asFile Pointer to a NULL file pointer, for the input file.
 * The input file will be opened and read,
 * and the pointer to it will be updated with this parameter,
 * so that access to it will be preserved from the calling function
 * @param amFile Pointer to a NULL file pointer, for the preassembler output file - after macro deployment.
 * This file will be created and written to,
 * and the pointer to it will be updated with this parameter,
 * so that access to it will be preserved from the calling function
 * @param macroTable Pointer to a NULL table pointer, for the macro table.
 * The macro table will be created and updated with all the macros defined in the input file,
 * and the pointer to it will be updated with this parameter,
 * so that access to it will be preserved from the calling function.
 * @return On success, NO_ERROR.
 * On an error related to the contents of the input file, INPUT_ERROR.
 * On an error related to system operation, SYSTEM_ERROR
 */
int preAssembler(FILE **asFile, FILE **amFile, Table **macroTable);


/**
 * Create the label table, and encode the data and operations, except for entry directives and label operands,
 * since this information cannot be encoded before the label table is completed by scanning the entire file.
 * @param amFile Pointer to a file pointer to the preassembler output file - after macro deployment (which was opened and written in the preassembler).
 * This file will be read and processed.
 * @param macroTable A pointer to a table-type pointer to the macro table as created and updated by the preassembler.
 * Access to this table is required to prevent duplication of label name and macro name
 * @param labelTable Pointer to a NULL table pointer, for the label table.
 * The label table will be created and updated with all labels and their addresses defined in the input file,
 * except for definitions related to entry directives,
 * and the pointer to it will be updated with this parameter,
 * so that access to it will be preserved from the calling function
 * @param dataTable A pointer to a NULL table pointer, for the data table.
 * The data table will be created and updated with all data defined in the input file,
 * and the pointer to it will be updated with this parameter,
 * so that access to it will be preserved from the calling function
 * @param operationTable Pointer to a NULL table pointer, for the operations table.
 * The operation table will be created and updated with all operations defined in the input file,
 * except for label operands,
 * and the pointer to it will be updated with this parameter,
 * so that access to it will be preserved from the calling function
 * @return On success, NO_ERROR.
 * On an error related to the contents of the input file, INPUT_ERROR.
 * On an error related to system operation, SYSTEM_ERROR
 */
int firstPass(FILE **amFile, Table **macroTable, Table **labelTable, Table **dataTable, Table **operationTable);



/**
 * Scan extended source file (amFile) and update entry labels. scan operation table and update labels values.
 * Create and fill "ob", "ent" and "ext" output files.
 * @param amFile Pointer to a file-type pointer to the preassembler output file - after macros have been deployed
 * (which was opened and written in the preassembler). This file will be read and processed.
 * @param obFile A pointer to a file pointer to NULL, for the output file with the extension "ob".
 * This file will be created and written with the translation of the assembly program into machine language,
 * and the pointer to it will be updated with this parameter,
 * so that access to it will be preserved from the calling function
 * @param entFile A pointer to a file pointer to NULL, for the output file with the extension "ent".
 * This file will be created and written with labels declared as entry in the assembly program,
 * and the pointer to it will be updated with this parameter,
 * so that access to it will be preserved from the calling function
 * @param extFile A pointer to a file pointer to NULL, for the output file with the extension "ext".
 * This file will be created and written to it with the usages of labels declared as extern as operands of operations in the assembly program,
 * and the pointer to it will be updated with this parameter,
 * so that access to it will be preserved from the calling function
 * @param labelTable A pointer to a table-type pointer to the label table as created and updated by the first pass.
 * This table will be used to update entry definitions, print entry labels to the output file with the extension "ent",
 * and decode label operands.
 * @param dataTable A pointer to a table-type pointer to the data table as created and updated by the first pass.
 * This table will be used to print the data image to the output file with the extension "ob".
 * @param operationTable A pointer to a table-type pointer to the operations table as created and updated by the first pass.
 * This table will be used to update the label operand details,
 * print external label usages to the output file "ext",
 * and print the code image to the output file with the extension "ob"
 * @return On success, NO_ERROR.
 * On an error related to the contents of the input file, INPUT_ERROR.
 * On an error related to system operation, SYSTEM_ERROR
 */
int secondPass(FILE **amFile, FILE **obFile, FILE **entFile, FILE **extFile, Table **labelTable, Table **dataTable, Table **
               operationTable);


/**
 * Initializes the values of the global variables (globalCurrentFile, globalCurrentLine,	globalIC, globalDC)
 * for the start of processing the current file.
 * @param fileName The name of the current file being processed by the assembler
 */
void initGlobalVars(char *fileName);


=======
#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#include <stdio.h>

#include "Table.h"

/** The address from which the program will start to be written into memory. This is the starting address of globalIC */
#define IC_INIT_VALUE (100)
/** the starting address of globalDC.
 * After the operation image encoding is complete, we'll can update all the values used in globalDC by adding the value of globalIC
 */
#define DC_INIT_VALUE (0)


/**
 * Runs the three stages of the assembler on a single file, and monitors the error status.
 * @param fileName A string that represents the name of the file on which the assembler should be run
 * @return On success, NO_ERROR.
 * On an error related to the contents of the input file, INPUT_ERROR.
 * On an error related to system operation, SYSTEM_ERROR
 */
int assemble(char * fileName);


/**
 * Copy the source file (input) to an extended source file (preAssembler's output),
 * while deploying the macros and storing them in the macro table.
 * @param asFile Pointer to a NULL file pointer, for the input file.
 * The input file will be opened and read,
 * and the pointer to it will be updated with this parameter,
 * so that access to it will be preserved from the calling function
 * @param amFile Pointer to a NULL file pointer, for the preassembler output file - after macro deployment.
 * This file will be created and written to,
 * and the pointer to it will be updated with this parameter,
 * so that access to it will be preserved from the calling function
 * @param macroTable Pointer to a NULL table pointer, for the macro table.
 * The macro table will be created and updated with all the macros defined in the input file,
 * and the pointer to it will be updated with this parameter,
 * so that access to it will be preserved from the calling function.
 * @return On success, NO_ERROR.
 * On an error related to the contents of the input file, INPUT_ERROR.
 * On an error related to system operation, SYSTEM_ERROR
 */
int preAssembler(FILE **asFile, FILE **amFile, Table **macroTable);


/**
 * Create the label table, and encode the data and operations, except for entry directives and label operands,
 * since this information cannot be encoded before the label table is completed by scanning the entire file.
 * @param amFile Pointer to a file pointer to the preassembler output file - after macro deployment (which was opened and written in the preassembler).
 * This file will be read and processed.
 * @param macroTable A pointer to a table-type pointer to the macro table as created and updated by the preassembler.
 * Access to this table is required to prevent duplication of label name and macro name
 * @param labelTable Pointer to a NULL table pointer, for the label table.
 * The label table will be created and updated with all labels and their addresses defined in the input file,
 * except for definitions related to entry directives,
 * and the pointer to it will be updated with this parameter,
 * so that access to it will be preserved from the calling function
 * @param dataTable A pointer to a NULL table pointer, for the data table.
 * The data table will be created and updated with all data defined in the input file,
 * and the pointer to it will be updated with this parameter,
 * so that access to it will be preserved from the calling function
 * @param operationTable Pointer to a NULL table pointer, for the operations table.
 * The operation table will be created and updated with all operations defined in the input file,
 * except for label operands,
 * and the pointer to it will be updated with this parameter,
 * so that access to it will be preserved from the calling function
 * @return On success, NO_ERROR.
 * On an error related to the contents of the input file, INPUT_ERROR.
 * On an error related to system operation, SYSTEM_ERROR
 */
int firstPass(FILE **amFile, Table **macroTable, Table **labelTable, Table **dataTable, Table **operationTable);



/**
 * Scan extended source file (amFile) and update entry labels. scan operation table and update labels values.
 * Create and fill "ob", "ent" and "ext" output files.
 * @param amFile Pointer to a file-type pointer to the preassembler output file - after macros have been deployed
 * (which was opened and written in the preassembler). This file will be read and processed.
 * @param obFile A pointer to a file pointer to NULL, for the output file with the extension "ob".
 * This file will be created and written with the translation of the assembly program into machine language,
 * and the pointer to it will be updated with this parameter,
 * so that access to it will be preserved from the calling function
 * @param entFile A pointer to a file pointer to NULL, for the output file with the extension "ent".
 * This file will be created and written with labels declared as entry in the assembly program,
 * and the pointer to it will be updated with this parameter,
 * so that access to it will be preserved from the calling function
 * @param extFile A pointer to a file pointer to NULL, for the output file with the extension "ext".
 * This file will be created and written to it with the usages of labels declared as extern as operands of operations in the assembly program,
 * and the pointer to it will be updated with this parameter,
 * so that access to it will be preserved from the calling function
 * @param labelTable A pointer to a table-type pointer to the label table as created and updated by the first pass.
 * This table will be used to update entry definitions, print entry labels to the output file with the extension "ent",
 * and decode label operands.
 * @param dataTable A pointer to a table-type pointer to the data table as created and updated by the first pass.
 * This table will be used to print the data image to the output file with the extension "ob".
 * @param operationTable A pointer to a table-type pointer to the operations table as created and updated by the first pass.
 * This table will be used to update the label operand details,
 * print external label usages to the output file "ext",
 * and print the code image to the output file with the extension "ob"
 * @return On success, NO_ERROR.
 * On an error related to the contents of the input file, INPUT_ERROR.
 * On an error related to system operation, SYSTEM_ERROR
 */
int secondPass(FILE **amFile, FILE **obFile, FILE **entFile, FILE **extFile, Table **labelTable, Table **dataTable, Table **
               operationTable);


/**
 * Initializes the values of the global variables (globalCurrentFile, globalCurrentLine,	globalIC, globalDC)
 * for the start of processing the current file.
 * @param fileName The name of the current file being processed by the assembler
 */
void initGlobalVars(char *fileName);


>>>>>>> db1e6bedf286cc1a18e377ce301b3cfe5f147529
#endif