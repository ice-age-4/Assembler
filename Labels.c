#include "Labels.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "Errors.h"
#include "globals.h"
#include "Macros.h"
#include "utils.h"

/* global variables, documented in "Assembler.h" */
extern char *globalCurrentFile;
extern unsigned long globalCurrentLine;


/**
 * creates a new label with all label details.
 * @param name new label's name, meaning the symbol tht represents the label's address
 * @param address new label's address
 * @param type new label's type (data, code, extern)
 * @param linkage new label's linkage (local, entry, extern)
 * @return On success, returns the pointer to the label. On failure, returns NULL
 */
static Label * createLabel(char *name, unsigned long address, LabelType type, LabelLinkage linkage);




Label * addLabelToTable(Table *labelTable, char *labelName, unsigned long address, LabelType type, LabelLinkage linkage) {
	Label *label = createLabel(labelName, address, type, linkage);

	/* check if memory allocation for the new label succeeded */
	if (label == NULL) {
		return NULL;
	}
	/* memory allocation of label succeeded -> add the new label to the label table */
	if (addToTable(labelTable, (void *) label) == SYSTEM_ERROR) {
		free(label);
		return NULL;
	}
	return label;
}

Label * addExternLabelToTable(Table *labelTable, char *labelName) {
	return addLabelToTable(labelTable, labelName, EXTERN_LABEL_ADDRESS, LABEL_TYPE_EXTERN, LABEL_LINKAGE_EXTERN);
}

Label * findLabelInTable(Table *labelTable, char *name) {
	return (Label *)(findInTable(labelTable, (void *)name, compareLabel));
}

int isExtern(Label *label) {
	return label->type == LABEL_TYPE_EXTERN ? TRUE : FALSE;
}


int isData(Label *label) {
	return label->type == LABEL_TYPE_DATA ? TRUE : FALSE;
}

int updateEntry(Label *label) {
	if (label->linkage == LABEL_LINKAGE_EXTERN) {
		reportInputError(ERROR_UPDATE_ENTRY_FOR_EXTERN_LABEL, globalCurrentFile, globalCurrentLine);
		return INPUT_ERROR;
	}
	label->linkage = LABEL_LINKAGE_ENTRY;
	return NO_ERROR;
}

void updateDataLabelsAddress(Table *labelTable, unsigned long icf) {
	updateTable(labelTable, (void *)&icf, addIcfToDataLabel);
}

void freeLabelTable(Table **labelTable) {
	freeTable(labelTable, freeLabel);
}

int compareLabel(void *label, void *name) {
	Label * tmp = (Label *)label;
	if (strcmp(tmp->name, name) == 0)
		return TRUE;
	return FALSE;
}

void fprintEntryLabel(FILE *file, void *label) {
	Label * tmp = (Label *)label;
	if (tmp->linkage == LABEL_LINKAGE_ENTRY) {
		fprintLabel(file, tmp->name, tmp->address);
	}
}


void addIcfToDataLabel(void *label, void *pIcf) {
	Label *tmp = (Label *)label;
	unsigned long *icf = (unsigned long *)pIcf;
	if (tmp->type == LABEL_TYPE_DATA) {
		tmp->address += *icf;
	}
}

void freeLabel(void **label) {
	Label **tmp = (Label **)label;
	if (*tmp != NULL) {
		free(*tmp);
		*tmp = NULL;
	}
}


/* static function. prototype and documentation above in this file */
static Label * createLabel(char *name, unsigned long address, LabelType type, LabelLinkage linkage) {
	Label *label = malloc(sizeof(Label));
	if (label != NULL) {
		strcpy(label->name, name);
		label->address = address;
		label->type = type;
		label->linkage = linkage;
	}

	return label;
}
	