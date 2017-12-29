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
#include "main.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

static void ReportErrorAndEndProgram();

// Constants  ------------------------------------------------------------------
static DWORD wait_res;
static BOOL release_res;

int PrintToReportFile(int num_of_roomate, char *path)  /* A function Which writes to the Peoprt file
													   using this key:
													   num of rooommates == -1 : Print "Robot Active"
													   num of rooommates == -2 : Print "Simulation started
													   num of rooommates == %d  : Print "Roomate %d Active""*/

{
	// Down(write)
	wait_res = WaitForSingleObject(write_to_file, INFINITE);
	if (wait_res != WAIT_OBJECT_0) ReportErrorAndEndProgram();
	
	errno_t retval;
	FILE *p_stream;
	retval = fopen_s(&p_stream, path, "a");
	if (0 != retval)
	{
		printf("Failed to open file.\n");
		return FAILURE;
	}

	if (num_of_roomate == -1)
	{
		// Write lines
		retval = fputs("bl33p bl00p, r0b0t 4ct1v3\n", p_stream);
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
	}
	else
	{
		if (num_of_roomate == -2)
		{
			// Write lines
			retval = fputs("Simulation Started\n", p_stream);
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
		}
		else 
		{
			// Write lines
			retval = fprintf(p_stream, "Roomate %d active\n", num_of_roomate);
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

		}
		
	}
	
	// Up(write)
	release_res = ReleaseMutex(write_to_file);
	if (release_res == FALSE) ReportErrorAndEndProgram();

	return SUCCESS;

}

static void ReportErrorAndEndProgram()
{
	printf("PrintToRePortFile error, ending program. Last Error = 0x%x\n", GetLastError());
	exit(1);
}