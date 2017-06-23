#include"arduino.h"
//#define _DEBUG
//#define _DEBUG_HEX
//#define _DEBUG_DATA


#define FROM ZigduinoRadio
#define DEBUG Serial

#define BUFFER_MAX 256
#define CHANNEL_NUM 8
#define TYPE_NUM_A 0xC8
#define TYPE_NUM_B 0xC9

#define SAFE_CHANNEL_VAL 950
#define SAFE_TIME_OUT 200

#define ROLL 0
#define PITCH 1
#define YAW 2
#define THROTTLE 3
#define AUX1 4
#define AUX2 5
#define AUX3 6
#define AUX4 7

//Select the right line depending on your radio brand. Feel free to modify the order in your PPM order is different
uint8_t PPM_LINE[8] =
{
  ROLL, PITCH, YAW, THROTTLE, AUX1, AUX2, AUX3, AUX4
};
