<<<<<<< HEAD
#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include <stdio.h>

/** This file implements a generic linked list data structure,
 * whose members can be of any type.
 */



/** This structure implements a node in a linked list
 */
typedef struct Node {
	void * data;				/* can point to any type of data */
	struct Node * next;			/* pointer to next node in list */
} Node;


/* -------- FUNCTIONS -------- */


/**
 * inserts a new node at the end of the list
 * @param head pointer to the head of the list by reference
 * @param tail pointer to the tail of the list by reference
 * @param data the contents of the node
 * @return on success, NO_ERROR. on failure of memory allocation, SYSTEM_ERROR.
 */
int insertToEndOfList(Node **head, Node **tail, void *data);



/**
 * finds a data item in a list
 * @param head pointer to the head of the list
 * @param key the data item to search for
 * @param compare a function to compare the node data with the data item to search for, which returns TRUE on match, FALSE on mismatch.
 * @return pointer to the data item if found, otherwise, NULL
 */
void *findInList(Node *head, void *key, int (*compare)(void *data, void *key));



/**
 * prints the contents of a list into a file
 * @param file the file to print to
 * @param head pointer to the head of the list
 * @param fprintData a function to print a node data
 */
void fprintList(FILE *file, Node *head, void (*fprintData)(FILE *, void *));


/* The function updates all data items of a list,
 * given the head of the list, an update argument,
 * and a function to update a node data according to an update argument
 */
/**
 * updates all data items of a list, according to a data argument
 * @param head pointer to the head of the list
 * @param updateArgument Pointer to the argument by which the data item should be updated
 * @param updateData A function that updates data item according to an update argument
 */
void updateList(Node *head, void *updateArgument, void (*updateData)(void * data, void * updateArgument));



/**
 * releases the memory allocated for the list and its data items
 * @param head pointer to the head of the list
 * @param freeData a function to free a node data
 */
void freeList(Node *head, void (*freeData)(void**));


#endif
=======
#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include <stdio.h>

/** This file implements a generic linked list data structure,
 * whose members can be of any type.
 */



/** This structure implements a node in a linked list
 */
typedef struct Node {
	void * data;				/* can point to any type of data */
	struct Node * next;			/* pointer to next node in list */
} Node;


/* -------- FUNCTIONS -------- */


/**
 * inserts a new node at the end of the list
 * @param head pointer to the head of the list by reference
 * @param tail pointer to the tail of the list by reference
 * @param data the contents of the node
 * @return on success, NO_ERROR. on failure of memory allocation, SYSTEM_ERROR.
 */
int insertToEndOfList(Node **head, Node **tail, void *data);



/**
 * finds a data item in a list
 * @param head pointer to the head of the list
 * @param key the data item to search for
 * @param compare a function to compare the node data with the data item to search for, which returns TRUE on match, FALSE on mismatch.
 * @return pointer to the data item if found, otherwise, NULL
 */
void *findInList(Node *head, void *key, int (*compare)(void *data, void *key));



/**
 * prints the contents of a list into a file
 * @param file the file to print to
 * @param head pointer to the head of the list
 * @param fprintData a function to print a node data
 */
void fprintList(FILE *file, Node *head, void (*fprintData)(FILE *, void *));


/* The function updates all data items of a list,
 * given the head of the list, an update argument,
 * and a function to update a node data according to an update argument
 */
/**
 * updates all data items of a list, according to a data argument
 * @param head pointer to the head of the list
 * @param updateArgument Pointer to the argument by which the data item should be updated
 * @param updateData A function that updates data item according to an update argument
 */
void updateList(Node *head, void *updateArgument, void (*updateData)(void * data, void * updateArgument));



/**
 * releases the memory allocated for the list and its data items
 * @param head pointer to the head of the list
 * @param freeData a function to free a node data
 */
void freeList(Node *head, void (*freeData)(void**));


#endif
>>>>>>> db1e6bedf286cc1a18e377ce301b3cfe5f147529
