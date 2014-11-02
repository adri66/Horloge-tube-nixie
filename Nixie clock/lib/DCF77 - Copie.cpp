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

volatile uint8_t dcf77_bits_capture[6]; //Tableau comportent les bits d�coder (6 * 8bits)
volatile unsigned long int time_low = 0;
volatile unsigned long int time_high = 0;
volatile bool dcf77_flag_start = 0; // Drapeau qui permet de savoir si on peut commenc� la capture
volatile bool dcf77_flag_decode = 0; //  "      "    "     "   "     "  "  "      "     le d�codage
volatile int dcf77_bit_count = 0; // Compte le nombre de bit d�ja encod�
void dcf77::start_capture()
{
	timer1_init();
	dcf77_flag_start = 0;
	DDRD |= 1<<PIND2;
	EICRA |= 1<<ISC00; //Interuption a chaque changement d'�tat de INT0
	EIMSK |= 1<<INT0; //Activation de l'interuption INT0
	sei();
}
void dcf77::stop_capture()
{
	EIMSK &= ~(1<<INT0);
	dcf77_flag_start = 0;
	dcf77_flag_decode = 0;
	dcf77_bit_count = 0;
}
bool dcf77::decode()
{
	int decodage_bit_n = 0;//Variable pour voir ou est le decodage des bits
	int place_bit = 0;//Pour savoir ou est plac� le bit
	for (int z = 0 ; z <= 6 ; z++) //Passage au crible des tableau
	{
		int place_bit = 0;//Pour savoir ou est plac� le bit
		for(int y= 8 ; y >= 0 ; y--)
		{
			if(z == 0 && !(dcf77_bits_capture[0] >> 8)) //Bit 00 TOUJOURS � 0
			{
				if(decodage_bit_n <= 19) // Bit 00 � 19 ne nous interesse pas
				{}
				else if(decodage_bit_n <= 28) //Partie minutes
				{
					if((dcf77_bits_capture[z]>>y) && decodage_bit_n == 20) //Bit 20 TOUJOURS � 1
					{}
					else
						return false;
					if (decodage_bit_n > 20 &&  decodage_bit_n <= 28)//Arriv� a la partie minute
					{
						if(decodage_bit_n == 21)
							place_bit = 0;
						if(decodage_bit_n != 28)
						{
							Minute |= ((dcf77_bits_capture[z]>>y) & 0x01) << place_bit;
							place_bit++;
						}
						else if((Minute&0x1) && !((dcf77_bits_capture[z]>>y)&0x01) || !(Minute&0x1) && ((dcf77_bits_capture[z]>>y)&0x01))
						{}
						else
							return false;
					}
				}
			}
			else
				return false;
			decodage_bit_n++;
		}
	}
}
ISR(INT0_vect)
{
	if(PORTD&(1<<PIND2))//Si c'est au niveau haut
	{
		time_high = millis(); //D�marage du comptage du temps haut
		time_low = millis() - time_low; //Calcul du Temps bas		
		if(dcf77_flag_start && dcf77_bit_count <= 59) //D�marage de la d�modulation si toute les condition sont OK
		{
			dcf77_flag_decode = 0; //Anonce que le decodage ne peut pas �tre �fectu�
			int bit;
			if(time_low <= 100 && time_low != 0) //Si le temp bas est <= 100ms le bit est � 0
				bit = 0;
			else if(time_low <= 200 && time_low != 0) //Si le temp bas est <= 200ms le bit est � 1
				bit = 1;
			else //ereur
			{
				dcf77_flag_start = 0;
				return;
			}
			if(dcf77_bit_count < 8)
			{
				dcf77_bits_capture[0] = (dcf77_bits_capture[0]<<1) | bit;
			}
			else if (dcf77_bit_count < 16)
			{
				dcf77_bits_capture[1] = (dcf77_bits_capture[1]<<1) | bit;
			}
			else if (dcf77_bit_count < 24)
			{
				dcf77_bits_capture[2] = (dcf77_bits_capture[2]<<1) | bit;
			}
			else if (dcf77_bit_count < 32)
			{
				dcf77_bits_capture[3] = (dcf77_bits_capture[3]<<1) | bit;
			}
			else if (dcf77_bit_count < 40)
			{
				dcf77_bits_capture[4] = (dcf77_bits_capture[4]<<1) | bit;
			}
			else if (dcf77_bit_count < 48)
			{
				dcf77_bits_capture[5] = (dcf77_bits_capture[5]<<1) | bit;
			}
			else if (dcf77_bit_count < 56)
			{
				dcf77_bits_capture[6] = (dcf77_bits_capture[6]<<1) | bit;
			}
			dcf77_bit_count++;
		}
		else if(dcf77_bit_count > 59)//Apr�s le d�codage 60e bit
		{
			dcf77_flag_start = 0; //Remise a zero du drapeau de d�marage
			dcf77_flag_decode = 1; //Mise a un du drapeau decode
			dcf77_bit_count = 0; //Mise � zero le nombre de bit decod�
		}
	}
	else //Passage au niveau bas
	{
		time_high = millis() - time_high; //Calcul du temps Haut
		time_low = millis(); //D�marage du comptage du temps bas
		if(time_high > 800) //Si le Temps bas est > � 800, d�but de la transmition
			dcf77_flag_start = 1;
	}
}