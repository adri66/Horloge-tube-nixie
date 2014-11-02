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
		uint8_t dcf77_bits[6];
		uint8_t Minute = 0x00;
		uint8_t Heur = 0x00;
		uint8_t Daymonth = 0x00;
		uint8_t Dayweek = 0x00;
		uint8_t Month = 0x00;
		uint8_t Year = 0x00;
		bool dcf77_flag_send_decode;
		bool decode();	
	public:
		void start_capture();
		void stop_capture();
		bool ready();
		uint8_t minutes();
		uint8_t heure();
		uint8_t day_month();
		uint8_t day_week();
		uint8_t month();
		uint8_t year();
};



#endif /* DCF77_H_ */