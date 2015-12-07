/************************************************************
Header Information
************************************************************/
/* 
* Puck Location General Function
*
* ADC switch - iterates over all ADC pins, returns 1 when complete
* Has Puck - returns 1 when have puck
*/

/*****************************************************************
==================================================================
### INTERNAL README: ADD TO EVERYTIME BEFORE SENDING TO GITHUB ###

Modified by Bahram on 12/2/15 to switch over ADC pins

==================================================================
*****************************************************************/


/************************************************************
Included Files & Libraries
************************************************************/

#include "m_general.h"
#include "initialization_function.h"
#include "motor_control_function.h"


/************************************************************
Private Function Definitions
************************************************************/


/************************************************************
Global Variables
************************************************************/

/* Other */
static int state = 0; // Used to control ADC switching
static float transistor_angles[10] = {-M_PI/2, -2*M_PI/9, -M_PI/9, 0.0, M_PI/9, 2*M_PI/9, M_PI/2, 5*M_PI/6, M_PI, -5*M_PI/6};

/* Phototransistor Voltages */
static int transistor_vals[10] = {0,0,0,0,0,0,0,0,0,0};
static int transistor_3 = 0;
static int transistor_9 = 0;
/*
0 transistor_1_16
1 transistor_2
2 transistor_4
3 transistor_5_6_7
4 transistor_8
5 transistor_10
6 transistor_11_12
7 transistor_13
8 transistor_14
9 transistor_15
*/


/************************************************************
Public Functions
************************************************************/

/* Returns x,y, location of puck (angle more accurate) */
void get_puck_location(float* puck_buffer)
{
	float position_buffer[3]; // x, y, theta buffer
	get_position(position_buffer); // get x, y, theta

	/* Calculate Weighted Average of angles & Max Value */
	int max_val = 0;
	int scaler = 0;
	float global_theta = 0;
	for(int i=0; i<10; i++){
		scaler += transistor_vals[i];
		global_theta += transistor_angles[i] * (float)transistor_vals[i];

		if(transistor_vals[i]>max_val){
			max_val = transistor_vals[i];
		}
	}
	global_theta = global_theta/((float)scaler);


	/* If no puck found */
	if(max_val<100){
		puck_buffer[0] = 0;
		puck_buffer[1] = 0;
		puck_buffer[2] = 0;
		return;
	}

	/* Create vector pointing to puck in global coordinates */
	global_theta += position_buffer[2];
	puck_buffer[0] = cos(global_theta)*PUCK_VECTOR_LEN*(1023-max_val)/200.0 + position_buffer[0]; // Assign X val
	puck_buffer[1] = sin(global_theta)*PUCK_VECTOR_LEN*(1023-max_val)/200.0 + position_buffer[1]; // Assign Y val
	puck_buffer[2] = max_val;

}

/* Return 1 If Has Puck */
char has_puck(void)
{
	static int had_puck = 5;
	if(check(ADCSRA,ADEN)){		//check if ADC is enabled 
		if(((transistor_3 + transistor_9) > HAS_PUCK_THRESHOLD)){
			had_puck = 5;
			return 1;
		}
		if(had_puck){
			had_puck -= 1;
			return 1;
		}
	}
	return 0;
}

/* Control ADC Pin Switching */
char adc_switch(void)
{
	char return_val = 0; // Return if last pin has been updated

	clear(ADCSRA,ADEN); // Disable ADC subsystem
	
	switch(state){

		case 0:
			state = 1;
			clear(ADCSRB,MUX5); // Select ADC0 at pin F0
			clear(ADMUX,MUX2); // Transistor 8
			clear(ADMUX,MUX1);
			clear(ADMUX,MUX0);
			transistor_vals[2] = ADC;			
			break;

		case 1:
			state = 2;
			clear(ADCSRB,MUX5); // Select ADC1 at pin F1
			clear(ADMUX,MUX2); // Transistor 9
			clear(ADMUX,MUX1);
			set(ADMUX,MUX0);
			transistor_vals[4] = ADC;			
			break;

		case 2:
			state = 3;
			clear(ADCSRB,MUX5); // Select ADC4 at pin F4
			set(ADMUX,MUX2); // Transistor 10
			clear(ADMUX,MUX1);
			clear(ADMUX,MUX0);
			transistor_9 = ADC;			
			break;

		case 3:
			state = 4;
			clear(ADCSRB,MUX5); // Select ADC5 at pin F5
			set(ADMUX,MUX2); // Transistor 13
			clear(ADMUX,MUX1);
			set(ADMUX,MUX0);
			transistor_vals[5] = ADC;			
			break;

		case 4:
			state = 5;
			clear(ADCSRB,MUX5); // Select ADC6 at pin F6
			set(ADMUX,MUX2); // Transistor 14
			set(ADMUX,MUX1);
			clear(ADMUX,MUX0);
			transistor_vals[7] = ADC;			
			break;

		case 5:
			state = 6;
			clear(ADCSRB,MUX5); // Select ADC7 at pin F7
			set(ADMUX,MUX2); // Transistor 15
			set(ADMUX,MUX1);
			set(ADMUX,MUX0);
			transistor_vals[8] = ADC;			
			break;

		case 6:
			state = 7;
			set(ADCSRB,MUX5); // Select ADC8 at pin D4
			clear(ADMUX,MUX2); // Transistor 1, 16
			clear(ADMUX,MUX1);
			clear(ADMUX,MUX0);
			transistor_vals[9] = ADC;			
			break;

		case 7:
			state = 8;
			set(ADCSRB,MUX5); // Select ADC9 at pin D6
			clear(ADMUX,MUX2); // Transistor 11, 12
			clear(ADMUX,MUX1);
			set(ADMUX,MUX0);
			transistor_vals[0] = ADC;			
			break;

		case 8:
			state = 9;
			set(ADCSRB,MUX5); // Select ADC10 at pin D7
			clear(ADMUX,MUX2); // Transistor 5, 6, 7
			set(ADMUX,MUX1);
			clear(ADMUX,MUX0);
			transistor_vals[6] = ADC;			
			break;

		case 9:
			state = 10;
			set(ADCSRB,MUX5); // Select ADC11 at pin B4
			clear(ADMUX,MUX2); // Transistor 2
			set(ADMUX,MUX1);
			set(ADMUX,MUX0);
			transistor_vals[3] = ADC;			
			break;

		case 10:
			state = 11;
			set(ADCSRB,MUX5); // Select ADC12 at pin B5
			set(ADMUX,MUX2); // Transistor 3
			clear(ADMUX,MUX1);
			clear(ADMUX,MUX0);
			transistor_vals[1] = ADC;			
			break;

		case 11:
			state = 0;
			set(ADCSRB,MUX5); // Select ADC13 at pin B6
			set(ADMUX,MUX2); // Transistor 4
			clear(ADMUX,MUX1);
			set(ADMUX,MUX0);
			transistor_3 = ADC;			
			return_val = 1;

			// m_usb_tx_string("\nIR1,16: ");
			// m_usb_tx_int(transistor_vals[0]);
			// m_usb_tx_string("  IR2: ");
			// m_usb_tx_int(transistor_vals[1]);
			// m_usb_tx_string("  IR3: ");
			// m_usb_tx_int(transistor_3);
			// m_usb_tx_string("  IR4: ");
			// m_usb_tx_int(transistor_vals[2]);
			// m_usb_tx_string("  IR5,6,7: ");
			// m_usb_tx_int(transistor_vals[3]);
			// m_usb_tx_string("  IR8: ");
			// m_usb_tx_int(transistor_vals[4]);
			// m_usb_tx_string("  IR9: ");
			// m_usb_tx_int(transistor_9);
			// m_usb_tx_string("  IR10: ");
			// m_usb_tx_int(transistor_vals[5]);
			// m_usb_tx_string("  IR11,12: ");
			// m_usb_tx_int(transistor_vals[6]);
			// m_usb_tx_string("  IR13: ");
			// m_usb_tx_int(transistor_vals[7]);
			// m_usb_tx_string("  IR14: ");
			// m_usb_tx_int(transistor_vals[8]);
			// m_usb_tx_string("  IR15: ");
			// m_usb_tx_int(transistor_vals[9]);

			break;
	}
	
	set(ADCSRA,ADEN); // Enable ADC subsystem
	set(ADCSRA,ADSC); // Begin new conversion

	return return_val;
}

/************************************************************
Private Functions
************************************************************/


/************************************************************
End of Function File
************************************************************/