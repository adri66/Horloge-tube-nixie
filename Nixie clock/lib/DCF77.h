/*
 * DCF77.h
 *
 * Created: 22/12/2013 15:47:27
 *  Author: Adrien
 */ 


#ifndef DCF77_H_
#define DCF77_H_

#include <stdint.h>

class dcf77
{
	private:
		//uint8_t dcf77_bits[6];
		//bool dcf77_flag_send_decode;
		unsigned long int time_low_dcf77 = 0;
		unsigned long int time_high_dcf77 = 0;
		bool dcf77_flag_start = 0; // Drapeau qui permet de savoir si on peut commencé la capture
		//bool dcf77_flag_end_decode = 0; //  "      "    "     "   "     " le décodage est terminé
		int dcf77_bit_count = 0; // Compte le nombre de bit déja encodé
		uint8_t dcf77_Minute = 0x00;
		uint8_t dcf77_Heur = 0x00;
		uint8_t dcf77_Daymonth = 0x00;
		uint8_t dcf77_Dayweek = 0x00;
		uint8_t dcf77_Month = 0x00;
		uint8_t dcf77_Year = 0x00;
		int dcf77_status = 0;
		int bit_dcode_n = 0;
		bool parasite_flag = false;
		unsigned int time_check = 0;
		
	public:
		void start_capture();
		void stop_capture();
		void lo();
		void hi();
		uint8_t minutes();
		uint8_t heure();
		uint8_t day_month();
		uint8_t day_week();
		uint8_t month();
		uint8_t year();
		bool decode(uint8_t bit);
		int status();
};



#endif /* DCF77_H_ */