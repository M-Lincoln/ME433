//main_neopixel.c

/********MAKE SURE CODE IS IN OPTIMIZATION LEVEL 1***********/

#include "nu32dip.h" // constants, functions for startup and UART
#include "ws2812b.h" // neopixel header file

void main() {
    NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
    ws2812b_setup(); // setup the timer and the output pin for the neopixel

    // command the color you want
    wsColor color[100];
    unsigned int a = 130;
    unsigned int b = 180;
    unsigned int c = 270;
    unsigned int d = 50;
    unsigned int e = 300;
    //    //datatype: wsColor c;
    //    //c.r = ired;
    //    //c.g = igreen;
    //    //c.b = iblue;
//    color[0] = HSBtoRGB(a, 1, 0.5); //output is an RGB number
//    color[1] = HSBtoRGB(b, 1, 0.5); //output is an RGB number
//    color[2] = HSBtoRGB(c, 1, 0.5); //output is an RGB number
//    color[3] = HSBtoRGB(d, 1, 0.5); //output is an RGB number
//    color[4] = HSBtoRGB(e, 1, 0.5); //output is an RGB number
//    
//     // send your original value to the 5 LEDs: 
//    ws2812b_setColor(color, 5);
    
    while(1){ // make a rainbow that goes around color wheel infinitely!
        
        //unsigned int a_i=0;
//        unsigned int b_i=0;
//        unsigned int c_i=0;
//        unsigned int d_i=0;
//        unsigned int e_i=0;
//      // build your array for colors to send to your 5 LEDs
        color[0] = HSBtoRGB(a+1, 1, 0.5); //output is an RGB number
        color[1] = HSBtoRGB(b+1, 1, 0.5); //output is an RGB number
        color[2] = HSBtoRGB(c+1, 1, 0.5); //output is an RGB number
        color[3] = HSBtoRGB(d+1, 1, 0.5); //output is an RGB number
        color[4] = HSBtoRGB(e+1, 1, 0.5); //output is an RGB number
        
        // send that value to your LEDs: 
        ws2812b_setColor(color, 5);
        
        a = a+1; // incrementing up
        b = b+1;
        c = c+1;
        d = d+1;
        e = e+1;
        
        if (a > 360) {
            a = 0; // set a back to start of color wheel
        }
        if (b > 360) {
            b = 0; // set a back to start of color wheel
        }
        if (c > 360) {
            c = 0; // set a back to start of color wheel
        }
        if (d > 360) {
            d = 0; // set a back to start of color wheel
        }
        if (e > 360) {
            e = 0; // set a back to start of color wheel
        }
        // delay
        _CP0_SET_COUNT(0); // set timer to zero
        while (_CP0_GET_COUNT() <  24000000/1000) { // should = 0.5s delay
            //wait
        }
    }
}