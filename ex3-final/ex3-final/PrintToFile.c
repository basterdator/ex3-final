/* ==============================================

Introduction to Systems Programming
Winter 2017-2018
TEL-AVIV UNIVERSITY
Exercise 3
Uri Cohen                 302825807
Anton Chaplianka          310224209
============================================== */

// Includes --------------------------------------------------------------------

#include "PrintToFile.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

// Constants  ------------------------------------------------------------------


int PrintToFile(char *p_msg, char *path)  /* A function Which takes a string
										  and writes the string to the specified path */

{
	errno_t retval;
	FILE *p_stream;
	printf("path is: %s\n\n", path);
	retval = fopen_s(&p_stream, path, "a");
	if (0 != retval)

	{

		printf("Failed to open file.\n");
		return FAILURE;

	}



	// Write lines

	retval = fputs(p_msg, p_stream);

	if (0 > retval)

	{

		printf("Failed to write to file.\n");

		// Don't return. Try and close the file first.

	}



	// Close file

	retval = fclose(p_stream);

	if (0 != retval)

	{

		printf("Failed to close file.\n");

		return FAILURE;

	}



	return SUCCESS;

}