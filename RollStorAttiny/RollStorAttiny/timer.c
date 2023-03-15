
#include "timer.h"

uint16_t g_timer0_ticked = 0;

ISR(TIMER0_COMPA_vect) //1ms
{
	++g_timer0_ticked;
}

//настраиваем прерывание по совпадению на таймер0 частоту 1 мс
void TIMER0_INIT()
{
	OCR0A = 125; // ~ 1 ms
	TCCR0A |= (1<<WGM01); //CTC - mode
	TCCR0B |= (1<<CS01) | (1<<CS00);// F_CLOCK/64  = 8'000'000 / 64 / 1'000 = 125
	TIMSK |= 1<<OCIE0A;
}

//получаем текущее кол-во тиков
uint16_t GetTickNow()
{
	return g_timer0_ticked;
}