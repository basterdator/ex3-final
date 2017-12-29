
// PrintToFile.h

#ifndef __PRINTOFILE_H__
#define __PRINTOFILE_H__

#include <windows.h>

// Constants -------------------------------------------------------------------

#define SUCCESS ((int)(0))
#define FAILURE ((int)(-1))

// Function Declarations -------------------------------------------------------



int PrintToReportFile(int num_of_roomate, char *path);
int PrintToDebugFile(char *p_msg, char *path);




#endif // __PRINTOFILE_H__