//RC_Motor.c
// PWM generation notes: look at Ch24_2_2_PWM.c in ME333 Hw14 folder
//#include <stdio.h>
//#include <xc.h>
#include "nu32dip.h"
// PIC UART2 code
#include "uart2.h" // include the library




int main(void) {
	NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
	// after NU32DIP_startup()
	UART2_Startup();
	// Setting up PWM for Motor A
	RPA0Rbits.RPA0R = 0b0101; // set pin2 as an OC1 pin
	T3CONbits.TCKPS = 0; // Timer3 prescaler N=1
	PR3 = 2399;         // period = (PR3+1) * N * (1/48000000) = 20 kHz, 200us
	TMR3 = 0;                // initial TMR3 count is 0
	OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
	OC1CONbits.OCTSEL = 1;   // Use timer3
	
	OC1RS = 0;               // initialize duty cycle to zero duty cycle = OC1RS/(PR3+1) = 0%
	OC1R = 0;                // initialize before turning OC1 on; afterward it is read-only
	T3CONbits.ON = 1;        // turn on Timer3
	OC1CONbits.ON = 1;       // turn on OC1

	// Setting up PWM for Motor B
	RPB1Rbits.RPB1R = 0b0101; // set pin5 as an OC2 pin
	T2CONbits.TCKPS = 0; // Timer2 prescaler N=1
	PR2 = 2399;         // period = (PR2+1) * N * (1/48000000) = 20kHz, 200us
	TMR2 = 0;                // initial TMR4 count is 0
	OC2CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
	OC2CONbits.OCTSEL = 0;   // Use timer2

	OC2RS = 0;               // initialize duty cycle to zero duty cycle = OC1RS/(PR3+1) = 0%
	OC2R = 0;                // initialize before turning OC1 on; afterward it is read-only
	T2CONbits.ON = 1;        // turn on Timer2
	OC2CONbits.ON = 1;       // turn on OC2
	
	
	while (1) { // infinite loop
		
		// PIC UART2 CODE TO READ THE LINE POSITION FROM THE PICO:
		// in while(1)
		int com = 0;
		// uart2_flag() is 1 when uart2 has rx a message and sprintf'd it into a value
		if (get_uart2_flag()) {
			set_uart2_flag(0); // set the flag to 0 to be ready for the next message
			com = get_uart2_value();
            char message2[100];
			sprintf(message2, "%d\r\n", com);
			NU32DIP_WriteUART1(message2);
		}
		/**********CODE FOR BLUE TT MOTOR CONTROL*********************/
		// where does the robot need to move to follow the line?:
		float K_p_A = 1; // define proportional control gain for motor A
		float K_p_B = 1; // define proportional control gain for motor B
		//float error_Position = com - 29.5 // calculate error
		if (25<=com<=35)
			// pretty much straight on the line!
			// code for right motor proportional control: FULL SPEED/PWM
			K_p_A = 1;
			// code for left motor proportional control: FULL SPEED/PWM
			K_p_B= 1;
		if (com>35)
			//Need to turn right:
			// code for right motor proportional control: NEEDS TO SLOW DOWN
			K_p_A = 0.2;
			// code for left motor proportional control: NEEDS TO SPEED UP
			K_p_B = 1;
		if (com<25) 
			// need to turn left:
			// code for right motor proportional control: Needs to SPEED UP
			K_p_A = 1;
			// code for left motor proportional control: NEEDS TO SLOW DOWN
			K_p_B = 0.2;
		// code for right motor PWM:
		
		OC1RS = K_p_A * 0.5 * (PR3 + 1);             // % duty cycle. ex: set duty cycle to 10%: 0.1*(PR3+1) duty cycle = OC1RS/(PR3+1)
		OC1R = K_p_A * 0.5 * (PR3 + 1);
		// CODE FOR SETTING PHASEA PIN TO 0 FOR FORWARD MOTION:
		LATAbits.LATA3 = 0; // set Pin A3 as low for FORWARD motion
		// code for left motor PWM:
		OC2RS = K_p_B * 0.5 * (PR2 + 1);             // 45% duty cycle. ex: set duty cycle to 10%: 0.1*(PR3+1) duty cycle = OC1RS/(PR3+1)
		OC2R = K_p_B * 0.5 * (PR2 + 1);
		// CODE FOR SETTING PHASEB PIN TO 0 FOR FORWARD MOTION:
		LATBbits.LATB0 = 0; // set Pin B0 as low for FORWARD motion
		}
    return 0;
	}
    