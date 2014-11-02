/*
 * LCD.cpp
 *
 * Created: 27/12/2013 13:32:02
 *  Author: Adrien
 */ 

/************************************************************************************************************
 *		Cette librairie permet de controlé un écrans LCD a traver un PCF8574 avec comme adresse 0x40		*
 *																											*
 *								##############################												*
 *								#----------------------------#												*
 *								#|	Configuration des pins	|#												*
 *								#|~~~~~~~~~~~~~~~~~~~~~~~~~~|#												*
 *								#|	 PCF8574	|	LCD		|#												*
 *								#|~~~~~~~~~~~~~~|~~~~~~~~~~~|#												*
 *								#|		0		|	D4		|#												*
 *								#|		1		|	D5		|#												*
 *								#|		2		|	D6		|#												*
 *								#|		3		|	D7		|#												*
 *								#|		4		|	NC		|#												*
 *								#|		5		|	E		|#												*
 *								#|		6		|	RS		|#												*
 *								#|		7		|	RW		|#												*
 *								#----------------------------#												*
 *								##############################												*
 *																											*
 *																											*
 *						   @@			§§			  §§	#######											*
 *						  @@@@			 §§			 §§		##	 #####										*
 *						 @@	 @@			  §§		§§		##	    ####									*
 *					    @@	  @@		   §§	   §§		##		  ##									*
 *					   @@ 	   @@ 			§§	  §§		##		  ##									*
 *					  @@~&&&&&&~@@		  	 §§	 §§			##		####									*
 *					 @@			 @@		 	  §§§§			##	 #####										*
 *					@@			  @@		   §§			#######											*
 *																											*
 ************************************************************************************************************
 */

#define adresse_DS1307 0x40
#define F_CPU 16000000UL

#define RS 0x01<<6
#define RW 0x01<<7
#define E 0x01<<5
#define D4 0x01<<0
#define D5 0x01<<1
#define D6 0x01<<2
#define D7 0x01<<3
#define D0 0x01<<0
#define D1 0x01<<1
#define D2 0x01<<2
#define D3 0x01<<3

#include <stdlib.h>
#include <stdio.h>
#include <avr/delay.h>
#include <avr/io.h>
#include "LCD.h"



		uint8_t PCF8574::send(uint8_t data, uint8_t read)
		{
			uint8_t read_data;
			//SCK Config (27.77Khz)
			TWBR = 0x0A;											//Bit rate division à 10
			TWSR = (TWSR & ~((1<<TWPS0) & (1<<TWPS1)));				//TWPS à 0
		
			//I2C Init
			TWCR = (1<<TWEN);										//Active l'I2C
			TWCR = (1<<TWEN)|(1<<TWSTA)|(1<<TWINT);					//Envoi la condiont de démarage
			while(!(TWCR&(1<<TWINT)))								//Attente de l'envoi
			;
		
			//DS1307 adresse write
			TWDR = adresse_DS1307|read	;			//Inscrit l'adresse, Mode
			TWCR = (1<<TWEN)|(1<<TWINT);							//Envoi l'adresse
			while(!(TWCR&(1<<TWINT)))								//Attente de l'envoi
			;
		
		
			//Data send
			if(!read)	//Si écriture
				TWDR = data;										//Inscrit les donnée
			TWCR = (1<<TWEN)|(1<<TWINT);							//Envoi les donnée
			while(!(TWCR&(1<<TWINT)))								//Attente de l'envoi
			;
			if(read) //Si lecture
				read_data = TWDR;
		
			//Stop condition
			TWCR = (1<<TWEN)|(1<<TWINT)|(1<<TWSTO);
			while((TWCR&(1<<TWSTO)))
			;
		
			_delay_ms(1);
			TWCR &= (~(1<<TWEN));
			if(read)
				return read_data;
		}
		bool LCD::check_busy_flag()
		{
			PCF8574::send(RW, 0);
			uint8_t bf = PCF8574::send(RW, 1)&D7;
			//_delay_ms(10000);
			if((bf&(D7)>>3))
				return true;
			else
				return false;
		}
		void LCD::send(uint8_t data, bool debut_instuction)
		{
			while(check_busy_flag() && debut_instuction) //Vérifie qu'il n'y a pas une oppétation en cours
			{}
			PCF8574::send(0x00, 0);	//Met les pins à0
			PCF8574::send(data, 0);	//Ecris les données
			data |= (E);
			//_delay_ms(1000);
			PCF8574::send(data, 0);	//Confirme les données
			_delay_us(1);						//Attente que la confiramation soit bien reçue
			data &= ~(E);
			PCF8574::send(data, 0);
			_delay_us(1);
		}
		void LCD::display_init()
		{
			send(0x03, true); //Mode 4bit
			
			send(0x08, false); //Mode 2ligne
			
			send(0x00, true);send(0x01, false); //Clear display
			send(0x00, true);send(0x02, false); //Return home
			send(0x00, true);send((D2|D1), false); //Incrémentation de la RAM et déplacement du curseur vers la droite
			send(0x00, true);send((D3|D2), false); //Allume l'afficheur sans curseur

		}
		void LCD::send_leter(uint8_t lettre)
		{
			uint8_t data = (RS)|(lettre>>4); //Inscrit les 4 dernier bit dans la RAM
			send(data, true);
			data = (RS)|(lettre&0b00001111); //Inscrit les 4 premier bit dans la RAM
			send(data, false);
		}
		void LCD::clear()
		{
			send(0x00, true); send(D0, false); //Clear Display
			send(0x00, true);send(0x02, false); //Return home
		}
		void LCD::set_cursor(int x, int y)
		{
			uint8_t data;
				if(y == 1)
					data = 0x00 + x;
				else if(y == 2)
					data = 0x40 + x;
			send((data>>4)|0x08, true);_delay_us(5);send(data&0x0F, false);
		}
		void LCD::print_text(char *txt)
		{
			for(int a = 0 ; a < strlen(txt) ; a++)
				send_leter(txt[a]);			
		}
		void LCD::print_number(int num)
		{
			char c[64];
			sprintf(c, "%d", num);
			print_text(c);
		}