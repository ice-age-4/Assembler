#ifndef GLOBALS_H
#define GLOBALS_H


/* It is important that there is no overlap between the following logical values and error values,
 * because there are logical functions that on error return error value, and on success return logical value */

/**
 * Represents error status when exiting functions.
 */
#define NO_ERROR (-2)			/** no error occurred */
#define INPUT_ERROR (-3)		/** An input error was detected. */
#define SYSTEM_ERROR (-4)		/** An internal system error has occurred. */

/**
 * macros that represent logical values
 */
#define FALSE (-10)
#define TRUE (-11)


/**
 * A constant value that can be returned by a search function in an array.
 * This value is always outside the bounds of the array.
 */
#define NOT_FOUND (-1)

#endif
