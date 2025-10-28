#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <string.h>

#include "Files.h"
#include "Table.h"

/** This file implements functions that handle everything related to the operation table.
 * The file manage the operations in a table data structure (see "table.h")
 * In general, it's user's responsibility to pass a not NULL pointer to a table that contains only operations.
 */


/* ---------- MACROS ---------- */

/** The following macros define assembly language and cpu constants */
#define NUM_OF_REGISTERS (8)				/** the number of registers in the cpu */
#define NUM_OF_OPERATIONS (16)				/** the number of operations in the assembly language */
#define MAX_OPERATION_NAME (5)				/** the maximum length of operation name in the assembly language */
#define NUM_OF_ADDRESSING_METHODS (4)		/** the number of addressing methods in the assembly language */


/** The following macros represent all possible values for placement in the ARE field of a machine word */
#define A (1<<2)
#define R (1<<1)
#define E (1<<0)

/** The following macros refer to the machine word representation of an instruction,
 * and represent the position of the rightmost bit of each field in an instruction word,
 * Which represents the amount of shift that must be made to the field to place it in the appropriate place in the word.
 */
#define OPCODE_SHIFT (18)
#define SRC_ADDRESSING_SHIFT (16)
#define SRC_REG_NUM_SHIFT (13)
#define DEST_ADDRESSING_SHIFT (11)
#define DEST_REG_NUM_SHIFT (8)
#define FUNCT_SHIFT (3)
#define ARE_SHIFT (0)
#define ADDITIONAL_WORD_VALUE_SHIFT (3)

/**
 * A constant value assigned to the funct field
 * in a machine word that represents an operation for which no funct value exists.
 * In such a case, the bits of the funct field should be set to zero
 */
#define NO_FUNCT (0)


/* ---------- STRUCTURES AND ENUMS ---------- */

/* --- GENERAL --- */
/**
 * Represents the set of rules for an assembly language operation
 */
typedef struct OperationRules {
	char *operationName;		/** the name of the operation */
	short opcode;				/** the opcode of the operation */
	short funct;				/** the funct of the operation */
	short numOfOperands;		/** the number of operands that the operation receives. Varies from 0 to 2 */
	/**
	 * The allowed addressing methods for the source operand in the operation.
	 * Represented as an array where at each entry the index corresponds to the number of a different addressing method,
	 * and the value (true or false) represents whether the addressing method is allowed for this operand or not
	 */
	short srcAllowedAddressing[NUM_OF_ADDRESSING_METHODS];

	/**
	 * The allowed addressing methods for the destination operand in the operation.
	 * Represented as an array where at each entry the index corresponds to the number of a different addressing method,
	 * and the value (true or false) represents whether the addressing method is allowed for this operand or not
	 */
	short destAllowedAddressing[NUM_OF_ADDRESSING_METHODS];
} OperationRules;


/* --- OPERAND --- */
/**
 * represent the different addressing methods in which an operand can be given
 */
typedef enum AddressingMethod {
	IMMEDIATE_ADDRESSING = 0,		/** for signed decimal integer */
	DIRECT_ADDRESSING,				/** for label (represent address in memory) */
	RELATIVE_ADDRESSING,			/** for label (Represents the address of an instruction in the current source file) */
	REGISTER_ADDRESSING,			/** for register name */

	/** Another definition to indicate that the operand is not present in the instruction.
	 * Used for operations that accept one or zero operands */
	OPERAND_NOT_EXIST
} Addressing;


/** Represents the type of operand: source operand or destination operand */
typedef enum OperandType {
	SRC_OP,
	DST_OP
} OperandType;


/**
 * represents a single operand (source or destination) in an instruction.
 * Not all fields are used for every operand. The fields used depend on the addressing method.
 * Some fields may be filled only in the second pass.
 */
typedef struct Operand {
	OperandType type;				/** source or destination operand */
	Addressing addressing;			/** immediate, direct, relative, register, or not exist */
	short regNum;					/** Register number, used if the operand uses register addressing */
	/**
	 * Operand's value is immediate, if the operand uses immediate addressing
	 * or a numeric value that represents an absolute address in memory or jump distance that will be filled in on the second pass,
	 * for operands given in direct and relative addressing methods.
	 */
	union {
		long signedVal;						/* uses for immediate and relative addressing operands */
		unsigned long unsignedVal;			/* uses for direct addressing operands */
	} value;

	/**
	 * A label name, for operands given in direct and relative addressing methods.
	 * Filled in on the first pass and decoded on the second pass.
	 */
	char labelName[MAX_LINE_LENGTH];

	/**
	 * Flag indicating whether the operand is an external label.
	 * Set up as FALSE when creating the operand,
	 * updated to TRUE for relevant operands when updating label values in the instruction table
	 * and intended for use during printing to the output file with the extension ".ext".
	 */
	int isExternLabel;

	/**
	 * ARE field for operands that require an additional information word in the instruction image (not registers).
	 * For immediate and relative addressing, it is set to A when the operand is created.
	 * For direct addressing, it is set on the second pass, when the operands that are labels are decoded, depending on the label type.
	 */
	short are;
} Operand;


/* --- OPERATION --- */

/**
 * represents a single operation in the instruction image of the program.
 */
typedef struct Operation {
	/**
	 * The index of the operation in the operation rules table.
	 * Used to retrieve information about the operation */
	short idx;

	Operand *srcOperand;		/** source operand of the operation. if not exists, it's addressing method set as NOT_EXIST_OPERAND */
	Operand *destOperand;		/** destination operand of the operation. if not exists, it's addressing method set as NOT_EXIST_OPERAND */
	unsigned long ic;			/** The machine word address where the representation of the instruction in memory begins */
	short size;					/** The size of the instruction representation in machine words. Depends on the addressing methods of the operands */
	/**
	 *  The line number in the extended source (.am) file from which the instruction was read.
	 *  Used to print errors when decoding operands given as labels
	 */
	unsigned long lineNum;
} Operation;








/* ---------- FUNCTIONS ---------- */
/**
 * finds the index of the operation name in the operation rules table.
 * @param operationName the operation name to search for
 * @return index of the operation name in the operation rules table, if found. otherwise, NOT_FOUND.
 */
short idxOfOperation(char *operationName);


/**
 * Allocates memory for a new operation and it's operands.
 * initializes all information that we can know about the operation only from its index.
 * for operands, only allocate memory and sets their type and isExternLabel (FALSE) fields.
 * @param operationIdx the index of the operation in the operation rules table.
 * @return on success, the new operation's pointer. on failure of memory allocation, NULL.
 */
Operation *createOperation(short operationIdx);


/**
 * Adds an operation to the operation table.
 * @param operationTable the operation table.
 * @param operation the operation to be added to the operation table
 * @return on success, NO_ERROR. on failure of memory allocation, SYSTEM_ERROR.
 */
int addOperationToTable(Table *operationTable, Operation *operation);


/**
 *	Sets the operand addressing method for not exist operand.
 * @param operand the operand to set its addressing method.
 */
void setNotExistOperand(Operand *operand);


/**
 * frees all memory allocated for the operation table.
 * @param operationTable the operation table
 */
void freeOperationTable(Table **operationTable);


/**
 * releases the memory allocated for single operation.
 * @param operation pointer to the operation.
 */
void freeOperation(void **operation);


/* ----- Second Pass Update Functions ----- */

/* next 3 functions are strongly related */

/**
 * Updates the operation table in the second pass:
 * Decodes operands that are labels, and updates on uses of external labels as an operand of an operation
 * @param operationTable the operation table
 * @param labelTable the label table. needed to search labels in the table by name.
 * @param error A pointer to a flag that holds the error status. If an error occurs during the function's runtime, it updates it accordingly.
 * @param createExt Pointer to a flag whether to create the "ext" file. If an external label is detected as an operand of an operation, it raises the flag.
 */
void updateOperationTable(Table *operationTable, Table * labelTable, int *error, int *createExt);



/* ----- Second Pass Print Functions ----- */


/**
 * prints to a file all the operations from a given operation table. uses to print to the ".ob" output file.
 * @param file the file to print to
 * @param operationTable the operation table to print
 */
void fprintOperationTable(FILE *file, Table *operationTable);


/**
 * prints a single operation to a file in the desired format.
 * @param file the file to print to
 * @param operation Pointer to the operation to be printed
 */
void fprintOperation(FILE *file, void *operation);



/**
 * Prints to the ".ext" file, the uses of external labels as operands of single operation
 * @param extFile pointer to the ext file to print to
 * @param operation	the operation to check and print its operands
 */
void fprintExtUsagesInOperation(FILE *extFile, void *operation);


#endif
