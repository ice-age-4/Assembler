#ifndef FILES_H
#define FILES_H

#include <stdio.h>

/**
 * This file contains basic functions for working with files.
 */


#define MAX_FILE_NAME_LENGTH (256)		/** 255 for max file name in ubuntu and 1 additional char for '\0' */
#define MAX_SUFFIX_LENGTH (4)			/** the longest suffix is ".ext" or ".ent" */

#define MAX_LINE_LENGTH (82)			/** 80 for max line length .+2 for "\n\0" in the end of each line */


/**
 * Verifies that the filename given to the assembler meets the allowed length conditions.
 * The filename must be able to be appended by the assembler and still meet the allowed length of a filename on the Ubuntu system.
 * @param fileName The name of the file received for processing by the assembler
 * @return NO_ERROR if length is valid. otherwise, INPUT_ERROR.
 */
int validateFileNameLength(char *fileName);


/**
 * Receives a request to open a file in a certain mode, and tries to open the file in this mode. doesn't report on errors.
 * @param fileName The base name of the file to be opened
 * @param suffix Extension in the format ".suffix"
 * @param mode the mode to open the file, as in fopen.
 * @return returns the pointer to the file. If not opened - returns NULL.
 */
FILE *openFile(const char *fileName, const char *suffix, const char *mode);


/**
 * deletes a file from the file system.
 * @param fileName The base name of file requested to be deleted
 * @param suffix Extension in the format ".suffix" of the file requested to be deleted
 */
void removeFile(const char *fileName, const char *suffix);


/**
 * Reads a line from a file until a newline or until the end of space in a buffer that can hold up to MAX_LINE_LENGTH characters
 * @param fp The name of the file to read from
 * @param line A buffer of size MAX_LINE_LENGTH, or more. If it is larger, no more characters will be read.
 * Of course, the function makes changes to the buffer
 * @return On success in reading a line from the file, NO_ERROR.
 * On failure in reading a line from the file due to reaching the end of the file, EOF.
 * On failure due to a system error, SYSTEM_ERROR
 */
int readLineFromFile(FILE *fp, char *line);


/**
 * Receives a line read from a file into a buffer of size MAX_LINE_LENGTH
 * and verifies that it contains a sentence of the correct length according to the assembler rules (no more than MAX_LINE_LENGTH)
 * @param line the string to check. buffer of max size MAX_LINE_LENGTH.
 * @param file The file from which the string was read.
 * @return If length of line is valid, NO_ERROR. if not valid, INPUT_ERROR.
 */
int validateLineLength(const char *line, FILE *file);



#endif