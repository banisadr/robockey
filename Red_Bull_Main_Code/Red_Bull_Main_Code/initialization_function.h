//Initialization General Function

#ifndef initialization_function__
#define initialization_function__

/* Clock Values */
#define CLOCK_DIVIDE 0
#define CLOCK 16000000
#define TIM3_PRESCALE 1
#define TIMESTEP 0.001
#define TIM0_PRESCALE 1024

/* Filter Values */
#define ALPHA_LOW 0.99

/* Motor Driver Values */
#define PWM_FREQ 1500
#define DUTY_CYCLE_PUCK 0.5
#define DUTY_CYCLE_SEEK 0.5

/* Wireless Comms */
#define CHANNEL 1
#define RXADDRESS_1 0x20
#define RXADDRESS_2 0x21
#define RXADDRESS_3 0x22
#define PACKET_LENGTH 10

/* Puck Finding */
#define HAS_PUCK_THRESHOLD 400
#define PUCK_VECTOR_LEN 700

/* Goal */
#define GOAL_X_DIST 400
#define OFF  0
#define BLUE 1
#define RED  2

#include <avr/io.h>
#include "m_general.h"
#include "m_bus.h"
#include "m_rf.h"
#include "m_usb.h"
#include "m_wii.h"
#include "localization_function.h"

void initialize_robockey(void);

#endif