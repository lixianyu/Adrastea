#include <ZigduinoRadio.h>
#include "def.h"
#include "RC.h"
#include "RCdata.h"

//#include  <Servo.h>
#include <RCEncoder.h>

void setup()
{
  FROM.begin(11);
#ifdef _DEBUG
  DEBUG.begin(115200);
#endif
  RC_begin();
}

unsigned long ERROR_NUM = 0;
unsigned long ERROR_NUM_CACHE = 0;
unsigned long ERROR_TIME = 0;
unsigned long ERROR_SPEED = 0;
// the loop routine runs over and over again forever:
void loop()
{
  if (Available())
  {
    switch (Protocol(CHANNEL_NUM))
    {
      case 1:
        ERROR_TIME = millis();
        break;
      case 3:
#ifdef _DEBUG
        DEBUG.println("\n\rsendDATA READY");
#endif
        if (type == TYPE_NUM_B) {
          Serial.println("OK!");
          delay(500);
        }
        RC_write(RC_data);
        ERROR_NUM++;
#ifdef _DEBUG
        DEBUG.print("True loop: ");
        DEBUG.print(ERROR_NUM);
        DEBUG.print(",Time: ");
        DEBUG.println(millis() - ERROR_TIME);
#endif
        break;
      case 0:
#ifdef _DEBUG
        DEBUG.println("DATA ERROR");
#endif
        ERROR_NUM = 0;
        break;
    }
  }

  //    if (millis() - ERROR_SPEED > 1000)
  //    {
  //      Serial.print("\t\t\t\t\t\t loop:");
  //      Serial.println((ERROR_NUM - ERROR_NUM_CACHE));
  //      ERROR_NUM_CACHE = ERROR_NUM;
  //      ERROR_SPEED = millis();
  //    }

  RC_safe_mode();
}
