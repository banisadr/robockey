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
Modified by Bahram on 11/18/2015 go to given X Y location (errors)
Modified by All on 11/18/2015 basic linear & angle control working
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
#include "m_wii.h"
#include "localization_function.h"

/************************************************************
Definitions
************************************************************/

/* Clock Values */
#define CLOCK_DIVIDE 0
#define CLOCK 16000000
#define TIM3_PRESCALE 1
#define TIMESTEP 0.001

/* Filter Values */
#define ALPHA_LOW 0.99

/* Motor Driver Values */
#define PWM_FREQ 1000

/* PD Controller Values */
#define THETA_Kp  1.0
#define THETA_Kd  0.0
#define LINEAR_Kp 0.20
#define LINEAR_Kd 0.00

/* Wireless Comms */
#define CHANNEL 1
#define RXADDRESS_1 0x20
#define RXADDRESS_2 0x21
#define RXADDRESS_3 0x22
#define PACKET_LENGTH 10

/* Other */
#define PI 3.14159265359
#define MAX_DUTY_CYCLE 0.4

/************************************************************
Prototype Functions
************************************************************/

void init(void); //Setup I/O, clockspeed, IMU, Interrupts
void usb_enable(void); //Setup USB
void timer1_init(void); //Setup timer1 for motor PWM control
void timer3_init(void); //Setup timer3 for fixed timestep calculations
void update_position(void); //Get IMU data, filter, update angle, update control
void run_control_loop(void); //Run Control Loop
float lowpass(float alpha, float previous_output, float reading); //Lowpass filter
float theta_error_correction(float error); // Ensures that bot turns efficiently

void wireless_enable(void); // Initialize the wireless system
void wireless_recieve(void); // Send data to slave
void update_game_state(void); // Update game state

/* Reactions to Game States */
void comm_test(void);
void play(void);
void pause(void);
void halftime(void);
void game_over(void);


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
float x_target = -350;
float y_target = 0;
float theta_target = 0;

/* PD Controler Values */
float previous_theta_error = 0;
float previous_linear_error = 0;

/* Wii Camera */
unsigned int blobs[12];
float robotCenterPrev[3] = {1023, 1023, 360};
float* robotCenter;

/* Wireless Comms*/
char buffer[PACKET_LENGTH] = {0,0,0,0,0,0,0,0,0,0}; // Wifi input
char game_state = 0x00; // Stores game state
char SR = 0; // Score R
char SB = 0; // Score B
int game_pause = 1; // If true, bot will not move

/************************************************************
Main Loop
************************************************************/
int main(void)
{
	/* Confirm Power */
	m_green(ON);
	m_red(ON);

	/* Initializations */
	init();
	usb_enable();
	wireless_enable();
	timer1_init();
	timer3_init();

	/* Confirm successful initialization(s) */
//	m_green(ON);

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
	while(!m_wii_open());
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
float lowpass(float alpha, float previous_output, float reading)
{
	return ((float)reading*alpha +(1-alpha)*(float)previous_output);
}

/* Get x, y, theta, filterupdate control */
void update_position(void)
{
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
	
	if (check(TIFR3,OCF3A)){	// Check if timestep has completed
		set(TIFR3,OCF3A);		// Reset flag
		theta_target = atan2(y_target-y,x_target-x); // Update theta target
		run_control_loop(); // Update control
	}
	
	robotCenterPrev[0] = x;
	robotCenterPrev[1] = y;
}

/* PID Control (Currently only for theta) */
void run_control_loop(void)
{
	// Get theta error based output
	float theta_error = theta_target - theta;
	theta_error = theta_error_correction(theta_error);
	float derivative = (theta_error-previous_theta_error)/TIMESTEP;
	float angular_output = THETA_Kp*theta_error - THETA_Kd*derivative; //If output > 0, turn left
	previous_theta_error = theta_error;
	angular_output = angular_output/PI; //Normalize to max value of 1 given Kp gain of 1
	
	// Update duty cycle based on angle
	left_duty_cycle = - angular_output;
	right_duty_cycle = angular_output;
	
	// Get linear error based output
	float y_delta = y_target-y;
	float x_delta = x_target-x;
	float linear_error = fabs(cos(theta_error))*sqrt(y_delta*y_delta+x_delta*x_delta); // Gets component of translational error in direction bot is facing
	derivative = (linear_error-previous_linear_error)/TIMESTEP;
	float linear_output = LINEAR_Kp*linear_error - LINEAR_Kd*derivative;
	previous_linear_error = linear_error;
	if(linear_output>40.0){linear_output=40.0;}
	linear_output = linear_output/40; //Normalize to value of 1 at 40 pixels (~10 cm) given Kp gain of 1
	
	// Update duty cycle based on linear distance
	if (fabs(theta_error) < PI/2){
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
	if (max > MAX_DUTY_CYCLE)
	{
		left_duty_cycle = left_duty_cycle/max*MAX_DUTY_CYCLE;
		right_duty_cycle = right_duty_cycle/max*MAX_DUTY_CYCLE;
	}
	
	// Check game state
	if (game_pause)
	{
		left_duty_cycle = 0;
		right_duty_cycle = 0;
	}
	
	// Update timer values
	OCR1B = ((float)OCR1A)*left_duty_cycle;
	OCR1C = ((float)OCR1A)*right_duty_cycle;

}

/* Ensures that bot doesn't turn more that PI in search of theta_target */
float theta_error_correction(float error)
{
	if(fabs(error)>PI){return error-2.0*PI*error/fabs(error);}
	else{return error;}
}

/* Initialize the Wireless System */
void wireless_enable(void)
{
	m_bus_init(); // Enable mBUS
	m_rf_open(CHANNEL,RXADDRESS_1,PACKET_LENGTH); // Configure mRF
}

/* Recieve Wireless Data */
void wireless_recieve(void)
{
	m_rf_read(buffer,PACKET_LENGTH); // Read RF Signal
// 	m_usb_tx_string("\n Game state: ");
// 	m_usb_tx_char(buffer[0]);
	game_state = buffer[0];
	
	
	update_game_state();
}

/* Update Game State Based on Comm Protocol */
void update_game_state(void)
{
	switch(game_state){
		case 0xA0: // Comm Test
		comm_test();
		break;
		case 0xA1: // Play
	//		m_green(TOGGLE);
			play();
			update_position();
			if (x>0) {
				x_target = -350;
			} else {
				x_target = 350;
			}
			break;
		case 0xA2: // Goal R
		SR = buffer[1];
		SB = buffer[2];
		pause();
		break;
		case 0xA3: // Goal B
		SR = buffer[1];
		SB = buffer[2];
		pause();
		break;
		case 0xA4: // Pause
		pause();
		break;
		case 0xA6: // Halftime
		halftime();
		break;
		case 0xA7: // Game Over
		game_over();
		break;
		case 0xA8: // Enemy Positions
		break;
		default: // Invalid Comm
		break;
	}
}

void comm_test(void)
{
	// Assign Defending goal
	// Flash color of LED for defending goal
}

void play(void)
{
	// Light LED of defending goal
	// Play
	game_pause = 0;
}

void pause(void)
{
	// Stop within 3  seconds
	game_pause = 1;
}

void halftime(void)
{
	// Stop play
	// Switch assigned goal
	game_pause = 1;
}

void game_over(void)
{
	// Stop play
	// Do a victory dance based on score?
	game_pause = 1;
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

/* Recieve Wireless Comm */
ISR(INT2_vect){
	wireless_recieve();
}

/************************************************************
End of Program
************************************************************/