/*
 * UART.h
 *
 * Created: 31/05/2014 23:16:48
 *  Author: Adrien
 */ 


#ifndef UART_H_
#define UART_H_


void USART_ini();
void USART_Transmit( unsigned char data );
unsigned char USART_Receive( void );


#endif /* UART_H_ */