/*
 * Motor_Test_Code.c
 *
 * Created: 12/7/2015 6:56:17 PM
 * Author : Pete
 */ 

#include "m_general.h"
#define CLOCK 16000000
#define PWM_FREQ 1000



int main(void)
{
	m_clockdivide(0);
	
	/************************************************************
	Timer1 Initialization for PWM Motor Control
	************************************************************/
	set(DDRB,0);
	set(DDRB,1);
	set(DDRB,2);
	set(DDRB,3);
	
	
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
	
	
	
	clear(PORTB,1);
	set(PORTB,3);

	set(TCCR1B,CS10);
	
    while (1) 
    {
    }
}

/* Motor PWM Control (Enable both at rollover) */
ISR(TIMER1_COMPA_vect){
	clear(PORTB,0); // B0 Left motor enable
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

