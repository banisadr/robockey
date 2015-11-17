/*
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
		
		m_usb_tx_char('\n');
		m_usb_tx_uint(x1);
		m_usb_tx_char('\t');
		m_usb_tx_uint(x2);
		m_usb_tx_char('\t');
		m_usb_tx_uint(x3);
		m_usb_tx_char('\t');
		m_usb_tx_uint(x4);
		m_usb_tx_char('\t');
		m_usb_tx_uint(y1);
		m_usb_tx_char('\t');
		m_usb_tx_uint(y2);
		m_usb_tx_char('\t');
		m_usb_tx_uint(y3);
		m_usb_tx_char('\t');
		m_usb_tx_uint(y4);
    }
}


void init() 
{
	m_red(ON);
	while(!m_wii_open());
	m_red(OFF);
	
	m_usb_init();
}

