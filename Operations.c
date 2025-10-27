#include "Operations.h"

#include <stdlib.h>

#include "Errors.h"
#include "globals.h"
#include "Labels.h"
#include "utils.h"

/* global variables, documented in "Assembler.h" */
extern char *globalCurrentFile;
extern unsigned long globalCurrentLine;
extern unsigned long globalIC;


/* A constant static array of assembly language operation names, and the rules for their use.
 * including opcodes, functs, exact number of operands allowed, and addressing methods allowed for each operand. */
const OperationRules OP_RULES[NUM_OF_OPERATIONS] = {
{"mov", 0, NO_FUNCT, 2, {TRUE, TRUE, FALSE, TRUE}, {FALSE,TRUE,FALSE,TRUE}},
	{"cmp", 1, NO_FUNCT, 2, {TRUE, TRUE, FALSE, TRUE}, {TRUE, TRUE, FALSE, TRUE}},
	{"add", 2, 1, 2, {TRUE, TRUE, FALSE, TRUE}, {FALSE,TRUE,FALSE,TRUE}},
	{"sub", 2, 2, 2, {TRUE, TRUE, FALSE, TRUE}, {FALSE,TRUE,FALSE,TRUE}},
	{"lea", 4, NO_FUNCT, 2, {FALSE, TRUE, FALSE, FALSE}, {FALSE,TRUE,FALSE,TRUE}},
	{"clr", 5, 1, 1, {FALSE, FALSE, FALSE, FALSE}, {FALSE,TRUE,FALSE,TRUE}},
	{"not", 5, 2, 1, {FALSE, FALSE, FALSE, FALSE}, {FALSE,TRUE,FALSE,TRUE}},
	{"inc", 5, 3, 1, {FALSE, FALSE, FALSE, FALSE}, {FALSE,TRUE,FALSE,TRUE}},
	{"dec", 5, 4, 1, {FALSE, FALSE, FALSE, FALSE}, {FALSE,TRUE,FALSE,TRUE}},
	{"jmp", 9, 1, 1, {FALSE, FALSE, FALSE, FALSE}, {FALSE,TRUE,TRUE,FALSE}},
	{"bne", 9, 2, 1, {FALSE, FALSE, FALSE, FALSE}, {FALSE,TRUE,TRUE,FALSE}},
	{"jsr", 9, 3, 1, {FALSE, FALSE, FALSE, FALSE}, {FALSE,TRUE,TRUE,FALSE}},
	{"red", 12,NO_FUNCT,1,{FALSE, FALSE, FALSE, FALSE}, {FALSE,TRUE,FALSE,TRUE}},
	{"prn", 13, NO_FUNCT, 1, {FALSE, FALSE, FALSE, FALSE}, {TRUE,TRUE,FALSE,TRUE}},
	{"rts", 14, NO_FUNCT, 0, {FALSE, FALSE, FALSE, FALSE}, {FALSE, FALSE, FALSE, FALSE}},
{"stop", 15, NO_FUNCT, 0, {FALSE, FALSE, FALSE, FALSE}, {FALSE, FALSE, FALSE, FALSE}}
};


/* ------- Static Functions -------- */


/**
 * Updates a single operation in the second pass of the assembler:
 * Decodes operands that are labels,
 * and in the case of using an external label as an operand of an operation, updates both the createExt flag and the isExternLabel field of the operand.
 * @param operation the operation to be updated
 * @param updateArgument An argument that contains all the data needed to update the instruction.
 *						 This is a structure that contains the relevant parameters from the updateOperationTable function
 */
static void updateOperation(void *operation, void *updateArgument);


/**
 * Verifies for an operand that is a label that the label exists and is of a type that is suitable for use in the operand's addressing method.
 * Also updates the values of the value, is externLabel and ARE fields and if the operand is an external address, also updates createExt.
 * @param operation the operation. Required for a label in relative addressing, which requires the instruction address to calculate the operand value.
 * @param operand the operand to check and update.
 * @param labelTable the label table. needed to search the operand in the table by name.
 * @param error A pointer to a flag that holds the error status. If an error occurs during the function's runtime, it updates it accordingly.
 * @param createExt Pointer to a flag whether to create the "ext" file. If an external label is detected as an operand of an operation, it raises the flag.
 */
static void validateAndUpdateLabelOperand(Operation * operation, Operand *operand, Table *labelTable, int *error, int *createExt);


/**
 * sets and prints first word of operation to a file: address and content. using printAddress and PrintWordInHexa.
 * @param file Pointer to a file to print to
 * @param operation the operation to print its first word. operation details determine the machine-word.
 */
static void fprintFirstWord(FILE *file, Operation *operation);


/**
 * Helps the function fprintFirstWord set the values of the operand fields in the first word of an instruction according to its addressing method.
 * @param addressing pointer to the addressing method field of the given operand
 * @param regNum pointer to the register number field of the given operand
 * @param operand pointer to operand of the operation to print
 */
static void setFirstWordOperandDetails(short *addressing, short *regNum, Operand *operand);


/**
 * prints additional word of operation to a file: address and content. using printAddress and PrintWordInHexa
 * @param file Pointer to a file to print to
 * @param operand the operand to print its value and ARE.
 * @param ic the address of operand's machine word in the instruction image.
 */
static void fprintAdditionalWord(FILE *file, Operand *operand, unsigned long ic);


/**
 * Prints a single use of an external label in an operation operand.
 * Checks if the operand is an external label, and if so prints its name and address in the desired format
 * @param extFile the file to print to
 * @param operand the operand to check and print
 * @param ic the address of operand's machine word in the instruction image.
 */
static void fprintExtSingleUsage(FILE *extFile, Operand *operand, unsigned long ic);


/* ----- End of Static Functions ------ */


/* linear search in OP_RULES array */
short idxOfOperation(char *operationName) {
	short i;

	for (i = 0; i < NUM_OF_OPERATIONS; i++) {
		if (strcmp(operationName, OP_RULES[i].operationName) == 0)
			return i;
	}
	return NOT_FOUND;
}


/* allocate memory and check success */
Operation * createOperation(short operationIdx) {
	Operation *operation = malloc(sizeof(Operation));
	if (operation != NULL) {
		operation->idx = operationIdx;
		operation->srcOperand = malloc(sizeof(Operand));
		operation->destOperand = malloc(sizeof(Operand));
		operation->ic = globalIC;
		operation->size = 1;
		operation->lineNum = globalCurrentLine;

		/* check if all memory allocation succeeded */
		if (operation->srcOperand == NULL || operation->destOperand == NULL) {
			freeOperation((void **)&operation);
		}
		else {	/* all memory allocation succeeded */
			operation->srcOperand->type = SRC_OP;
			operation->srcOperand->isExternLabel = FALSE;
			operation->destOperand->type = DST_OP;
			operation->destOperand->isExternLabel = FALSE;
		}
	}

	return operation;
}

/* use addToTable funcion */
int addOperationToTable(Table *operationTable, Operation *operation) {
	if (addToTable(operationTable, (void *) operation) == SYSTEM_ERROR) {
		return SYSTEM_ERROR;
	}
	return NO_ERROR;
}


void setNotExistOperand(Operand *operand) {
	/* type of every operand is already set in create operation function */

	operand->addressing = OPERAND_NOT_EXIST;

	/* other fields are irrelevant in this case */
}


/* ----- Free Functions ----- */

/* use freeTable function with freeOperation as parameter */
void freeOperationTable(Table **operationTable) {
	freeTable(operationTable, freeOperation);
}

/* frees all allocated memory of a single operation */
void freeOperation(void **operation) {
	if (*operation != NULL) {
		Operation **tmp = (Operation **) operation;
		free((*tmp)->srcOperand);
		(*tmp)->srcOperand = NULL;
		free((*tmp)->destOperand);
		(*tmp)->destOperand = NULL;
		free(*tmp);
		*tmp = NULL;
	}
}


/* ----- Update Functions ----- */

/* This function uses the updateTable function and the updateOperation function.
 * To do this, it needs to pack all the variables needed for the update into one structure,
 * to be suitable for passing to the generic update table function.
 * The structure is also defined in the updateOperation function in exactly the same way,
 * where it will be decomposed back into variables.
 */
void updateOperationTable(Table *operationTable, Table *labelTable, int *error, int *createExt) {
	/* defining the struct */
	typedef struct argumentForUpdateTable {
		Table *labelTable;
		int *error;
		int *createExt;
	} argumentForUpdateTable;

	/* packing up all the variables needed for the update into the structure */
	argumentForUpdateTable *updateArgument;
	argumentForUpdateTable tmp;
	tmp.labelTable = labelTable;
	tmp.error = error;
	tmp.createExt = createExt;

	updateArgument = &tmp;

	/* call updateTable function */
	updateTable(operationTable, updateArgument, updateOperation);
}

static void updateOperation(void *operation, void *updateArgument) {
	/* defining the same struct as in updateOperationTable function */
	typedef struct argumentForUpdateTable {
		Table *labelTable;
		int *error;
		int *createExt;
	} argumentForUpdateTable;

	/* initialize local variables and decompose the struct variable back into simpler variables. */
	Operation *tmp = (Operation *) operation;
	argumentForUpdateTable *updateArg = (argumentForUpdateTable *) updateArgument;
	Table *labelTable = (Table *) updateArg->labelTable;
	int *error = (int *) updateArg->error;
	int *createExt = (int *) updateArg->createExt;


	/* handle source operand */
	/* for every other addressing method, nothing has to be done. */
	if (tmp->srcOperand->addressing == DIRECT_ADDRESSING || tmp->srcOperand->addressing == RELATIVE_ADDRESSING) {
		validateAndUpdateLabelOperand(tmp, tmp->srcOperand, labelTable, error, createExt);
	}

	/* handle dest operand */
	/* for every other addressing method, nothing has to be done. */
	if (tmp->destOperand->addressing == DIRECT_ADDRESSING || tmp->destOperand->addressing == RELATIVE_ADDRESSING) {
		validateAndUpdateLabelOperand(tmp, tmp->destOperand, labelTable, error, createExt);
	}
}



static void validateAndUpdateLabelOperand(Operation * operation, Operand *operand, Table *labelTable, int *error, int *createExt) {
	Label *label;

	/* search the label in label table. if not found it's input error. */
	label = findLabelInTable(labelTable, operand->labelName);
	if (label == NULL) {
		reportInputError(ERROR_LABEL_NOT_DEFINED, globalCurrentFile, operation->lineNum);
		*error = INPUT_ERROR;
		return;
	}

	if (validateLabelTypeMatchAdrsMthd(label, operand) == INPUT_ERROR) {
		reportInputError(ERROR_INVALID_LABEL_TYPE, globalCurrentFile, operation->lineNum);
		*error = INPUT_ERROR;
		return;
	}

	/* if there's use in extern label, raise the createExt flag and indicate that the operand uses extern label */
	if (isExtern(label) == TRUE) {
		*createExt = TRUE;
		operand->isExternLabel = TRUE;
	}

	/* update operand value and are fields according to addressing method */
	if (operand->addressing == DIRECT_ADDRESSING) {
		operand->value.unsignedVal = label->address;
		operand->are = (isExtern(label) == TRUE) ? E : R;

	}
	if (operand->addressing == RELATIVE_ADDRESSING)
		operand->value.signedVal = (signed long)label->address - (signed long)operation->ic;

}


/* ------ print functions ------ */


void fprintOperationTable(FILE *file, Table *operationTable) {
	fprintTable(file, operationTable, fprintOperation);
}


/* Prints the first word, and then, depending on the size of the operation, selects the printing order with the appropriate parameters */
void fprintOperation(FILE *file, void *operation) {
	Operation *tmp = (Operation *) operation;

	fprintFirstWord(file, tmp);

	switch (tmp->size) {
		/* For an operation that occupies one word in memory, nothing else needs to be printed. */
		case 1: {
			break;
		}

		/* For an operation that takes two words in memory, you need to check which operand takes an additional word.
		 * The rule is that an operand takes an additional memory word if it is given in an immediate, direct, or relative addressing mode.
		 * The following condition reflects this situation exactly */
		case 2: {
			if (tmp->srcOperand->addressing != REGISTER_ADDRESSING && tmp->srcOperand->addressing != OPERAND_NOT_EXIST)
				fprintAdditionalWord(file, tmp->srcOperand, tmp->ic + 1);
			else
				fprintAdditionalWord(file, tmp->destOperand, tmp->ic + 1);
			break;
		}

		/* For an operation that occupies three words in memory, both the source and destination operands occupy a machine word,
		 * so both are printed, the source first and then the destination. */
		case 3: {
			fprintAdditionalWord(file, tmp->srcOperand, tmp->ic + 1);
			fprintAdditionalWord(file, tmp->destOperand, tmp->ic + 2);
			break;
		}
		default:
			break;
	}
}


/* the word fields are set according to the operation index and the operands' details */
static void fprintFirstWord(FILE *file, Operation *operation) {
	/* the word to be printed */
	unsigned long machineWord;

	/* all following local variables represent the operation's first machine word fields.  */

	/* declare and set operation details */
	short opcode = OP_RULES[operation->idx].opcode;
	short funct = OP_RULES[operation->idx].funct;
	short are = A;

	/* declare operands details */
	short srcAddressing;
	short srcRegNum;
	short destAddressing;
	short destRegNum;

	/* set operands details */
	setFirstWordOperandDetails(&srcAddressing, &srcRegNum, operation->srcOperand);
	setFirstWordOperandDetails(&destAddressing, &destRegNum, operation->destOperand);

	/* set machine word */
	machineWord = (opcode<<OPCODE_SHIFT) | (srcAddressing<<SRC_ADDRESSING_SHIFT) | (srcRegNum<<SRC_REG_NUM_SHIFT) | (destAddressing<<DEST_ADDRESSING_SHIFT) | (destRegNum<<DEST_REG_NUM_SHIFT) | (funct<<FUNCT_SHIFT) | (are<<ARE_SHIFT);

	/* first, print address of current word (which is address of operation). then print space and operation's hexa representation */
	fprintAddress(file, operation->ic);
	fprintf(file, " ");
	fprintWordInHexa(file, machineWord);
	fprintf(file, "\n");
}


/* The values of some of the fields in the machine word depend on the existence of the operands, their addressing methods, and their register number */
static void setFirstWordOperandDetails(short *addressing, short *regNum, Operand *operand) {

	/* if operand exists, the addressing method of operand is already known.
	 * if operand doesn't exist, its addressing method bits are zero */
	*addressing = operand->addressing;
	if (*addressing == OPERAND_NOT_EXIST)
		*addressing = 0;

	/* in each addressing method except register addressing, register number bits are zero. */
	*regNum = 0;
	if (*addressing == REGISTER_ADDRESSING)
		*regNum = operand->regNum;
}


/* the fields of additional word are known easily from the operand fields after the second pass update to the operation table  */
static void fprintAdditionalWord(FILE *file, Operand *operand, unsigned long ic) {
	/* the word to be printed */
	unsigned long machineWord;

	/* set machine word by fields */
	/* all operation table details are fully updated, so we can just take information from there */

	if (operand->addressing == IMMEDIATE_ADDRESSING)
		machineWord = (operand->value.signedVal<<ADDITIONAL_WORD_VALUE_SHIFT) | (operand->are<<ARE_SHIFT);
	else
		machineWord = (operand->value.unsignedVal<<ADDITIONAL_WORD_VALUE_SHIFT) | (operand->are<<ARE_SHIFT);

	/* first, print address of current word (given as parameter). then print space and operation's hexa representation */
	fprintAddress(file, ic);
	fprintf(file, " ");
	fprintWordInHexa(file, machineWord);
	fprintf(file, "\n");
}


/* depending on the size of the operation, selects the printing order with the appropriate parameters */
void fprintExtUsagesInOperation(FILE *extFile, void *operation) {
	Operation *tmp = (Operation *) operation;

	switch (tmp->size) {
		case 1: {
			break;
		}
		case 2: {
			fprintExtSingleUsage(extFile, tmp->srcOperand, tmp->ic + 1);
			fprintExtSingleUsage(extFile, tmp->destOperand, tmp->ic + 1);
			break;
		}
		case 3: {
			fprintExtSingleUsage(extFile, tmp->srcOperand, tmp->ic + 1);
			fprintExtSingleUsage(extFile, tmp->destOperand, tmp->ic + 2);
			break;
		}
		default:
			break;
	}


}


/* Prints to a file a single use of an external label as an operand of an operation if the given operand is indeed an external label */
void fprintExtSingleUsage(FILE *extFile, Operand *operand, unsigned long ic) {
	if (operand->addressing == DIRECT_ADDRESSING) {
		if (operand->isExternLabel == TRUE) {
			fprintLabel(extFile, operand->labelName, ic);
		}
	}
}


