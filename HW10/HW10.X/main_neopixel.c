//main_neopixel.c

/********MAKE SURE CODE IS IN OPTIMIZATION LEVEL 1***********/

#include "nu32dip.h" // constants, functions for startup and UART
#include "ws2812b.h" // neopixel header file

void main() {
    NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
    ws2812b_setup(); // setup the timer and the output pin for the neopixel

    // command the color you want
    wsColor c[100];
    c[0] = HSBtoRGB(130, 1, 0.5); //output is an RGB number
    c[1] = HSBtoRGB(180, 1, 0.5); //output is an RGB number
    c[2] = HSBtoRGB(270, 1, 0.5); //output is an RGB number
    c[3] = HSBtoRGB(130, 1, 0.5); //output is an RGB number
    c[4] = HSBtoRGB(130, 1, 0.5); //output is an RGB number
    //// output is:
    //    //datatype: wsColor c;
    //    //c.r = ired;
    //    //c.g = igreen;
    //    //c.b = iblue;


    //// send that value to: 
    ws2812b_setColor(c, 5);
}