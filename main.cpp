#include "mbed.h"
#include "pinmap.h"         // pinmap needed for hardware flow control
#include "InitDevice.h"
#include "em_system.h"

 
bool SkywireInit();

#define CMD_TIMEOUT 10000    // modem response timeout in milliseconds

DigitalOut myled0(LED0);
DigitalOut myled1(LED1);
DigitalOut myled2(LED2);
LowPowerTicker toggleTicker;

// Digital I/O on board
DigitalIn button0(BTN0);
DigitalIn button1(BTN1);

volatile bool wakeup;


/**
* This is a callback! Do not call frequency-dependent operations here.
*
* For a more thorough explanation, go here: 
* https://developer.mbed.org/teams/SiliconLabs/wiki/Using-the-improved-mbed-sleep-API#mixing-sleep-with-synchronous-code
**/
void ledToggler(void) {
    myled0 = !myled0;
//		myled1 = !myled1;
//		myled2 = !myled2;
}

void watchdog_wakeup() {
    wakeup = true;
}

// Function to blink LED0 for debugging
void blink_leds(int num)
{
    for (int i = 0; i < num; i++) {
        myled1 = 0;
        wait(0.5);
        myled1 = 1;
        wait(0.5);
    }
}

int main() {
	    	
  GPIO_PinModeSet((GPIO_Port_TypeDef) AF_USART1_TX_PORT(gpioPortC), AF_USART1_TX_PIN (USART0_TX_PIN), gpioModePushPull, 1);
  GPIO_PinModeSet((GPIO_Port_TypeDef) AF_USART1_RX_PORT(gpioPortC), AF_USART1_RX_PIN (USART0_RX_PIN), gpioModeInput, 0);

  GPIO_PinModeSet((GPIO_Port_TypeDef)AF_USART0_TX_PORT(gpioPortE), AF_USART0_TX_PIN(USART1_TX_PIN), gpioModePushPull, 1);
  GPIO_PinModeSet((GPIO_Port_TypeDef)AF_USART0_RX_PORT(gpioPortE), AF_USART0_RX_PIN(USART1_RX_PIN), gpioModeInput, 0);

	
	 //toggleTicker.attach(&ledToggler, 0.2f);	

	
	 myled0 = 1;
	 myled1 = 1;
	 myled2 = 0;
	 SkywireInit();

}





