/*
 * RollStorAttiny.c
 *
 * Created: 26.08.2021 20:10:20
 * Author : user
 */ 
#include <avr/io.h>

#include "timer.h"
#include "button.h"
#include "motor.h"
#include "uart.h"

extern uint32_t e_max_step_count_motor_okno EEMEM;
extern uint32_t e_max_step_count_motor_dver EEMEM;

BUTTON BUTTON_DVER = {.PORT = &BUT_PIN, .PIN = BUT_1, .CURRENT_STATUS = UNPUSHED, .STATUS = UNPUSHED, .press_time = 0};
BUTTON BUTTON_OKNO = {.PORT = &BUT_PIN, .PIN = BUT_2, .CURRENT_STATUS = UNPUSHED, .STATUS = UNPUSHED, .press_time = 0};
//BUTTON BUTTON_SERVIS = {.PORT = &BUT_PIN, .PIN = BUT_3, .CURRENT_STATUS = UNPUSHED, .STATUS = UNPUSHED, .press_time = 0};
	
MOTOR MOTOR_DVER = {.fuse.PORT = &FUSE_PORT, .fuse.pin = FUSE_DVER, .PORT = &MOTOR_PORT, .offset = 0, .STATE = IDLE1, .change_state_motor = 0, .step = 0, .curent_step_count = 0, .last_time = 0};
MOTOR MOTOR_OKNO = {.fuse.PORT = &FUSE_PORT, .fuse.pin = FUSE_OKNO, .PORT = &MOTOR_PORT, .offset = 4, .STATE = IDLE1, .change_state_motor = 0, .step = 0, .curent_step_count = 0, .last_time = 0};

//void ClearBuf(uint8_t* buf, uint8_t len);

int main(void)
{
	ACSR |= (1<<ACD);//analog compare off
	
	TIMER0_INIT();
	ButtonsInit();
	MotorInit();
	UartInit(baudrate(9600));
	
    sei();
	
    for(;;)
    {
		ButtonProc(&BUTTON_DVER);
		ButtonProc(&BUTTON_OKNO);
		MotorProc(&MOTOR_DVER, &BUTTON_DVER, &max_step_count_motor_dver, &e_max_step_count_motor_dver);
		MotorProc(&MOTOR_OKNO, &BUTTON_OKNO, &max_step_count_motor_okno, &e_max_step_count_motor_okno);
    }//while
}

		/*
		if (fBufRead)
		{
			uint32_t step = 0;
			uint8_t id, action;
			fBufRead = 0;
			id = BUF_RX[0]; // 0-okno 1-dver
			action = BUF_RX[1]; //open-1 close-3
			step = ((uint32_t)BUF_RX[2] << 24) | ((uint32_t)BUF_RX[3] << 16) | ((uint32_t)BUF_RX[4] << 8) | (BUF_RX[5]);
			ClearBuf(BUF_RX, BUFSIZE);
			if (id == 0)
			{
				step_okno = step;
				switch_state_okno = action;
				steps_count_okno = 0;
			}
			else if(id == 1)
			{
				step_dver = step;
				switch_state_dver = action;
				steps_count_dver = 0;
			}
		}
		*/


/*
void ClearBuf(uint8_t* buf, uint8_t len)
{
	for(uint8_t i=0; i<len; ++i)
	{
		buf[i] = 0;
	}
}
*/
/*

#define idle1 0
#define open 1
#define idle2 2
#define close 3

void stDver(uint8_t state, uint32_t* step)
{
	static uint8_t i = 0;
	switch (state)
	{
		case idle1:
		case idle2:
			PORTB &= 0xF0;
			if (steps_count_dver > 0)
			{
				UART_Send(steps_count_dver);
				//steps_count_dver = 0;
			}
			break;
		
		case open:
			if (tick_ms_dver >= step_delay_ms && *step > 0)
			{
				tick_ms_dver = 0;
				PORTB &= 0xF0;
				asm("nop");asm("nop");
				PORTB = steps[i];
				if(++i > 7) {i = 0;}
				if(--*step == 0) switch_state_dver = idle2;
				--steps_count_dver;
			}
			break;
		case close:
			if (tick_ms_dver >= step_delay_ms && *step > 0)
			{
				tick_ms_dver = 0;
				PORTB &= 0xF0;
				asm("nop");asm("nop");
				PORTB = steps[i];
				if(--i > 7) {i = 7;}
				if(--*step == 0) switch_state_dver = idle1;
				++steps_count_dver;
			}
			break;
		default:
			PORTB &= 0xF0;
	}
}
void stOkno(uint8_t state, uint32_t* step)
{
	static uint8_t i = 0;
	
	switch (state)
	{
		case idle1:
		case idle2:
			PORTB &= 0x0F;
			if (steps_count_okno > 0)
			{
				UART_Send(steps_count_okno);
				//steps_count_okno = 0;
			}
			break;
		
		case open:
			if (tick_ms_okno >= step_delay_ms && *step > 0)
			{
				tick_ms_okno = 0;
				PORTB &= 0x0F;
				asm("nop");asm("nop");
				PORTB = steps[i]<<4;
				if(++i > 7) {i = 0;}
				if(--*step == 0) switch_state_okno = idle2;
				--steps_count_okno;
			}

			break;
		case close:
			if (tick_ms_okno >= step_delay_ms && *step > 0)
			{
				tick_ms_okno = 0;
				PORTB &= 0x0F;
				asm("nop");asm("nop");
				PORTB = steps[i]<<4;
				if(--i > 7) {i = 7;}
				if(--*step == 0) switch_state_okno = idle1;
				++steps_count_okno;
			}

			break;
		default:
			PORTB &= 0x0F;
	}
}
*/
/*
void Open(uint8_t id_m, uint32_t steps)
{
	switch(id_m)
	{
		case MOTOR_DVER:
			while(steps--)
			{
				MotorForward(id_m);
			}
			PORTB &= 0xF0;
			break;
			
		case MOTOR_OKNO:
			while(steps--)
			{
				MotorForward(id_m);
			}
			PORTB &= 0x0F;
			break;
	}
}

void Close(uint8_t id_m, uint32_t steps)
{
	switch(id_m)
	{
		case MOTOR_DVER:
		while(steps--)
		{
			MotorBackward(id_m);
		}
		PORTB &= 0xF0;
		break;
		
		case MOTOR_OKNO:
		while(steps--)
		{
			MotorBackward(id_m);
		}
		PORTB &= 0x0F;
		break;
	}
}

inline void MotorForward(uint8_t id)
{
	uint8_t port_mask, i;
	
	for (i=0;i<8;i++)
	{
		port_mask = PORTB;
		if(!id) port_mask &= 0xF0;
		else port_mask &= 0x0F;
		port_mask |= (steps[i] << id);
		PORTB = port_mask;
		_delay_ms(delay_ms);
	}
}

inline void MotorBackward(uint8_t id)
{
	uint8_t port_mask, i;
	
	for (i=0;i<8;i++)
	{
		port_mask = PORTB;
		if(!id) port_mask &= 0xF0;
		else port_mask &= 0x0F;
		port_mask |= (steps_r[i] << id);
		PORTB = port_mask;
		_delay_ms(delay_ms);
	}
}*/
/*
void StateDver()
{
	static uint8_t state_dver = idle;
	static uint8_t i = 0;
	static uint16_t current_step = 0;
	static uint8_t switch_state = 0;
	
	switch (state_dver)
	{
	case idle:
		if (fBut1 == 1) //если кнопка нажата
		{
			state_dver = close;
			fBut1 = 0;
			tick_ms_dver = delay_ms;
			i=0;
		}
		break;
	case open:
		if (tick_ms_dver >= delay_ms && step_dver > 0)
		{
			tick_ms_dver = 0;
			PORTB &= 0xF0;
			PORTB = steps[i];
			if(++i > 7) {i = 0; step_dver--;}
			--current_step;
		}
		else if(tick_ms_dver >= delay_ms && step_dver == 0)
		{
			PORTB &= 0xF0;
			state_dver = idle;
		}
		break;
	case close:
		if (tick_ms_dver >= delay_ms && step_dver > 0)
		{
			tick_ms_dver = 0;
			PORTB &= 0xF0;
			PORTB = steps_r[i];
			if(++i > 7) {i = 0; step_dver--;}
			++current_step;
		}
		else if(tick_ms_dver >= delay_ms && step_dver == 0)
		{
			PORTB &= 0xF0;
			state_dver = idle;
		}
		break;
	}
}
*/
/*
void StateOkno()
{
	static uint8_t state_okno = idle;
	static uint8_t i = 0;
	
	switch (state_okno)
	{
		case idle:
			if (fBut2 == 1) //если кнопка нажата
			{
				fBut2 = 0;
				tick_ms_okno = delay_ms;
				i=0;
				state_okno = open;
			}
			break;
		case open:
			if (tick_ms_okno >= delay_ms && step_okno > 0)
			{
				tick_ms_okno = 0;
				PORTB &= 0x0F;
				PORTB = steps[i]<<4;
				if(++i > 7) {i = 0; step_okno--;}
			}
			else if(tick_ms_okno >= delay_ms && step_okno == 0)
			{
				PORTB &= 0x0F;
				state_okno = idle;
			}
			break;
		case close:
			if (tick_ms_okno >= delay_ms && step_okno > 0)
			{
				tick_ms_okno = 0;
				PORTB &= 0x0F;
				PORTB = steps_r[i]<<4;
				if(++i > 7) {i = 0; step_okno--;}
			}
			else if(tick_ms_okno >= delay_ms && step_okno == 0)
			{
				PORTB &= 0x0F;
				state_okno = idle;
			}
			break;
	}
}
*/