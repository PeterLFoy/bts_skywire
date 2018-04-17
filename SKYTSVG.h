#ifndef SKYTSVG_H
#define SKYTSVG_H
 
#include "ATParser.h"
 
/** SKYTSVGInterface class.
    This is an interface to a SKYTSVG Skywire CAT 3 module.
 */


class SKYTSVG
{
public:
    
    SKYTSVG(PinName tx, PinName rx, PinName reset=NC, PinName wakeup=NC, PinName rts=NC, bool debug=false);
    
    /**
    * Init the SKYTSVG
    *
    * @param mode mode in which to startup
    * @return true only if SKYTSVG has started up correctly
    */
    bool startup(int mode);
 
    void waitSKYReady(void);
    /**
    * Reset SKYTSVG
    *
    * @return true only if SKYTSVG resets successfully
    */
    bool reset(void);
    
    bool hw_reset(void);
    
		int GetSkywireModel(void);
		
		bool dhcp(int);

    /**
    * Connect SKYTSVG to Cellular Network
    *
    * @param ap the name of the AP
    * @param passPhrase the password of AP
    * @param securityMode the security mode of AP (WPA/WPA2, WEP, Open)
    * @return true only if SKYTSVG is connected successfully
    */
    bool connect(const char *ap, const char *passPhrase, int securityMode);
 
    /**
    * Disconnect SKYTSVG from AP
    *
    * @return true only if SKYTSVG is disconnected successfully
    */
    bool disconnect(void);
 
    /**
    * Get the IP address of SKYTSVG
    *
    * @return null-teriminated IP address or null if no IP address is assigned
    */
    const char *getIPAddress(void);
 
    /**
    * Get the MAC address of SKYTSVG
    *
    * @return null-terminated MAC address or null if no MAC address is assigned
    */
    const char *getMACAddress(void);
 
    /**
    * Check if SKYTSVG is conenected
    *
    * @return true only if the chip has an IP address
    */
    bool isConnected(void);

    /**
    * @brief  wifi_socket_client_security
    *         Set the security certificates and key for secure socket (TLS)
    * @param  None
    * @retval return nonzero in case of error
    */
    int setSocketClientSecurity(uint8_t* tls_mode, uint8_t* root_ca_server, uint8_t* client_cert, uint8_t* client_key, uint8_t* client_domain, uint32_t tls_epoch_time);
 
    /**
    * Open a socketed connection
    *
    * @param type the type of socket to open "u" (UDP) or "t" (TCP)
    * @param id id to get the new socket number, valid 0-7
    * @param port port to open connection with
    * @param addr the IP address of the destination
    * @return true only if socket opened successfully
    */
    bool open(const char *type, int* id, const char* addr, int port);
 
    /**
    * Sends data to an open socket
    *
    * @param id id of socket to send to
    * @param data data to be sent
    * @param amount amount of data to be sent - max 1024
    * @return true only if data sent successfully
    */
    bool send(int id, const void *data, uint32_t amount);
 
    /**
    * Receives data from an open socket
    *
    * @param id id to receive from
    * @param data placeholder for returned information
    * @param amount number of bytes to be received
    * @return the number of bytes received
    */
    int32_t recv(int id, void *data, uint32_t amount);
 
    /**
    * Closes a socket
    *
    * @param id id of socket to close, valid only 0-4
    * @return true only if socket is closed successfully
    */
    bool close(int id);
 
 
    /**
    * Checks if data is available
    */
    bool readable();
 
    /**
    * Checks if data can be written
    */
    bool writeable();
    
    /** Sets debug mode */
    inline void set_debug(bool state) { dbg_on = state;_parser.debugOn(state);};
 
private:
    BufferedSerial _serial;
    ATParser _parser;
    DigitalInOut _wakeup;
    DigitalInOut _reset; 
		DigitalInOut	_rts;
    char _ip_buffer[16];
    char _mac_buffer[18];
    bool dbg_on;
//    int _timeout; // FIXME LICIO we have "virtual" socket tmo, module socket tmo, 
// AT parser tmo, recv/send tmo, actually used the NetworksocketAPI socket tmo
    unsigned int _recv_timeout; // see SO_RCVTIMEO setsockopt
    unsigned int _send_timeout; // see SO_SNDTIMEO setsockopt   
    unsigned int socket_closed;
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

};
 
#endif  //SKYTSVG_H
