#include "SKYTSVG.h"
#include "mbed_debug.h"
#include "stdint.h"

#define SKYTSVG_CONNECT_TIMEOUT    15000
#define SKYTSVG_SEND_TIMEOUT       5000
#define SKYTSVG_RECV_TIMEOUT       1500//some commands like AT&F/W takes some time to get the result back!
#define SKYTSVG_MISC_TIMEOUT       500
#define SKYTSVG_SOCKQ_TIMEOUT      3000

#define EPOCH_TIME                  1453727657//Human time (GMT): Mon, 25 Jan 2016 13:14:17 GMT

   SKYTSVG::SKYTSVG(PinName tx, PinName rx, PinName reset, PinName wakeup, PinName rts, bool debug)
    : _serial(tx, rx, 1024), _parser(_serial,"\r\n", 2048),      
      _wakeup(wakeup, PIN_OUTPUT, PullNone, 0),
      _reset(reset, PIN_OUTPUT, PullNone, 1),
	    _rts(rts, PIN_OUTPUT, PullNone, 0),
      dbg_on(debug)
{
    _serial.baud(115200);  // LICIO  FIXME increase the speed
    _parser.debugOn(debug);
}

bool SKYTSVG::startup(int mode)
{
    _parser.setTimeout(SKYTSVG_MISC_TIMEOUT);    
    /*Test module before reset*/
		hw_reset();

    waitSKYReady();
    /*Reset module*/
    reset();
     
    /*set local echo to 0*/
    if(!(_parser.send("AT+S.SCFG=localecho1,%d\r", 0) && _parser.recv("OK"))) 
        {
            debug_if(dbg_on, "SKY> error local echo set\r\n");
            return false;
        }   
    /*reset factory settings*/
    if(!(_parser.send("AT&F") && _parser.recv("OK"))) 
        {
            debug_if(dbg_on, "SKY> error AT&F\r\n");
            return false;
        }
        
    /*set Wi-Fi mode and rate to b/g*/
    if(!(_parser.send("AT+S.SCFG=wifi_ht_mode,%d\r", 0) && _parser.recv("OK")))
	{
		debug_if(dbg_on, "SKY> error setting ht_mode\r\n");
		return false;
	}
        
    //if(!(_parser.send("AT+S.SCFG=wifi_opr_rate_mask,0x003FFFCF\r") && _parser.recv("OK")))
    if(!(_parser.send("AT+S.SCFG=wifi_opr_rate_mask,0x00000001\r") && _parser.recv("OK")))// set most effective speed for such kind of device
        {
            debug_if(dbg_on, "SPWF> error setting operational rates\r\n");
            return false;
        }

    if(!(_parser.send("AT+S.SCFG=wifi_bas_rate_mask,0x0000000F\r") && _parser.recv("OK")))// set most effective speed for such kind of device
	{
		debug_if(dbg_on, "SPWF> error setting basic rates\r\n");
		return false;
	}

    if(!(_parser.send("AT+S.SCFG=wifi_powersave,0\r") && _parser.recv("OK")))// set most effective speed for such kind of device
	{
	   debug_if(dbg_on, "SPWF> error setting power save mode\r\n");
	   return false;
	}

    if(!(_parser.send("AT+S.SCFG=sleep_enabled,0\r") && _parser.recv("OK")))// set most effective speed for such kind of device
	{
	   debug_if(dbg_on, "SPWF> error setting sleep  mode\r\n");
	   return false;
	}

    if(!(_parser.send("AT+S.SCFG=wifi_tx_power,18\r") && _parser.recv("OK")))// set most effective speed for such kind of device
	{
	   debug_if(dbg_on, "SPWF> error setting transmit power\r\n");
	   return false;
	}

    if(!(_parser.send("AT+S.SCFG=ip_use_dhcp,1\r") && _parser.recv("OK")))// set most effective speed for such kind of device
	{
	   debug_if(dbg_on, "SPWF> error setting dhcp on\r\n");
	   return false;
	}

    if(!(_parser.send("AT+S.SCFG=ip_dhcp_timeout,20\r") && _parser.recv("OK")))// set most effective speed for such kind of device
	{
	   debug_if(dbg_on, "SPWF> error setting dhcp timeout\r\n");
	   return false;
	}

    /*set idle mode (0->idle, 1->STA,3->miniAP, 2->IBSS)*/
    if(!(_parser.send("AT+S.SCFG=wifi_mode,%d\r", mode) && _parser.recv("OK")))
        {
            debug_if(dbg_on, "SPWF> error wifi mode set\r\n");
            return false;
        } 

    /* save current setting in flash */
    if(!(_parser.send("AT&W") && _parser.recv("OK")))
        {
            debug_if(dbg_on, "SPWF> error AT&W\r\n");
            return false;
        }
        
    /*reset again and send AT command and check for result (AT->OK)*/
    reset();
        
    return true;    
}

bool SKYTSVG::hw_reset(void)
{    
		// Skywire Reset_nIN = Pin 9 on Modem and connected to PE14 on EFM32WG
  // Controls HW_SHUTDOWN input on LE910, tie low for 200ms and release to activate. 
	// Internally pulled up to Vcc. Drive with open collector output. Assert only in an 
	// emerency as the module will not gracefully exit the cellular network when asserted
				// DTR pin must be driven low if unused!
		// skywire_dtr = 0; // Pin 9 (DTR) of Skywire is tied to GND!
		// RTS Pin must be driven low if unused! Pin 5 
		 _rts = 0; 
  	 _wakeup = 1;
	 // Delay for 5.1 Seconds since using NL_SW_LTE_TSVG 
	 // Wait time is different for each modem, 10s is enough for the LE910
   //wait_print(6);  // Modem requires >5s pulse
	  wait(6.0);
    _wakeup = 0;
		// Modem requires 15 seconds for initialization
		wait(16.0); 
	  //wait_ms(200);
	
    if (_reset.is_connected()) {
    /* reset the pin PF5 */  
    _reset.write(1); // Inverse logic due to buffer to modem
    wait_ms(201.0);
    _reset.write(0); 
    wait_ms(100);
    return 1;
    } else { return 0; }
}

bool SKYTSVG::reset(void)
{
    if(!_parser.send("AT+CFUN=1")) return false;
    while(1) {
        if (_parser.recv("+WIND:32:WiFi Hardware Started\r")) {
            return true;
        }
    }
}

void SKYTSVG::waitSKYReady(void)
{

    while(1){
        if(_parser.send("AT") && _parser.recv("OK"))
            //till we get OK from AT command
            //printf("\r\nwaiting for reset to complete..\n");
					debug_if(dbg_on, " Received OK \r\n");
            return;
			}    
}
int SKYTSVG::GetSkywireModel()
{
		int MODEM;

		_parser.setTimeout(SKYTSVG_CONNECT_TIMEOUT);   
    // send 3GPP standard command for model name
    debug_if(dbg_on,"Sending AT+CGMM\r");
	 if(!(_parser.send("AT+CGMM\r") && _parser.recv("AT+CGMM:%d\r\nOK", &MODEM)))
	 {
			debug_if(dbg_on,"ERROR: Unable to detect modem\n");
      return 0;
    }
    return MODEM;
		
}


/* Security Mode
    None          = 0, 
    WEP           = 1,
    WPA_Personal  = 2,
*/
bool SKYTSVG::connect(const char *ap, const char *passPhrase, int securityMode)
{
    uint32_t n1, n2, n3, n4;

    _parser.setTimeout(SKYTSVG_CONNECT_TIMEOUT);       
    //AT+S.SCFG=wifi_wpa_psk_text,%s\r
    if(!(_parser.send("AT+S.SCFG=wifi_wpa_psk_text,%s", passPhrase) && _parser.recv("OK"))) 
        {
            debug_if(dbg_on, "SPWF> error pass set\r\n");
            return false;
        } 
    //AT+S.SSIDTXT=%s\r
    if(!(_parser.send("AT+S.SSIDTXT=%s", ap) && _parser.recv("OK"))) 
        {
            debug_if(dbg_on, "SPWF> error ssid set\r\n");
            return false;
        }
    //AT+S.SCFG=wifi_priv_mode,%d\r
    if(!(_parser.send("AT+S.SCFG=wifi_priv_mode,%d", securityMode) && _parser.recv("OK"))) 
        {
            debug_if(dbg_on, "SPWF> error security mode set\r\n");
            return false;
        } 
    //"AT+S.SCFG=wifi_mode,%d\r"
    /*set idle mode (0->idle, 1->STA,3->miniAP, 2->IBSS)*/
    if(!(_parser.send("AT+S.SCFG=wifi_mode,%d\r", 1) && _parser.recv("OK")))
        {
            debug_if(dbg_on, "SPWF> error wifi mode set\r\n");
            return false;
        }
    //AT&W
    /* save current setting in flash */
    if(!(_parser.send("AT&W") && _parser.recv("OK")))
        {
            debug_if(dbg_on, "SPWF> error AT&W\r\n");
            return false;
        }
    //reset module
    reset();
    
    while(1)
        if((_parser.recv("+WIND:24:WiFi Up:%u.%u.%u.%u",&n1, &n2, &n3, &n4)))
            {
                break;
            }            
        
    return true;
}

bool SKYTSVG::disconnect(void)
{
    //"AT+S.SCFG=wifi_mode,%d\r"
    /*set idle mode (0->idle, 1->STA,3->miniAP, 2->IBSS)*/
    if(!(_parser.send("AT+S.SCFG=wifi_mode,%d\r", 0) && _parser.recv("OK")))
        {
            debug_if(dbg_on, "SPWF> error wifi mode set\r\n");
            return false;
        }
    //AT&W
    /* save current setting in flash */
    if(!(_parser.send("AT&W") && _parser.recv("OK")))
        {
            debug_if(dbg_on, "SPWF> error AT&W\r\n");
            return false;
        }
    //reset module
    reset();
    return true;
}

bool SKYTSVG::dhcp(int mode)
{
    //only 3 valid modes
    //0->off(ip_addr must be set by user), 1->on(auto set by AP), 2->on&customize(miniAP ip_addr can be set by user)
    if(mode < 0 || mode > 2) {
        return false;
    }
        
    return _parser.send("AT+S.SCFG=ip_use_dhcp,%d\r", mode)
        && _parser.recv("OK");
}


const char *SKYTSVG::getIPAddress(void)
{
    uint32_t n1, n2, n3, n4;
    
    if (!(_parser.send("AT+S.STS=ip_ipaddr")
        && _parser.recv("#  ip_ipaddr = %u.%u.%u.%u", &n1, &n2, &n3, &n4)
        && _parser.recv("OK"))) {
            debug_if(dbg_on, "SPWF> getIPAddress error\r\n");
        return 0;
    }

    sprintf((char*)_ip_buffer,"%u.%u.%u.%u", n1, n2, n3, n4);

    return _ip_buffer;
}

const char *SKYTSVG::getMACAddress(void)
{
    uint32_t n1, n2, n3, n4, n5, n6;
    
    if (!(_parser.send("AT+S.GCFG=nv_wifi_macaddr")
        && _parser.recv("#  nv_wifi_macaddr = %x:%x:%x:%x:%x:%x", &n1, &n2, &n3, &n4, &n5, &n6)
        && _parser.recv("OK"))) {
            debug_if(dbg_on, "SPWF> getMACAddress error\r\n");
        return 0;
    }

    sprintf((char*)_mac_buffer,"%02X:%02X:%02X:%02X:%02X:%02X", n1, n2, n3, n4, n5, n6);

    return _mac_buffer;
}

bool SKYTSVG::isConnected(void)
{
    return getIPAddress() != 0; 
}

bool SKYTSVG::open(const char *type, int* id, const char* addr, int port)
{
    Timer timer;
    timer.start();
    socket_closed = 0; 
       
    if(!_parser.send("AT+S.SOCKON=%s,%d,%s,ind", addr, port, type))
        {
            debug_if(dbg_on, "SPWF> error opening socket\r\n");
            return false;
        }
        
    while(1)
        {
            if( _parser.recv(" ID: %d", id)
                && _parser.recv("OK"))
                break;
            
            if (timer.read_ms() > SKYTSVG_CONNECT_TIMEOUT) {
                return false;
            }
        
            //TODO:implement time-out functionality in case of no response
            //if(timeout) return false;
            //TODO: deal with errors like "ERROR: Failed to resolve name"
            //TODO: deal with errors like "ERROR: Data mode not available"
        }

    return true;
}

bool SKYTSVG::send(int id, const void *data, uint32_t amount)
{    
    char _buf[18];
    _parser.setTimeout(SKYTSVG_SEND_TIMEOUT);
    
    sprintf((char*)_buf,"AT+S.SOCKW=%d,%d\r", id, amount);   
    
    //May take a second try if device is busy
    for (unsigned i = 0; i < 2; i++) {
        if (_parser.write((char*)_buf, strlen(_buf)) >=0
            && _parser.write((char*)data, (int)amount) >= 0
            && _parser.recv("OK")) {
            return true;
        }     
    }
    return false;
}


int32_t SKYTSVG::recv(int id, void *data, uint32_t amount)
{
    uint32_t recv_amount=0;
    int wind_id;    
 
    if (socket_closed) {
        socket_closed = 0;
        return -3;
    }
    if(!(_parser.send("AT+S.SOCKQ=%d", id)  //send a query (will be required for secure sockets)
        && _parser.recv(" DATALEN: %u", &recv_amount)
        && _parser.recv("OK"))) {
        return -2;
    }            
    if (recv_amount==0) { return -1; } 
    if(recv_amount > amount)
        recv_amount = amount;
        
    int par_timeout = 0;        
    _parser.setTimeout(par_timeout);
    
     while(_parser.recv("+WIND:%d:", &wind_id)) {
//        printf("Wind received: %d\n\r", wind_id);
        if (wind_id == 58) {
            socket_closed = 1;
            _parser.flush();            
        }
    }       
    _parser.setTimeout(par_timeout);
    
    _parser.flush();
    if(!(_parser.send("AT+S.SOCKR=%d,%d", id, recv_amount))){
        return -2;    
    }
    if(!((_parser.read((char*)data, recv_amount) >0)
            && _parser.recv("OK"))) {
        return -2;
    }    
    return recv_amount;
}

bool SKYTSVG::close(int id)
{
    uint32_t recv_amount=0;    
    void * data = NULL;    

    _parser.setTimeout(SKYTSVG_MISC_TIMEOUT);    
    _parser.flush();
    /* socket flush */
    if(!(_parser.send("AT+S.SOCKQ=%d", id)  //send a query (will be required for secure sockets)
        && _parser.recv(" DATALEN: %u", &recv_amount)
        && _parser.recv("OK"))) {
            return -2;
    } 
    if (recv_amount>0) {
        data = malloc (recv_amount+4);
        if(!(_parser.send("AT+S.SOCKR=%d,%d", id, recv_amount))) { 
            free (data); 
            return -2; 
        }
 //       printf ("--->>>Close flushing recv_amount: %d  \n\r",recv_amount);            
        if(!((_parser.read((char*)data, recv_amount) >0)
            && _parser.recv("OK"))) {
             free (data);
             return -2;
        }
        free (data);                            
    }     
    
    //May take a second try if device is busy or error is returned
    for (unsigned i = 0; i < 2; i++) {
        if (_parser.send("AT+S.SOCKC=%d", id)
            && _parser.recv("OK")) {
            socket_closed = 1;     
            return true;
        }
        else
        {
            if(_parser.recv("ERROR: Pending data")) {
                    debug_if(dbg_on, "SPWF> ERROR!!!!\r\n");
                    return false;
            }
        }
        //TODO: Deal with "ERROR: Pending data" (Closing a socket with pending data)
    }
    return false;
}


bool SKYTSVG::readable()
{
    return _serial.readable();
}

bool SKYTSVG::writeable()
{
    return _serial.writeable();
}

int SKYTSVG::setSocketClientSecurity(uint8_t* tls_mode, uint8_t* root_ca_server, uint8_t* client_cert, uint8_t* client_key, uint8_t* client_domain, uint32_t tls_epoch_time)
{
	int err = 0;
	unsigned long epoch_time;

	if(!(_parser.send("AT+S.TLSCERT2=clean,all\r", 0) && _parser.recv("OK")))
	{
	  debug_if(dbg_on, "SPWF> TLSCERT2 cleared\r\n");
	  return -1;
	}

	if(tls_epoch_time==0)
		epoch_time = EPOCH_TIME;
	else
		epoch_time = tls_epoch_time;

	if(!(_parser.send("AT+S.SETTIME=%lu\r", (unsigned long)epoch_time) && _parser.recv("OK")))
	{
	  debug_if(dbg_on, "SPWF> time is set to %d\r\n", epoch_time);
	  return -1;
	}

	/*AT+S.TLSCERT=f_ca,<size><CR><data>*/
	if(!(_parser.send("AT+S.TLSCERT=f_ca,%d\r%s\r", strlen((const char *)root_ca_server) - 1, root_ca_server) && _parser.recv("OK")))
	{
	  debug_if(dbg_on, "SPWF> error TLSCERT=f_ca set\r\n");
	  return -1;
	}

	/*AT+S.TLSCERT=f_cert,<size><CR><data>*/
	if(tls_mode[0]=='m')
	{
		if(!(_parser.send("AT+S.TLSCERT=f_cert,%d\r%s", strlen((const char *)client_cert) - 1, client_cert) && _parser.recv("OK")))
		{
		  debug_if(dbg_on, "SPWF> error TLSCERT=f_cert set\r\n");
		  return -1;
		}

	    /*AT+S.TLSCERT=f_key,<size><CR><data>*/
	    if(!(_parser.send("AT+S.TLSCERT=f_key,%d\r%s", strlen((const char *)client_key) - 1, client_key) && _parser.recv("OK")))
		{
		  debug_if(dbg_on, "SPWF> error TLSCERT=f_key set\r\n");
		  return -1;
		}
	}

	/*AT+S.TLSDOMAIN=f_domain,<server domain>*/
	if(!(_parser.send("AT+S.TLSDOMAIN=f_domain,%s\r", client_domain) && _parser.recv("OK")))
	{
	  debug_if(dbg_on, "SPWF> error TLSDOMAIN=f_domain set\r\n");
	  return -1;
	}

	return err;
}
