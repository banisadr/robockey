/************************************************************
Header Information
************************************************************/
/*
* Goalie role function
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
#include "motor_control_function.h"


/************************************************************
Private Function Definitions
************************************************************/

/************************************************************
Global Variables
************************************************************/

/************************************************************
Public Functions
************************************************************/


void goalie_action(float x_goal, float y_goal, float x_puck, float y_puck, int puck_dist, float* x_target, float* y_target,
float* max_theta, float* theta_kd, float* theta_kp, float* linear_kd, float* linear_kp, float* max_duty_cycle, int* role)
{
	if (has_puck())
	{	//If has the puck, turn into an attacker
		*role = ATTACK;
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
	

		*role = GOALIE;
		// 		if (puck_dist > 800)
		// 		{	//If puck is close, go get the puck
		// 			*role = ATTACK;
		// 			*x_target = x_puck;
		// 			*y_target = y_puck;
		// 			*max_theta = M_PI;
		// 			*theta_kd = 0;
		// 			*theta_kp = 1.8;
		// 			*linear_kd = 0.01;
		// 			*linear_kp = 0.2;
		// 			*max_duty_cycle = DUTY_CYCLE_SEEK;
		// 			m_green(ON);
		// 			return;
		// 		}
		//
		//If puck isn't close
		
		/* Find current position */
		float position_buffer[3];
		get_position(position_buffer);
		
		float x = position_buffer[0];
		float y = position_buffer[1];
		float theta = position_buffer[2];
		
		/*decide action based on displacement from goal*/
		float goalie_displacement = sqrt((x+x_goal)*(x+x_goal)+(y-y_goal)*(y-y_goal));
		
		m_usb_tx_string("\ngoalie displacement: ");
		m_usb_tx_int((int)goalie_displacement);
//		if (goalie_displacement > 10) { //if you're far from the goal
			*x_target = -x_goal;
			*y_target = y_goal;
			*max_theta = M_PI;
			*theta_kd = 0;
			*theta_kp = 1.8;
			*linear_kd = 0.01;
			*linear_kp = 0.2;
			*max_duty_cycle = DUTY_CYCLE_SEEK;
			m_green(ON);
			return;
//		}
// 		if (puck_dist > 200)
// 		{	//If puck is in sight, face the puck
// 			*x_target = x_puck;
// 			*y_target = y_puck;
// 			*max_theta = M_PI;
// 			*theta_kd = 0.05;
// 			*theta_kp = 1.2;
// 			*linear_kd = 0.0;
// 			*linear_kp = 0.0;
// 			*max_duty_cycle = 0.4;
// 			m_green(ON);
// 			return;
// 			
// 		}
// 		
// 		*max_theta = M_PI;
// 		*theta_kd = 0.01;
// 		*theta_kp = 1.2;
// 		*linear_kd = 0.0;
// 		*linear_kp = 0.0;
// 		*max_duty_cycle = 0.4;
// 		m_green(ON);
// 		
// 		if (x_goal>0) {
// 			*x_target = 100;
// 			*y_target = 0;
// 			} else {
// 			*x_target = -100;
// 			*y_target = 0;
// 		}
		
		return;
		
		

		
		
		
		
		
}