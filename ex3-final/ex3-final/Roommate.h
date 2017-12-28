
// Roommate.h

#ifndef __ROOMMATE_H__
#define __ROOMMATE_H__


// Includes --------------------------------------------------------------------
#include <windows.h>
#include "parse_parameters.h"


typedef struct roommate_s
{
	int num_of_roomate;
	DWORD T_i;
	int total_clothes;
	int curret_clothes;
	HANDLE NoClothes;
}roommate;

typedef struct roommate_thread_s
{
	roomate *p_roomate_list;
	BOOL *TimeIsUp;
	int num_of_roomate;
}roommate_thread;

typedef enum
{
	ROOMMATE_THREAD__CODE_SUCCESS,
	ROOMMATE_THREAD__CODE_NULL_PTR,
} ROOMMATE_THREAD__return_code_t;

DWORD WINAPI Roommate(LPVOID lpParam);

#endif // __ROOMMATE_H__
