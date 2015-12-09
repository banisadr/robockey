/************************************************************
Header Information
************************************************************/
/* 
* Wireless Comms General Function
*
*/

/*****************************************************************
==================================================================
### INTERNAL README: ADD TO EVERYTIME BEFORE SENDING TO GITHUB ###

Modified by Bahram on 12/8/15 to functionize wireless protocols

==================================================================
*****************************************************************/


/************************************************************
Included Files & Libraries
************************************************************/

#include "m_general.h"
#include "initialization_function.h"


/************************************************************
Private Function Definitions
************************************************************/

/************************************************************
Global Variables
************************************************************/


/************************************************************
Public Functions
************************************************************/

/* Send Wireless Data */
void wireless_send(int sending_bot, char* send_buffer)
{
	if (sending_bot == RED_BULL)
	{
		m_rf_send(RXADDRESS_BLUE_WHALE,send_buffer,PACKET_LENGTH); // Send RF Signal
		m_rf_send(RXADDRESS_GREEN_MONSTER,send_buffer,PACKET_LENGTH); // Send RF Signal
	}
	if (sending_bot == GREEN_MONSTER)
	{
		m_rf_send(RXADDRESS_BLUE_WHALE,send_buffer,PACKET_LENGTH); // Send RF Signal
		m_rf_send(RXADDRESS_RED_BULL,send_buffer,PACKET_LENGTH); // Send RF Signal	
	}
	if (sending_bot == BLUE_WHALE)
	{
		m_rf_send(RXADDRESS_GREEN_MONSTER,send_buffer,PACKET_LENGTH); // Send RF Signal
		m_rf_send(RXADDRESS_RED_BULL,send_buffer,PACKET_LENGTH); // Send RF Signal
	}
}

/************************************************************
Private Functions
************************************************************/

/************************************************************
End of Function File
************************************************************/