#include "nu32dip.h" // constants, functions for startup and UART
#include "i2c_master_noint_oled.h"
#include "mpu6050.h"
#include <stdio.h>

void blink(int, int); // blink the LEDs function

//int main(void) {
//    NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
//    init_mpu6050(); // initialize the IMU
//    i2c_master_setup(); // initialize the I2C pins
//
//	// char array for the raw data //SEE YOUTUBE VIDEO 2022 15:11 FOR CODE HERE
//    unsigned char d[14]; // functions read all of the data out. we will get 7 pieces of info, will need 14 unsigned chars to store that data
//	// MAKE floats to store the data (converted from deg/s to g's)
//	float ax, ay, az, gx, gy, gz, t; // acceleration in x,y,z directions, gyroscope in x,y,z directions, and temperature
//	// read whoami
//    unsigned char who;
//    who = whoami();
//	// print whoami
//    char m[100];
//    sprintf(m,"0x%x\r\n",who); // prints the number as hex
//    NU32DIP_WriteUART1(m);
//	// if whoami is not 0x68, stuck in loop with LEDs on
//    if(who!=0x68){ // THIS MIGHT ACTUALLY NOT WORK
//        while(1){
//            //blink Yellow LED
//        }
//    }
//    
//	// wait to print until you get a newline (Python code will hit enter for us)
//    NU32DIP_ReadUART1(m,100);
//
//    while (1) {
//		// use core timer for exactly 100Hz loop
//        _CP0_SET_COUNT(0);
//        blink(1, 5); // blink yellow and green LEDs to make sure everything is working
//
//        // read IMU
//        burst_read_mpu6050(d); // read all the raw data into d
//		// convert data (send to conversion fnc) to g's
//        ax = conv_xXL(d); //
//        ay = conv_yXL(d); // 
//        az = conv_zXL(d);
//        gx = conv_xG(d);
//        gy = conv_yG(d);
//        gz = conv_zG(d);
//        t = conv_temp(d);
//
//        // print out the data
//        // sprintf(m,"%f\t %f\t %f\t %f\t %f\t %f\t %f\t \r\n",ax, ay, az, gx, gy, gz, t); // print all accels and temp to putty
//        sprintf(m,"%f\r\n",ax); // print x accel ONLY to putty. THIS WORKS WITH PYTHON PLOTTER.PY CODE
//        NU32DIP_WriteUART1(m);
//        while (_CP0_GET_COUNT() < 48000000 / 2 / 100) { // reading data out at 100hz
//        }
//    }
//}

// blink the LEDs
void blink(int iterations, int time_ms) {
    int i;
    unsigned int t;
    for (i = 0; i < iterations; i++) {
        NU32DIP_GREEN = 0; // on
        NU32DIP_YELLOW = 1; // off
        t = _CP0_GET_COUNT(); // should really check for overflow here
        // the core timer ticks at half the SYSCLK, so 24000000 times per second
        // so each millisecond is 24000 ticks
        // wait half in each delay
        while (_CP0_GET_COUNT() < t + 12000 * time_ms) {
        }

        NU32DIP_GREEN = 1; // off
        NU32DIP_YELLOW = 0; // on
        t = _CP0_GET_COUNT(); // should really check for overflow here
        while (_CP0_GET_COUNT() < t + 12000 * time_ms) {
        }
    }
}
