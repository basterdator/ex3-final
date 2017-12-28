#ifndef __TIMER_THREAD_H__

#define __TIMER_THREAD_H__

#include<stdlib.h>
#include<stdio.h>
#include <windows.h>



/*

* A type for thread return codes

*/



typedef enum

{

	TIMER_THREAD__CODE_SUCCESS,

	TIMER_THREAD__CODE_NULL_PTR,

} TIMER_THREAD__return_code_t;

typedef struct timer_params_s {
	DWORD t_d;
	BOOL time_is_up;
}timer_params;


DWORD WINAPI Timer(LPVOID lpParam);



#endif // __TIMER_THREAD_H__
