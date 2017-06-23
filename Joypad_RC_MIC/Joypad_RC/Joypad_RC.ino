#include <ESP8266.h>
#include "userDef.h"
#include "bat.h"
#include "time.h"
#if defined(LARGE_FLASH)
#include "mpu.h"
#endif
#include "com.h"
#include "data.h"
#include "tft.h"
#include "eep.h"

//eeprom================================
#include <EEPROM.h>

#if defined(LARGE_FLASH)
//MPU===================================
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#endif

void setup()
{
    delay(500);
#ifdef Serial_DEBUG
    Serial.begin(115200);
#endif
    JOY_LOGLN("========hello========");
    //---------------
    key_init();

    //---------------
    JOY_LOGLN("\n\r EEPROM READ...");
    if (!eeprom_read())
    {
        eeprom_write(false);
   	}

    //---------------
    JOY_LOGLN("\n\r TFT INIT...");
    TFT_init(true, tft_rotation);

    //---------------
    JOY_LOGLN("\n\r TFT BEGIN...");
    TIME1 = millis();
    while (millis() - TIME1 < interval_TIME1)
    {
        TFT_begin();
        if (key_get(0, 1))
        {
            JOY_LOGLN("\n\rCorrect IN...");
            //---------------
            JOY_LOGLN("\n\r TFT INIT...");
            TFT_init(false, tft_rotation);

            while (1)
            {
                if (!TFT_config())
                    break;
            }
            JOY_LOGLN("\n\rCorrect OUT...");

            //---------------
            JOY_LOGLN("\n\r EEPROM WRITE...");
            eeprom_write(true);
        }
    }

    //---------------
    JOY_LOGLN("\n\r TFT CLEAR...");
    TFT_clear();

    //---------------
    JOY_LOGLN("\n\r TFT READY...");
    TFT_ready(false);

    //---------------
    comBegin();

#if defined(LARGE_FLASH)
    if (mode_mpu) initMPU(); //initialize device
#endif
    JOY_LOGLN("===========start===========");
}

void loop()
{
#if defined(LARGE_FLASH)
    if (mode_mpu) getMPU();
#endif

    //DATA_begin------------------------------
    buf_code = true;
    if (!data_begin( & outBuf[0], & outBuf[1], & outBuf[2], & outBuf[3], & outBuf[4], & outBuf[5], & outBuf[6], & outBuf[7]))
    {
        buf_code = false;
        TFT_clear();
        TFT_ready(true);
        time4 = millis() + 500;
    }

    //BAT--------------------------------
    if (time3 > millis()) time3 = millis();
    if (millis() - time3 > interval_time3)
    {
        _V_bat = batVoltage();
        time3 = millis();
    }

    //DATA_send-------------------------------
    if (millis() < time2) time2 = millis();
    if (millis() - time2 > interval_time2)
    {
        if (comSend(buf_code ? buf_code_A : buf_code_B, outBuf)) comRece();
        time2 = millis();
    }

    //TFT------------------------------------
    if (millis() < time4) time4 = millis();
    if (millis() - time4 > interval_time4)
    {
        TFT_run();
        time4 = millis();
    }
}
