// Puck Location General Function

#ifndef puck_location_function__
#define puck_location_function__

#include "m_general.h"
#include "initialization_function.h"

char adc_switch(void); // Switch ADC read pin - if returns 1, all pins have read
char has_puck(void); // Returns 1 if has puck - 0 otherwise
void get_puck_location(float* puck_buffer); // Returns vector pointing to puck in global coordinates

#endif