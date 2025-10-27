#include <stdio.h>
#include <stdlib.h>

#include "Assembler.h"
#include "Errors.h"
#include "globals.h"

/**
* This project implements a three-pass assembler for a hypothetical assembly language.
The assembler translates an assembly language program into a machine language program according to the language's rules.
Input: Any number of files written in assembly language.
Output:
1. An expanded source file with the extension "am" after macro deployment, if no error was detected during the macro deployment phase.
2. A file translated into machine language with the extension "ob", if no error was detected during the assembler operation.
3. A file listing all labels declared as "entry" for use in external files, and their addresses, with the extension "ent",
if such labels exist.
4. A file containing a list of all uses found in the file of external labels as operands of an operation,
with the addresses that represent these uses, with the extension "ext", if such uses exist.

In the first pass, called the pre-assembler, the assembler interprets macros written in the program.
In the next pass, called the first pass, the assembler analyzes the program statements,
fills in a table containing the names of the labels and their details,
and encodes the commands and directives except for operands given by labels.
In the last pass, called the second pass, the assembler completes the details that were not possible to encode in the first pass:
operands of operations given by labels, as well as "entry" directives that accept an operand that is a label,
and whose existence and value can only be known after building the label table in the first pass.
Finally, in the second pass, the relevant output files are created and filled in.

Errors: The assembler detects and reports in detail input and system errors.
The execution of each stage in the assembler depends on the error-free success of the stages preceding it.
 */






/**
 * Entry point of the assembler program.
 *
 * Accepts file names via command line arguments (without extensions),
 * and runs the assembler on each file.
 *
 * return 0 on success, or non-zero if errors occurred
 */
int main(int argc, char *argv[]) {

	int i = 0; 	/* counter of input files */

	/* check that there is at least one file name in the input */
	if (argc < 2) {
		reportSystemError(ERROR_NO_INPUT);
		exit(EXIT_FAILURE);
	}

	for (i = 1; i < argc; i++) {
		/* on system error, stop the program */
		if (assemble(argv[i]) == SYSTEM_ERROR)
			exit(EXIT_FAILURE);

		/* else, nothing has to be done, just continue to assemble next file */
	}

	return 0;
}