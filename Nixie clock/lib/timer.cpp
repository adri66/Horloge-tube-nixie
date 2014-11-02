#define F_CPU 16000000UL // 16 MHz

// Calculate the value needed for
// the CTC match value in OCR1A.
#define CTC_MATCH_OVERFLOW ((F_CPU / 1000) / 8)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
volatile unsigned long timer1_millis;
void timer1_init()
{
	 // CTC mode, Clock/8
	 TCCR1B |= (1 << WGM12) | (1 << CS11);

	 // Load the high byte, then the low byte
	 // into the output compare
	 OCR1AH = (CTC_MATCH_OVERFLOW >> 8);
	 OCR1AL = CTC_MATCH_OVERFLOW;

	 // Enable the compare match interrupt
	 TIMSK1 |= (1 << OCIE1A);

	 // Now enable global interrupts
	 sei();

}
void interup_1hz_init()
{
	EICRA |= 1<<ISC11 | 1<< ISC10; //Configuration pour activer INT1 sur un front montant
	EIMSK |= 1<<INT1; // Activation de l'interuption INT1
	sei();
}
unsigned long millis ()
{
	unsigned long millis_return;

	// Ensure this cannot be disrupted
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		millis_return = timer1_millis;
	}

	return millis_return;
}
ISR (TIMER1_COMPA_vect)
{
	timer1_millis++;
}

/*
 * timer.cpp
 *
 * Created: 21/12/2013 15:45:30
 *  Author: Adrien
 *
#include <avr/interrupt.h>
#include <avr/io.h>

#include "timer.h"
volatile unsigned long us = 0;
volatile unsigned long us_s = 0;
volatile unsigned long int s = 0;
#define calibration 1;
void timer1_init()
{
    TCCR1A = 0x00; //D�sactive les comparateur + Mode normal
    TCCR1B = 0x00;//Mise a zero de toutes les valeurs
	us = 0;
	s = 0;
	TCNT1 = 0x0000;
    /*
    Horloge � 16Mhz
    1 p�riode = 62.5ns
    clk/8 = 0.5�s d�bordement � 32768 �s
    clk/64 = 4�s d�bordement � 262144 �s
	clk/256 = 16�s d�bordement � 1 048 576�s
	clk/1024 = 64�s d�bordement �  4 194 304�s
    Le prescale sera � 8
    *
    TCCR1B |= (1<<CS12) | (0<<CS11) | (1<<CS10); //Prescale � 8
    TIMSK1 = 1<<TOIE1; // Initialisation de l'interuption de d�bordement 
    sei();
}
void interup_1hz_init()
{
	EICRA |= 1<<ISC11 | 1<< ISC10; //Configuration pour activer INT1 sur un front montant
	EIMSK |= 1<<INT1; // Activation de l'interuption INT1
	sei();	
}
unsigned long int millis()
{
	return (TCNT1/64)/100;
}
unsigned long int secondes()
{
	return s;
}
ISR(TIMER1_OVF_vect)
{
   // us += 32768; //Lors du d�bordement le timer est �gale � (2^16)*0.5 ms
   TCNT1 = 0;
}
ISR(INT1_vect)
{
	s++;
}*/