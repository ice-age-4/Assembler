#ifndef LABELS_H
#define LABELS_H

#include "Table.h"

/** This file implements functions that handle everything related to label table.
 * The functions manage the labels in a table data structure (see "table.h")
 * In general, it's user's responsibility to pass a not NULL pointer to label table that contains only labels.
 */


/** The maximum name length allowed for a label name in assembly language */
#define MAX_LABEL_NAME 31

/** The value entered for the address of an external label. The final address will be updated at the link stage */
#define EXTERN_LABEL_ADDRESS 0


/** A label can be declared in one of three ways:
 * 1. in an operation sentence
 * 2. in a directive sentence of ".data" or ".string" (store data in memory)
 * 3. in a directive sentence of ".extern" (declaration of an external label)
 * this enum represents those 3 ways of declaration.
 */
typedef enum LabelType {
	LABEL_TYPE_CODE,
	LABEL_TYPE_DATA,
	LABEL_TYPE_EXTERN
} LabelType;


/** Label Linkage status can be one of three:
 * Local label - defined and intended for use in the current file
 * Entry Label - defined in the current file and intended for use in external files as well
 * External label - defined in an external file and intended for use in this file as well
 * this enum represents those 3 possible situations of linkage
*/
typedef enum LabelLinkage {
	LABEL_LINKAGE_LOCAL,
	LABEL_LINKAGE_ENTRY,
	LABEL_LINKAGE_EXTERN
} LabelLinkage;


/** represents a single label
 */
typedef struct Label {
	char name[MAX_LABEL_NAME];		/** the name of the label */
	/**
	 * The address in memory that the label represents.
	 * When set, the address is determined according to the context in which it is set (instruction counter, data counter, or zero for an external label).
	 * The final address will be updated after the first pass.
	 */
	unsigned long address;
	/**
	 * label type: LABEL_TYPE_CODE, LABEL_TYPE_DATA, or LABEL_TYPE_EXTERN.
	 * Determined at definition time according to the context in which the label was defined and does not change.
	 */
	LabelType type;
	/**
	 * the label linkage: LABEL_LINKAGE_LOCAL, LABEL_LINKAGE_ENTRY, or LABEL_LINKAGE_EXTERN
	 * Determined at definition time as external or local according to the context in which the label was defined.
	 * Local labels may change to entry labels on second pass.
	 */
	LabelLinkage linkage;
} Label;




/* -------- FUNCTIONS -------- */


/** Allocates memory for a new label and adds it to the label table.
 * Doesn't check the validity of the label definition.
 *
 * @param labelTable the label table
 * @param labelName the label name
 * @param address The address of the label when it is defined,
 *				  depending on the context of its definition (instruction counter, data counter, or zero for an external label).
 *				  The final address will be updated later
 * @param type label type (LABEL_TYPE_CODE, LABEL_TYPE_DATA, LABEL_TYPE_EXTERN)
 * @param linkage label linkage (LABEL_LINKAGE_LOCAL, LABEL_LINKAGE_ENTRY, LABEL_LINKAGE_EXTERN)
 * all label details has to be previously validated.
 * @return on success, the new macro's pointer. on failure of memory allocation, NULL.
 */
Label * addLabelToTable(Table *labelTable, char *labelName, unsigned long address, LabelType type, LabelLinkage linkage);


/** Allocates memory for a new external label and adds it to the label table.
 * Doesn't check the validity of the label definition.
 * @param labelTable the label table
 * @param labelName validated label name
 * other label details set with fixed values for extern label.
 * @return on success, the new macro's pointer. on failure of memory allocation, NULL.
 */
Label * addExternLabelToTable(Table *labelTable, char *labelName);



/**
 * finds a label by name in a given label table
 * @param labelTable pointer to the label table
 * @param name the name to search in the table
 * @return pointer to the label if found. Otherwise, NULL.
 */
Label * findLabelInTable(Table *labelTable, char *name);



/**
 * checks if a label defined as extern or not
 * @param label pointer to a label
 * @return TRUE if the label defined as extern, FALSE otherwise
 */
int isExtern(Label *label);


/**
 * checks if a label defined as data type or not
 * @param label pointer to a label
 * @return TRUE if the label is defined as data type, FALSE otherwise
 */
int isData(Label *label);



/**
 * updates the linkage definition of the given label to be entry,
 * if it was not previously extern.
 * In case the label's linkage was defined as extern, an error will be reported.
 * @param label pointer to a label
 * @return NO_ERROR on success, INPUT_ERROR on failure
 */
int updateEntry(Label *label);



/**
 * adds icf to the address of each label defined as data type in a given label table.
 * @param labelTable pointer to the label table
 * @param icf The final value of globalIC, which reflects the last address in memory occupied by the instruction image
 */
void updateDataLabelsAddress(Table * labelTable, unsigned long icf);



/**
 * frees all memory allocated for a given label table.
 * @param labelTable pointer to the label table
 */
void freeLabelTable(Table **labelTable);



/**
 * compares a label name with a name given as a string.
 * @param label a label to compare its name
 * @param name a string to compare
 * @return TRUE on match, FALSE on mismatch
 */
int compareLabel(void * label, void * name);



/**
 * prints a label to a file, if it is defined as entry
 * @param file the file to print to
 * @param label the label to print
 */
void fprintEntryLabel(FILE *file, void *label);



/**
 * adds icf to the address of a specific label, if it's defined as data type
 * @param label pointer to a label
 * @param icf The final value of globalIC, which reflects the last address in memory occupied by the instruction image
 */
void addIcfToDataLabel(void *label, void *icf);


/**
 * releases the memory allocated for single label
 * @param label pointer to a label
 */
void freeLabel(void **label);


#endif
