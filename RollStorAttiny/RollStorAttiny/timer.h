//#ifndef TIMER_H_
//#define TIMER_H_
#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>

void TIMER0_INIT(void);
uint16_t GetTickNow(void);

//#endif