#include "arduino.h"
//======================
/*
  [head,2byte,0xAA 0xBB] [type,1byte,TYPE_NUM] [data,16byte] [body,1byte,getChecksum()]
  Example:
  AA BB C8 DC 05 DC 05 D0 07 EF 03 DC 05 DC 05 DC 05 DC 05 E3
*/

//-------------------------
int16_t RC_data[8];
uint8_t type;

//=============================
int p;
static uint8_t read8(byte* _Buf)  {
  return (uint8_t)_Buf[p++] & 0xff;
}

static uint16_t read16(byte* _Buf) {
  uint16_t t = read8(_Buf);
  t += (uint16_t)read8(_Buf) << 8;
  return t;
}

static uint32_t read32(byte* _Buf) {
  uint32_t t = read16(_Buf);
  t += (uint32_t)read16(_Buf) << 16;
  return t;
}
/*
  int p;
  int16_t read16(byte* _Buf) {
  byte _Buf1 = _Buf[p++];
  byte _Buf2 = _Buf[p++];
  int16_t r = (_Buf1 & 0xff) | ((_Buf2 << 8) & 0xff00);
  return r;
  }
*/

void read_data(int _num, byte* _buf) {
  p = 0;
  _num = _num / 2;

  int16_t _bufin[_num];

  for (int i = 0; i < _num; i++) {
    _bufin[i] = read16(_buf);
  }

#ifdef _DEBUG
#ifdef _DEBUG_DATA
  DEBUG.println("\n\r");
#endif
#endif

#ifdef _DEBUG
#ifdef _DEBUG_DATA
  DEBUG.print("Data: ");
#endif
#endif
  int16_t RC_data_c[16];
  for (int a = 0; a < _num; a++) {
#ifdef _DEBUG
#ifdef _DEBUG_DATA
    DEBUG.print(_bufin[a]);
    if (a < (_num - 1))
      DEBUG.print(",");
#endif
#endif
    RC_data_c[a] = _bufin[a];
  }

  for (int a = 0; a < _num; a++) {
    RC_data[a] = RC_data_c[PPM_LINE[a]];
  }


#ifdef _DEBUG
#ifdef _DEBUG_DATA
  DEBUG.println(" ");
#endif
#endif
}

byte getChecksum(byte length, byte cmd, byte mydata[]) {
  //三个参数分别为： 数据长度  ，  指令代码  ，  实际数据数组
  byte checksum = 0;
  checksum ^= (length & 0xFF);
  checksum ^= (cmd & 0xFF);
  for (int i = 0; i < length; i++) {
    checksum ^= (mydata[i] & 0xFF);
  }
  return checksum;
}

byte inChar, inCache;
byte buffer[BUFFER_MAX];
unsigned long num = 0;
boolean Available() {
  if (FROM.available() > 0) {
    inCache = inChar;
    inChar = FROM.read();

    buffer[num] = inChar;
#ifdef _DEBUG
    //DEBUG.print(inCache,HEX);
    //DEBUG.print(inChar,HEX);
#endif
    if (num > BUFFER_MAX - 1) {
      num = 0;
      return false;
    }
    else
      num++;
    delayMicroseconds(200);
    return true;
  }
  return false;
}

boolean sta = false;
boolean error = false;
int8_t Protocol(uint8_t _num) {
  if (sta) {
    sta = false;
    num = 0;
    if (inChar == TYPE_NUM_A || inChar == TYPE_NUM_B) {
      error = false;
      type = inChar;
#ifdef _DEBUG
      DEBUG.print("\n\rType[");
      DEBUG.print(type, HEX);
      DEBUG.println("] RAW: ");
#endif
      return 2;
    }
    else  {
      error = true;
      type = 0;
      return 0;
    }
  }

  if (inChar == 0xBB && inCache == 0xAA) {
    sta = true;
#ifdef _DEBUG
    DEBUG.println("\n\r");
    DEBUG.println("\n\r----START----");
#endif
    return 1;
  }

  if (num  == (_num * 2 + 1) && !error) {
    inCache = buffer[_num * 2];
    buffer[_num * 2] = NULL;
    inChar = getChecksum(_num * 2, type, buffer);

#ifdef _DEBUG
    DEBUG.print("Sum[");
    DEBUG.print(inChar, HEX);
    DEBUG.print("],");
    DEBUG.print("Num[");
    DEBUG.print(_num * 2);
    DEBUG.print("]");
#ifdef _DEBUG_HEX
    DEBUG.print(":");
    for (long a = 0; a < num; a++) {
      DEBUG.print(buffer[a], HEX);
      DEBUG.print(" ");
    }
#endif
    DEBUG.println(" ");
#endif

    num = 0;
    if (!error && inCache == inChar) {
#ifdef _DEBUG
      DEBUG.println("Checksum OK");
#endif
      read_data(_num * 2, buffer);
      ///      delay(8);
#ifdef _DEBUG
      DEBUG.println("\n\r----END----");
#endif
      return 3;
    }
    else
      return 0;
  }
  else
    return -1;
}
