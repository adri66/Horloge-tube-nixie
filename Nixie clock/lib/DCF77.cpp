/*
 * DCF77.cpp
 *
 * Created: 19/12/2013 14:06:30
 *  Author: Adrien
 */ 

#include "DCF77.h"
#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "LCD.h"

#define LCD_DEBUG false
#define time_low_dcf77_min_0 80
#define time_low_dcf77_max_0 120
#define time_low_dcf77_min_1 180
#define time_low_dcf77_max_1 220
#define time_no_modulation 1000
#define en_attente_de_signal 0
#define decodage_en_cours 1
#define decodage_effectuer 2
#define decodage_erreur 3
#define temps_haut_min 500
#define temps_bas_min 20


dcf77 atc_;
//#if LCD_DEBUG
	LCD lcd1;
//#endif

void dcf77::start_capture()
{
	DDRB |= 1<<PINB5;
	timer1_init();
	dcf77_flag_start = 0;
	DDRD &= ~(1<<PIND2); //PIND2 en entrée
	EICRA = 1<<ISC00; //Interuption a chaque changement d'état de INT0
	EIMSK |= 1<<INT0; //Activation de l'interuption INT0
	sei(); //Activation des interuption en générale
}
void dcf77::stop_capture()
{
	EIMSK &= ~(1<<INT0); //Désactive l'interuption
	dcf77_flag_start = 0;
	dcf77_bit_count = 0;
	cli();
}
void dcf77::hi()
{
	
	time_check = millis() - time_low_dcf77;
	if( time_check >= temps_bas_min || time_low_dcf77 == 0)
	{
		PORTB |= 1<<PINB5; //Led de Debug à 1
		parasite_flag = false;
			time_high_dcf77 = millis(); //Démarage du comptage du temps haut
			time_low_dcf77 = millis() - time_low_dcf77; //Calcul du Temps bas
			if(dcf77_flag_start && dcf77_bit_count <= 58) //Démarage de la démodulation si toute les condition sont OK
			{
				dcf77_status = decodage_en_cours; //En cours de décodage
				int bit;
				if(time_low_dcf77 >= time_low_dcf77_min_0 && time_low_dcf77 <= time_low_dcf77_max_0) //Si le temp bas est <= 100ms le bit est à 0
					bit = 0;
				else if(time_low_dcf77 >= time_low_dcf77_min_1 && time_low_dcf77 <= time_low_dcf77_max_1) //Si le temp bas est <= 200ms le bit est à 1
					bit = 1;
				else //ereur
				{
					//dcf77_bit_count = 0;
					//dcf77_flag_start = 0;
					//return;
				}
				if(dcf77::decode(bit))
				{
					dcf77_bit_count = 0;
					dcf77_flag_start = 0;
					//return;
				}
				dcf77_bit_count++;
			}
			else if(dcf77_bit_count > 58)//Après le décodage 60e bit
			{
				//atc_.stop_capture(); //Arêt de la capture
				dcf77_flag_start = 0; //Remise a zero du drapeau de démarage
				dcf77_bit_count = 0; //Mise à zero le nombre de bit decodé
				dcf77_status = decodage_effectuer;
			}
			else
			{
			//	lcd1.set_cursor(5, 1);
			//	lcd1.print_text("Wt S.F.");
			}
	}
	else
		parasite_flag = true;
}
void dcf77::lo()
{
	
	time_check = millis() - time_high_dcf77;
		if( time_check >= temps_haut_min || time_high_dcf77 == 0)
		{
			PORTB &= ~(1<<PINB5);//Led de debug à 0
			parasite_flag = false;
			time_high_dcf77 = millis() - time_high_dcf77; //Calcul du temps Haut
			time_low_dcf77 = millis(); //Démarage du comptage du temps bas
			if(time_high_dcf77 > time_no_modulation | dcf77_flag_start) //Si le Temps bas est > à 800, début de la transmition
			{
				dcf77_flag_start = 1;
			}
			else
			{
				dcf77_status = en_attente_de_signal;
			}
		}
		else
			parasite_flag = true;
}
bool dcf77::decode(uint8_t bit)
{
	bool flag_no_error = true;
	if(bit_dcode_n == 0 && !bit) //Bit 00 Toujours à 0 si le contraire erreur
		flag_no_error = false;
	else if(bit_dcode_n >= 1 && bit_dcode_n <= 19); //Bits 01 à 19 ne nous interesse pas
	else if (bit_dcode_n == 20 && !bit) //Bit 20 Toujours à 1
		flag_no_error = false;
	else if (bit_dcode_n >= 21 && bit_dcode_n <= 27) //Bit 21 à 27 = Minute
	{
		if(bit)
			dcf77_Minute |= 1 << (bit_dcode_n-21);
		else
			dcf77_Minute &= ~(1 << (bit_dcode_n-21));
	}
	else if(bit_dcode_n == 28 && (dcf77_Minute&0x01) && !bit || bit_dcode_n == 28 && !(dcf77_Minute&0x01) && bit) //Bit de parité des minutes
		flag_no_error = false;
	else if(bit_dcode_n >= 29 && bit_dcode_n <= 34) //Bit 29 à 34 = Heure
	{
		if(bit)
			dcf77_Heur |= 1 << (bit_dcode_n-29);
		else
			dcf77_Heur &= ~(1 << (bit_dcode_n-29));
	}
	else if(bit_dcode_n == 35 && (dcf77_Heur&0x01) && !bit || bit_dcode_n == 35 && !(dcf77_Heur&0x01) && bit) //Bit de parité des Heure
		flag_no_error = false;
	else if(bit_dcode_n >= 36 && bit_dcode_n <= 41) //Bit 36 à 41 = Jours du moi
	{
		if(bit)
			dcf77_Daymonth |= 1 << (bit_dcode_n-36);
		else
			dcf77_Daymonth &= ~(1 << (bit_dcode_n-36));
	}
	else if(bit_dcode_n >= 42 && bit_dcode_n <= 44); //Bit 42 à 44 = Jours de la semaine, ne nous interesse pas
	else if(bit_dcode_n >= 45 && bit_dcode_n <= 49) //Bit 45 à 49 = Mois
	{
		if(bit)
			dcf77_Month |= 1 << (bit_dcode_n-45);
		else
			dcf77_Month &= ~(1 << (bit_dcode_n-45));
	}
	else if(bit_dcode_n>= 50 && bit_dcode_n <= 57) //Bit 50 à 57 = Année
	{
		if(bit)
			dcf77_Year |= 1 << (bit_dcode_n-50);
		else
			dcf77_Year &= ~(1 << (bit_dcode_n-50));
	}
	else if(bit_dcode_n == 58 && (dcf77_Month&0x01) && !bit || bit_dcode_n && !(dcf77_Month&0x01) && bit);//Parity
	bit_dcode_n++;
	return flag_no_error;
}
int dcf77::status()
{
	return dcf77_status;
}
uint8_t dcf77::minutes()
{
	return dcf77_Minute;
}
uint8_t dcf77::heure()
{
	return dcf77_Heur;
}
uint8_t dcf77::month()
{
	return dcf77_Month;
}
uint8_t dcf77::day_month()
{
	return dcf77_Daymonth;
}
uint8_t dcf77::year()
{
	return dcf77_Year;
}
ISR(INT0_vect)//Vecteur d'interuption INT0
{
	ATOMIC_BLOCK(ATOMIC_FORCEON)
	{
		if(((PIND&(1<<PIND2))>>PIND2))//Si c'est au niveau haut
		{
			atc_.hi();
		}
		else //Passage au niveau bas
		{
			atc_.lo();
		}
	}
}