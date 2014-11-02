/*
 * DS1307.h
 *
 * Created: 22/12/2013 15:28:08
 *  Author: Adrien
 */ 


#ifndef DS1307_H_
#define DS1307_H_
#include <stdint.h>
class ds1307
{
	private:
		uint8_t curent_couleur;
		bool iic_error;
	public:
		ds1307();
		void write(int adresse_m, uint8_t data);
		uint8_t read(uint8_t adresse_m);
		uint8_t r_seconde();
		uint8_t r_minute();
		uint8_t r_heur();
		uint8_t r_jour();
		uint8_t r_moi();
		uint8_t r_year();
		void w_seconde(uint8_t data);
		void w_minute(uint8_t data);
		void w_heur(uint8_t data, uint8_t pm);
		void w_date(uint8_t data);
		void w_moi(uint8_t data);
		void w_year(uint8_t data);
};



#endif /* DS1307_H_ */