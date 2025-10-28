<<<<<<< HEAD
#ifndef TABLE_H
#define TABLE_H
#include <stdio.h>

/** This file implements a generic table data structure.
 * Each table can contain any type of data,
 * but a single table cannot contain different data types, because some table functions might fail on multy-type table.
 *
 * It's the user's responsibility to pass one-type tables as parameter to the table's functions.
 * It's the user's responsibility to pass the functions in this library a not NULL pointer to a table.
 */



/** implements a table as explained above
 */
typedef struct Table Table;



/* -------- FUNCTIONS -------- */


/**
 * creates a new table of any data type. notice: it is important to use each table variable to only one type of data.
 * @return on success, pointer to the new table. on failure of memory allocation, NULL.
 */
Table *createTable(void);



/**
 * adds a data item to the end of a given table
 * @param table pointer to a table
 * @param data pointer to data item that matches the data stored in the table
 * @return on success, NO_ERROR. on failure of memory allocation, SYSTEM_ERROR.
 */
int addToTable(Table* table, void* data);



/**
 * finds a data item in a table
 * @param table pointer to a table
 * @param key the data item to search for
 * @param compare a function to compare a single data in the table with the data item to search for,
 * which returns TRUE on match, FALSE on mismatch.
 * @return pointer to the data item if found. otherwise NULL
 */
void* findInTable(Table* table, void* key, int (*compare)(void* data, void* key));



/**
 * prints the contents of a table into a file
 * @param file the file to print to
 * @param table pointer to a table
 * @param fprintData a function to print a single data in the table to a file
 */
void fprintTable(FILE * file, Table *table, void (*fprintData)(FILE *, void *));



/**
 * updates all data items of a table, according to a data argument
 * @param table pointer to a table
 * @param updateArgument Pointer to the argument by which the data item should be updated
 * @param updateData A function that updates data item according to an update argument
 */
void updateTable(Table* table, void *updateArgument, void (*updateData)(void* data, void* updateArgument));



/**
 * releases the memory allocated for the table and its data items
 * @param table pointer to a table to release
 * @param freeData a function to free a single data item in the table.
 */
void freeTable(Table **table, void (*freeData)(void**));


#endif
=======
#ifndef TABLE_H
#define TABLE_H
#include <stdio.h>

/** This file implements a generic table data structure.
 * Each table can contain any type of data,
 * but a single table cannot contain different data types, because some table functions might fail on multy-type table.
 *
 * It's the user's responsibility to pass one-type tables as parameter to the table's functions.
 * It's the user's responsibility to pass the functions in this library a not NULL pointer to a table.
 */



/** implements a table as explained above
 */
typedef struct Table Table;



/* -------- FUNCTIONS -------- */


/**
 * creates a new table of any data type. notice: it is important to use each table variable to only one type of data.
 * @return on success, pointer to the new table. on failure of memory allocation, NULL.
 */
Table *createTable(void);



/**
 * adds a data item to the end of a given table
 * @param table pointer to a table
 * @param data pointer to data item that matches the data stored in the table
 * @return on success, NO_ERROR. on failure of memory allocation, SYSTEM_ERROR.
 */
int addToTable(Table* table, void* data);



/**
 * finds a data item in a table
 * @param table pointer to a table
 * @param key the data item to search for
 * @param compare a function to compare a single data in the table with the data item to search for,
 * which returns TRUE on match, FALSE on mismatch.
 * @return pointer to the data item if found. otherwise NULL
 */
void* findInTable(Table* table, void* key, int (*compare)(void* data, void* key));



/**
 * prints the contents of a table into a file
 * @param file the file to print to
 * @param table pointer to a table
 * @param fprintData a function to print a single data in the table to a file
 */
void fprintTable(FILE * file, Table *table, void (*fprintData)(FILE *, void *));



/**
 * updates all data items of a table, according to a data argument
 * @param table pointer to a table
 * @param updateArgument Pointer to the argument by which the data item should be updated
 * @param updateData A function that updates data item according to an update argument
 */
void updateTable(Table* table, void *updateArgument, void (*updateData)(void* data, void* updateArgument));



/**
 * releases the memory allocated for the table and its data items
 * @param table pointer to a table to release
 * @param freeData a function to free a single data item in the table.
 */
void freeTable(Table **table, void (*freeData)(void**));


#endif
>>>>>>> db1e6bedf286cc1a18e377ce301b3cfe5f147529
