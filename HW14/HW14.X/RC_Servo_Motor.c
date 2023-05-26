//RC_Motor.c
// PWM generation notes: look at Ch24_2_2_PWM.c in ME333 Hw14 folder
//#include <stdio.h>
//#include <xc.h>
#include "nu32dip.h"

int main(void) {
	NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init

	RPA0Rbits.RPA0R = 0b0101; // set pin2 as an OC1 pin
	T3CONbits.TCKPS = 0b100; // Timer3 prescaler N=16 (1:16)
	PR3 = 60000;         // period = (PR3+1) * N * (1/48000000) = 6 kHz
	TMR3 = 0;                // initial TMR3 count is 0
	OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
	OC1CONbits.OCTSEL = 1;   // Use timer3
	
	OC1RS = 0;               // initialize duty cycle to zero duty cycle = OC1RS/(PR3+1) = 0%
	OC1R = 0;                // initialize before turning OC1 on; afterward it is read-only
	T3CONbits.ON = 1;        // turn on Timer3
	OC1CONbits.ON = 1;       // turn on OC1

	//Calculate duty cycle needed
	float forty5_deg_time =0.9; //try to find the zero degree time 2ms
    float one35_deg_time =1.8; //try to find the zero degree time 2ms
	float dutycycle_time = 20; // 20ms time of 1 period
	float forty5_dutycycle = forty5_deg_time / dutycycle_time; // calculate duty cycle needed
    float one35_dutycycle = one35_deg_time / dutycycle_time; // calculate duty cycle needed

	
	
	while (1) { // infinite loop
		OC1RS = forty5_dutycycle * (PR3+1);             // 45% duty cycle. ex: set duty cycle to 10%: 0.1*(PR3+1) duty cycle = OC1RS/(PR3+1)
        OC1R = forty5_dutycycle * (PR3 + 1);          // initialize before turning OC1 on; afterward it is read-only
        // delay for 4 seconds
        _CP0_SET_COUNT(0); // set timer to 0
        while(_CP0_GET_COUNT()<96000000) {      // delay 4s
            ;// do nothing
        } 
        OC1RS = one35_dutycycle * (PR3+1);             // 45% duty cycle. ex: set duty cycle to 10%: 0.1*(PR3+1) duty cycle = OC1RS/(PR3+1)
        OC1R = one35_dutycycle * (PR3 + 1);          // initialize before turning OC1 on; afterward it is read-only
        _CP0_SET_COUNT(0); // set timer to 0
        while(_CP0_GET_COUNT()<96000000) {      // delay 4s
            ;// do nothing
        } 
	}
    return 0;
}