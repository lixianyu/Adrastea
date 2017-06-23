#include "Arduino.h"

/*
    Let us use ESP8266 to comunitate with ESP32.
 */

#define buf_length 0x10   //16
#define buf_code_A 0xC8   //200
#define buf_code_B 0xC9   //201
//boolean buf_code = true;
//boolean buf_type = true;

static byte header[3] =
{
    0x24, 0x4D, 0x3C
};

//static byte buf_data[buf_length];
//static byte buf_body;
#define SEND_BUF_LEN 22 // 3 + 1 + 1 + 16 + 1(header + len + _code + buf_data + checksum)
static byte g_send_buf[SEND_BUF_LEN] = {0x24, 0x4D, 0x3C, 0x10};

#if 0 //Just for porting , are going to delete
boolean mwc_send(uint8_t _code, int16_t _channal[8])
{
    for (uint8_t a = 0; a < 3; a++)
    {
        mwc_port.write(header[a]);
    }
    mwc_port.write(buf_length);
    mwc_port.write(_code);
    for (uint8_t a = 0; a < (buf_length / 2); a++)
    {
        buf_data[2 * a] = write16(LOW, _channal[a]);
        buf_data[2 * a + 1] = write16(HIGH, _channal[a]);
    }
    for (uint8_t a = 0; a < buf_length; a++)
    {
        mwc_port.write(buf_data[a]);
    }
    buf_body = getChecksum(buf_length, _code, buf_data);
    mwc_port.write(buf_body);
    return true;
}
#endif

boolean wifi_send(uint8_t _code, int16_t _channal[8])
{
    uint8_t i = 4;
    uint8_t j = 0;
    uint8_t lens = buf_length / 2; // 8
    g_send_buf[i++] = _code;
    while (1)
    {
        g_send_buf[i++] = _channal[j] & 0x00FF;
        g_send_buf[i++] = _channal[j] >> 8;
        j++;
        if (j >= lens)
        {
            break;
        }
    }
    g_send_buf[i] = getChecksum(buf_length, _code, g_send_buf + 5);

    for (uint32_t k = 0; k < 22; k++) {
		mwc_port.write(g_send_buf[k]);
	}
    return true;
}

uint32_t wifi_rece()
{
    _V_bat_RC = _V_max;
    return millis();
    //return 0;
}
