/*
 * Horloge à Tube Nixie
 *
 *
 * V:1.1Beta
 * Dernière modification : 27.05.14
 *
 */ 
#define F_CPU 16000000UL //Fréquence horloge à 16Mhz
#include <avr/io.h>
#include <util/delay.h>

#include "./fonction.h" //Inclusion des fonction

int main(void) //Fonction Principale
{
	setup_pins(); //Configuration des pins et du registre à décalage
	int a = 0;
	while(1)//Boucle infinie
	{
		if(a<=10000)
		{
			first();
			routine_affichage(); //Routinne d'affihage des tube nixie
			bouton(); //Gestion des boutons de configuration
			couleur(); //Gestion des LED RGB
			uart_ds1307();
			a++;
		}
		else
			affichage_shutdwon();
		if(!(PIND&(1<<PIND2)))
			a=0;
		_delay_ms(1);
	}
}