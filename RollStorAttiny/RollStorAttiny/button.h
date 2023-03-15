//#ifndef BUTTON_H_
//#define BUTTON_H_
#pragma once

#include <avr/io.h>
#include "timer.h"

#define BUT_PORT PORTD
#define BUT_PIN PIND
#define BUT_1 1<<PD4
#define BUT_2 1<<PD5
#define BUT_3 1<<PD6

#define CLICK_TIME 50 //ms
#define LONG_CLICK_TIME 1000 //ms 1sec
#define LONG_LONG_CLICK_TIME 4000 //ms

typedef enum {UNPUSHED, CLICK, LONG_CLICK, LONG_LONG_CLICK} BUTTON_STATUS;

typedef struct
{
	volatile uint8_t *PORT;
	volatile uint8_t PIN;
	BUTTON_STATUS CURRENT_STATUS;
	BUTTON_STATUS STATUS;
	uint16_t press_time;
} BUTTON;


void ButtonsInit(void);
BUTTON_STATUS GetButStatus(BUTTON *bt);
void ButtonProc(BUTTON *bt);

//#endif /* BUTTON_H_ */