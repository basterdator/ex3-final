/* ==============================================
Introduction to Systems Programming
Winter 2017-2018
TEL-AVIV UNIVERSITY
Exercise 3
Uri Cohen                 302825807
Anton Chaplianka          310224209
============================================== */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

// Includes --------------------------------------------------------------------
#include "Roommate.h"
#include "timer_thread.h"
#include "main.h"
#include "parse_parameters.h"
#include "PrintToFile.h"

static int i;
extern int num_of_active_roomates;
extern int num_of_clothes_in_basket;
extern int NUM_OF_ROOMMATES;
extern int M;
extern HANDLE laundry_room;
extern HANDLE laundry_full;
extern HANDLE laundry_empty;
extern HANDLE write_to_file;

static DWORD wait_res;
static BOOL release_res;

DWORD ExitProcedure(roomate *p_roommate_list);
void LaundryRoomProcedure(roomate *p_current_roommate);
static void ReportErrorAndEndProgram();

DWORD WINAPI Roommate(LPVOID lpParam)
{

	if (NULL == lpParam)
	{
		return ROOMMATE_THREAD__CODE_NULL_PTR;
	}
	roomate_params *p_params_for_roomate;

	p_params_for_roomate  = (roomate_params *)lpParam;
	roomate_params params_for_roomate = *p_params_for_roomate;
	roommate_thread roommate_params;
	roommate_params.num_of_roomate = params_for_roomate.roomate_index;
	roommate_params.p_roomate_list = params_for_roomate.roomates_array;
	roommate_params.TimeIsUp = params_for_roomate.time_is_up;

	int roommate_num = roommate_params.num_of_roomate;
	roomate *p_roommate_list = roommate_params.p_roomate_list;
	roomate *p_current_roommate = &p_roommate_list[roommate_num];
	BOOL TimeIsUp;
	while(TRUE)
	{
		printf("go to sleep - , %d \n", roommate_num);
		Sleep((*p_current_roommate).time);
		printf("woke up - , %d \n", roommate_num);

		// Down(write)
		wait_res = WaitForSingleObject(write_to_file, INFINITE);
		if (wait_res != WAIT_OBJECT_0) ReportErrorAndEndProgram();
		printf("Down(write_to_file), %d \n", roommate_num);

		// cretae the string to write to the file
	//	char numc = roommate_num + "0";
	//	sprintf(numc,"%d ", roommate_num);
		printf("Roomate Active -  %d \n", roommate_num);
		PrintToFile("Roommate Active", p_params_for_roomate->report_file_path);
	//	PrintToFile(numc, params_for_roomate.report_file_path);
	//	PrintToFile("active\n", params_for_roomate.report_file_path);

		// Up(write)
		release_res = ReleaseMutex(write_to_file);
		if (release_res == FALSE) ReportErrorAndEndProgram();
		printf("down(write_to_file), %d \n", roommate_num);

		// down(laundry_room)

		wait_res = WaitForSingleObject(laundry_room, INFINITE);
		if (wait_res != WAIT_OBJECT_0) ReportErrorAndEndProgram();

		printf("down(laundry_room), %d \n", roommate_num);



		// If times up:
		TimeIsUp = *(p_params_for_roomate->time_is_up);
		if (TimeIsUp == TRUE)
		{
			// If the num of active roomates is 1 :
			if (num_of_active_roomates == 1)
			{
				// Up(laundry_full)
				release_res = ReleaseSemaphore(
					laundry_full,
					1, 		/* Signal that exactly one cell was filled */
					NULL);
				if (release_res == FALSE) ReportErrorAndEndProgram();
				// ExitProcedure()
				ExitProcedure(p_roommate_list);
			}
			else {

				// ExitProcedure()
				ExitProcedure(p_roommate_list);
			}

		}
		// LaundryRoomProcedure()
		LaundryRoomProcedure(p_current_roommate);

		// up(laundry_room)

		release_res = ReleaseMutex(laundry_room);
		if (release_res == FALSE) ReportErrorAndEndProgram();

		// If there are zero clothe in your basket:
		if ((*p_current_roommate).curret_clothes == 0)
		{
			// down(stuck_without_clothes)
			release_res = ReleaseSemaphore(
				(*p_current_roommate).NoClothes,
				1, 		/* Signal that exactly one cell was filled */
				NULL);
			if (release_res == FALSE) ReportErrorAndEndProgram();
		}

		// Repeat
	}
}





// exit_procedure():
DWORD ExitProcedure(roomate *p_roommate_list) /* The exit procedure gets a pointer to the list of roomates
											  so it could release the semaphore of those who are stuck without clothes */
{
	roomate *current_roommate;

	//	num_of_active_roomates--
	num_of_active_roomates--;

	//	up(laundry_room)
	release_res = ReleaseMutex(laundry_room);
	if (release_res == FALSE) ReportErrorAndEndProgram();

	//	for(each roomate that has 0 clothes in his closet and has a nonzero amount of total clothes):
	for (i = 0; i < NUM_OF_ROOMMATES; i++)
	{
		current_roommate = &p_roommate_list[i];
		if ((*current_roommate).curret_clothes == 0 && (*current_roommate).total_clothes != 0)

		{
			//		up(stuck_without_clothes)
			release_res = ReleaseSemaphore(
				(*current_roommate).NoClothes,
				1, 		/* Signal that exactly one cell was filled */
				NULL);
			if (release_res == FALSE) ReportErrorAndEndProgram();

		}

	}
	//	return 0
	return ROOMMATE_THREAD__CODE_SUCCESS;


}



void LaundryRoomProcedure(roomate *p_current_roommate)
{
	//	if there are M-1 Clothes in the basket
	if (num_of_clothes_in_basket == M)
	{
		//		up(basketfull)
		release_res = ReleaseSemaphore(
			laundry_full,
			1, 		/* Signal that exactly one cell was filled */
			NULL);
		if (release_res == FALSE) ReportErrorAndEndProgram();
		//		down(basketempty)
		wait_res = WaitForSingleObject(laundry_empty, INFINITE);
		if (wait_res != WAIT_OBJECT_0) ReportErrorAndEndProgram();
		//		clothesinbasket = 0
		num_of_clothes_in_basket = 0;

	}
	else
	{
		num_of_clothes_in_basket++;
		(*p_current_roommate).curret_clothes--;
	}

}

static void ReportErrorAndEndProgram()
{
	printf("Encountered error, ending program. Last Error = 0x%x\n", GetLastError());
	exit(1);
}