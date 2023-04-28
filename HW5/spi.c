#include "nu32dip.h"
#include "spi.h"

void initSPI(); // function prototype
unsigned char spi_io(unsigned char); // function prototype

int main() {
    NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
    initSPI(); // initialize SPI1
    NU32DIP_WriteUART1("hello\r\n"); // test if pic is working
    
    // pre-calculate all the voltages in the sine wave:
    unsigned int sin_array[200];
    float t = 0; // initial value inside of sin(t)
    for (i = 0; i <200; i++) {
        float sine_value = 511.5 * (sine(t) + 1); // scales the amplitude of the sine wave to be between 0-1023
        sin_array[i] = sine_value; // assign an integer 0-1023 representing the voltage from the sine wave.
        t = t + (pi / 50); // increment up along x-axis for sine calculation (2 periods: 0->4pi). pi/50 = 4pi/200s
    }

    
    while (1) { // enter infinite while loop
        // figure out voltage for 2Hz sine wave
        for (j = 0; j < 200; j++) {
            int sin_V = sin_array[i]; // grab the pre-calculated voltage
            //[0 1 1 1 [10-bit voltage] 0 0];
            unsigned short V_a = 0;
            unsigned short V_a_start = 0;
            V_a_start = 0b0111 << 12; // assign the first part of the bits to be sent to the DAC_A port. 
            // stick in the 10-bit voltage value
            unsigned short V_a_10_bit = 0;
            V_a_10_bit = sin_V << 2; // take the voltage value and stick it in bits 11-2. ****IS THIS RIGHT? NEED TO CONVERT TO BINARY???
            V_a = V_a_start | V_a_10_bit; // turn voltage into 16-bit number
            // send the voltage with SPI
            LATAbits.LATA0 = 0; //manually turn chip select pin (pin A0) low
            //call spi_io function twice:
            //1 for first 8 bits,
            spi_io(V_a>>8); // send the first 8 bits of the voltage message
            //another for 2nd 8-bits
            spi_io(V_a & 0xFF); // send the last 8 bits of the voltage message
            //manually turn CS pin high again (idle)
            LATAbits.LATA0 = 1; //manually turn chip select pin (pin A0) high to signal done sending data
            // delay 5ms for a 2Hz sine wave:
            _CP0_SET_COUNT(0); // set timer to 0
            while (_CP0_GET_COUNT() < 24000 * 5) {} // Delay for 5ms before grabbing the next voltage for the sine wave
        }
        
        // 
        // figure out voltage for triangle wave
        // send the voltage with SPI
        //
        // delay and then loop
    }
}


// initialize SPI1
void initSPI() {
    // Pin B14 has to be SCK1 *****NEED TO DO ANYTHING FOR SCK1 IN CODE???****
    // Turn ofF analog pins b/c they'll override the spi pins
    //...//ANSEL bits
    // Make an output pin for CS (chip select)
    TRISAbits.TRISA0 = 0;// make pin A0 (Chip select) an output pin and... 
    LATAbits.LATA0 = 1; // set pin A0 high
    // Set SDO1 as an output for SPI:
    TRISBbits.TRISB1 = 0; // make pin B1 an output pin
    RPB1bits.RPB1R = 0b0011; // set it as SD01 pin for SPI communication
    // Set SDI1 as an input. we don't actually wire this up to anything b/c our DAC doesn't have a pin for it
    TRISAbits.TRISA1 = 1; // make pin A1 an input pin
    SDI1bits.SDIR = 0b0000; // set pin A1 as SDI1 input.

    // setup SPI1
    SPI1CON = 0; // turn off the spi module and reset it. clear all the bits in the control register.
    SPI1BUF; // clear the rx buffer by reading from it. read the data in the buffer to clear it out
    SPI1BRG = 1000; // Set Baud Rate: 1000 for 24kHz, 1 for 12MHz; // baud rate to 10 MHz [SPI1BRG = (48000000/(2*desired))-1] ONCE DONE DEBUGGING, SET = 1
    SPI1STATbits.SPIROV = 0; // clear the overflow bit (A flag we're clearing and not reading)
    SPI1CONbits.CKE = 1; // data changes when clock goes from hi to lo (since CKP is 0)
    SPI1CONbits.MSTEN = 1; // master operation (turn on "master" mode)
    SPI1CONbits.ON = 1; // turn on spi 
}


// send a byte via spi and return the response
unsigned char spi_io(unsigned char o) { //send an unsigned 8-bit #
  SPI1BUF = o; //sends it to this SFR
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte (wait until all 8 bits have been sent.
    ;
  }
  return SPI1BUF; // and when it does, it then returns the number that it got (just throw it away, it's random)
}