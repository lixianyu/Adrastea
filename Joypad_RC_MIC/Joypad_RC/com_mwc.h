#include "Arduino.h"
#if defined(RF_PORT)
#include <ZigduinoRadio.h>
#endif

byte getChecksum(byte length, byte cmd, byte mydata[]) {
  byte checksum = 0;
  checksum ^= (length & 0xFF);
  checksum ^= (cmd & 0xFF);
  for (uint8_t i = 0; i < length; i++)
    checksum ^= (mydata[i] & 0xFF);
  return checksum;
}

uint16_t write16(boolean a, int16_t b) {
  return a ? (b >> 8) : (b & 0x00ff);
}

/*
  if Core RF
  [head,2byte,0xAA 0xBB] [type,1byte,TYPE_NUM] [data,16byte] [body,1byte,getChecksum()]
  Example:
  AA BB C8 DC 05 DC 05 D0 07 EF 03 DC 05 DC 05 DC 05 DC 05 E3
*/
#define buf_length 0x10   //16
#define buf_code_A 0xC8   //200
#define buf_code_B 0xC9   //201
boolean buf_code = true;
boolean buf_type = true;
#if !defined(RF_PORT)
static byte buf_head[3] = {
  0x24, 0x4D, 0x3C
};
#endif
static byte buf_data[buf_length];
static byte buf_body;
boolean mwc_send(uint8_t _code, int16_t _channal[8]) {
#if defined(RF_PORT)
  mwc_port.beginTransmission();
  mwc_port.write(0xaa);
  mwc_port.write(0xbb);
#else
  for (uint8_t a = 0; a < 3; a++)
    mwc_port.write(buf_head[a]);
  mwc_port.write(buf_length);
#endif
  mwc_port.write(_code);
  for (uint8_t a = 0; a < (buf_length / 2); a++) {
    buf_data[2 * a] = write16(LOW, _channal[a]);
    buf_data[2 * a + 1] = write16(HIGH, _channal[a]);
  }
  for (uint8_t a = 0; a < buf_length; a++)
    mwc_port.write(buf_data[a]);
  buf_body = getChecksum(buf_length, _code, buf_data);
  mwc_port.write(buf_body);
#if defined(RF_PORT)
  mwc_port.endTransmission();
#endif
  return true;
}

uint32_t mwc_rece() {
  if (mwc_port.available()) {
    while (mwc_port.available()) {
      _V_bat_RC = mwc_port.read();
    }
    _V_bat_RC = constrain(_V_bat_RC, _V_min, _V_max);
#if defined(RF_PORT)
    RSSI = mwc_port.getLastRssi();
#else
    RSSI = -40;
#endif
    return millis();
  }
  return 0;
}
