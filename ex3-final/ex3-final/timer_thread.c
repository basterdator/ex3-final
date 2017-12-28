#include <windows.h>

#include<stdlib.h>
#include<stdio.h>
#include "timer_thread.h"



DWORD WINAPI Timer(LPVOID lpParam)

{

	timer_params *params_for_timer;
	//DWORD *T_D;

	if (NULL == lpParam)

	{

		return TIMER_THREAD__CODE_NULL_PTR;

	}

	params_for_timer = (timer_params *)lpParam;
	//T_D = (DWORD *)lpParam;
	printf("in timer\n");
	Sleep((*params_for_timer).t_d);
	//Sleep(1000);
	printf("after timer\n");
	
	(*params_for_timer).time_is_up = 1;

	return TIMER_THREAD__CODE_SUCCESS;



}