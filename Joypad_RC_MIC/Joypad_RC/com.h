#include "Arduino.h"
#include "com_nrf.h"
#include "com_mwc.h"
#include "com_wifi.h"

#define SSID        "esp32-ap"
#define PASSWORD    "MakerModule"
#define HOST_NAME   "192.168.10.1"
#define HOST_PORT   (4567)

static void wifi_init(void)
{
    JOY_LOG("wifi setup begin\r\n");
    JOY_LOG("FW Version:");
    JOY_LOGLN(wifi.getVersion().c_str());
    delay(500);

    if (wifi.setOprToStation()) {
		JOY_LOG("to station ok\r\n");
	} else {
		JOY_LOG("to station err\r\n");
	}
 
    if (wifi.joinAP(SSID, PASSWORD)) {
        JOY_LOG("Join AP success\r\n");
        JOY_LOG("IP:");
        JOY_LOGLN( wifi.getLocalIP().c_str());       
    } else {
        JOY_LOG("Join AP failure\r\n");
    }
    
    if (wifi.disableMUX()) {
        JOY_LOG("single ok\r\n");
    } else {
        JOY_LOG("single err\r\n");
    }
#if 1
	if (wifi.createTCP(HOST_NAME, HOST_PORT)) {
        JOY_LOG("create tcp ok\r\n");
    } else {
        JOY_LOG("create tcp err\r\n");
    }
#endif
	mwc_port.println("AT+CIPMODE=1"); // Transparent transmission
	delay(500);
	mwc_port.println("AT+CIPSEND");
	delay(500);
    JOY_LOG("wifi setup end\r\n");
}

void comBegin()
{
    if (mode_protocol == 2)
    {
        unsigned long _channel;
#if !defined(RF_PORT)
        switch (mwc_channal)
        {
        case 0:
            _channel = 9600;
            break;
        case 1:
            _channel = 19200;
            break;
        case 2:
            _channel = 38400;
            break;
        case 3:
            _channel = 57600;
            break;
        case 4:
            _channel = 115200;
            break;
        }
#else
        _channel = mwc_channal;
#endif
        mwc_port.begin(_channel);
        wifi_init();
    }
    else if (mode_protocol == 1)    //Robot
    {
        SPI.begin();
        radio.begin();
        network.begin(/*channel*/ nrf_channal, /*node address*/ this_node);
    }
    else if (mode_protocol == 0)   //QuadCopter
    {
        unsigned long _channel;
#if !defined(RF_PORT)
        switch (mwc_channal)
        {
        case 0:
            _channel = 9600;
            break;
        case 1:
            _channel = 19200;
            break;
        case 2:
            _channel = 38400;
            break;
        case 3:
            _channel = 57600;
            break;
        case 4:
            _channel = 115200;
            break;
        }
#else
        _channel = mwc_channal;
#endif
        mwc_port.begin(_channel);
    }
}

unsigned long node_clock = 0;
boolean nodeClock()
{
    node_clock++;
    return (node_clock > 20) ? false : true;
}

boolean comSend(uint8_t _code, int16_t _channal[8])
{
    switch (mode_protocol)
    {
    case 0:
        return mwc_send(_code, _channal);
        break;
    case 1:
        return nrf_send(_code, _channal);
        break;
    case 2:
        return wifi_send(_code, _channal);
        break;
    default:
        return false;
        break;
    }
    return false;
#if 0
    if (mode_protocol)
    {
        if (nrf_send(_code, _channal))  //Robot
            return true;
    }
    else
    {
        if (mwc_send(_code, _channal)) //QuadCopter
            return true;
    }
    return false;
#endif
}

boolean comRece()
{
    uint32_t _c;
    switch (mode_protocol)
    {
    case 0:
        _c = mwc_rece();
        break;
    case 1:
        delay(25);
        _c = nrf_rece();
        break;
    case 2:
        _c = wifi_rece();
        break;
    default:
        _c = 0;
        break;
    }

#if 0
    if (mode_protocol)
    {
        delay(25);
        _c = nrf_rece();
    }
    else
    {
        _c = mwc_rece();
    }
#endif
    if (_c)
    {
        node_clock = 0;
        return true;
    }
    else
        return false;
}
