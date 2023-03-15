
#include "uart.h"

//uint8_t BUF_RX[BUFSIZE];
//uint8_t buf_index = 0;
//uint8_t fBufRead = 0;
uint8_t recv = 0;

void UART(MOTOR *m)
{
	//static uint16_t last_time;
	//uint16_t now_time = GetTickNow();
	
	//if((uint16_t)(now_time - last_time) >= 5000)
	//{
		//UART_Send(curent_step_count_motor_one);
		//last_time = now_time;
	//}
	if (recv)
	{
		m->change_state_motor = recv;
		recv = 0;
	}
}

ISR(USART_RX_vect)
{
	uint8_t status = UCSRA;
	if (status & ((1<<FE)|(1<<DOR)|(1<<UPE)))
	{
		status = UDR;
		return;
	}
	//BUF_RX[buf_index++] = UDR;
	//if(buf_index > BUFSIZE)
	//{
	//	buf_index = 0;
	//	fBufRead = 1;
	//}
	recv = UDR;
}

void UartInit(uint16_t baud)
{
	/* Set baud rate */
	UBRRH = (uint8_t)(baud>>8);
	UBRRL = (uint8_t)baud;
	/* Enable receiver and transmitter */
	UCSRB = (1<<RXCIE)|(1<<RXEN)|(1<<TXEN);
	/* Set frame format: 8data, 1stop bit */
	UCSRC = (3<<UCSZ0);
}

void UART_SendB(int8_t b)
{
	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) )
	;
	/* Put data into buffer, sends the data */
	UDR = b;
}

void UART_SendDW(uint32_t st)
{
	UART_SendB((st & 0xFF000000) >> 24);
	UART_SendB((st & 0x00FF0000) >> 16);
	UART_SendB((st & 0x0000FF00) >> 8);
	UART_SendB(st & 0x000000FF);
}