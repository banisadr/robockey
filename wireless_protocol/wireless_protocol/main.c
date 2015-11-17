/************************************************************
Header Information
************************************************************/

/*
 * main.c
 *
 * Authors: Wyatt Shapiro, Pete Furlong, Bahram Banisadr
 * MEAM 510
 * Lab 7: Robockey
 */ 


/*****************************************************************
==================================================================
### INTERNAL README: ADD TO EVERYTIME BEFORE SENDING TO GITHUB ###

Modified by Bahram on 11/15/15

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

#define CHANNEL 1
#define RXADDRESS_1 0x20
#define RXADDRESS_2 0x21
#define RXADDRESS_3 0x22
#define PACKET_LENGTH 10

/************************************************************
Prototype Functions
************************************************************/

void init(void); // Initialize system clock & pins
void usb_enable(void); // Enable USB
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

char buffer[PACKET_LENGTH] = {0,0,0,0,0,0,0,0,0,0}; // Wifi input
char game_state = 0x00; // Stores game state
char SR = 0; // Score R
char SB = 0; // Score B

/************************************************************
Main Loop
************************************************************/

int main(void)
{
	/* Confirm Power */
	m_red(ON);

	/* Initializations */
	init();
	usb_enable();
	wireless_enable();

	/* Confirm successful initialization(s) */
	m_green(ON);
	
    while (1) 
    {}
}

/************************************************************
Subroutines and Functions
************************************************************/

/* Initialization of Pins and System Clock */
void init(void){
	m_clockdivide(0); // Set to 16 MHz
	
	sei(); // Enable global interrupts
}

/* Setup USB */
void usb_enable(void)
{
	m_usb_init();
	while(!m_usb_isconnected());
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
	state = buffer[0];
	
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
			SR = buffer[1]
			SB = buffer[2]
			pause();
			break;
		case 0xA3: // Goal B
			SR = buffer[1]
			SB = buffer[2]
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
}

void pause(void)
{
	// Stop within 3 seconds
}

void halftime(void)
{
	// Stop play
	// Switch assigned goal
}

void game_over(void);
{
	// Stop play
	// Do a victory dance based on score?
}

/************************************************************
Interrupts
************************************************************/

ISR(INT2_vect){
	wireless_recieve();
}

/************************************************************
End of Program
************************************************************/