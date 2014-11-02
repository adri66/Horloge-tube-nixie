/*
 * ds1307.cpp
 *
 * Created: 19/12/2013 14:06:21
 *  Author: Adrien
 */

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "DS1307.h"

#define adresse_DS1307 0xD0 //Adresse I2C du DS1307

#define LED_RED 1<<PINB3									/*Ici direct en Hex*/
#define LED_GREEN 1<<PINB4									/*Ici direct en Hex*/
#define LED_BLUE 1<<PINB5									/*Ici direct en Hex*/

ds1307::ds1307()
{
	iic_error = false;
}

void ds1307::write(int adresse_m, uint8_t data)
{
	//SCK Config à 80Khz (MAX DS1307 = 100Khz)
	TWBR = 0x5C;											//Bit rate division à 92
	TWSR &= ~((1<<TWPS1)|(1<<TWPS0))	;					// Prescale à 1
			
	//I2C Init
	TWCR = (1<<TWEN);										//Active l'I2C
	TWCR = (1<<TWEN)|(1<<TWSTA)|(1<<TWINT);					//Envoi la condiont de démarage
	while(!(TWCR&(1<<TWINT)))								//Attente de l'envoi
	{
		
	}
			
	//DS1307 adresse write
	TWDR = adresse_DS1307;											//Inscrit l'adresse, Mode write
	TWCR = (1<<TWEN)|(1<<TWINT);							//Envoi l'adresse
	while(!(TWCR&(1<<TWINT)))								//Attente de l'envoi
	{
		
	}
			
	//Adresse memoir send
	TWDR = adresse_m;										//Inscrit l'adresse mémoire
	TWCR = (1<<TWEN)|(1<<TWINT);							//Envoi l'adresse
	while(!(TWCR&(1<<TWINT)))								//Attente de l'envoi
	{
		
	}
			
	//Data send
	TWDR = data;											//Inscrit les donnée
	TWCR = (1<<TWEN)|(1<<TWINT);							//Envoi les donnée
	while(!(TWCR&(1<<TWINT)))								//Attente de l'envoi
	{
		
	}
			
	//Stop condition
	TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWSTO);
	while((TWCR&(1<<TWSTO)))
	{
		
	}
			
	_delay_ms(1);
	TWCR &= (~(1<<TWEN));
}
uint8_t ds1307::read(uint8_t adresse_m)
{
	TWBR = 0x5C;											//Bit rate division à 92
	TWSR &= ~((1<<TWPS1)|(1<<TWPS0));						// Prescale à 1
			
	//I2C Init
	TWCR = (1<<TWEN);												//Active l'I2C
	TWCR = (1<<TWEN)|(1<<TWSTA)|(1<<TWINT);							//Envoi la condiont de démarage
	while(!(TWCR&(1<<TWINT)))										//Attente de l'envoi
	{
		
				
	}
			
	//DS1307 adresse write
	TWDR = adresse_DS1307;													//Inscrit l'adresse, Mode write
	TWCR = (1<<TWEN)|(1<<TWINT);									//Envoi l'adresse
	while(!(TWCR&(1<<TWINT)))										//Attente de l'envoi
	{
		
	}
			
	//DS1307 adresse mem
	TWDR = adresse_m;												//Inscrit l'adresse mémoir
	TWCR = (1<<TWEN)|(1<<TWINT);									//Envoi l'adresse
	while(!(TWCR&(1<<TWINT)))										//Attente de l'envoi
	{
		
	}
			
	//Restart condition
	TWCR = (1<<TWEN)|(1<<TWSTA)|(1<<TWINT);							//Envoi la condiont de démarage
	while(!(TWCR&(1<<TWINT)))										//Attente de l'envoi
	{
		
	}
			
	//DS1307 adresse + read
	TWDR = adresse_DS1307|0x01;											//Inscrit l'adresse + Mode read
	TWCR = (1<<TWEN)|(1<<TWINT);									//Envoi l'adresse
	while(!(TWCR&(1<<TWINT)))										//Attente de l'envoi
	{
		
	}
			
	//Receive
	TWCR = (1<<TWEN)|(1<<TWINT);									//Demande de réception
	while(!(TWCR&(1<<TWINT)))										//Attente de la réception
	{
		
	}
			
			
				
	uint8_t data = TWDR;
						
	//Stop condition
	TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWSTO);
	while((TWCR&(1<<TWSTO)))
	{
		
	}
			_delay_ms(1);
			TWCR&=(~(1<<TWEN));
	return data;
}
uint8_t ds1307::r_seconde()
{
	return ds1307::read(0x00);
}
uint8_t ds1307::r_minute()
{
	return ds1307::read(0x01);
}
uint8_t ds1307::r_heur()
{
	return ds1307::read(0x02);
}
uint8_t ds1307::r_jour()
{
	return ds1307::read(0x04);
}
uint8_t ds1307::r_moi()
{
	return ds1307::read(0x05);
}
uint8_t ds1307::r_year()
{
	return ds1307::read(0x06);
}
void ds1307::w_seconde(uint8_t data)
{
	ds1307::write(0x00, data);
}
void ds1307::w_minute(uint8_t data)
{
	ds1307::write(0x01, data);
}
void ds1307::w_heur(uint8_t data, uint8_t pm)
{
	if(pm)
		data |= (1<<6);
	else
		data &= ~(1<<6);
	ds1307::write(0x02, data);
}
void ds1307::w_date(uint8_t data)
{
	ds1307::write(0x04, data);
}
void ds1307::w_moi(uint8_t data)
{
	ds1307::write(0x05, data);
}
void ds1307::w_year(uint8_t data)
{
	ds1307::write(0x06, data);
}