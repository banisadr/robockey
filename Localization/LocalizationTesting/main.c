/*9
 * LocalizationTesting.c
 *
 * Created: 11/17/2015 2:27:33 PM
 * Author : Pete
 */ 

#include "m_general.h"
#include "m_wii.h"
#include "m_bus.h"
#include "m_usb.h"
#include "m_rf.h"

void init(); 

int main(void)
{
	init();
	m_green(ON);
	unsigned int blobs[12];
	char rx_buffer;
	
    while (1) 
    {
		
		m_wii_read(blobs);
		unsigned int x1 = blobs[0];
		unsigned int y1 = blobs[1];
		unsigned int x2 = blobs[3];
		unsigned int y2 = blobs[4];
		unsigned int x3 = blobs[6];
		unsigned int y3 = blobs[7];
		unsigned int x4 = blobs[9];
		unsigned int y4 = blobs[10];
		
		while(!m_usb_rx_available());  			// Wait for an indication from the computer
		rx_buffer = m_usb_rx_char();  			// Read the packet from the computer
		
		m_usb_rx_flush();  						// Flush the buffer
		
		if(rx_buffer == 1)						// MATLAB is expecting IMU data
		{
			m_red(TOGGLE);
			m_usb_tx_int((int)x1);
			m_usb_tx_string(" ");
			m_usb_tx_int((int)x2);
			m_usb_tx_string(" ");
			m_usb_tx_int((int)x3);
			m_usb_tx_string(" ");
			m_usb_tx_int((int)x4);
			m_usb_tx_string(" ");
			m_usb_tx_int((int)y1);
			m_usb_tx_string(" ");
			m_usb_tx_int((int)y2);
			m_usb_tx_string(" ");
			m_usb_tx_int((int)y3);
			m_usb_tx_string(" ");
			m_usb_tx_int((int)y4);
			m_usb_tx_string("\n");

		}
		
		
    }
}


void init() 
{
	m_red(ON);
	while(!m_wii_open());
	m_red(OFF);
	
	m_usb_init();
}

