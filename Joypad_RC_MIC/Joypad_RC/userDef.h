#include "Arduino.h"

#define USE_WIFI
//DEBUG-----------
//#define Serial_DEBUG

#if defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega128RFA1__)
#define LARGE_FLASH
#endif
#if defined(__AVR_ATmega128RFA1__)
#define RF_PORT
#endif

//mode------------
#if defined(LARGE_FLASH)
boolean mode_mpu = false; // (mode_mpu: 0 is no mpu, 1 is mpu)
#endif
boolean mode_protocol = 2; // mode_protocol: 0 is mwc, 1 is nrf, 2 is wifi

//RSSI------------
int8_t RSSI = -40;

//MWC-------------
#if defined(RF_PORT)
uint8_t mwc_channal = 11; //RF channel
#else
uint8_t mwc_channal = 4; //RF channel "9600", "19200", "38400", "57600", "115200"
#endif

#if  defined(__AVR_ATmega32U4__)
#define mwc_port Serial1    //Serial1 is D0 D1
#elif defined(USE_WIFI)
#define mwc_port Serial1
ESP8266 wifi(mwc_port, 115200);
#elif defined(RF_PORT)
#define mwc_port ZigduinoRadio    //RF
#else
#define mwc_port Serial    //Serial is D0 D1
#endif

//nRF-------------
#define interval_debug  100
uint8_t nrf_channal = 70;  //0~125

#if defined(LARGE_FLASH)
//MPU-------------
#define MPU_maximum 70
#endif

//Time------------
#define interval_TIME1 1000    //setup delay
#define interval_time2 40      //send interval
#define interval_time3 1000    //battery interval
#define interval_time4 50      //tft interval

#ifdef Serial_DEBUG
#define JOY_LOG(x) Serial.print(x)
#define JOY_LOGLN(x) Serial.println(x)
#else
#define JOY_LOG(x) 
#define JOY_LOGLN(x) 
#endif
