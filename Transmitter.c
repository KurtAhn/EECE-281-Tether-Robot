#include <stdio.h>
//#include <stdlib.h>
#include <at89lp51rd2.h>

// ~C51~ 
 
#define CLK 22118400L
#define BAUD 115200L
#define BRG_VAL (0x100-(CLK/(32L*BAUD)))

//We want timer 0 and 1 freq 2 x 15.92 kHz (15920Hz)
#define FREQ 31840L
#define TIMER0_RELOAD_VALUE (65536L-(CLK/(12L*FREQ)))

//Definitions for Bonus Calculations
#define RPM_RATIO 0.09
#define PI 3.14159 

//These variables are used in the ISR
volatile unsigned char pwmcount1;
volatile unsigned char pwmcount2;
volatile unsigned char pwm;

unsigned char _c51_external_startup(void)
{
	// Configure ports as a bidirectional with internal pull-ups.
	P0M0=0;	P0M1=0;
	P1M0=0;	P1M1=0;
	P2M0=0;	P2M1=0;
	P3M0=0;	P3M1=0;
	AUXR=0B_0001_0001; // 1152 bytes of internal XDATA, P4.4 is a general purpose I/O
	P4M0=0;	P4M1=0;
    
    // Initialize the serial port and baud rate generator
    PCON|=0x80;
	SCON = 0x52;
    BDRCON=0;
    BRL=BRG_VAL;
    BDRCON=BRR|TBCK|RBCK|SPD;
	
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
	
	pwmcount1=0;
	pwmcount2=1;
    
    return 0;
}


// Interrupt 1 is for timer 0.  This function is executed every time
// timer 0 overflows: 15.92 kHz.
void pwmcounter (void) interrupt 1
{
	if(++pwmcount1>1) pwmcount1=0;
	if(++pwmcount2>1) pwmcount2=0;
	P1_0=(pwm>pwmcount1)?1:0;
	P1_1=(pwm>pwmcount2)?1:0;
}

void main (void)
{
	pwm=1; //default 50% duty cycle 1 wave at 100Hz
	
	
}

