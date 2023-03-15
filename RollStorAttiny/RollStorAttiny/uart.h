//#ifndef UART_H_
//#define  UART_H_
#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>

#include "motor.h"
#include "timer.h"

//UART set speed
#define baudrate(x) F_CPU/(16UL*x)-1
#define BUFSIZE 6

void UartInit(uint16_t baud);
void UART_SendB(int8_t b);
void UART_SendDW(uint32_t st);


void UART(MOTOR *m);

//#endif /*UART_H_*/