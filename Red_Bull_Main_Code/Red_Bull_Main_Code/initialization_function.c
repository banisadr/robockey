// Initialization General Function

#include <avr/io.h>
#include "m_general.h"
#include "m_bus.h"
#include "m_rf.h"
#include "m_usb.h"
#include "m_wii.h"
#include "initialization_function.h"

void initialize_robockey(int self) {

/************************************************************
Initialization of Pins and System Clock
************************************************************/

	// Set to 16 MHz
	m_clockdivide(CLOCK_DIVIDE);

	// Disable JTAG
	m_disableJTAG(); // turn off JTAG port and allow access to F4-F7 as GPIO
	
	//Set to Input
	clear(DDRD,0); // D0
	clear(DDRD,1); // D1
	
	
	//Set to Output
	set(DDRB,0); // B0 Left motor enable
	set(DDRB,1); // B1 Left motor direction
	set(DDRB,2); // B2 Right motor enable
	set(DDRB,3); // B3 Right motor direction
	
	set(DDRD,3); //C6 Red positioning LED
	set(DDRD,5); //C7 Blue positioning LED 
	
	//Set pin low
	clear(PORTB,0);	//start with motor pins low
	clear(PORTB,1);
	clear(PORTB,2);
	clear(PORTB,3);
	
	clear(PORTC,6); //Start with positioning LED off
	clear(PORTC,7); 


/************************************************************
Setup Subsystems
************************************************************/

	m_bus_init();
	while(!m_wii_open());
	sei();
	m_usb_init();
	if (self == RED_BULL)
	{
		while(!m_rf_open(CHANNEL,RXADDRESS_RED_BULL,PACKET_LENGTH)); // Configure mRF
	}
	if (self == GREEN_MONSTER)
	{
		while(!m_rf_open(CHANNEL,RXADDRESS_GREEN_MONSTER,PACKET_LENGTH)); // Configure mRF
	}
	if (self == BLUE_WHALE)
	{
		while(!m_rf_open(CHANNEL,RXADDRESS_BLUE_WHALE,PACKET_LENGTH)); // Configure mRF
	}
	


/************************************************************
Timer1 Initialization for PWM Motor Control
************************************************************/

	//Timer initialization
	clear(TCCR1B,CS12);	//Set timer1 prescaler to /1
	clear(TCCR1B,CS11);
	clear(TCCR1B,CS10);
	
	clear(TCCR1B,WGM13);	//Use timer mode 4 (up to OCR1A)
	set(TCCR1B,WGM12);
	clear(TCCR1A,WGM11);
	clear(TCCR1A,WGM10);

	clear(TCCR1A,COM1B1);		//No change of B6
	clear(TCCR1A,COM1B0);

	OCR1A = CLOCK/PWM_FREQ;
	OCR1B = (float)OCR1A*0.1;
	OCR1C = (float)OCR1A*0.1;
	
	set(TIMSK1,OCIE1A); // OCR1A interrupt vector
	set(TIMSK1,OCIE1B); // OCR1B interrupt vector
	set(TIMSK1,OCIE1C); // OCR1C interrupt vector


/************************************************************
Timer3 Initialization for fixed timestep calculations
************************************************************/

	clear(TCCR3B,CS32); // prescale /1
	clear(TCCR3B,CS31);
	set(TCCR3B,CS30);

	clear(TCCR3B,WGM33); // Up to OCR3A (mode 4)
	set(TCCR3B,WGM32);
	clear(TCCR3A,WGM31);
	clear(TCCR3A,WGM30);
	
	OCR3A = TIMESTEP*(CLOCK/TIM3_PRESCALE); // initialize OCR3A or duration

/************************************************************
Timer0 Initialization for COMM test 
************************************************************/
	clear(TCCR0B,CS02); //start with timer off
	clear(TCCR0B,CS01);
	clear(TCCR0B,CS00);
	
	clear(TCCR0B,WGM02); //Up to OCR0A mode 
	clear(TCCR0A,WGM01);
	clear(TCCR0A,WGM00);
		
	set(TIMSK0,TOIE0); //enable interupt at OCR0A
	
	
/************************************************************
Setup ADC
************************************************************/
	
	clear(ADMUX,REFS1); // Set reference voltage to Vcc
	set(ADMUX,REFS0);
	
	clear(ADCSRA,ADPS2); // Set prescaler to /8
	set(ADCSRA,ADPS1);
	set(ADCSRA,ADPS0);
	
	// Disable Digital input to:
	set(DIDR0,ADC0D); // ADC0
	set(DIDR0,ADC1D); // ADC1
	set(DIDR0,ADC4D); // ADC4
	set(DIDR0,ADC5D); // ADC5
	set(DIDR0,ADC6D); // ADC6
	set(DIDR0,ADC7D); // ADC7
	set(DIDR2,ADC8D); // ADC8
	set(DIDR2,ADC9D); // ADC9
	set(DIDR2,ADC10D); // ADC10
	set(DIDR2,ADC11D); // ADC11
	set(DIDR2,ADC12D); // ADC812
	set(DIDR2,ADC13D); // ADC13
	
	clear(ADCSRA,ADATE); // Turn off 'free-running' mode
	
	set(ADCSRB,MUX5); // Select ADC13 at pin B6
	set(ADMUX,MUX2); // Transistor 4
	clear(ADMUX,MUX1);
	set(ADMUX,MUX0);
	
	set(ADCSRA,ADEN); // Enable ADC subsystem
	
	set(ADCSRA,ADSC); // Begin first conversion
	
}