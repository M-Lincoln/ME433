// this program sends a number 0-3.3V with 100 increments with a 1 ms delay between every write.
#include "nu32dip.h" // constants, functions for startup and UART

void blink(int, int); // blink the LEDs function

int main(void) {
  char message[100];
  
  NU32DIP_Startup(); // cache on, interrupts on, LED/button init, UART init
  while (1) {
      int i = 0;
      for(i=0;i<100; i++) {
          sprintf(message,"%f\r\n",i*3.3/100.0);
          NU32DIP_WriteUART1(message); // send message back
          //make 1 ms delay between printing
          _CP0_SET_COUNT(0); // set timer to 0
          while(_CP0_GET_COUNT()<24000) {} // Delay for 1ms
      }
      // wait 5 seconds after for loop
      _CP0_SET_COUNT(0); // set timer to 0
      while(_CP0_GET_COUNT()<24000000 * 5) {} // Delay for 5s
      }
}

// blink the LEDs
void blink(int iterations, int time_ms){
	int i;
	unsigned int t;
	for (i=0; i< iterations; i++){
		NU32DIP_GREEN = 0; // on
		NU32DIP_YELLOW = 1; // off
		t = _CP0_GET_COUNT(); // should really check for overflow here
		// the core timer ticks at half the SYSCLK, so 24000000 times per second
		// so each millisecond is 24000 ticks
		// wait half in each delay
		while(_CP0_GET_COUNT() < t + 12000*time_ms){}
		
		NU32DIP_GREEN = 1; // off
		NU32DIP_YELLOW = 0; // on
		t = _CP0_GET_COUNT(); // should really check for overflow here
		while(_CP0_GET_COUNT() < t + 12000*time_ms){}
	}
}
		