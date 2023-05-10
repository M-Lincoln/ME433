// based on adafruit and sparkfun libraries

#include <stdio.h>
#include <string.h> // for memset
#include <xc.h> // for the core timer delay
#include "ssd1306.h"
#include "i2c_master_noint_oled.h"
#include "font.h"
#include "nu32dip.h"

unsigned char ssd1306_write = 0b01111000; // i2c write address
unsigned char ssd1306_read = 0b01111001; // i2c read address
unsigned char ssd1306_buffer[512]; // 128x32/8. Every bit is a pixel. the organization of bits in this array matches the organization of pixels on the screen

int main(){
    NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
    i2c_master_setup(); // initialize the I2C pins. ***make baudrate as fast as possible to minimize amt of time talking
    ssd1306_setup(); // initialize the OLED screen and its registers
    
    NU32DIP_GREEN = 1; // turn green LED OFF
    while(1) {
        //blink green LED & 1 pixel on OLED for debugging purposes only
        NU32DIP_GREEN = 0;// turn on GREEN LED
        // blink one pixel on screen for debugging purposes only
        ssd1306_drawPixel(1,1,1); // turn ON 1 pixel (x =1, y = 1);
        ssd1306_update();//sends data to screen
        _CP0_SET_COUNT(0); // set timer to 0
        while (_CP0_GET_COUNT() < 24000000) {} // Delay for 1s
        NU32DIP_GREEN = 1;// blink green LED (turn it off) 
        ssd1306_drawPixel(1,1,0); // turn OFF pixel (x =1, y = 1);
        ssd1306_update();//sends data to screen
        _CP0_SET_COUNT(0); // set timer to 0
        while (_CP0_GET_COUNT() < 24000000) {} // Delay for 1s
    }
    // 
}
void ssd1306_setup() {
    
    // give a little delay for the ssd1306 to power up:
    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT() < 48000000 / 2 / 50) {
    }
    // start writing to the registers:
    ssd1306_command(SSD1306_DISPLAYOFF); // reset screen
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);    // at this register...
    ssd1306_command(0x80);                          // put this number
    ssd1306_command(SSD1306_SETMULTIPLEX);          // at this register...
    ssd1306_command(0x1F);                          // put this number (height-1 = 31)
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);
    ssd1306_command(0x0);
    ssd1306_command(SSD1306_SETSTARTLINE);
    ssd1306_command(SSD1306_CHARGEPUMP);
    ssd1306_command(0x14);
    ssd1306_command(SSD1306_MEMORYMODE);
    ssd1306_command(0x00);
    ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);
    ssd1306_command(SSD1306_SETCOMPINS);
    ssd1306_command(0x02);
    ssd1306_command(SSD1306_SETCONTRAST); // might want to change the contrast
    ssd1306_command(0x8F);
    ssd1306_command(SSD1306_SETPRECHARGE);
    ssd1306_command(0xF1);
    ssd1306_command(SSD1306_SETVCOMDETECT);
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYON);
    ssd1306_clear();
    ssd1306_update();
}

// send a command instruction (not pixel data)
void ssd1306_command(unsigned char c) {
    i2c_master_start();
    i2c_master_send(ssd1306_write);
    i2c_master_send(0x00); // bit 7 is 0 for Co bit (data bytes only), bit 6 is 0 for DC (data is a command))
    i2c_master_send(c);
    i2c_master_stop();
}

// update every pixel on the screen. sends the entire array to the screen
void ssd1306_update() {
    ssd1306_command(SSD1306_PAGEADDR);  // communicates w device, move your cursor position to...
    ssd1306_command(0);                 // position 0,0 (top left)
    ssd1306_command(0xFF);
    ssd1306_command(SSD1306_COLUMNADDR);
    ssd1306_command(0);
    ssd1306_command(128 - 1); // Width

    unsigned short count = 512; // WIDTH * ((HEIGHT + 7) / 8)
    unsigned char * ptr = ssd1306_buffer; // first address of the pixel buffer
    i2c_master_start();
    i2c_master_send(ssd1306_write);
    i2c_master_send(0x40); // send pixel data
    // send every pixel
    while (count--) {               // loop through 512 bytes to send each bit to each pixel
        i2c_master_send(*ptr++);
    }
    i2c_master_stop();
}

// set a pixel value. Call update() to push to the display)
void ssd1306_drawPixel(unsigned char x, unsigned char y, unsigned char color) { // at position x, y, bit 1 or 0 (pixel on or off)
    if ((x < 0) || (x >= 128) || (y < 0) || (y >= 32)) { // are you writing to a valid pixel?
        return;
    }

    if (color == 1) {           // if it is a valid pixel, 
        ssd1306_buffer[x + (y / 8)*128] |= (1 << (y & 7)); // math to convert matrix position into linear array of pixels
    } else {
        ssd1306_buffer[x + (y / 8)*128] &= ~(1 << (y & 7));
    }
}

// zero every pixel value
void ssd1306_clear() {
    memset(ssd1306_buffer, 0, 512); // make every bit a 0, memset in string.h
                                    // memset is a func available in c. at this address ("ssd1306_buffer", which is our pixel array/map), from this position (0), to this position (512), make it the # you specify.
}


