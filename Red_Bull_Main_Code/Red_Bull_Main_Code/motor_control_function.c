/************************************************************
Header Information
************************************************************/
/* 
* Motor Control General Function
*
* Update position - updates static x, y, theta
* Get position - sends x, y, theta to 3 float array pointer
* Run control loop - updates OCR1a/b/c to control motor speed/dir
*/

/*****************************************************************
==================================================================
### INTERNAL README: ADD TO EVERYTIME BEFORE SENDING TO GITHUB ###

Modified by Bahram on 12/1/15 to modularize motor control

==================================================================
*****************************************************************/


/************************************************************
Included Files & Libraries
************************************************************/

#include "m_general.h"
#include "m_wii.h"
#include "localization_function.h"
#include "initialization_function.h"

/************************************************************
Private Function Definitions
************************************************************/

float lowpass(float alpha, float previous_output, float reading); //Lowpass filter
float theta_error_correction(float error); // Ensures that bot turns efficiently

/************************************************************
Global Variables
************************************************************/

/* Positioning */
static float x = 0;
static float y = 0;
static float theta = 0;
static float previous_theta_error = 0;
static float previous_linear_error = 0;

/* Wii Camera */
static float robotCenterPrev[3] = {1023, 1023, 360};

/* Motor Control */
float left_duty_cycle = 0.1;
float right_duty_cycle = 0.1;

/************************************************************
Public Functions
************************************************************/

/* Return current x,y, theta */
char get_position(float* postition_buffer){
	postition_buffer[0] = x;
	postition_buffer[1] = y;
	postition_buffer[2] = theta;
	return 1;
}


/* Get x, y, theta, filterupdate control */
void update_position(void)
{

	// Create Buffer
	unsigned int blobs[12];
	float* robotCenter;

	// Read in Locations
	m_wii_read(blobs);
	unsigned int x1 = blobs[0];
	unsigned int y1 = blobs[1];
	unsigned int x2 = blobs[3];
	unsigned int y2 = blobs[4];
	unsigned int x3 = blobs[6];
	unsigned int y3 = blobs[7];
	unsigned int x4 = blobs[9];
	unsigned int y4 = blobs[10];
	
	int rawStarData[8] = {x1, x2, x3, x4, y1, y2, y3, y4};
	robotCenter = localization_calc(rawStarData, robotCenterPrev);
	
	robotCenterPrev[2] = robotCenter[2];
	
	// Correct for offset discontinuity
	robotCenter[2] = theta_error_correction(robotCenter[2]);
	
	x = lowpass(ALPHA_LOW, x, robotCenter[0]);
	y = lowpass(ALPHA_LOW, y, robotCenter[1]);
	theta = lowpass(ALPHA_LOW, theta, robotCenter[2]);
	
	robotCenterPrev[0] = x;
	robotCenterPrev[1] = y;
}



/* PID Control */
void run_motor_control_loop(float x_target, float y_target, float max_duty_cycle, float max_theta, float theta_kp, float theta_kd, float linear_kp, float linear_kd)
{
	// Set theta target
	float theta_target = atan2(y_target-y,x_target-x);

	// Get theta error based output
	float theta_error = theta_target - theta;
	theta_error = theta_error_correction(theta_error);

	// Resctict turn radius based on max_theta
	if(fabs(theta_error)>max_theta){
		theta_error = fabs(theta_error)/theta_error*max_theta;
	}

	float derivative = (theta_error-previous_theta_error)/TIMESTEP;
	float angular_output = theta_kp*theta_error - theta_kd*derivative; //If output > 0, turn left
	previous_theta_error = theta_error;
	angular_output = angular_output/M_PI; //Normalize to max value of 1 given Kp gain of 1
	
	// Update duty cycle based on angle
	left_duty_cycle = - angular_output;
	right_duty_cycle = angular_output;
	
	// Get linear error based output
	float y_delta = y_target-y;
	float x_delta = x_target-x;
	float linear_error = fabs(cos(theta_error))*sqrt(y_delta*y_delta+x_delta*x_delta); // Gets component of translational error in direction bot is facing
	derivative = (linear_error-previous_linear_error)/TIMESTEP;
	float linear_output = linear_kp*linear_error - linear_kd*derivative;
	previous_linear_error = linear_error;
	if(linear_output>40.0){linear_output=40.0;}
	linear_output = linear_output/40; //Normalize to value of 1 at 40 pixels (~10 cm) given Kp gain of 1
	
	// Update duty cycle based on linear distance
	if (fabs(theta_error) < M_PI/2){
		left_duty_cycle += linear_output;
		right_duty_cycle += linear_output;
		} else{
		left_duty_cycle -= linear_output;
		right_duty_cycle -= linear_output;
	}
	
	// Set motor direction based on pos/neg
	if (left_duty_cycle<0){clear(PORTB,1);}
	else{set(PORTB,1);}
	
	if (right_duty_cycle<0){clear(PORTB,3);}
	else{set(PORTB,3);}
	
	// Set duty cycle
	left_duty_cycle = fabs(left_duty_cycle);
	right_duty_cycle = fabs(right_duty_cycle);
	
	// Get larger of two duty cycles
	float max = 0;
	if (left_duty_cycle > right_duty_cycle)
	{
		max = left_duty_cycle;
	} else
	{
		max = right_duty_cycle;
	}
	
	// Normalize duty cycles
	if (max > max_duty_cycle)
	{
		left_duty_cycle = left_duty_cycle/max*max_duty_cycle;
		right_duty_cycle = right_duty_cycle/max*max_duty_cycle;
	}

	// Update timer values
	OCR1B = ((float)OCR1A)*left_duty_cycle;
	OCR1C = ((float)OCR1A)*right_duty_cycle;

}

/************************************************************
Private Functions
************************************************************/

/* Lowpass Filter using Alpha_low */
float lowpass(float alpha, float previous_output, float reading)
{
	return ((float)reading*alpha +(1-alpha)*(float)previous_output);
}

/* Ensures that bot doesn't turn more that PI in search of theta_target */
float theta_error_correction(float error)
{
	if(fabs(error)>M_PI){return error-2.0*M_PI*error/fabs(error);}
	else{return error;}
}

/************************************************************
End of Function File
************************************************************/