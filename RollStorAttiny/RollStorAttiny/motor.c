
#include "motor.h"

uint32_t e_max_step_count_motor_okno EEMEM;
uint32_t e_max_step_count_motor_dver EEMEM;

const uint8_t steps[] = {step1, step2, step3, step4, step5, step6, step7, step8};//øàãè âðàùåíèå

ISR(INT0_vect)
{
}

ISR(INT1_vect)
{
}

void step_f(MOTOR *motor)
{
	uint8_t tmp;
	++motor->curent_step_count;
	tmp = *((motor->PORT)-2); //PORTB - 2 == PINB
	tmp &= ~(0x0F << motor->offset);
	*(motor->PORT) = tmp | (steps[motor->step] << motor->offset);
	if(--motor->step > 7) {motor->step = 7;}
}

void step_b(MOTOR *motor)
{
	uint8_t tmp;
	--motor->curent_step_count;
	tmp = *((motor->PORT)-2); //PORTB - 2 == PINB
	tmp &= ~(0x0F << motor->offset);
	*(motor->PORT) = tmp | (steps[motor->step] << motor->offset);
	if(++motor->step > 7) {motor->step = 0;}
}


void MotorInit()
{
	max_step_count_motor_okno = 0;
	max_step_count_motor_dver = 0;
	//ïèíû íà âûõîä, äëÿ óïðàâëåíèÿ øàãîâûìè ìîòîðàìè
	DDRB = 0xFF;//óïð. uln2803
	
	if (!(PIND & (1<<PD6))) //сброс калибровок удерживая кнопку PD6 при reset on power
	{
		eeprom_write_dword(&e_max_step_count_motor_okno, 0);
		eeprom_write_dword(&e_max_step_count_motor_dver, 0);
	}
	
	max_step_count_motor_okno = eeprom_read_dword(&e_max_step_count_motor_okno);
	max_step_count_motor_dver = eeprom_read_dword(&e_max_step_count_motor_dver);
	
	//íàñòðàèâàåì ïðåðûâàíèÿ íà êîíöåâèêè ïî íèçêîìó óðîâíþ
	//GIFR |= (1<<INTF1) | (1<<INTF0); //ñáðàñûâàåì ôëàãè ïðåðûâàíèÿ
	//GIMSK |= (1<<INT1) | (1<<INT0); //âêëþ÷àåì ïðåðûâàíèå
}

void MotorProc(MOTOR *motor, BUTTON *button, uint32_t *max_steps_count, uint32_t *emem) 
{
	UART(motor);
	uint16_t now_time = GetTickNow();
	BUTTON_STATUS BTN = GetButStatus(button);
	uint8_t tmp;

	if(*max_steps_count == 0){motor->STATE = CALIBRATION;} //проверяем калибровку(если счетчик максимального кол-ва шагов пуст)
		
	switch(motor->STATE)
	{
		case IDLE1:
		if(BTN == CLICK  || (motor->change_state_motor & 0x80))
		{
			if(motor->change_state_motor)
			{
				motor->STATE = MOVE_TO;
			}
			else
			{
				motor->STATE = OPEN;
			}
			//motor->STATE = OPEN;
			//motor->last_time = now_time - step_delay_ms;
		}
		break;
		
		case IDLE2:
		if(BTN == CLICK  || (motor->change_state_motor & 0x80))
		{
			if(motor->change_state_motor)
			{
				motor->STATE = MOVE_TO;
			}
			else
			{
				motor->STATE = CLOSE;
			}
			//motor->STATE = CLOSE;
			//motor->last_time = now_time - step_delay_ms;
		}
		break;
		
		case OPEN:
		if((uint16_t)(now_time - motor->last_time) >= step_delay_ms)
		{
			step_b(motor);
			motor->last_time = now_time;
		}
		if(BTN == CLICK || !(*(motor->fuse.PORT) & motor->fuse.pin) || motor->curent_step_count == 0 || (motor->change_state_motor & 0x80)) //либо кнопка либо концевик либо счетчик шагов либо внешняя команда по UART
		{
			//motor->change_state_motor = false;
			if (!(*(motor->fuse.PORT) & motor->fuse.pin)) //если концевик сработал раньше счетчика
			{
				motor->curent_step_count = 0;
			}
			if(motor->change_state_motor)
			{
				motor->STATE = MOVE_TO;
			}
			else
			{
				motor->STATE = IDLE2;
			}
			//motor->STATE = IDLE2;
			if(--motor->step > 7) {motor->step = 7;}
			if(--motor->step > 7) {motor->step = 7;}
			*(motor->PORT) &= ~(0x0F << motor->offset);
			UART_SendDW(motor->curent_step_count);
		}
		break;
		
		case CLOSE:
		if((uint16_t)(now_time - motor->last_time) >= step_delay_ms)
		{
			step_f(motor);
			motor->last_time = now_time;
		}
		if(BTN == CLICK || motor->curent_step_count >= *max_steps_count || (motor->change_state_motor & 0x80))
		{
			//motor->change_state_motor = false;
			if(motor->change_state_motor)
			{
				motor->STATE = MOVE_TO;
			}
			else
			{
				motor->STATE = IDLE1;
			}
			if(++motor->step > 7) {motor->step = 0;}
			if(++motor->step > 7) {motor->step = 0;}
			*(motor->PORT) &= ~(0x0F << motor->offset);
			UART_SendDW(motor->curent_step_count);
		}
		break;
		
		case MOVE_TO:
		if(motor->change_state_motor & 0x80 && BTN != CLICK)
		{
			*max_steps_count = eeprom_read_dword(emem);
			motor->change_state_motor &= 0x7F;
			*max_steps_count = *max_steps_count / 100 * motor->change_state_motor;
			if (motor->curent_step_count >  *max_steps_count)
			{
				motor->STATE = OPEN;
			}
			else if (motor->curent_step_count < *max_steps_count)
			{
				motor->STATE = CLOSE;
			}
			else
			{
				*max_steps_count = eeprom_read_dword(emem);
				motor->STATE = IDLE1;
			}
		}
		else
		{
			motor->change_state_motor = 0;
			*max_steps_count = eeprom_read_dword(emem);
			motor->STATE = IDLE1;
			*(motor->PORT) &= ~(0x0F << motor->offset);
		}
		
		break;

		case CALIBRATION:
		if(GetButStatus(button) == CLICK)
		{
			while( *(motor->fuse.PORT) & motor->fuse.pin )
			{
				now_time = GetTickNow();
				if((uint16_t)(now_time - motor->last_time) >= step_delay_ms)
				{
					++motor->curent_step_count;
					tmp = *((motor->PORT)-2); //PORTB - 2 == PINB
					tmp &= ~(0x0F << motor->offset);
					*(motor->PORT) = tmp | (steps[motor->step] << motor->offset);
					if(++motor->step > 7) {motor->step = 0;}
					motor->last_time = now_time;
				}
			}
			eeprom_write_dword(emem, motor->curent_step_count);
			*max_steps_count = motor->curent_step_count;
			motor->curent_step_count = 0;
			motor->STATE = IDLE2;
			if(--motor->step > 7) {motor->step = 7;}
			if(--motor->step > 7) {motor->step = 7;}
			*(motor->PORT) &= ~(0x0F << motor->offset);
		}
		break;
		
		case ERROR:
		MOTOR_PORT = 0;
		while(1);
		break;
		
		default:
		*(motor->PORT) &= ~(0x0F << motor->offset);
	}
}

/*

void MotorOne(BUTTON *bt)
{
	static motor_status state = IDLE1;
	static uint8_t step = 0;
	static uint16_t last_time;
	uint16_t now_time = GetTickNow();

	if(max_step_count_motor_one == 0){state = CALIBRATION;} //проверяем калибровку
	
	switch(state)
	{
		case IDLE1:
		if(GetButStatus(&BUTTON_1) == CLICK || change_state_motor_one)
		{
			change_state_motor_one = false;
			state = OPEN;
			last_time = now_time - step_delay_ms;
		}
		break;
		
		case IDLE2:
		if(GetButStatus(&BUTTON_1) == CLICK || change_state_motor_one)
		{
			change_state_motor_one = false;
			state = CLOSE;
			last_time = now_time - step_delay_ms;
		}
		break;
		
		case OPEN:
		if((uint16_t)(now_time - last_time) >= step_delay_ms)
		{
			--curent_step_count_motor_one;
			int8_t tmp = PINB;
			tmp &= 0xF0;
			//asm("nop");asm("nop");
			PORTB = tmp | steps[step];
			if(++step > 7) {step = 0;}
			last_time = now_time;
		}
		if(GetButStatus(&BUTTON_1) == CLICK || !(PIND & (1<<PD2)) || curent_step_count_motor_one == 0 || change_state_motor_one) //либо кнопка либо концевик либо счетчик шагов либо внешняя команда по UART
		{
			change_state_motor_one = false;
			if((PIND & (1<<PD2)) && curent_step_count_motor_one > max_step_count_motor_one){break;} //если после сброса питания неизвестно положение, то поднимаем в 0 точку
			curent_step_count_motor_one = 0;
			state = IDLE2;
			if(--step > 7) {step = 7;}
			if(--step > 7) {step = 7;}
			PORTB &= 0xF0;
		}
		break;
		
		case CLOSE:
		if((uint16_t)(now_time - last_time) >= step_delay_ms)
		{
			++curent_step_count_motor_one;
			int8_t tmp = PINB;
			tmp &= 0xF0;
			//asm("nop");asm("nop");
			PORTB = tmp | steps[step];
			if(--step > 7) {step = 7;}
			last_time = now_time;
		}
		if(GetButStatus(&BUTTON_1) == CLICK || curent_step_count_motor_one == max_step_count_motor_one || change_state_motor_one)
		{
			change_state_motor_one = false;
			state = IDLE1;
			if(++step > 7) {step = 0;}
			if(++step > 7) {step = 0;}
			PORTB &= 0xF0;
		}
		break;

		case CALIBRATION:
		if(GetButStatus(&BUTTON_1) == CLICK)
		{
			while( (PIND & (1<<PD2)) )
			{
				now_time = GetTickNow();
				if((uint16_t)(now_time - last_time) >= step_delay_ms)
				{
					++curent_step_count_motor_one;
					int8_t tmp = PINB;
					tmp &= 0xF0;
					//asm("nop");asm("nop");
					PORTB = tmp | steps[step];
					if(++step > 7) {step = 0;}
					last_time = now_time;
				}
			}
			eeprom_write_dword(&e_max_step_count_motor_one, curent_step_count_motor_one);
			max_step_count_motor_one = curent_step_count_motor_one;
			curent_step_count_motor_one = 0;
			state = IDLE2;
			if(--step > 7) {step = 7;}
			//if(--step > 7) {step = 7;}
			PORTB &= 0xF0;
		}
		break;
		
		case MOVE_TO:
		break;
		
		case ERROR:
		break;
		
		default:
		PORTB &= 0xF0;
	}
}

void MotorTwo(BUTTON *bt)
{
	static motor_status state = IDLE1;
	static uint8_t step = 0;
	static uint16_t last_time;
	uint16_t now_time = GetTickNow();

	if(max_step_count_motor_two == 0){state = CALIBRATION;}
	
	switch(state)
	{
		case IDLE1:
		if(GetButStatus(&BUTTON_2) == CLICK)
		{
			state = OPEN;
			last_time = now_time - step_delay_ms;
		}
		break;
		
		case IDLE2:
		if(GetButStatus(&BUTTON_2) == CLICK)
		{
			state = CLOSE;
			last_time = now_time - step_delay_ms;
		}
		break;
		
		case OPEN:
		if((uint16_t)(now_time - last_time) >= step_delay_ms)
		{
			--curent_step_count_motor_two;
			int8_t tmp = PINB;
			tmp &= 0x0F;
			//asm("nop");asm("nop");
			PORTB = tmp | steps[step]<<4;
			if(++step > 7) {step = 0;}
			last_time = now_time;
		}
		if(GetButStatus(&BUTTON_2) == CLICK || !(PIND & (1<<PD3)) || curent_step_count_motor_two == 0) //либо кнопка либо концевик
		{
			curent_step_count_motor_two = 0;
			state = IDLE2;
			if(--step > 7) {step = 7;}
			if(--step > 7) {step = 7;}
			PORTB &= 0x0F;
		}
		break;
		
		case CLOSE:
		if((uint16_t)(now_time - last_time) >= step_delay_ms)
		{
			++curent_step_count_motor_two;
			int8_t tmp = PINB;
			tmp &= 0x0F;
			//asm("nop");asm("nop");
			PORTB = tmp | steps[step]<<4;
			if(--step > 7) {step = 7;}
			last_time = now_time;
		}
		if(GetButStatus(&BUTTON_2) == CLICK || curent_step_count_motor_two == max_step_count_motor_two)
		{
			state = IDLE1;
			if(++step > 7) {step = 0;}
			if(++step > 7) {step = 0;}
			PORTB &= 0x0F;
		}
		break;

		case CALIBRATION:
		if(GetButStatus(&BUTTON_2) == CLICK)
		{
			while( (PIND & (1<<PD3)) )
			{
				now_time = GetTickNow();
				if((uint16_t)(now_time - last_time) >= step_delay_ms)
				{
					++curent_step_count_motor_two;
					int8_t tmp = PINB;
					tmp &= 0x0F;
					//asm("nop");asm("nop");
					PORTB = tmp | steps[step]<<4;
					if(++step > 7) {step = 0;}
					last_time = now_time;
				}
			}
			eeprom_write_dword(&e_max_step_count_motor_two, curent_step_count_motor_two);
			max_step_count_motor_two = curent_step_count_motor_two;
			curent_step_count_motor_two = 0;
			state = IDLE2;
			if(--step > 7) {step = 7;}
			//if(--step > 7) {step = 7;}
			PORTB &= 0x0F;
		}
		break;
		
		default:
		PORTB &= 0x0F;
	}
}

*/