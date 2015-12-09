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
#include "wireless_comms_function.h"

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
void select_goal(void); //chooses goal direction based on position
void attack_action();	//dictates attack behavior
int goalie_action(int goalie_home);	//dictates goalie behavior
float home_dist();
void assign_positions(void);

/* Wireless Comms */
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
/* Other */
int self = RED_BULL;

/* Motor Control */
float max_duty_cycle = 0.7;
float max_theta = M_PI;

/* Positioning */
float x_target = 0;
float y_target = 0;

/* PD Controller Values */
float theta_kp  = 1.0;
float theta_kd  = 0.00;
float linear_kp = 0.20;
float linear_kd = 0.01;

/* Wireless Comms */
char buffer[PACKET_LENGTH] = {0,0,0,0,0,0,0,0,0,0}; // Wifi input
char send_buffer[PACKET_LENGTH] = {0xA9,0,0,0,0,0,0,0,0,0};
char game_state = 0x00; // Stores game state
char SR = 0; // Score R
char SB = 0; // Score B
int wifi_flag = 0;
int tim0_counts = 0;
int role = ATTACK; //

/* Goal */
float x_goal = 0;
float y_goal = 0;
int goal = 0;
int goal_init = 0;

/* Puck */
float x_puck = 0;
float y_puck = 0;
int puck_dist = 0;

/* Role Decision Making */
char team_puck_capture_buffer[3] = {0,0,0};
char team_puck_dist_buffer[3] = {0,0,0};

/************************************************************
Main Loop
************************************************************/
int main(void)
{
	/* Confirm Power */
	m_red(ON);

	/* Initializations */
	initialize_robockey(self);
	send_buffer[1] = (char)self;
	pause();

	/* Confirm successful initialization(s) */
	m_green(ON);

	/* Run */
	while (1){
		update_position();
		if (check(ADCSRA,ADIF)){adc_update();} // Check if ADC conversion has completed
		bot_behavior_update();
		if (check(TIFR3,OCF3A)){motor_update();}	// Check if timestep has completed
		if (wifi_flag) {
			wifi_flag = 0;
			m_red(TOGGLE);
			m_rf_read(buffer,PACKET_LENGTH); // Read RF Signal
			update_game_state();			
		}
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
	static int goalie_home = 0;
	switch (role) {
		case ATTACK: 
			attack_action();
			break;		
		case GOALIE: 
			goalie_home = goalie_action(goalie_home);
			break;
			
	}
}

/* Called on ADC Conversion Completion */
void adc_update(void)
{
	set(ADCSRA,ADIF);	 // Reset flag
	if(adc_switch()){
		float puck_buffer[2];
		puck_dist = get_puck_location(puck_buffer);
		x_puck = puck_buffer[0];
		y_puck = puck_buffer[1];
		
		//Update other bots on status
		send_buffer[2] = team_puck_capture_buffer[self];
		send_buffer[3] = (char)(puck_dist/4);
		team_puck_dist_buffer[self] = (char)(puck_dist/4);
		wireless_send(self,send_buffer);
	}
}

/* Update Game State Based on Comm Protocol */
void update_game_state(void)
{
	switch(buffer[0]){
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
		case 0xA9: ;// Receiving comms from other bots
			int incoming_bot = buffer[1];
			team_puck_capture_buffer[incoming_bot] = buffer[2];
			team_puck_dist_buffer[incoming_bot] = buffer[3];
			break;
		default: // Invalid Comm
			break;
	}
}

void comm_test(void)
{
	if (!goal_init){
		select_goal();
	}
	/* Flash color of LED for defending goal */
	set(TCCR0B,CS02); //start timer 
	set(TCCR0B,CS00);
	positioning_LED(goal); //turn on LED 
	
}

void play(void)
{
	/* Light LED of defending goal */
	
	/* Play */
	set(TCCR1B,CS10);
	positioning_LED(goal);

}

void pause(void)
{
	/* Stop within 3  seconds */
	clear(TCCR1B,CS10);
	clear(PORTB,0); // B0 Left motor off
	clear(PORTB,2); // B2 Right motor off
	positioning_LED(OFF);
	
}

void halftime(void)
{
	/* Stop play */
	pause();
	
	/* Switch assigned goal */
	if (x_goal>0) {
		x_goal = -1*GOAL_X_DIST;
		goal = RED;
		} else {
		x_goal = GOAL_X_DIST;
		goal = BLUE;
	}
	
}

void game_over(void)
{
	// Stop play
	pause();
	
	// Do a victory dance based on score?	
}

void select_goal(void) 
{
	goal_init = 1;
	/* Assign Defending goal */
	update_position();
	
	float position_buffer[3];
	get_position(position_buffer);
	//m_usb_tx_int((int)position_buffer[0]);
	
	if (position_buffer[0]>0) {
		x_goal = -1*GOAL_X_DIST;
		goal = RED;
		//positioning_LED(RED);
		} else {
		x_goal = GOAL_X_DIST;
		goal = BLUE;
		//positioning_LED(BLUE);
	}
	
	goal_init = 1;
}

void attack_action(){
	if (has_puck())
	{
		team_puck_capture_buffer[self] = 1;
		x_target = x_goal;
		y_target = y_goal;
		max_theta = M_PI/2;
		theta_kd = 0.05;
		theta_kp = 1.2;
		linear_kd = 0.01;
		linear_kp = 0.2;
		max_duty_cycle = DUTY_CYCLE_PUCK;
		m_green(OFF);
		return;
	
	}

	if (!has_puck())
	{
		team_puck_capture_buffer[self] = 0;
		x_target = x_puck;
		y_target = y_puck;
		max_theta = M_PI;
		theta_kd = 0;
		theta_kp = 1.8;
		linear_kd = 0.01;
		linear_kp = 0.2;
		max_duty_cycle = DUTY_CYCLE_SEEK;
		m_green(ON);
		return;
	}
}

int goalie_action(int goalie_home)
{
	/* If the puck is within range, become an attacker and get the puck */
	if (puck_dist > 90){	
		role = ATTACK;
		return goalie_home;
	}
	
	float dist = home_dist();	//find distance from own goal
	//m_usb_tx_string("\n distance");
	//m_usb_tx_int((int) dist);
	
	/* If the goalie is far from our goal, and hasn't registered as home, return home*/
	if (dist > 150 && (!goalie_home)) {	
		x_target = -x_goal*0.8;
		y_target = y_goal;
		max_theta = M_PI;
		theta_kd = 0.01;
		theta_kp = 1.8;
		linear_kd = 0.01;
		linear_kp = 0.2;
		max_duty_cycle = DUTY_CYCLE_PUCK;
		m_green(ON)
		
		if (dist < 155){	//If you are within 5 pixels of home, consider yourself home
			goalie_home = 1;
		}
		return goalie_home;
	}
	
	/* If goalie is home, turn off linear motion */
	linear_kd = 0.0;
	linear_kp = 0.0;
	theta_kd = 0.02;
	theta_kp = 0.8;
	
	/* If the robot drifts away from its own goal, return home*/
	if (dist > 160){
		goalie_home = 0;
		return goalie_home;
	}
	
	/* if the puck is visible, face the puck */
	if (puck_dist>60){
		x_target = x_puck;
		y_target = y_puck;
		return goalie_home;
	}
	
	/*otherwise, face the center */
	x_target = 0;
	y_target = 0;

	m_green(OFF);
	return goalie_home;
}

float home_dist()
{	
	/* Calculates the distance to from your own goal */
	float position_buffer[3];
	get_position(position_buffer);
	float x = position_buffer[0];
	float y = position_buffer[1];
	return sqrtf((x+x_goal)*(x+x_goal)+(y-y_goal)*(y-y_goal));
}

void assign_positions(void)
{
	if(self == RED_BULL)
	{
		role = ATTACK;
	}
	if(self == GREEN_MONSTER)
	{
		role = GOALIE;
	}
	if(self == BLUE_WHALE)
	{
		role = ATTACK;
	}
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
	wifi_flag = 1;
}

ISR(TIMER0_OVF_vect){
	
	if (tim0_counts < 20) {
		tim0_counts++; //increment timer counter
		
	} else {
		positioning_LED(OFF); //turn LED off
		
		clear(TCCR0B,CS02); //turn timer off
		clear(TCCR0B,CS01);
		clear(TCCR0B,CS00);
	}
}

/************************************************************
End of Program
************************************************************/