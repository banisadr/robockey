/************************************************************
Header Information
************************************************************/

/*
* main.c
*
* Authors: Wyatt Shapiro, Pete Furlong, Bahram Banisadr
* MEAM 510
* Robockey
*/


/*****************************************************************
==================================================================
### INTERNAL README: ADD TO EVERYTIME BEFORE SENDING TO GITHUB ###
Modified by Bahram on 11/18/2015 go to given X Y location
==================================================================
*****************************************************************/


/************************************************************
Included Files & Libraries
************************************************************/
#include <avr/io.h>
#include "m_general.h"
#include "m_bus.h"
#include "m_rf.h"
#include "m_usb.h"

/************************************************************
Definitions
************************************************************/

/* Clock Values */
#define CLOCK_DIVIDE 0
#define CLOCK 16000000
#define TIM3_PRESCALE 1
#define TIMESTEP 0.001

/* Filter Values */
#define ALPHA_LOW 0.9

/* Motor Driver Values */
#define PWM_FREQ 1000

/* PD Controller Values */
#define THETA_Kp 2.33
#define THETA_Kd 0.01
#define LINEAR_Kp 0.5
#define LINEAR_Kd 0.1

/* Other */
#define PI 3.14159265359

/************************************************************
Prototype Functions
************************************************************/

void init(void); //Setup I/O, clockspeed, IMU, Interrupts
void usb_enable(void); //Setup USB
void timer1_init(void); //Setup timer1 for motor PWM control
void timer3_init(void); //Setup timer3 for fixed timestep calculations
void update_position(void); //Get IMU data, filter, update angle, update control
void run_control_loop(void); //Run Control Loop
int lowpass(float alpha, int previous_output, int reading); //Lowpass filter
float theta_error_correction(float error); // Ensures that bot turns efficiently


/************************************************************
Global Variables
************************************************************/

/* Motor Control */
float left_duty_cycle = 0.1;
float right_duty_cycle = 0.1;

/* Positioning */
float x = 0;
float y = 0;
float theta = 0;
float x_target = -300;
float y_target = 0;
float theta_target = PI;

/* PD Controler Values */
float previous_theta_error = 0;
float previous_linear_error = 0;

/************************************************************
Main Loop
************************************************************/
int main(void)
{
	/* Confirm Power */
	m_red(ON);

	/* Initializations */
	init();
	//usb_enable();
	timer1_init();
	timer3_init();

	/* Confirm successful initialization(s) */
	m_green(ON);

	/* Run */
	while (1){
		update_position();
	}
}

/************************************************************
Initialization of Subsystem Components
************************************************************/

/* Initialization of Pins and System Clock */
void init(void){
	
	m_clockdivide(CLOCK_DIVIDE); // Set to 2 MHz
	
	//Set to Input
	clear(DDRD,0); // D0
	clear(DDRD,1); // D1
	clear(DDRD,2); // D2
	
	
	//Set to Output
	set(DDRB,0); // B0 Left motor enable
	set(DDRB,1); // B1 Left motor direction
	set(DDRB,2); // B2 Right motor enable
	set(DDRB,3); // B3 Right motor direction
	
	//Set pin low
	clear(PORTB,0);	//start with motor pins low
	clear(PORTB,1);
	clear(PORTB,2);
	clear(PORTB,3);
	
	m_bus_init();
	sei();
}

/* Setup USB */
void usb_enable(void)
{
	m_usb_init();
	//while(!m_usb_isconnected());
}

/* Timer1 Initialization for PWM Motor Control */
void timer1_init(void)
{
	//Timer initialization
	clear(TCCR1B,CS12);	//Set timer1 prescaler to /1
	clear(TCCR1B,CS11);
	set(TCCR1B,CS10);
	
	clear(TCCR1B,WGM13);	//Use timer mode 4 (up to OCR1A)
	set(TCCR1B,WGM12);
	clear(TCCR1A,WGM11);
	clear(TCCR1A,WGM10);

	clear(TCCR1A,COM1B1);		//No change of B6
	clear(TCCR1A,COM1B0);

	OCR1A = CLOCK/PWM_FREQ;
	OCR1B = (float)OCR1A*left_duty_cycle;
	OCR1C = (float)OCR1A*right_duty_cycle;
	
	set(TIMSK1,OCIE1A); // OCR1A interrupt vector
	set(TIMSK1,OCIE1B); // OCR1B interrupt vector
	set(TIMSK1,OCIE1C); // OCR1C interrupt vector
}

/* Timer3 Initialization for fixed timestep calculations */
void timer3_init(void)
{
	clear(TCCR3B,CS32); // prescale /1
	clear(TCCR3B,CS31);
	set(TCCR3B,CS30);

	clear(TCCR3B,WGM33); // Up to OCR3A (mode 4)
	set(TCCR3B,WGM32);
	clear(TCCR3A,WGM31);
	clear(TCCR3A,WGM30);
	
	OCR3A = TIMESTEP*(CLOCK/TIM3_PRESCALE); // initialize OCR3A or duration
}


/* Lowpass Filter using Alpha_low */
int lowpass(float alpha, int previous_output, int reading)
{
	return (int)((float)reading*alpha +(1-alpha)*(float)previous_output);
}

/* Get x, y, theta, filterupdate control */
void update_position(void)
{

	#TODO: location = [x, y, theta]
	
	x = lowpass(ALPHA_LOW, x, location[0]);
	y = lowpass(ALPHA_LOW, y, location[1]);
	theta = lowpass(ALPHA_LOW, theta, location[2]);
	
	if (check(TIFR3,OCF3A)){	// Check if timestep has completed
		set(TIFR3,OCF3A);		// Reset flag
		theta_target = atan2(y_target-y,x_target-x); // Update theta target
		run_control_loop(); // Update control
	}
}

/* PID Control (Currently only for theta) */
void run_control_loop(void)
{
	// Get theta error based output
	float theta_error = theta_target - theta;
	theta_error = theta_error_correction(theta_error);
	float derivative = (theta_error-previous_theta_error)/TIMESTEP;
	float output = THETA_Kp*theta_error - THETA_Kd*derivative; //If output > 0, turn left
	previous_theta_error = theta_error;
	output = output/PI; //Normalize to max value of 1 given Kp gain of 1
	
	// Update duty cycle based on angle
	if (output > 0){
		left_duty_cycle = - output;
		right_duty_cycle = output;
	} else{
		left_duty_cycle = output;
		right_duty_cycle = -output;
	}
	
	// Get linear error based output
	float y_delta = y_target-y;
	float x_delta = x_target-x;
	float linear_error = cos(abs(theta_error))*sqrt(y_delta*y_delta+x_delta*x_delta); // Gets component of translational error in direction bot is facing
	derivative = (linear_error-previous_linear_error)/TIMESTEP;
	output = LINEAR_Kp*linear_error - LINEAR_Kd*derivative;
	previous_linear_error = linear_error;
	output = output/40; //Normalize to value of 1 at 40 pixles (~10 cm) given Kp gain of 1
	
	// Update duty cycle based on linear distance
	if (abs(theta_error) < PI/2){
		left_duty_cycle += output;
		right_duty_cycle += output;
		} else{
		left_duty_cycle -= output;
		right_duty_cycle -= output;
	}
	
	// Set motor direction based on pos/neg
	if (left_duty_cycle<0){set(PORTB,1);}
	else{clear(PORTB,1);}
		
	if (right_duty_cycle<0){set(PORTB,3);}
	else{clear(PORTB,3);}
	
	
	// Set duty cycle
	left_duty_cycle = abs(left_duty_cycle)/(PI);
	if (left_duty_cycle > 1) {left_duty_cycle = 1;}
	
	right_duty_cycle = abs(right_duty_cycle)/(PI);
	if (right_duty_cycle > 1) {	right_duty_cycle = 1;}
	
	// Update timer values
	OCR1B = (float)OCR1A*left_duty_cycle;
	OCR1C = (float)OCR1A*right_duty_cycle;
}

/* Ensures that bot doesn't turn more that PI in search of theta_target */
float theta_error_correction(float error)
{
	if(abs(error)>PI){return error-2.0*PI*error/abs(error);}
	else{return error;}
}

/************************************************************
Interrupts
************************************************************/

/* Motor PWM Control (Enable both at rollover) */
ISR(TIMER1_COMPA_vect){
	set(PORTB,0); // B0 Left motor enable
	set(PORTB,2); // B2 Right motor enable	
}

/* Motor PWM Control (Disable left at TCNT1 = OCR1B) */
ISR(TIMER1_COMPB_vect){
	clear(PORTB,0); // B0 Left motor disable
}

/* Motor PWM Control (Disable right at TCNT1 = OCR1C) */
ISR(TIMER1_COMPC_vect){
	clear(PORTB,2); // B2 Right motor disable
}

/************************************************************
End of Program
************************************************************/