/*
 * UART.cpp
 *
 * Created: 31/05/2014 23:16:31
 *  Author: Adrien
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "UART.h"
#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR (((F_CPU / (BAUD * 16UL))) - 1)

void USART_ini()
{
	UBRR0H = (unsigned char)(MYUBRR>>8);
	UBRR0L = (unsigned char)MYUBRR;
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0C = ((1<<UCSZ00)|(1<<UCSZ01));
}
void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) )
	;
	/* Put data into buffer, sends the data */
	UDR0 = data;
}
unsigned char USART_Receive( void )
{
	/* Wait for data to be received */
	if((UCSR0A & (1<<RXC0)))
		return UDR0;
	else
		return false;
}