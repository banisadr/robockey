/************************************************************
Header Information
************************************************************/
/*
* Attack role function
*
* 
*/

/*****************************************************************
==================================================================
### INTERNAL README: ADD TO EVERYTIME BEFORE SENDING TO GITHUB ###

Created by Pete 12/6/15

==================================================================
*****************************************************************/


/************************************************************
Included Files & Libraries
************************************************************/

#include "m_general.h"
#include "m_wii.h"
#include "localization_function.h"
#include "initialization_function.h"
#include "puck_location_function.h"


/************************************************************
Private Function Definitions
************************************************************/

/************************************************************
Global Variables
************************************************************/

/************************************************************
Public Functions
************************************************************/


void attack_action(float x_goal, float y_goal, float x_puck, float y_puck, float* x_target, float* y_target, float* max_theta, 
	float* theta_kd, float* theta_kp, float* linear_kd, float* linear_kp, float* max_duty_cycle)
{
		if (has_puck())
		{
			*x_target = x_goal;
			*y_target = y_goal;
			*max_theta = M_PI/2;
			*theta_kd = 0.05;
			*theta_kp = 1.2;
			*linear_kd = 0.01;
			*linear_kp = 0.2;
			*max_duty_cycle = DUTY_CYCLE_PUCK;
			m_green(OFF);
			return;
			
		}
		
		if (!has_puck())
		{
			*x_target = x_puck;
			*y_target = y_puck;
			*max_theta = M_PI;
			*theta_kd = 0;
			*theta_kp = 1.8;
			*linear_kd = 0.01;
			*linear_kp = 0.2;
			*max_duty_cycle = DUTY_CYCLE_SEEK;
			m_green(ON);
			return;
		}
}