#include "Macros.h"

#include <stdlib.h>
#include <string.h>
#include "globals.h"

/* global variables, documented in "Assembler.h" */
extern char *globalCurrentFile;
extern unsigned long globalCurrentLine;


/**
 * creates a new macro with name and without content.
 * @param name the name to give the new macro
 * @return On success, returns the pointer to the Macro.
 * On failure of memory allocation, returns NULL.
 */
static Macro * createMacro(char *name);


Macro * addMacroToTable(Table *macroTable, char *macroName) {
	Macro *macro = createMacro(macroName);

	/* check if memory allocation for the new macro succeeded */
	if (macro == NULL) {
		return NULL;
	}
	/* memory allocation of macro succeeded -> add the new macro to the macro table */
	if (addToTable(macroTable, (void *) macro) == SYSTEM_ERROR) {
		free(macro);
		return NULL;
	}
	return macro;
}

int addLineToMacro(Macro *macro, char *line) {
	char *copy = malloc(strlen(line) + 1);
	if (copy == NULL)
		return SYSTEM_ERROR;

	strcpy(copy, line);
	if (addToTable(macro->content, (void *) copy) == SYSTEM_ERROR) {
		free(copy);
		return SYSTEM_ERROR;
	}

	return NO_ERROR;
}

Macro * findMacroInTable(Table *macroTable, char *name) {
	return findInTable(macroTable, (void *) name, compareMacro);
}


/* Macro content is saved as a table,
 * with each new record added to the end of the table,
 * and printing a table is done in order from the first record that was added most recently,
 * so you can print the macro content using the Print Table function */
void fprintMacroContent(FILE *file, Macro *macro) {
	fprintTable(file, macro->content, fprintLine);
}



void freeMacroTable(Table **macroTable) {
	freeTable(macroTable, freeMacro);
}

int compareMacro(void *macro, void *name) {
	Macro * tmp = (Macro *)macro;
	if (strcmp(tmp->name, name) == 0)
		return TRUE;
	return FALSE;
}

void fprintLine(FILE *file, void *line) {
	char *l = (char *) line;
	fputs(l, file);
	if (l == '\0' || l[strlen(l) - 1] != '\n') {
		fputc('\n', file);
	}
}

void freeMacro(void **macro) {
	Macro **tmp = (Macro **) macro;
	if (*tmp != NULL) {
		if ((*tmp)->content != NULL)
			freeTable(&((*tmp)->content), freeLine);
		free(*tmp);
		*tmp = NULL;
	}
}

void freeLine(void **line) {
	char **tmp = (char **) line;
	if (*tmp != NULL) {
		free(*tmp);
		*tmp = NULL;
	}
}

/* static function. prototype and documentation above in this file */
static Macro * createMacro(char *name) {
	Macro *macro = malloc(sizeof(Macro));
	if (macro != NULL) {
		strcpy(macro->name, name);
		macro->content = createTable();
		/* check if memory allocation for the new macro content table succeeded */
		if (macro->content == NULL)
			/* not succeeded -> the macro is not useful -> destroy it */
				free(macro);
	}
	return macro;
}