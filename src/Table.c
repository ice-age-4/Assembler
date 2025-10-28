#include "Table.h"

#include <stdlib.h>
#include "LinkedList.h"


/* The data structure of a table is implemented using a linked list
 */

struct Table {
	Node *head;			/* pointer to first element of the table */
	Node *tail;			/* pointer to last element of the table */
};



Table * createTable() {
	Table *table = malloc(sizeof(Table));
	if(table != NULL) {
		table->head = NULL;
		table->tail = NULL;
	}

	return table;
}

int addToTable(Table *table, void *data) {
	return insertToEndOfList(&(table->head), &(table->tail), data);
}

void * findInTable(Table *table, void *key, int(*compare)(void *data, void *key)) {
	return findInList(table->head, key, compare);
}

void fprintTable(FILE *file, Table *table, void(*fprintData)(FILE *, void *)) {
	fprintList(file, table->head, fprintData);
}

void updateTable(Table *table, void *updateArgument, void(*updateData)(void *data, void *updateArgument)) {
	updateList(table->head, updateArgument, updateData);
}


/* gets pointer to pointer, to set the original pointer to the table to NULL after releasing all table memory. */
void freeTable(Table **table, void (*freeData)(void**)) {
	if(*table != NULL) {
		freeList((*table)->head, freeData);
	}
	free(*table);
	*table = NULL;
}