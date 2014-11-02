/*
 * _74595.cpp
 *
 * Created: 19/12/2013 14:06:10
 *  Author: Adrien
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "hc74595.h"

hc74595::hc74595(int data, int clk, int stcp,volatile uint8_t *port_uc/*, volatile uint8_t *ddr_uc*/)
{
	pin_DATA = data;
	pin_CLK = clk;
	pin_STCP = stcp;
	port = port_uc;
	ddr = --port_uc;
	*ddr |= (0x01 << data | 0x01 << clk | 0x01 << stcp);
}
void hc74595::send(uint8_t data, bool inverted) //Envoi des donnée au Shift Register
{

			for(int a = 0; a < 8 ; a++)
			{
				if(!inverted)
				{
					*port &= ~(0x01 << pin_CLK); //Met l'orloge à 0
					if(data & (0x01 << a)) //Regarde la valeur de chaque bit
						*port |= 0x01 << pin_DATA;
					else
						*port &= ~(0x01 << pin_DATA);
					*port|= 0x01 << pin_CLK; //Met l'orloge à 1
				}
				else
				{
					*port &= ~(0x01 << pin_CLK); //Met l'orloge à 0
					if(data & (0x80 >> a)) //Regarde la valeur de chaque bit
						*port |= 0x01 << pin_DATA;
					else
						*port &= ~(0x01 << pin_DATA);
					*port |= 0x01 << pin_CLK; //Met l'orloge à 1
				}
			}
}
void hc74595::confirm()																								//Envoi le signal du storage register
{
			*port |= 0x01 << pin_STCP;
			_delay_us(1);
			*port &= ~(0x01 << pin_STCP);
}



/*-----------------------------------------------------------------------------------------------------------
/*
 * _74595.cpp
 *
 * Created: 19/12/2013 14:06:10
 *  Author: Adrien
 * 

#include <avr/io.h>
#include "hc74595.h"

void hc74595::setup(int data, int clk, int stcp, char port_uc)
{
	pin_DATA = data;
	pin_CLK = clk;
	pin_STCP = stcp;
	port = port_uc;
	switch (port)																							//Configure les pin nésessaire en sortie
	{
		case 'B' :
			DDRB |= (0x01 << data | 0x01 << clk | 0x01 << stcp);
			break;
		case 'C' :
			DDRC |= (0x01 << data | 0x01 << clk | 0x01 << stcp);
			break;
		case 'D' :
			DDRD |= (0x01 << data | 0x01 << clk | 0x01 << stcp);
			break;
	}
}
void hc74595::hc74595(int data, int clk, int stcp, uint8_t *port_uc)
{
	pin_DATA = data;
	pin_CLK = clk;
	pin_STCP = stcp;
	*port = port_uc;
	*port |= (0x01 << data | 0x01 << clk | 0x01 << stcp);
}
void hc74595::send(uint8_t data, bool inverted) //Envoi des donnée au Shift Register
{
	switch (port)																								
	{
		case 'B' :
			for(int a = 0; a < 8 ; a++)
			{
				if(!inverted)
				{
					PORTB &= ~(0x01 << pin_CLK); //Met l'orloge à 0
					if(data & (0x01 << a)) //Regarde la valeur de chaque bit
						PORTB |= 0x01 << pin_DATA;
					else
						PORTB &= ~(0x01 << pin_DATA);
					PORTB |= 0x01 << pin_CLK; //Met l'orloge à 1
				}
				else
				{
					PORTB &= ~(0x01 << pin_CLK); //Met l'orloge à 0
					if(data & (0x80 >> a)) //Regarde la valeur de chaque bit
						PORTB |= 0x01 << pin_DATA;
					else
						PORTB &= ~(0x01 << pin_DATA);
					PORTB |= 0x01 << pin_CLK; //Met l'orloge à 1
				}
			}
			break;
		case 'C' :
			for(int a = 0; a < 8 ; a++)
			{
				if(!inverted)
				{
					PORTC &= ~(0x01 << pin_CLK); //Met l'orloge à 0
					if(data & (0x01 << a)) //Regarde la valeur de chaque bit
						PORTC |= 0x01 << pin_DATA;
					else
						PORTC &= ~(0x01 << pin_DATA);
					PORTC |= 0x01 << pin_CLK; //Met l'orloge à 1
				}
				else
				{
					PORTC &= ~(0x01 << pin_CLK); //Met l'orloge à 0
					if(data & (0x80 >> a)) //Regarde la valeur de chaque bit
						PORTC |= 0x01 << pin_DATA;
					else
						PORTC &= ~(0x01 << pin_DATA);
					PORTC |= 0x01 << pin_CLK; //Met l'orloge à 1
				}
			}
			break;
		case 'D' :
			for(int a = 0; a < 8 ; a++)
			{
				if(!inverted)
				{
					PORTD &= ~(0x01 << pin_CLK); //Met l'orloge à 0
					if(data & (0x01 << a)) //Regarde la valeur de chaque bit
						PORTD |= 0x01 << pin_DATA;
					else
						PORTD &= ~(0x01 << pin_DATA);
					PORTD |= 0x01 << pin_CLK; //Met l'orloge à 1
				}
				else
				{
					PORTD &= ~(0x01 << pin_CLK); //Met l'orloge à 0
					if(data & (0x80 >> a)) //Regarde la valeur de chaque bit
						PORTD |= 0x01 << pin_DATA;
					else
						PORTD &= ~(0x01 << pin_DATA);
					PORTD |= 0x01 << pin_CLK; //Met l'orloge à 1
				}
			}
			break;
	}
}
void hc74595::confirm()																								//Envoi le signal du storage register
{
	switch(port)
	{
		case 'B' :
			PORTB |= 0x01 << pin_STCP;
			PORTB &= ~(0x01 << pin_STCP);
			break;
		case 'C' :
			PORTC |= 0x01 << pin_STCP;
			PORTC &= ~(0x01 << pin_STCP);
			break;
		case 'D' :
			PORTD |= 0x01 << pin_STCP;
			PORTD &= ~(0x01 << pin_STCP);
			break;
	}
}
*/