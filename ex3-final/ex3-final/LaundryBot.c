/* ==============================================
Introduction to Systems Programming
Winter 2017-2018
TEL-AVIV UNIVERSITY
Exercise 3
Uri Cohen                 302825807
Anton Chaplianka          310224209
============================================== */

// Includes --------------------------------------------------------------------
#include <windows.h>
#include "LaundryBot.h"
#include "timer_thread.h"
#include "PrintToFile.h"
#include "main.h"
#include "parse_parameters.h"

extern int num_of_active_roomates;
extern int num_of_clothes_in_basket;
extern int NUM_OF_ROOMMATES;
extern int M;
extern HANDLE laundry_full;
extern HANDLE laundry_empty;
extern HANDLE write_to_file;


DWORD WINAPI LaundryBot(LPVOID lpParam)
{
	int i;


	static DWORD wait_res;
	static BOOL release_res;

	robot_params *p_params_for_robot;
	p_params_for_robot = (robot_params *)lpParam;
	robot_params params_for_robot = *p_params_for_robot;
	roomate *p_roomate_list = params_for_robot.roomates_array;
	BOOL TimeIsUp;
	char *p_report_file_path = params_for_robot.report_file_path;
	char *p_debug_file_path = params_for_robot.debug_file_path;
	while(TRUE)
	{
		// Down(laundry_full)
		wait_res = WaitForSingleObject(laundry_full, INFINITE);
		if (wait_res == FALSE) ReportErrorAndEndProgram();


		// If time is up and the number of active threads is 1 - then the last roomate turns the robot off - so the robot shuts itself down.
		TimeIsUp = *params_for_robot.time_is_up;
		if (TimeIsUp == TRUE && num_of_active_roomates <= 1)
		{
			return LAUNDRYBOT_THREAD__CODE_SUCCESS;
		}

		// Else: the robot needs to complete it's operation
		// Writing in the log


		// write in the report file
		printf("Robot Active \n");
		//PrintToReportFile(-1 , p_report_file_path);


		// Check each roomate whether he has zero clothes in his closet and he had a non zero num of clothes in the first place
		for (i = 0; i < NUM_OF_ROOMMATES; i++)
		{
			if (p_roomate_list[i].curret_clothes == 0 && p_roomate_list[i].total_clothes != 0)
			{
				printf("%d", p_roomate_list[i].curret_clothes);
				// empty the basket
				num_of_clothes_in_basket = 0;
				p_roomate_list[i].curret_clothes = p_roomate_list[i].total_clothes;  // fill up the closet
				
				release_res = ReleaseSemaphore(
					p_roomate_list[i].NoClothes,
					1, 		/* Signal that exactly one cell was filled */
					NULL);
				if (release_res == FALSE) { ReportErrorAndEndProgram(); }

			}
			else
			{
				if (p_roomate_list[i].total_clothes != 0)
				{
					p_roomate_list[i].curret_clothes = p_roomate_list[i].total_clothes;
				}
			}
		}

		// Up(laundry_empty)

		release_res = ReleaseSemaphore(
			laundry_empty,
			1, 		/* Signal that exactly one cell was filled */
			NULL);
		if (release_res == FALSE) ReportErrorAndEndProgram();
	}


}

static void ReportErrorAndEndProgram()
{
	printf("Encountered error, ending program. Last Error = 0x%x\n", GetLastError());
	exit(1);
}