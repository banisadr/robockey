//Goalie Code Function

#ifndef goalie_function__
#define goalie_function__

#include "m_general.h"
#include "m_wii.h"
#include "localization_function.h"
#include "initialization_function.h"
#include "motor_control_function.h"
#include "puck_location_function.h"

void goalie_action(float x_goal, float y_goal, float x_puck, float y_puck, int puck_dist, float* x_target, float* y_target, 
	float* max_theta, float* theta_kd, float* theta_kp, float* linear_kd, float* linear_kp, float* max_duty_cycle, int* role);




#endif