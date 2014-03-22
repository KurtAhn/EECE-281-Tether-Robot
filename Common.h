#ifndef COMMON_H
#define COMMON_H

#define CLOCK 22118400L
#define BAUD_RATE 300L
//#define TIMER0_RELOAD (0x10000L - CLOCK / (12L * BAUD_RATE))
#define FREQUENCY 15920L
//#define TIMER1_RELOAD (0x10000L - CLOCK / (24L * FREQUENCY))

void wait_half_bit() {
	_asm
		mov r1, #49
	WAIT_1:
		mov r0, #250
	WAIT_0:
		djnz r0, WAIT_0
		djnz r1, WAIT_1
	_endasm;
}

#endif
