
#include "button.h"

//BUTTON BUTTON_1;
//BUTTON BUTTON_2;
//BUTTON BUTTON_3;

//BUTTON BUTTON_V = {.PORT = &BUT_PIN, .PIN = BUT_1, .CURRENT_STATUS = UNPUSHED, .STATUS = UNPUSHED, .press_time = 0};
//BUTTON BUTTON_R = {.PORT = &BUT_PIN, .PIN = BUT_2, .CURRENT_STATUS = UNPUSHED, .STATUS = UNPUSHED, .press_time = 0};
//BUTTON BUTTON_P = {.PORT = &BUT_PIN, .PIN = BUT_3, .CURRENT_STATUS = UNPUSHED, .STATUS = UNPUSHED, .press_time = 0};
	
void ButtonsInit(void)
{
	//включаем подтяжку
	BUT_PORT |= BUT_1 | BUT_2 | BUT_3;//pull up
	
	//настраиваем прерывание на кнопках
	//GIMSK |= 1<<PCIE2;
	//PCMSK2 = (1<<PCINT17) | (1<<PCINT16) | (1<<PCINT15);
	//GIFR |= 1<<PCIF2;
}

BUTTON_STATUS GetButStatus(BUTTON *bt)
{
	BUTTON_STATUS tmp = bt->STATUS;
	bt->STATUS = UNPUSHED;
	return tmp;
}

void ButtonProc(BUTTON *bt)
{
	//PIN PULL-UP +5V
	//PIN == 0 == PUSHED
	//PIN == 1 == UNPUSHED
	uint16_t now_time = GetTickNow();

	switch(bt->CURRENT_STATUS)
	{
		case UNPUSHED:
		if (!(*(bt->PORT) & bt->PIN))
		{
			bt->CURRENT_STATUS = CLICK;
			bt->press_time = now_time;
		}
		break;
		
		case CLICK:
		if ((*(bt->PORT) & bt->PIN))
		{
			if ((uint16_t)(now_time - bt->press_time) >= CLICK_TIME)
			{
				bt->STATUS = CLICK;
			}
			bt->CURRENT_STATUS = UNPUSHED;
		}
		else
		{
			if ((uint16_t)(now_time - bt->press_time) >= LONG_CLICK_TIME)
			{
				bt->CURRENT_STATUS = LONG_CLICK;
			}
		}
		break;
		
		case LONG_CLICK: // > 1sec
		if ((*(bt->PORT) & bt->PIN))
		{
			if ((uint16_t)(now_time - bt->press_time) >= LONG_CLICK_TIME/*1500*/)
			{
				bt->STATUS = LONG_CLICK;
			}
			bt->CURRENT_STATUS = UNPUSHED;
		}
		else
		{
			if ((uint16_t)(now_time - bt->press_time) >= LONG_LONG_CLICK_TIME)
			{
				bt->STATUS = LONG_LONG_CLICK;
				bt->CURRENT_STATUS = LONG_LONG_CLICK;
			}
		}
		break;
		
		case LONG_LONG_CLICK: // > 4sec
		if ((*(bt->PORT) & bt->PIN))
		{
			bt->CURRENT_STATUS = UNPUSHED;
		}
		break;
	}
}

/*
void BUTTON1()
{
	static BUTTON_STATUS but_status = UNPUSHED;
	static uint16_t press_time;
	uint16_t now_time = GetTickNow();
	switch(but_status)
	{
		case UNPUSHED:
		if (!(BUT_PIN & BUT_1))
		{
			but_status = CLICK;
			press_time = now_time;
		}
		break;
		
		case CLICK:
		if (BUT_PIN & BUT_1)
		{
			if ((uint16_t)(now_time - press_time) >= 50)
			{
				BUTTON_1.STATUS = CLICK;
			}
			but_status = UNPUSHED;
		}
		else
		{
			if ((uint16_t)(now_time - press_time) >= 1000)
			{
				BUTTON_1.STATUS = LONG_CLICK;
				but_status = LONG_CLICK;
			}
		}
		break;
		
		case LONG_CLICK:
		if (BUT_PIN & BUT_1)
		{
			but_status = UNPUSHED;
		}
		break;
	}
}

void BUTTON2()
{
	static BUTTON_STATUS but_status = UNPUSHED;
	static uint16_t press_time;
	uint16_t now_time = GetTickNow();
	switch(but_status)
	{
		case UNPUSHED:
		if (!(BUT_PIN & BUT_2))
		{
			but_status = CLICK;
			press_time = now_time;
		}
		break;
		case CLICK:
		if (BUT_PIN & BUT_2)
		{
			if ((uint16_t)(now_time - press_time) >= 50)
			{
				BUTTON_2.STATUS = CLICK;
			}
			but_status = UNPUSHED;
		}
		else
		{
			if ((uint16_t)(now_time - press_time) >= 1000)
			{
				BUTTON_2.STATUS = LONG_CLICK;
				but_status = LONG_CLICK;
			}
		}
		break;
		case LONG_CLICK:
		if (BUT_PIN & BUT_2)
		{
			but_status = UNPUSHED;
		}
		break;
	}
}

void BUTTON3()
{
	static BUTTON_STATUS but_status = UNPUSHED;
	static uint16_t press_time;
	uint16_t now_time = GetTickNow();
	switch(but_status)
	{
		case UNPUSHED:
		if (!(BUT_PIN & BUT_3))
		{
			but_status = CLICK;
			press_time = now_time;
		}
		break;
		case CLICK:
		if (BUT_PIN & BUT_3)
		{
			if ((uint16_t)(now_time - press_time) >= 50)
			{
				BUTTON_3.STATUS = CLICK;
			}
			but_status = UNPUSHED;
		}
		else
		{
			if ((uint16_t)(now_time - press_time) >= 1000)
			{
				BUTTON_3.STATUS = LONG_CLICK;
				but_status = LONG_CLICK;
			}
		}
		break;
		case LONG_CLICK:
		if (BUT_PIN & BUT_3)
		{
			but_status = UNPUSHED;
		}
		break;
	}
}

*/