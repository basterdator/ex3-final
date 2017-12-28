#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include<stdlib.h>
#include<stdio.h>
#include "parse_parameters.h"
#include "timer_thread.h"
#include "PrintToFile.h"
#include "main.h"
#include"LaundryBot.h"
#include"Roommate.h"

#define SUCCESS ((int)(0))
#define FAILURE ((int)(-1))
#define FILE_NUM 3
#define MAX_PARAM_DIGITS 20 


char *p_report_file_path;
char *p_debug_file_path;
int num_of_active_roomates;
int NUM_OF_ROOMMATES;
int num_of_clothes_in_basket = 0;
int M;


//------------functions-----------------//

int GetNumLines(char *filename, int *num_of_lines);

int ImportroomatesArray(char *filename, int num_of_roomates, roomate **roomates_array,
	int *total_run_time, int *laundry_basket_size);

static HANDLE CreateThreadSimple_timer(LPTHREAD_START_ROUTINE p_start_routine,
	LPDWORD p_thread_id, timer_params *lpParameter);

static HANDLE CreateThreadSimple_roomate(LPTHREAD_START_ROUTINE p_start_routine,
	LPDWORD p_thread_id, roomate_params *lpParameter);

static HANDLE CreateThreadSimple_robot(LPTHREAD_START_ROUTINE p_start_routine,
	LPDWORD p_thread_id, robot_params *lpParameter);
void AllocateParams(int n_roomates, roomate_params **roomates_params);
void AssignParams(int n_roomates, roomate_params *roomates_params, int *time_is_up
	, roomate *roomates_array, char *p_debug_file_path, char *p_report_file_path);

int main(int argc,char *argv[]) {

	//---------------testing that given arguments are legal---------------//

	if (argc != (FILE_NUM+1))
	{// verfiy that the number of arguments is 3, if not, there is no point
		//in writing an error to the debug file
		printf("error: wrong arg num\n");
		return FAILURE;
	}

	//argv[1] = paramters file name
	//argv[2] = report file name
	//argv[3] = debug file name
	p_report_file_path = argv[2];
	p_debug_file_path = argv[3];
	printf("p_report_file_path %s\n", p_report_file_path);
	printf("p_debug_file_path %s\n", p_debug_file_path);

	//reset the debug and report file
	int i;
	for (i = 2; i < 4; i++)
	{
		FILE *writing_file = fopen(argv[i], "w");
		if (writing_file == NULL) {
			printf("error: bad file path");
			return FAILURE;
		}
		fclose(writing_file);
	}
	//verify paramters file argument - argv[1]
	FILE *parameters_file;
	parameters_file = fopen(argv[1], "r");//open parameters file
	if (parameters_file == NULL) {//check the path of parameters file
		printf("error: bad file path");
		return FAILURE;
	}

	//--------------------------print "simulation started"------------------------//
	PrintToFile("simulation started\n", p_report_file_path);

	//-------------------------parse parameters file------------------------------//

	int *p_num_of_paramters = malloc(sizeof(int));
	GetNumLines(argv[1], p_num_of_paramters);//count the lines in the parameters file
	int num_of_paramters = *p_num_of_paramters;
	int num_of_roomates = num_of_paramters-2;//the first 2 lines are T_d and M
	num_of_active_roomates = num_of_roomates;
	NUM_OF_ROOMMATES = num_of_roomates;
	roomate *roomates_array = NULL;
	int *total_run_time = malloc(sizeof(int));
	int *laundry_basket_size = malloc(sizeof(int));

	//creating an array of roomates
	ImportroomatesArray(argv[1], num_of_paramters, &roomates_array, total_run_time, laundry_basket_size);
	for (i = 0; i < num_of_roomates; i++) {
		printf("roomates array[%d] is :%d %d\n", i, roomates_array[i].time, roomates_array[i].total_clothes);
	}
	M = *laundry_basket_size;
	printf("T_d is: %d,basket size is: %d\n", *total_run_time, *laundry_basket_size);
	//-------------------creating semaophores and handles--------------//
	write_to_file = CreateMutex(
		NULL,   /* default security attributes */
		FALSE,	/* don't lock mutex immediately */
		NULL); /* un-named */
	if (write_to_file == NULL)
	{
		CloseHandle(write_to_file);
		return -1;
	}

	laundry_room = CreateMutex(
		NULL,   /* default security attributes */
		FALSE,	/* don't lock mutex immediately */
		NULL); /* un-named */

	if (laundry_room == NULL)
	{
		CloseHandle(laundry_room);
		return -1;
	}

	laundry_full = CreateSemaphore(
		NULL,	/* Default security attributes */
		0,		/* Initial Count - all slots are empty */
		1,		/* Maximum Count */
		NULL); /* un-named */
	if (laundry_full == NULL)
	{
		CloseHandle(laundry_full);
		return -1;
	}

	laundry_empty = CreateSemaphore(
		NULL,	/* Default security attributes */
		0,		/* Initial Count - all slots are empty */
		1,		/* Maximum Count */
		NULL); /* un-named */
	if (laundry_empty == NULL)
	{
		CloseHandle(laundry_empty);
		return -1;
	}

	for (i = 0; i < num_of_roomates; i++)
	{
		(*roomates_array).NoClothes = CreateSemaphore(
			NULL,	/* Default security attributes */
			0,		/* Initial Count - all slots are empty */
			1,		/* Maximum Count */
			NULL); /* un-named */
		if ((*roomates_array).NoClothes == NULL)
		{
			CloseHandle((*roomates_array).NoClothes);
			return -1;
		}

	}
	//-------------------------define thread handles-------------------//

	int num_of_threads = num_of_paramters;//+2 for robot and timer
	printf("num of threads: %d\n",num_of_threads);
	HANDLE *p_thread_handles = malloc(sizeof(HANDLE)*num_of_threads);//array of thread handles
	DWORD *p_thread_ids = malloc(sizeof(DWORD)*num_of_threads);//array of thread ids
	DWORD *exitcode_array = malloc(sizeof(DWORD)*num_of_threads);//array of the 

	//----------------------------run timer----------------------------//

	DWORD total_run_time_dw = (DWORD)*total_run_time;
	printf("time dw is: %d\n", total_run_time_dw);
	//setting parameters for timer thread//
	timer_params params_for_timer;
	params_for_timer.time_is_up = 0;
	params_for_timer.t_d = total_run_time_dw;
	p_thread_handles[0] = CreateThreadSimple_timer(Timer, p_thread_ids, &params_for_timer);//calling the timer thread

	//--------------------------run robot-----------------------------//
	//setting parameters for robot thread//
	robot_params params_for_robot;
	params_for_robot.time_is_up = &params_for_timer.time_is_up;
	params_for_robot.roomates_array = roomates_array;
	params_for_robot.debug_file_path = p_debug_file_path;
	params_for_robot.report_file_path = p_report_file_path;
	p_thread_handles[1] = CreateThreadSimple_robot(LaundryBot, (p_thread_ids + 1) , &params_for_robot);//calling the robot thread

	//---------------------------run roomates-------------------------//
	roomate_params *params_for_roomate;
	AllocateParams(num_of_roomates, &params_for_roomate);
	AssignParams(num_of_roomates, params_for_roomate, &params_for_timer.time_is_up, roomates_array, p_debug_file_path, p_report_file_path);
	for (i = 0; i < num_of_roomates; ++i)
	{

		printf("calling roomate, %d \n", (params_for_roomate + i)->roomate_index);

		//calling roomate i thread//
		p_thread_handles[i+2] = CreateThreadSimple_roomate(Roommate, (p_thread_ids + i+2), (params_for_roomate + i));
	}

	//----------------------------wait for all threads----------------------------//

	DWORD waitcode;
	waitcode = WaitForMultipleObjects(num_of_threads, p_thread_handles, 1, INFINITE);
	if (waitcode != WAIT_OBJECT_0) {//if not all threads succeeded:
		printf("error wait for multiple objects");
		for (i = 0; i < num_of_threads; i++) /* If one of the thread dies we print the return codes of all threads in order to know exactly
											 which thread caused the main process to crash*/
		{
			GetExitCodeThread(p_thread_handles[i], &exitcode_array[i]);
			printf("Thread %d finished with exitcode 0x%x\n", i, exitcode_array[i]);
		}
		return -1;
	}

	printf("time is up: %d\n", params_for_timer.time_is_up);

	printf("end of main\n");
	return SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
										//end of main//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

int GetNumLines(char *filename, int *num_of_lines)
{
	// count the number of lines in the file called filename                                    
	FILE *fp;
	fopen_s(&fp, filename, "r");
	int ch = 0;
	int lines = 0;

	if (fp == NULL) // checking if the file pointer is null or not
	{
		printf("Error, no such file!\n");
		return -1;
	}

	lines++; // there's atlest a single line
	while (!feof(fp)) // while we havn't reached the end of the file
	{
		ch = fgetc(fp);
		if (ch == '\n')
		{
			lines++;  // if the char is equal to '\n' it means there is a new line
		}
	}
	fclose(fp);
	*num_of_lines = lines;
	return 0;
}

int ImportroomatesArray(char *filename, int num_of_params, roomate **roomates_array,
	int *total_run_time,int *laundry_basket_size) {
	int allocation_flag;//flags to indicate errors
	allocation_flag = AllocateRoomatesArray(num_of_params-2, roomates_array);//flag indicates if memory allocation for the array faild
	if (allocation_flag < 0) {//return if there was an error
		return -1;
	}
	int i;
	FILE *input_file;
	fopen_s(&input_file, filename, "r");//open the input file that contains a raw list of roomates and candys
										//	char *line_for_parse = malloc(sizeof(char)*MAX_LINE_SIZE);
	line *lines_from_file = malloc(MAX_LINE_SIZE*num_of_params * sizeof(char));
	char *p_roomate_time;
	p_roomate_time = malloc(MAX_PARAM_DIGITS * sizeof(char));
	char *p_roomate_total_clothes;
	p_roomate_total_clothes = malloc(MAX_PARAM_DIGITS * sizeof(char));
	int parse_line_flag = 0;//flag indicates if line parsing faild
	int roomate_appending_flag = 0;//flag indicates if appending a roomate to the array faild 
	for (i = 0; i < num_of_params; i++) {
		fgets(lines_from_file[i].line_string, MAX_LINE_SIZE, input_file);
		printf("line is: %s\n", lines_from_file[i].line_string);
	}
	fclose(input_file);
	*total_run_time = atoi(lines_from_file[0].line_string);
	*laundry_basket_size = atoi(lines_from_file[1].line_string);
	int j;
	for (j = 0; j < num_of_params-2; j++) {
		parse_line_flag = ParseLine(lines_from_file[j+2].line_string, p_roomate_time, p_roomate_total_clothes);
		if (parse_line_flag < 0) {
			printf("parse line error\n");
			return -1;
		}
		int roomate_time = atoi(p_roomate_time);
		int roomate_total_clothes = atoi(p_roomate_total_clothes);
		roomate_appending_flag = AppendRoomate(*roomates_array, j, roomate_time, roomate_total_clothes);
		//	printf("roomates array[%d] is :%s\n",j , *roomates_array[j].name);
		if (roomate_appending_flag < 0) {
			printf("roomate append error\n");
			return -1;
		}
	}

	return 0;


}

static HANDLE CreateThreadSimple_timer(LPTHREAD_START_ROUTINE p_start_routine,
	LPDWORD p_thread_id, timer_params *lpParameter) // A wrapper to CreateThread() that also gets an input parameter
{
	return CreateThread(
		NULL,            /*  default security attributes */
		0,               /*  use default stack size */
		p_start_routine, /*  thread function */
		lpParameter,     /*  argument to thread function */
		0,               /*  use default creation flags */
		p_thread_id);    /*  returns the thread identifier */
}

static HANDLE CreateThreadSimple_roomate(LPTHREAD_START_ROUTINE p_start_routine,
	LPDWORD p_thread_id, roomate_params *lpParameter) // A wrapper to CreateThread() that also gets an input parameter
{
	return CreateThread(
		NULL,            /*  default security attributes */
		0,               /*  use default stack size */
		p_start_routine, /*  thread function */
		lpParameter,     /*  argument to thread function */
		0,               /*  use default creation flags */
		p_thread_id);    /*  returns the thread identifier */
}

static HANDLE CreateThreadSimple_robot(LPTHREAD_START_ROUTINE p_start_routine,
	LPDWORD p_thread_id, robot_params *lpParameter) // A wrapper to CreateThread() that also gets an input parameter
{
	return CreateThread(
		NULL,            /*  default security attributes */
		0,               /*  use default stack size */
		p_start_routine, /*  thread function */
		lpParameter,     /*  argument to thread function */
		0,               /*  use default creation flags */
		p_thread_id);    /*  returns the thread identifier */
}

void AllocateParams(int n_roomates, roomate_params **roomates_params) {

	
	*roomates_params = (roomate_params *)calloc(n_roomates, sizeof(roomate_params));


}

void AssignParams(int n_roomates, roomate_params *roomates_params, int *time_is_up
	, roomate *roomates_array, char *p_debug_file_path, char *p_report_file_path) {

	for (int i = 0; i < n_roomates; ++i) {
		(roomates_params + i)->roomate_index = i;
		(roomates_params + i)->report_file_path = p_report_file_path;
		(roomates_params + i)->debug_file_path = p_debug_file_path;
		(roomates_params + i)->roomates_array = (roomates_array + i);
		(roomates_params + i)->time_is_up = time_is_up;
	}
}