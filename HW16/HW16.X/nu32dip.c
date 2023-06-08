#include "nu32dip.h"

void NU32DIP_Startup() {
    __builtin_disable_interrupts(); // disable interrupts while initializing things. FUNCTIONS STARTING WITH UNDERSCORES CAME WITH THE COMPILER THAT MICROCHIP HAS GIVEN US

    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);

    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;

    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;

    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;

    ANSELA = 0;// SET ALL PINS TO DIGITAL
    ANSELB = 0; 
    
    // do your TRIS and LAT commands here
    TRISBCLR = 0b110001; // B4 and B5 LEDs, B0 output pins
    TRISAbits.TRISA3 = 0; // set Pin A3 as an output pin
    TRISASET = 0b010000; // A4 user button BY SETTING THAT BIT TO 1
  

    //NU32_LED1 = 1; // LED1 is off
    NU32DIP_YELLOW = 1;
    //NU32_LED2 = 0; // LED2 is on
    NU32DIP_GREEN = 0;

    //UART1 pins
    U1RXRbits.U1RXR = 0b0000; // Set A2 to U1RX
    RPB3Rbits.RPB3R = 0b0001; // Set B3 to U1TX

    // turn on UART1 without an interrupt
    U1MODEbits.BRGH = 0; // set baud to NU32_DESIRED_BAUD
    U1BRG = ((NU32DIP_SYS_FREQ / NU32DIP_DESIRED_BAUD) / 16) - 1; //BRG = BAUD RATE GENERATOR. WHEN PIC NEESD TO SEND DATA TO COMPUTER, 
    /*********************/
    // UART2 pins:
    U2RXRbits.U2RXR = 0b0011; // Set B11 to U2RX
    RPB14Rbits.RPB14R = 0b0010; // Set B14 to U2TX

    // turn on UART2 without an interrupt
    U2MODEbits.BRGH = 0; // set baud to NU32_DESIRED_BAUD
    U2BRG = ((NU32DIP_SYS_FREQ / NU32DIP_DESIRED_BAUD) / 16) - 1; //BRG = BAUD RATE GENERATOR. WHEN PIC NEESD TO SEND DATA TO COMPUTER, 

    // 8 bit, no parity bit, and 1 stop bit (8N1 setup)
    U1MODEbits.PDSEL = 0;
    U1MODEbits.STSEL = 0;

    // configure TX & RX pins as output & input pins
    U1STAbits.UTXEN = 1;
    U1STAbits.URXEN = 1;
    // configure without hardware flow control
    U1MODEbits.UEN = 0;

    // enable the uart
    U1MODEbits.ON = 1;

    __builtin_enable_interrupts();
}

// Read from UART1
// block other functions until you get a '\r' or '\n'
// send the pointer to your char array and the number of elements in the array

void NU32DIP_ReadUART1(char * message, int maxLength) {
    char data = 0;
    int complete = 0, num_bytes = 0;
    // loop until you get a '\r' or '\n'
    while (!complete) {
        if (U1STAbits.URXDA) { // if data is available
            data = U1RXREG; // read the data
            if ((data == '\n') || (data == '\r')) {
                complete = 1;
            } else {
                message[num_bytes] = data;
                ++num_bytes;
                // roll over if the array is too small
                if (num_bytes >= maxLength) {
                    num_bytes = 0;
                }
            }
        }
    }
    // end the string
    message[num_bytes] = '\0';
}

// Write a character array using UART1

void NU32DIP_WriteUART1(const char * string) {
    while (*string != '\0') {
        while (U1STAbits.UTXBF) {
            ; // wait until tx buffer isn't full
        }
        U1TXREG = *string;
        ++string;
    }
}
