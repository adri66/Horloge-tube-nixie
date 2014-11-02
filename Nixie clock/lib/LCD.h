/*
 * LCD.h
 *
 * Created: 27/12/2013 13:32:19
 *  Author: Adrien
 */ 


#ifndef LCD_H_
#define LCD_H_


#include <stdint.h>
#include <string.h>

class PCF8574
{
	public:
		static uint8_t send(uint8_t data, uint8_t read);
};
class LCD
{
	private:
		bool check_busy_flag();
	public:
		void setup(int rs_pin, int rw_pin, int e_pin, int d4_pin, int d5_pin, int d6_pin, int d7_pin);
		void send(uint8_t data, bool BF);
		void display_init();
		void send_leter(uint8_t lettre);
		void print_text(char *txt);
		void print_number(int num);
		void clear();
		void set_cursor(int x, int y);
};


#endif /* LCD_H_ */