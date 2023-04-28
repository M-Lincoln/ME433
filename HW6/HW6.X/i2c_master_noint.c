// I2C Master utilities, using polling rather than interrupts
// The functions must be called in the correct order as per the I2C protocol
// I2C pins need pull-up resistors, 2k-10k
#include "i2c_master_noint.h"
#include <stdio.h>
#include "nu32dip.h"


int main (){
    NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
    i2c_master_setup(); // initialize the I2C pins
    
    // Set GP0-GP6 as inputs and GP7 as output. GP1-6 could be output, but doesn't matter b/c not using them
    generic_i2c_write(0b01000000,0x00,0b01111111);
//    i2c_master_start(); // send start bit
//    i2c_master_send(0b01000000); // send the address of the chip: address = 0b01000000 (A0-A2 is grounded therefore = 0, and writing to chip so = 0)
//    i2c_master_send(0x00); // send the register name: IODIR to denote direction (in or out)
//    i2c_master_send(0b01111111); // send the value to make GP0-GP6 inputs (1) and GP7 output (0))
//    i2c_master_stop(); // send stop bit
    // blink the yellow LED for a heartbeat, everything is ok
    NU32DIP_YELLOW = 0; // turn yellow LED ON
    
    while(1) {
        // blink GP7:
        turnOn_GP7(); // turn on GP7 (call func)
        NU32DIP_YELLOW = 1;// blink yellow LED (turn it off)
        _CP0_SET_COUNT(0); // set timer to 0
        while (_CP0_GET_COUNT() < 24000000) {} // Delay for 1s
        turnOff_GP7();// turn off GP7
        NU32DIP_YELLOW = 0;// blink yellow LED (turn it on)
        _CP0_SET_COUNT(0); // set timer to 0
        while (_CP0_GET_COUNT() < 24000000) {} // Delay for 1s
    }
}

void generic_i2c_write(unsigned char address, unsigned char reg, unsigned char value){
    i2c_master_start(); // send start bit
    i2c_master_send(address); // send the address of the chip: address = 0b01000000 (A0-A2 is grounded therefore = 0, and writing to chip so = 0)
    i2c_master_send(reg); // send the register name: IODIR to denote direction (in or out)
    i2c_master_send(value); // send the value to make GP0-GP6 inputs (1) and GP7 output (0))
    i2c_master_stop(); // send stop bit
}

void turnOn_GP7(){
    // send start bit
    i2c_master_start();
    // send the address of the chip: 
    i2c_master_send(0b01000000); // address = 0b01000000 (A0-A2 is grounded therefore = 0, and writing to chip so = 0)
    // send the register name
    i2c_master_send(0x0A); // OLAT, turn gp7 on
    // send the value to turn on GP7
    i2c_master_send(0b10000000); 
    // send stop bit
    i2c_master_stop();
}

void turnOff_GP7(){
    // send start bit
    i2c_master_start();
    // send the address of the chip: 
    i2c_master_send(0b01000000); // address = 0b01000000 (A0-A2 is grounded therefore = 0, and writing to chip so = 0)
    // send the register name
    i2c_master_send(0x0A); // OLAT register
    // send the value to turn off GP7
    i2c_master_send(0b00000000); 
    // send stop bit
    i2c_master_stop();
}

void i2c_master_setup(void) {
    // using a large BRG to see it on the nScope, make it smaller after verifying that code works
    // look up TPGD in the datasheet
    I2C1BRG = 1000; // I2CBRG = [1/(2*Fsck) - TPGD]*Pblck - 2 (TPGD is the Pulse Gobbler Delay)
    I2C1CONbits.ON = 1; // turn on the I2C1 module
}

void i2c_master_start(void) {
    I2C1CONbits.SEN = 1; // send the start bit
    while (I2C1CONbits.SEN) {
        ;
    } // wait for the start bit to be sent
}

void i2c_master_restart(void) {
    I2C1CONbits.RSEN = 1; // send a restart 
    while (I2C1CONbits.RSEN) {
        ;
    } // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
    I2C1TRN = byte; // if an address, bit 0 = 0 for write, 1 for read
    while (I2C1STATbits.TRSTAT) {
        ;
    } // wait for the transmission to finish
    if (I2C1STATbits.ACKSTAT) { // if this is high, slave has not acknowledged
        // ("I2C1 Master: failed to receive ACK\r\n");
        while(1){
        // blink an LED as a sign it's stuck
        } // get stuck here if the chip does not ACK back
        
    }
}

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C1CONbits.RCEN = 1; // start receiving data
    while (!I2C1STATbits.RBF) {
        ;
    } // wait to receive the data
    return I2C1RCV; // read and return the data
}

void i2c_master_ack(int val) { // sends ACK = 0 (slave should send another byte)
    // or NACK = 1 (no more bytes requested from slave)
    I2C1CONbits.ACKDT = val; // store ACK/NACK in ACKDT
    I2C1CONbits.ACKEN = 1; // send ACKDT
    while (I2C1CONbits.ACKEN) {
        ;
    } // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) { // send a STOP:
    I2C1CONbits.PEN = 1; // comm is complete and master relinquishes bus
    while (I2C1CONbits.PEN) {
        ;
    } // wait for STOP to complete
}