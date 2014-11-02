/*
 * _74hc595.h
 *
 * Created: 22/12/2013 15:19:47
 *  Author: Adrien
 */ 


#ifndef HC74595_H_
#define HC74595_H_

#include <stdint.h>
class hc74595
{
	private:
		int pin_DATA;
		int pin_CLK;
		int pin_STCP;
		volatile uint8_t *port;
		volatile uint8_t *ddr;
	public:
		hc74595(int data, int clk, int stcp, volatile uint8_t *port_uc);
		void send(uint8_t data, bool inverted);
		void confirm();
};

#endif /* 74HC595_H_ */