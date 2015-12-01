// Motor Control General Function

#ifndef motor_control_function__
#define motor_control_function__

#include "m_general.h"
#include "m_wii.h"
#include "localization_function.h"
#include "initialization_function.h"

void update_position(void); //Get IMU data, filter, update angle, update control
void run_motor_control_loop(float x_target, float y_target, float max_duty_cycle, float max_theta, float theta_kp, float theta_kd, float linear_kp, float linear_kd, int game_pause); //Run Control Loop
char get_position(float* postition_buffer); // Return current x,y, theta

#endif