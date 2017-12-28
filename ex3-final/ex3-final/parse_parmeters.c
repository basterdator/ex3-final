#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include<stdio.h>
#include<stdlib.h>
#include "parse_parameters.h"


int AllocateRoomatesArray(int num_of_roomates, roomate **roomates_array) {
	*roomates_array = (roomate*)malloc(num_of_roomates, sizeof(roomate));//allocates space in the memory
	return 0;
}

int AppendRoomate(roomate *roomates_array, int size_of_roomates_array,
	int roomate_time, int roomate_total_clothes) {//adds a roomate to the roomates array
	if (roomates_array == NULL) {
		return -1;
	}
	roomate new_roomate;
	printf("append roomate\n");
	//intiate new roomate
	new_roomate.total_clothes = roomate_total_clothes;
	new_roomate.curret_clothes = roomate_total_clothes;
	new_roomate.time = (DWORD)roomate_time;
	new_roomate.NoClothes = 0;
	roomates_array[size_of_roomates_array] = new_roomate;//adds the roomate in the end of the array
	return 0;
}

int ParseLine(char *line, char *roomate_time, char *roomate_total_clothes)
{
	if ((*roomate_time != NULL) & (*roomate_total_clothes != NULL)) // checking if the parameters given are nit null
	{
		char *roomate_time_char;
		char *roomate_total_clothes_char;
		char *time;
		char *total_clothes;
		time = strtok(line, ","); // find the string from the beginning until ','
		total_clothes = strtok(NULL, "\n"); // find the string from ',' to \n"
		if (total_clothes == NULL) { // if there is no "\n" look for the end of the string instead
			time = strtok(line, ",");
			total_clothes = strtok(NULL, "\0");
		}

		if (time != NULL & total_clothes != NULL) {
			strcpy(roomate_time, time);
			//	printf("%s\n", name);
			strcpy(roomate_total_clothes, total_clothes);
			return 0;
		}
		else {
			return -1;
		}
	}
	else
	{
		return -1;
	}
}
