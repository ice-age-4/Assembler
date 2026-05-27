#ifndef GLOBALS_H
#define GLOBALS_H


/* It is important that there is no overlap between the following logical values and error values,
 * because there are logical functions that on error return error value, and on success return logical value */

/**
 * Represents error status when exiting functions.
 */
typedef enum StatusCode {
    NO_ERROR = -2,		    	/** no error occurred */
    INPUT_ERROR = -3,	       	/** An input error was detected. */
    SYSTEM_ERROR = -4,		    /** An internal system error has occurred. */
    PRE_PROCESSOR_ERROR = -5,	/** An input error was detected during the pre-processing stage. */
    COMPILATION_ERROR = -6,		/** An input error was detected during the compilation stage. */
    FILE_NAME_TOO_LONG_ERROR = -7	/** The file name provided is too long to be processed. */
} StatusCode;


/**
 * Macros that represent custom logical values.
 * Note: These are negative to avoid overlap with array indices, 
 * always compare explicitly (e.g., status == TRUE).
 */
#define FALSE (-10)
#define TRUE (-11)

/**
 * A constant value returned by search functions when an item is not found.
 */
#define NOT_FOUND (-1)

#endif
