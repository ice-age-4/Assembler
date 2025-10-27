#include "LinkedList.h"
#include <stdlib.h>
#include "globals.h"



/**
 * allocates memory for a node and fills it with data.
 * @param data pointer to data of any type to store in a new node
 * @return on success, pointer to the new node. on failure of memory allocation, NULL.
 */
static Node *createNode(void *data);

static Node * createNode(void *data) {
	Node *node = (Node *)malloc(sizeof(Node));
	if (node != NULL) {
		node->data = data;
		node->next = NULL;
	}

	return node;
}

int insertToEndOfList(Node **head, Node **tail, void *data) {
	Node *newNode = createNode(data);

	if (newNode == NULL)	/* memory allocation failed */
		return SYSTEM_ERROR;

	if (*head == NULL) {	/* insert to empty list */
		*head = newNode;
		*tail = newNode;
	}
	else {					/* insert to non-empty list */
		(*tail)->next = newNode;
		*tail = newNode;
	}
	return NO_ERROR;
}

void * findInList(Node *head, void *key, int(*compare)(void * data, void *key)) {
	while(head != NULL)
	{
		if(compare(head->data, key) == TRUE)
			return head->data;
		head=head->next;
	}
	return NULL;
}

void fprintList(FILE *file, Node *head, void(*fprintData)(FILE *, void *)) {
	while(head != NULL) {
		fprintData(file, head->data);
		head=head->next;
	}
}

void updateList(Node *head, void *updateArgument, void(*updateData)(void * data, void * updateArgument)) {
	while(head != NULL) {
		updateData(head->data, updateArgument);
		head=head->next;
	}
}

void freeList(Node *head, void (*freeData)(void**)) {
	Node *tmp;
	while (head != NULL) {
		tmp = head->next;

		if (freeData !=	NULL) {
			freeData(&(head->data));
		}
		free(head);
		head = tmp;
	}
}
