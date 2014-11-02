/*
 * fonction.h
 *
 * Created: 22/12/2013 15:47:39
 *  Author: Adrien
 */ 


#ifndef FONCTION_H_
#define FONCTION_H_

void first();
void setup_pins();
void affichage_heur();
void affichage_date();
void clignotement(int n, bool date);
void routine_affichage();
void couleur();
void bouton();
void uart_ds1307();
void affichage_shutdwon();
int int_to_bcd(int val);


#endif /* FONCTION_H_ */