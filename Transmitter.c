#include <at89lp51rd2.h>
#include "Common.h"
#include "Transmitter.h"

volatile unsigned char serial_bit;

unsigned char _c51_external_startup(void)
{
	// Configure ports as a bidirectional with internal pull-ups.
	P0M0=0;	P0M1=0;
	P1M0=0;	P1M1=0;
	P2M0=0;	P2M1=0;
	P3M0=0;	P3M1=0;
	AUXR=0B_0001_0001; // 1152 bytes of internal XDATA, P4.4 is a general purpose I/O
	P4M0=0;	P4M1=0;
    	
	TR0 = 0;
	
	TMOD = 0x01;
	
	TH0 = TIMER0_RELOAD / 0x100;
	TL0 = TIMER0_RELOAD % 0x100;	
	
	TR0 = 1;
	
	ET0 = 1;
	
	EA = 1;
	
    return 0;
}

void modulate (void) interrupt 1
{
	TH0 = TIMER0_RELOAD / 0x100;
	TL0 = TIMER0_RELOAD % 0x100;
	/*
	if(++pwmcount1>1) pwmcount1=0;
	if(++pwmcount2>1) pwmcount2=0;
	P1_0=(pwm>pwmcount1)?1:0;
	P1_1=(pwm>pwmcount2)?1:0;
	*/
	if (serial_bit) {
		OUT_MODULATE1 = !OUT_MODULATE1;
		OUT_MODULATE2 = !OUT_MODULATE1;
	}
	else {
		OUT_MODULATE1 = 0;
		OUT_MODULATE2 = 0;
	}
}

void send_byte(unsigned char byte) {
	unsigned char i;
	
	serial_bit = 0;
	wait_half_bit();
	wait_half_bit();
	
	for (i = 0; i < 8; ++i) {
		serial_bit = byte & (0x01 << i) ? 1 : 0;
		wait_half_bit();
		wait_half_bit();
	}
	
	serial_bit = 1;
	wait_half_bit();
	wait_half_bit();
	wait_half_bit();
	wait_half_bit();
}

void main (void)
{
	unsigned char instruction = 0;
	
	
	while (1) {
		send_byte(instruction++);
		//serial_bit = 1;
	}
	
}

