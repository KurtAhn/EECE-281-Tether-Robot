//baud rate: 300

#include <stdio.h>
#include <at89lp51rd2.h>
#include "Common.h"

#define WHEEL_LEFT1 P3_4
#define WHEEL_LEFT2 P3_5
#define WHEEL_RIGHT1 P3_6
#define WHEEL_RIGHT2 P3_7
#define RADIUS 4.5
#define C_WHEEL 21.362
#define CLK 22118400L
#define BAUD 115200L
#define BRG_VAL (0x100-(CLK/(32L*BAUD)))
#define FREQ 10000L
#define TIMER0_RELOAD_VALUE (65536L-(CLK/(12L*FREQ)))

volatile unsigned char pwmcount;
volatile unsigned char pwm1, pwm2, pwm3, pwm4;

unsigned char _c51_external_startup(void)
{
	// Configure ports as a bidirectional with internal pull-ups.
	P0M0=0;	P0M1=0;
	P1M0=0;	P1M1=0;
	P2M0=0;	P2M1=0;
	P3M0=0;	P3M1=0;
	AUXR=0B_0001_0001; // 1152 bytes of internal XDATA, P4.4 is a general purpose I/O
	P4M0=0;	P4M1=0;
	
	// Initialize timer 0 for ISR 'pwmcounter()' below
	TR0=0; // Stop timer 0
	TMOD=0x01; // 16-bit timer
	// Use the autoreload feature available in the AT89LP51RB2
	// WARNING: There was an error in at89lp51rd2.h that prevents the
	// autoreload feature to work.  Please download a newer at89lp51rd2.h
	// file and copy it to the crosside\call51\include folder.
	TH0=RH0=TIMER0_RELOAD_VALUE/0x100;
	TL0=RL0=TIMER0_RELOAD_VALUE%0x100;
	TR0=1; // Start timer 0 (bit 4 in TCON)
	ET0=1; // Enable timer 0 interrupt
	EA=1;  // Enable global interrupts
	
	pwmcount=0;
    
    return 0;
}

void pwmcounter (void) interrupt 1
{
	if(++pwmcount>99){
		pwmcount=0;
	}
	WHEEL_LEFT1=(pwm1>pwmcount)?1:0;
	WHEEL_LEFT2=(pwm2>pwmcount)?1:0;
	WHEEL_RIGHT1=(pwm3>pwmcount)?1:0;
	WHEEL_RIGHT2=(pwm4>pwmcount)?1:0;
}

void wait_one_and_half_bit_time()
{
	wait_half_bit();
	wait_half_bit();
	wait_half_bit();	
}

void wait_bit_time()
{
	wait_half_bit();
	wait_half_bit();
}



void SPIWrite(unsigned char value)
{
	SPSTA&=(~SPIF); // Clear the SPIF flag in SPSTA
	SPDAT=value;
	while((SPSTA & SPIF)!=SPIF); //Wait for transmission to end
}

unsigned int GetADC(unsigned char channel)
{
	unsigned int adc;

	// initialize the SPI port to read the MCP3004 ADC attached to it.
	SPCON&=(~SPEN); // Disable SPI
	SPCON=MSTR|CPOL|CPHA|SPR1|SPR0|SSDIS;
	SPCON|=SPEN; // Enable SPI
	
	P1_4=0; // Activate the MCP3004 ADC.
	SPIWrite(channel|0x18);	// Send start bit, single/diff* bit, D2, D1, and D0 bits.
	for(adc=0; adc<10; adc++); // Wait for S/H to setup
	SPIWrite(0x55); // Read bits 9 down to 4
	adc=((SPDAT&0x3f)*0x100);
	SPIWrite(0x55);// Read bits 3 down to 0
	P1_4=1; // Deactivate the MCP3004 ADC.
	adc+=(SPDAT&0xf0); // SPDR contains the low part of the result. 
	adc>>=4;
		
	return adc;
}

unsigned char rx_byte ( int min )
{
	unsigned char j, val;
	int v;
	//Skip the start bit
	val=0;
	wait_one_and_half_bit_time();
	for(j=0; j<8; j++)
	{
		v=GetADC(0);
		val|=(v>min)?(0x01<<j):0x00;
		wait_bit_time();
	}
	//Wait for stop bits
	wait_one_and_half_bit_time();
	return val;
}



void move_wheel (unsigned char wheel_num, char speed)
{
	TR0 = 0;
	
	speed = speed > 50 ? 50 : (speed < -50 ? -50 : speed);
	
	if (wheel_num == 0) {
		pwm1 = 50 - speed;
		pwm2 = 50 + speed;
	}
	if (wheel_num == 1) {
		pwm3 = 50 - speed;
		pwm4 = 50 + speed;
	}
	
	TR0 = 1;
}

void move_straight (char speed)
{
	move_wheel(0, -speed);
	move_wheel(1, speed);
}

void turn_right (char degree) 
{
	float turn_pwm = 2*3.14*4.65*degree/360/20.7348/0.8*100;
	move_wheel(0, turn_pwm/2);
	move_wheel(1, turn_pwm/2);
}

void main()
{
	
	turn_right(90);

}
	
