#include "arduino.h"

/*
  #include <Servo.h>
  Servo myservo[8];   // a maximum of eight servo objects can be created
  int servo_pin[8]=
  {
  4,5,6,7,8,9,10,11
  };
*/
#include <RCEncoder.h>
#define PPM_PIN 2

unsigned long safe_time = 0;

void RC_begin()
{
  /*
    for(int a=0;a<8;a++)
    {
    myservo[a].attach(servo_pin[a]);  // attaches the servo on pin 9 to the servo object
    }
  */
  encoderBegin(PPM_PIN);
}

//void RC_write(int RC_write_i,int RC_write_x)
void RC_write(int* RC_write_i)
{
  for (int i = 0; i < 8; i++)
  {
    if (RC_write_i[i] >= 1000 && RC_write_i[i] <= 2000)
    {
      //myservo[i].writeMicroseconds(RC_write_i[i]);
      encoderWrite(i, RC_write_i[i]);
    }
  }

  safe_time = millis();
}

void RC_safe_mode()
{
  if (millis() < safe_time) safe_time = millis();
  if (millis() - safe_time > SAFE_TIME_OUT)
  {
#ifdef _DEBUG
    DEBUG.println("\n\rSAFE_TIME_OUT");
#endif
    for (int i = 0; i < 8; i++)
    {
      //myservo[i].writeMicroseconds(SAFE_CHANNEL_VAL);
      encoderWrite(i, SAFE_CHANNEL_VAL);
    }
  }
}
