/*
 * fonction.cpp
 *
 * Created: 19/12/2013 14:18:08
 *  Author: Adrien
 */ 

#define F_CPU 16000000UL									//Fréquence horloge à 16Mhz

#include <avr/io.h>
#include <util/delay.h>
/* Chargement des librairie */
#include "./lib/hc74595.h"
#include "./lib/DS1307.h"
#include "lib/UART.h"

/* Config des pin */
#define LED_RED 1<<PINB3									/*Ici direct en Hex*/
#define LED_GREEN 1<<PINB4									/*Ici direct en Hex*/
#define LED_BLUE 1<<PINB5									/*Ici direct en Hex*/

#define SHIFT_REGISTER_CLK 7								/*Ici pour config donc pas en hex*/
#define SHIFT_REGISTER_CONFIRM 5							/*Ici pour config donc pas en hex*/
#define SHIFT_REGISTER_DATA 6								/*Ici pour config donc pas en hex*/
#define SHIFT_REGISTER_PORT 123

/*BP sur PORT B*/
#define BP_PLUS 2
#define BP_MOINS 1
#define BP_CONF 0

/*Pin Enable du Shift Register sur PORT C*/
#define SR_SECONDE 2
#define SR_MINUTE 1
#define SR_HEUR 0

#define SR_JOURS 0
#define SR_MOIS 1
#define SR_ANNEE 2


#define tmps_max_bp_plus_moins 1000							//Temps avent que le script continue sa route lorsque le bp reste enfoncé
#define tmps_clignotement 100								//Temps du clignotements (/!\ Eteint les registe à décalage => Passage à 0 sur les Nixie Tube ...)
#define LOW(bit) ~(bit)



/* Création des objets */

ds1307 rtc;
hc74595 shiftRegister(SHIFT_REGISTER_DATA, SHIFT_REGISTER_CLK, SHIFT_REGISTER_CONFIRM, &PORTD);
uint8_t usart;
void first()
{
	usart = USART_Receive();
}
int int_to_bcd(int val)
{
	int high = 0;
	
	
	while (val >= 10)                 // Count tens
	{
		high++;
		val -= 10;
	}

	return  (high << 4) | val;
	/*uint8_t bcd;
	if(val >= 10)
	{
		bcd = val%10;
		val /= 10;
	}
	bcd = bcd&0x0F|(val&0x0f<<4);
	return bcd;*/
}
void setup_pins()
{
	USART_ini();
	DDRD &= LOW(1<<PIND2);
	DDRC &= LOW(1<<PORTC5|1<<PORTC4);
	DDRB &= LOW(1<<BP_CONF|1<<BP_MOINS|1<<BP_PLUS);			//Config des entrée pour les bp
	DDRC |= 1<<SR_HEUR|1<<SR_MINUTE|1<<SR_SECONDE;			//Config des sortie Pour les pin enable
	PORTC &= LOW(1<<SR_HEUR|1<<SR_MINUTE|1<<SR_SECONDE);	//Active tous les registre a décalage
	DDRB |= LED_RED | LED_GREEN | LED_BLUE ;				//Config des sortie pour les LED
	PORTB &= LOW(LED_RED|LED_GREEN|LED_BLUE);				//Mise à 0 de toutes les LED
	if(rtc.r_seconde() == 0x80)
		rtc.w_seconde(0x00);
}
void affichage_heur()
{
	uint8_t envoi = rtc.r_heur();
	envoi = (envoi>>4)|((envoi&0x0f)<<4);
	shiftRegister.send(envoi, 0);
	envoi = rtc.r_minute();
	envoi = (envoi>>4)|((envoi&0x0f)<<4);
	shiftRegister.send(envoi, 0);
	envoi = rtc.r_seconde();
	envoi = (envoi>>4)|((envoi&0x0f)<<4);
	shiftRegister.send(envoi, 0);
	shiftRegister.confirm();
}
void affichage_date()
{
	//PORTB &= ~(LED_RED|LED_GREEN|LED_BLUE);
	uint8_t envoi = rtc.r_jour();
	envoi = (envoi>>4)|((envoi&0x0f)<<4);
	shiftRegister.send(envoi, false);
	envoi = rtc.r_moi();
	envoi = (envoi>>4)|((envoi&0x0f)<<4);
	shiftRegister.send(envoi, false);
	envoi = rtc.r_year();
	envoi = (envoi>>4)|((envoi&0x0f)<<4);
	shiftRegister.send(envoi, false);
	shiftRegister.confirm();
}
void affichage_shutdwon()
{
	shiftRegister.send(0xFF, false);
	shiftRegister.send(0xFF, false);
	shiftRegister.send(0xFF, false);
	shiftRegister.confirm();
}
void routine_affichage()
{
	uint8_t secondes_var = rtc.r_seconde();
	if(secondes_var >= 0x20 && secondes_var <= 0x30 && 1==2)		//Affiche la date pandant 10 seconde
	affichage_date();
	else
	affichage_heur();
}
void clignotement(int n, bool date)
{
	n++;
	uint8_t envoi;
	if(date)
	{
		switch(n)
		{
			case 1:
				envoi = 0xff;
				shiftRegister.send(envoi, false);
				envoi = rtc.r_moi();
				envoi = (envoi>>4)|((envoi&0x0f)<<4);
				shiftRegister.send(envoi, false);
				envoi = rtc.r_year();
				envoi = (envoi>>4)|((envoi&0x0f)<<4);
				shiftRegister.send(envoi, false);
				shiftRegister.confirm();
			break;
			case 2:
				envoi = rtc.r_jour();
				envoi = (envoi>>4)|((envoi&0x0f)<<4);
				shiftRegister.send(envoi, false);
				envoi = 0xff;
				shiftRegister.send(envoi, false);
				envoi = rtc.r_year();
				envoi = (envoi>>4)|((envoi&0x0f)<<4);
				shiftRegister.send(envoi, false);
				shiftRegister.confirm();
			break;
			case 3:
				envoi = rtc.r_jour();
				envoi = (envoi>>4)|((envoi&0x0f)<<4);
				shiftRegister.send(envoi, false);
				envoi = rtc.r_moi();
				envoi = (envoi>>4)|((envoi&0x0f)<<4);
				shiftRegister.send(envoi, false);
				envoi = 0xff;
				shiftRegister.send(envoi, false);
				shiftRegister.confirm();
			break;
		}
	}
	else
	{
		switch(n)
		{
			case 1:
				envoi = 0xff;
				shiftRegister.send(envoi, 0);
				envoi = rtc.r_minute();
				envoi = (envoi>>4)|((envoi&0x0f)<<4);
				shiftRegister.send(envoi, 0);
				envoi = rtc.r_seconde();
				envoi = (envoi>>4)|((envoi&0x0f)<<4);
				shiftRegister.send(envoi, 0);
				shiftRegister.confirm();
			break;
			case 2:
				envoi = rtc.r_heur();
				envoi = (envoi>>4)|((envoi&0x0f)<<4);
				shiftRegister.send(envoi, 0);
				envoi = 0xff;
				shiftRegister.send(envoi, 0);
				envoi = rtc.r_seconde();
				envoi = (envoi>>4)|((envoi&0x0f)<<4);
				shiftRegister.send(envoi, 0);
				shiftRegister.confirm();
			break;
			case 3:
				envoi = rtc.r_heur();
				envoi = (envoi>>4)|((envoi&0x0f)<<4);
				shiftRegister.send(envoi, 0);
				envoi = rtc.r_minute();
				envoi = (envoi>>4)|((envoi&0x0f)<<4);
				shiftRegister.send(envoi, 0);
				envoi = 0xff;
				shiftRegister.send(envoi, 0);
				shiftRegister.confirm();
			break;			
		}
	}
}
void couleur()
{
	uint8_t mask = LED_BLUE|LED_GREEN|(LED_RED);				//COnfigure le mask
	uint8_t curent_couleur;
	if((PINB & (1<<BP_PLUS)) || usart == 0XC1)
	{
		for(int b = 0 ; b < tmps_max_bp_plus_moins && (PINB & (1<<BP_PLUS)); b++)
			_delay_ms(1);
		curent_couleur = ((PINB & mask)+(1<<PINB3))&mask;
		if(curent_couleur > mask)
			curent_couleur = 0x00;
		if (curent_couleur&LED_BLUE)
			PORTB |= LED_BLUE;
		else
			PORTB &= LOW(LED_BLUE);
		if(curent_couleur&LED_GREEN)
			PORTB |= LED_GREEN;
		else
			PORTB &= LOW(LED_GREEN);
		if(curent_couleur & LED_RED)
			PORTB |= LED_RED;
		else
			PORTB &= LOW(LED_RED);
	}
	else if((PINB & (1<<BP_MOINS)) || usart == 0XC0)
	{
		for(int b = 0 ; b < tmps_max_bp_plus_moins && (PINB & (1<<BP_MOINS)) ; b++)
			_delay_ms(1);
		curent_couleur = ((PINB & mask)-(1<<PINB3))&mask;
		if(curent_couleur > mask)
			curent_couleur = 0x00;
		if (curent_couleur&LED_BLUE)
			PORTB |= LED_BLUE;
		else
			PORTB &= LOW(LED_BLUE);
		if(curent_couleur&LED_GREEN)
			PORTB |= LED_GREEN;
		else
			PORTB &= LOW(LED_GREEN);
		if(curent_couleur & LED_RED)
			PORTB |= LED_RED;
		else
			PORTB &= LOW(LED_RED);
	}
}
void bouton()												//Gestion des BP pour configurer l'heur
{
	if (PINB & (1<<BP_CONF))								//Si le BP Conf est activé
	{
		while(PINB & (1<<BP_CONF))							//Attente que le BP sois relaché
			_delay_ms(1);
		int pousse = 1;
		for( int a = 0 ; pousse <= 6 ; a++)						//Boucle infinie tant qu'on a pas poussé 6 fois sur le bp et init de la variable pour clignotement
		{
			if(a >= (tmps_clignotement*2))
				a = 0;
			if (PINB & (1<<BP_CONF))
			{
				while(PINB & (1<<BP_CONF))
					_delay_ms(1);
				pousse++;
			}
			switch(pousse)
			{
				case 1:										//Config des secondes
					if(a > tmps_clignotement)				//Clignotement toute les ~500ms
						clignotement(SR_SECONDE, false);
					else
						affichage_heur();
					if((PINB & (1<<BP_PLUS)) || PINB & (1<<BP_MOINS))//Mise à 00 si + ou - est actionné
						rtc.w_seconde(0x00);
					break;
				case 2:										//Config des minutes
					if(a > tmps_clignotement)				//Clignotement toute les ~500ms
						clignotement(SR_MINUTE, false);
					else
						affichage_heur();
					if(PINB & (1<<BP_PLUS))
					{
						for(int b = 0 ; b < tmps_max_bp_plus_moins && (PINB & (1<<BP_PLUS)); b++)
							_delay_ms(1);
						uint8_t curent_minute = rtc.r_minute();//Lecture du temps DS1307
						if((curent_minute&0x0F) == 0x09)	// Si 1e digit = 9
							curent_minute = (((curent_minute&0xF0)>>4)+1)<<4; //Incrémente de 10
						else
							curent_minute++;				//Incrémente de 1
						if(curent_minute >= 0x60)			//Si supérieur à 60 retour à 00 /!\ Pas d'incrémentation de l'heur !!
							curent_minute = 0x00;
						rtc.w_minute(curent_minute);		//Met à jour le DS1307
					}
					else if(PINB & (1<<BP_MOINS))
					{
						for(int b = 0 ; b < tmps_max_bp_plus_moins && (PINB & (1<<BP_MOINS)) ; b++)
							_delay_ms(1);
						uint8_t curent_minute = rtc.r_minute();//Lecture du temps DS1307
						if((curent_minute&0x0F) == 0x00)	// Si 1e digit = 0
							curent_minute = ((((curent_minute&0xF0)>>4)-1)<<4)|0x09; //Décrémente de 10
						else if(curent_minute == 0x00)		// Bug en simulation ..........???????????~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
							curent_minute = 0x59;
						else
							curent_minute--;				//Décrémente de 1
						if(curent_minute > 0x60)
							curent_minute = 0x59;
						rtc.w_minute(curent_minute);		//Met à jour le DS1307
					}
					
				break;
				case 3:										//Config de l'heur
					if(a > tmps_clignotement)				//Clignotement toute les ~500ms
						clignotement(SR_HEUR, false);
					else
						affichage_heur();
					if(PINB & (1<<BP_PLUS))
					{
						for(int b = 0 ; b < tmps_max_bp_plus_moins && (PINB & (1<<BP_PLUS)); b++)
							_delay_ms(1);
						uint8_t curent_heur = rtc.r_heur();	//Lecture du temps DS1307
						if((curent_heur&0x0F) == 0x09)		// Si 1e digit = 9
							curent_heur = (((curent_heur&0xF0)>>4)+1)<<4; //Incrémente de 10
						else if(curent_heur >= 0x23)		//Si arrivé à 24H retour à 00H, /!\ Attention n'incrémente pas la date !!
							curent_heur = 0x00;
						else
							curent_heur = ((curent_heur&0x0F)+1)|(curent_heur&0xF0); //Incrémente de 1
						rtc.w_heur(curent_heur, 0);			//Met à jour le DS1307
					}
					else if(PINB & (1<<BP_MOINS))
					{
						for(int b = 0 ; b < tmps_max_bp_plus_moins && (PINB & (1<<BP_MOINS)); b++)
							_delay_ms(1);
						uint8_t curent_heur = rtc.r_heur();	//Lecture du temps DS1307
						if(curent_heur == 0x00)		//Si arrive à 00H, /!\ Attention ne décrémente pas la date !!
							curent_heur = 0x23;				//Retour à 24H					
						else if((curent_heur&0x0F) == 0)			// Si 1e digit = 0
							curent_heur = ((((curent_heur&0xF0)>>4)-1)<<4)|0x09; //Décrémente de 10
						else
							curent_heur = ((curent_heur&0x0f)-1)|(curent_heur&0xf0); //Décrémente de 1
						rtc.w_heur(curent_heur, 0);			//Met à jour le DS1307
					}
				break;
				case  4:									//Config du jour
					PORTC &= LOW(1<<SR_HEUR);
					if(a > tmps_clignotement)				//Clignotement toute les ~500ms
						clignotement(SR_JOURS, true);
					else
						affichage_date();
					if(PINB & (1<<BP_PLUS))
					{
						for(int b = 0 ; b < tmps_max_bp_plus_moins && (PINB & (1<<BP_PLUS)); b++)
							_delay_ms(1);
						uint8_t curent_day = rtc.r_jour();	//Lecture du temps DS1307
						if((curent_day&0x0F) == 0x09)		// Si 1e digit = 9
							curent_day = (((curent_day&0xF0)>>4)+1)<<4; //Incrémente de 10
						else
							curent_day = ((curent_day&0x0F)+1)|(curent_day&0xF0); //Incrémente de 1
						if(curent_day > 0x31)
							curent_day = 0x00;
						rtc.w_date(curent_day);				//Met à jour le DS1307
					}
					else if(PINB & (1<<BP_MOINS))
					{
						for(int b = 0 ; b < tmps_max_bp_plus_moins && (PINB & 1<<BP_MOINS); b++)
							_delay_ms(1);
						uint8_t curent_heur = rtc.r_jour();	//Lecture du temps DS1307
						if((curent_heur&0x0F) == 0)			// Si 1e digit = 0
							curent_heur = ((((curent_heur&0xF0)>>4)-1)<<4)|0x09; //Décrémente de 10
						else
							curent_heur = ((curent_heur&0x0F)-1)|(curent_heur&0xF0); //Décrémente de 1
						if(curent_heur == 0x00)
							curent_heur = 0x31;
						rtc.w_date(curent_heur);			//Met à jour le DS1307
					}
				break;
				case 5:										//config du mois
					if(a > tmps_clignotement)				//Clignotement toute les ~500ms
						clignotement(SR_MOIS, true);
					else
						affichage_date();
					if(PINB & (1<<BP_PLUS))
					{
						for(int b = 0 ; b < tmps_max_bp_plus_moins && (PINB & (1<<BP_PLUS)); b++)
							_delay_ms(1);
						uint8_t curent_months = rtc.r_moi();//Lecture du temps DS1307
						if((curent_months&0x0F) == 0x09)	// Si 1e digit = 9
							curent_months = (((curent_months&0xF0)>>4)+1)<<4; //Incrémente de 10
						else
							curent_months = ((curent_months&0x0F)+1)|(curent_months&0xF0); //Incrémente de 1
						if(curent_months > 0x12)
							curent_months = 0x01;
						rtc.w_moi(curent_months);			//Met à jour le DS1307
					}
					else if(PINB & (1<<BP_MOINS))
					{
						for(int b = 0 ; b < tmps_max_bp_plus_moins && (PINB & 1<<BP_MOINS); b++)
							_delay_ms(1);
						uint8_t curent_months = rtc.r_moi();//Lecture du temps DS1307
						if((curent_months&0x0F) == 0)		// Si 1e digit = 0
							curent_months = ((((curent_months&0xF0)>>4)-1)<<4)|0x09; //Décrémente de 10
						else
							curent_months = ((curent_months&0x0F)-1)|(curent_months&0xF0); //Décrémente de 1
						if(curent_months < 0x01)
							curent_months = 0x12;
						rtc.w_moi(curent_months);			//Met à jour le DS1307
					}
				break;
				case 6:										//Config de l'année
					if(a > tmps_clignotement)				//Clignotement toute les ~500ms
						clignotement(SR_ANNEE, true);
					else
						affichage_date();
					if(PINB & (1<<BP_PLUS))
					{
						for(int b = 0 ; b < tmps_max_bp_plus_moins && (PINB & (1<<BP_PLUS)); b++)
							_delay_ms(1);
						uint8_t curent_year = rtc.r_year();//Lecture du temps DS1307
						if((curent_year&0x0F) == 0x09)	// Si 1e digit = 9
							curent_year = (((curent_year&0xF0)>>4)+1)<<4; //Incrémente de 10
						else
							curent_year = ((curent_year&0x0F)+1)|(curent_year&0xF0); //Incrémente de 1
						if(curent_year > 0x99)
							curent_year = 0x00;
						rtc.w_year(curent_year);			//Met à jour le DS1307
					}
					else if(PINB & (1<<BP_MOINS))
					{
						for(int b = 0 ; b < tmps_max_bp_plus_moins && (PINB & 1<<BP_MOINS); b++)
							_delay_ms(1);
						uint8_t curent_year = rtc.r_year();//Lecture du temps DS1307
						if((curent_year&0x0F) == 0 && curent_year != 0x00)		// Si 1e digit = 0
							curent_year = ((((curent_year&0xF0)>>4)-1)<<4)|0x09; //Décrémente de 10
						else if (curent_year != 0x00)
							curent_year = ((curent_year&0x0F)-1)|(curent_year&0xF0); //Décrémente de 1
						else
							curent_year = 0x99;
						rtc.w_year(curent_year);			//Met à jour le DS1307
					}
					break;
				
			}
			PORTC &= LOW(1<<SR_SECONDE|1<<SR_MINUTE|1<<SR_HEUR); //Active tous les registre à décalage
			_delay_ms(1);									//Attente de 1ms pour le comptage de la variable a
		}
	}
}
void uart_ds1307()
{
	if(usart == 0xCF)
	{
		bool cfg = true;
		for(int a = 0; cfg ; a++)
		{
			usart = USART_Receive();
			if(a<=50)
				PORTB &= LOW(LED_RED|LED_GREEN|LED_BLUE);
			else
				PORTB |= LED_RED|LED_GREEN|LED_BLUE;
			if(a>100)
				a=0;
			if(usart == 0xC1)
			{
				usart = USART_Receive();
				for(int b = 0; !usart ; b++)
				{
					usart = USART_Receive();
					affichage_heur();
					
					if(b<=50)
						PORTB &= LOW(LED_RED|LED_GREEN|LED_BLUE);
					else
						PORTB |= LED_GREEN|LED_BLUE;
					if(b>100)
					b=0;
					_delay_ms(1);
				}
				rtc.w_seconde(int_to_bcd(usart));
				USART_Transmit(0xC1);
			}
			if(usart == 0xC2)
			{
				usart = USART_Receive();
				for(int b = 0; !usart ; b++)
				{
					usart = USART_Receive();
					affichage_heur();
					
					if(b<=50)
					PORTB &= LOW(LED_RED|LED_GREEN|LED_BLUE);
					else
					PORTB |= LED_GREEN|LED_BLUE;
					if(b>100)
					b=0;
					_delay_ms(1);
				}
				rtc.w_minute(int_to_bcd(usart));
				USART_Transmit(0xC2);
			}
			if(usart == 0xC3)
			{
				usart = USART_Receive();
				for(int b = 0; !usart ; b++)
				{
					usart = USART_Receive();
					affichage_heur();
					
					if(b<=50)
					PORTB &= LOW(LED_RED|LED_GREEN|LED_BLUE);
					else
					PORTB |= LED_GREEN|LED_BLUE;
					if(b>100)
					b=0;
					_delay_ms(1);
				}
				rtc.w_heur(int_to_bcd(usart), 0);
				USART_Transmit(0xC3);
			}
			if(usart == 0xC4)
			{
				PORTB &= LOW(LED_RED|LED_GREEN|LED_BLUE);
				cfg =false;
			}
		}
	}
}
