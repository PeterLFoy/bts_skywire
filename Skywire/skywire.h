#ifndef __SKYWIRE_H
#define __SKYWIRE_H

#include "mbed.h"
#include "pinmap.h"         // pinmap needed for hardware flow control
#include "InitDevice.h"
#include "em_system.h"
#include "BufferedSerial.h"
#include "ATParser.h"


enum Skywire_Modem {
    NL_SW_1xRTT_V,      // Verizon 2G Modem - CE910-DUAL
    NL_SW_1xRTT_S,      // Sprint 2G Modem - CE910-DUAL
    NL_SW_1xRTT_A,      // Aeris 2G Modem - CE910-DUAL
    NL_SW_GPRS,         // AT&T/T-Mobile 2G Modem
    NL_SW_EVDO_V,       // Verizon 3G Modem
    NL_SW_EVDO_A,       // Aeris 3G Modem
    NL_SW_HSPAP,        // AT&T/T-Mobile 3G Modem
    NL_SW_HSPAPG,       // AT&T/T-Mobile 3G Modem w/ GPS
    NL_SW_HSPAPE,       // GSM 3G Modem, EU
    NL_SW_LTE_TSVG,     // Verizon 4G LTE Modem ***** Our CAT3 Modem!  *****
    NL_SW_LTE_TNAG,     // AT&T/T-Mobile 4G LTE Modem
    NL_SW_LTE_TEUG,     // GSM 4G LTE Modem, EU
    NL_SW_LTE_GELS3,    // VZW LTE CAT 1 Modem
    NL_SW_LTE_S7588     // VZW LTE CAT 4 Modem
};
Skywire_Modem MODEM;

int rssi;
// Skywire configuration
//DigitalOut GPI_EN_n(PA2);         // GPI_EN_n (active low)
//DigitalOut GPO_EN_n(PB12);        // GPO_EN_n (active low)
//DigitalOut skywire_power(PA4);	  // ?
DigitalOut skywire_reset(PF5);    // Skywire Enable  U5 to Reset Pchannel FET - When High Cell is Reset
DigitalOut skywire_rts(PE13);		// CTS and RTS are connected
DigitalOut skywire_on(PF4);    // Turn on skywire modem

BufferedSerial debug_pc(PC2, PC3); // Serial connection to PC
BufferedSerial skywire(PE10, PE11);        // Serial comms to Skywire
BufferedSerial skygps(PC0, PC1);            // Serial comms to Skywire

//BufferedSerial pc(USBTX, USBRX);

// --CHANGE THIS FOR YOUR SETUP (IF APPLICABLE)--
// the APN below is for private static IP addresses
// the LE910-based Skywire can automatically get the required APN
char const *APN = "NIMBLINK.GW12.VZWENTP";
Timer t;            // for timeout in WaitForResponse()
Timer t2;           // for timeout in read_line()
Ticker watchdog;    // to break out of infinite sleep() loops


bool sw1;       // Boolean to check if button 1 is pressed
bool sw2;       // Boolean to check if button 2 is pressed

ATParser at = ATParser(skywire, "\r\n");
int value;
char buffer[100];

// char array for reading from Skywire
char str[255];

// Device IMEI to use as dweet thingname
char IMEI[16];
// Device Unique ID
uint64_t UniqueID; 

// Variables for GPS
float latitude;
float longitude;
int number;
// Variables for UART comms
volatile int rx_in=0;
volatile int rx_out=0;
const int buffer_size = 600;
char volatile rx_buffer[buffer_size+1];
char rx_line[buffer_size];

void ledToggler(void);
void watchdog_wakeup();
void blink_leds(int);
void Skywire_Rx_interrupt();
bool DataAvailable();
void ClearBuffer();
void read_line();
bool WaitForResponse(const char* const);
bool SendUntilSuccess(const char* const, const char* const, int);
int GetGMMResponse();
int GetSkywireModel();
int GetMEID();
int GetRSSI();
void wait_print(int);
bool AutoAPN();
bool SkywireInit();



#endif //SKYWIRE_H
