// Initialization General Function

#include <avr/io.h>
#include "m_general.h"
#include "m_bus.h"
#include "m_rf.h"
#include "m_usb.h"
#include "m_wii.h"
#include "initialization_function.h"

void initialize_robockey(void) {

/************************************************************
Initialization of Pins and System Clock
************************************************************/

	// Set to 16 MHz
	m_clockdivide(CLOCK_DIVIDE);
	
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



/************************************************************
Setup USB
************************************************************/

	m_usb_init();



/************************************************************
Timer1 Initialization for PWM Motor Control
************************************************************/

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

	set(TIMSK3,OCIE3A); // OCR3A interrupt vector



/************************************************************
Initialize the Wireless System
************************************************************/

	m_bus_init(); // Enable mBUS
	m_rf_open(CHANNEL,RXADDRESS_1,PACKET_LENGTH); // Configure mRF



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

	
	set(ADCSRA,ADIE); // Enable interrupt for when conversion is finished
	
	clear(ADCSRA,ADATE); // Turn off 'free-running' mode
	
	clear(ADCSRB,MUX5); // Select ADC0 at pin F4
	clear(ADMUX,MUX2);
	clear(ADMUX,MUX1);
	clear(ADMUX,MUX0);
	
	set(ADCSRA,ADEN); // Enable ADC subsystem
	
	set(ADCSRA,ADSC); // Begin first conversion
}