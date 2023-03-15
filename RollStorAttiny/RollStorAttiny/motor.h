//#ifndef MOTOR_H_
//#define MOTOR_H_
#pragma once

#include <stdbool.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include "timer.h"
//#include "uart.h"
#include "button.h"

#define MOTOR_PORT PORTB
#define MOTOR_PIN PINB
#define FUSE_PORT PIND
#define FUSE_DVER 1 << PD2
#define FUSE_OKNO 1 << PD3

typedef enum {IDLE1, IDLE2, OPEN, CLOSE, MOVE_TO, CALIBRATION, ERROR} MOTOR_STATE;
	
typedef struct 
{
	volatile uint8_t *PORT;
	volatile uint8_t pin;
} FUSE_PIN;
	
typedef struct
{
	FUSE_PIN fuse;
	volatile uint8_t *PORT;
	volatile uint8_t offset;
	MOTOR_STATE STATE;
	uint8_t change_state_motor;
	uint8_t step;
	uint32_t curent_step_count;
	uint16_t last_time;
} MOTOR;

#ifdef DEBUG
#define step_delay_ms 400
#else
#define step_delay_ms 2
#endif

//motor define
#define step1 0b0001
#define step2 0b0011
#define step3 0b0010
#define step4 0b0110
#define step5 0b0100
#define step6 0b1100
#define step7 0b1000
#define step8 0b1001

#define oborot 4096 //1 oborot = 4096 steps

uint32_t max_step_count_motor_okno;
uint32_t max_step_count_motor_dver;

void MotorInit(void);
void MotorProc(MOTOR *motor, BUTTON *button, uint32_t *max_steps, uint32_t *emem);

void UART(MOTOR *m);
void UART_SendDW(uint32_t st);

//#endif /*MOTOR_H_*/
