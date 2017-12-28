#ifndef __PARSE_PARAMETERS_H__
#define __PARSE_PARAMETERS_H__
#include<stdlib.h>
#include<stdio.h>
#include<windows.h>
#define MAX_LINE_SIZE 20

typedef struct line_s {
	char line_string[MAX_LINE_SIZE];
}line;

typedef struct roomate_s
{
	DWORD time;
	int total_clothes;
	int curret_clothes;
	HANDLE NoClothes;
}roomate;

typedef struct roomate_params_s {
	roomate *roomates_array;
	int roomate_index;
	BOOL *time_is_up;
	char *report_file_path;
	char *debug_file_path;
}roomate_params;

typedef struct robot_params_s {
	roomate *roomates_array;
	BOOL *time_is_up;
	char *report_file_path;
	char *debug_file_path;
}robot_params;

int AllocateRoomatesArray(int num_of_roomates, roomate **roomates_array);
//input param: num of roomates
//output param: pointer to the roomates array ater space in the memory is allocated

int AppendRoomate(roomate *roomates_array, int size_of_roomates_array,
	int roomate_time, int roomate_total_clothes);//adds a roomate to the roomates array

int ParseLine(char *line, char *roomate_time, char *roomate_total_clothes);

#endif //
