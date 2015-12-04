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
Modified by Bahram on 12/1/15 Modularized Initialization and Motor
==================================================================
*****************************************************************/


/************************************************************
Included Files & Libraries
************************************************************/
#include "m_general.h"
#include "m_bus.h"
#include "m_rf.h"
#include "m_usb.h"
#include "m_wii.h"
#include "localization_function.h"
#include "initialization_function.h"
#include "motor_control_function.h"
#include "puck_location_function.h"

/************************************************************
Definitions
************************************************************/

// In initialization_function.h

/************************************************************
Prototype Functions
************************************************************/

/* Other */
void motor_update(void);
void adc_update(void);
void bot_behavior_update(void);

/* Wireless Comms */
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
float max_duty_cycle = 0.99;
float max_theta = M_PI;

/* Positioning */
float x_target = 0;
float y_target = 0;

/* PD Controller Values */
float theta_kp  = 1.0;
float theta_kd  = 0.0;
float linear_kp = 0.20;
float linear_kd = 0.00;

/* Wireless Comms */
char buffer[PACKET_LENGTH] = {0,0,0,0,0,0,0,0,0,0}; // Wifi input
char game_state = 0x00; // Stores game state
char SR = 0; // Score R
char SB = 0; // Score B

/* Goal */
float x_goal = 0;
float y_goal = 0;

/* Puck */
float x_puck = 0;
float y_puck = 0;

/************************************************************
Main Loop
************************************************************/
int main(void)
{
	/* Confirm Power */
	m_red(ON);

	/* Initializations */
	initialize_robockey();
	pause();

	/* Confirm successful initialization(s) */
	m_green(ON);

	/* Run */
	while (1){
		update_position();
		if (check(ADCSRA,ADIF)){adc_update();} // Check if ADC conversion has completed
		bot_behavior_update();
		if (check(TIFR3,OCF3A)){motor_update();}	// Check if timestep has completed
	}
}

/************************************************************
Initialization of Subsystem Components
************************************************************/

/* Called On Timer 3 Rollover */
void motor_update(void)
{
	set(TIFR3,OCF3A);		// Reset flag
	run_motor_control_loop(x_target, y_target, max_duty_cycle, max_theta, theta_kp, theta_kd, linear_kp, linear_kd); // Update control
}

/* Update Targets, Gains, and Max Vals */
void bot_behavior_update()
{
	if (has_puck())
	{
		x_target = x_goal;
		y_target = y_goal;
		max_theta = M_PI/3;
		max_duty_cycle = 0.7;
		return;
	}
	
	if (!has_puck())
	{
		x_target = x_puck;
		y_target = y_puck;
		max_theta = M_PI;
		max_duty_cycle = 0.99;
		return;
	}
}

/* Called on ADC Conversion Completion */
void adc_update(void)
{
	set(ADCSRA,ADIF);	 // Reset flag
	if(adc_switch()){
		float puck_buffer[2];
		get_puck_location(puck_buffer);
		x_puck = puck_buffer[0];
		y_puck = puck_buffer[1];
	}
}

/* Recieve Wireless Data */
void wireless_recieve(void)
{
	m_rf_read(buffer,PACKET_LENGTH); // Read RF Signal
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
		play();
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
	/* Assign Defending goal */
	update_position();
	float position_buffer[3];
	get_position(position_buffer);
	if (position_buffer[0]>0) {
		x_goal = -1*GOAL_X_DIST;
		} else {
		x_goal = GOAL_X_DIST;
	}
	/* Flash color of LED for defending goal */
}

void play(void)
{
	/* Light LED of defending goal */
	
	/* Play */
	set(TCCR1B,CS10);
}

void pause(void)
{
	/* Stop within 3  seconds */
	clear(TCCR1B,CS10);
	clear(PORTB,0); // B0 Left motor off
	clear(PORTB,2); // B2 Right motor off
}

void halftime(void)
{
	/* Stop play */
	pause();
	
	/* Switch assigned goal */
	if (x_goal>0) {
		x_goal = -1*GOAL_X_DIST;
		} else {
		x_goal = GOAL_X_DIST;
	}
	
}

void game_over(void)
{
	// Stop play
	pause();
	
	// Do a victory dance based on score?
	
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