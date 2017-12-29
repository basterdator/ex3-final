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

int PrintToDebugFile(char *p_msg, char *path) 
{
	// Down(write)
	wait_res = WaitForSingleObject(write_to_file, INFINITE);
	if (wait_res != WAIT_OBJECT_0) ReportErrorAndEndProgram();

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