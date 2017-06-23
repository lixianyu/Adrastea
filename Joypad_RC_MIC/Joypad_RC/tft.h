#include "Arduino.h"

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>

Adafruit_ST7735 tft = Adafruit_ST7735(5, 4, -1);    //cs,dc,rst

#define setFont_M tft.setTextSize(2)
#define setFont_S tft.setTextSize(0)

#define tft_width  128
#define tft_height 160

boolean tft_theme = false;  //0 is white,1 is black
boolean tft_rotation = 1;

#define TFT_TOP ST7735_BLACK
#define TFT_BUT ST7735_WHITE

uint16_t  tft_colorA = TFT_BUT;
uint16_t  tft_colorB = TFT_TOP;
uint16_t  tft_colorC = 0x06FF;
uint16_t  tft_colorD = 0xEABF;

#define tft_bat_x 18
#define tft_bat_y 9
#define tft_bat_x_s 2
#define tft_bat_y_s 4

#define tft_font_s_height 8
#define tft_font_m_height 16
#define tft_font_l_height 24

#define _Q_x 33
#define _Q_y 36
#define _W_x 93
#define _W_y 5

#define _Q_font_x 2
#define _Q_font_y (_Q_y - 1)

int8_t tft_cache = 1;

//======================================
void TFT_clear() {
  tft.fillScreen(tft_colorB);
}

void TFT_init(boolean _init, boolean _rot) {
  tft_colorB = tft_theme ? TFT_TOP : TFT_BUT;
  tft_colorA = tft_theme ? TFT_BUT : TFT_TOP;

  if (_init) {
    tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
    //  Serial.println("init");
    tft.fillScreen(tft_colorB);

    if (_rot)
      tft.setRotation(2);
  }

  tft.fillRect(0, 0, tft_width, 40, tft_colorA);
  tft.setTextColor(tft_colorB);
  setFont_M;
  tft.setCursor(26, 6);
  tft.print("Joypad");
  setFont_S;
  tft.setCursor(32, 24);
  tft.print("Microduino");
  tft.fillRect(0, 40, tft_width, 120, tft_colorB);
}

void TFT_begin() {
  setFont_S;

  tft.setTextColor(tft_colorA);
  tft.setCursor(_Q_font_x, 44);
  tft.println("[key1] enter config");

  setFont_M;
  tft.setCursor(4, 150);
  for (uint8_t a = 0; a < (millis() - TIME1) / (interval_TIME1 / 10); a++) {
    tft.print("-");
  }
}

int8_t menu_num_A = 0;
int8_t menu_num_B = 0;
int8_t menu_sta = 0;

#if defined(LARGE_FLASH)
char *menu_str_a[5] = {
  "Joystick Config", "Protocol Config", "System Config", "Gyroscope Config", "Exit"
};
#else
char *menu_str_a[4] = {
  "Joystick Config", "Protocol Config", "System Config", "Exit"
};
#endif

#if defined(LARGE_FLASH)
char *menu_str_b[4][3] = {
#else
char *menu_str_b[3][3] = {
#endif
  {"Joystick Correct.", "Dead Zone config"},
  {"Mode", "Quadrotor Channel", "nRF24 Channel"},
  {"TFT Theme", "TFT Rotation"}
#if defined(LARGE_FLASH)
  , {"Gyroscope OFF", "Gyroscope ON"}
#endif
};

void TFT_menu(int8_t _num, char *_data) {
  tft.drawRect(7, 49 + 15 * _num, 114, 16, tft_colorA);
  tft.setCursor(10, 54 + 15 * _num);
  tft.print(_data);
}

void TFT_menu(int8_t _num, int16_t _data) {
  tft.drawRect(7, 49 + 15 * _num, 114, 16, tft_colorA);
  tft.setCursor(10, 54 + 15 * _num);
  tft.print(_data);
}

void TFT_cursor(int8_t _num) {
  tft.drawLine(1, 51 + 15 * _num, 4, 56 + 15 * _num, tft_colorA);
  tft.drawLine(4, 57 + 15 * _num, 1, 62 + 15 * _num, tft_colorA);
  tft.drawLine(1, 51 + 15 * _num, 1, 62 + 15 * _num, tft_colorA);
}

boolean return_menu = false;

boolean TFT_config() {
  tft.setTextColor( tft_colorA);

  if (key_get(0, 1)) {
    menu_sta --;
    tft_cache = 1;

    if (menu_sta <= 0)
      menu_num_B = 0; //zero
  }
  if (key_get(1, 1)) {
    if (return_menu)
      menu_sta --;
    else
      menu_sta ++;
    tft_cache = 1;
  }

  if (menu_sta > 2)
    menu_sta = 2;
  if (menu_sta < 0)
    menu_sta = 0;

  return_menu = false;
  //-------------------------------
  if (tft_cache) {
    tft.fillRect(0, 40, tft_width, 100, tft_colorB);
  }

  if (menu_sta == 2) {
    switch (menu_num_A) {
      case 0: {
          switch (menu_num_B) {
            case 0: {
                if (tft_cache)
                {
                  for (uint8_t a = 0; a < 4; a++)
                  {
                    joy_correct_min[a] = 0;
                    joy_correct_max[a] = 0;
                  }
                }
                for (uint8_t a = 0; a < 4; a++) {
                  tft.setCursor(2, 120);
                  tft.print("Move Joystick MaxGear");
                  int16_t _c = Joy_i(a, false, Joy_MID - Joy_maximum, Joy_MID + Joy_maximum);
                  if (_c > joy_correct_max[a]) {
                    tft.fillRect(0, 40, tft_width, 100, tft_colorB);
                    joy_correct_max[a] = _c;
                  }
                  //                  joy_correct_max[a] = constrain(joy_correct_max[a], 0, Joy_maximum);
                  if (_c < joy_correct_min[a]) {
                    tft.fillRect(0, 40, tft_width, 100, tft_colorB);
                    joy_correct_min[a] = _c;
                  }
                  //                  joy_correct_min[a] = constrain(joy_correct_min[a], -Joy_maximum, 0);
                }

                for (uint8_t d = 0; d < 2; d++) {
                  tft.drawFastHLine(12 + 70 * d, 80, 33, tft_colorA);
                  tft.drawFastVLine(28 + 70 * d, 64, 33, tft_colorA);
                  //                tft.fillRect(2, 90-4, 20, 12, tft_colorB);
                  tft.drawCircle(44 + 70 * d, 80, map(joy_correct_min[0 + 2 * d], 0, -512, 1, 10), tft_colorA);
                  tft.drawCircle(12 + 70 * d, 80, map(joy_correct_max[0 + 2 * d], 0, 512, 1, 10), tft_colorA);
                  tft.drawCircle(28 + 70 * d, 64, map(joy_correct_min[1 + 2 * d], 0, -512, 1, 10), tft_colorA);
                  tft.drawCircle(28 + 70 * d, 96, map(joy_correct_max[1 + 2 * d], 0, 512, 1, 10), tft_colorA);
                }
                return_menu = true;
              }
              break;
            case 1: {
                if (key_get(2, 1)) {
                  Joy_deadzone_val++;
                  tft.fillRect(0, 40, tft_width, 100, tft_colorB);
                }
                if (key_get(3, 1)) {
                  Joy_deadzone_val--;
                  tft.fillRect(0, 40, tft_width, 100, tft_colorB);
                }
                Joy_deadzone_val = constrain(Joy_deadzone_val, 0, 25);

                TFT_menu(0, Joy_deadzone_val);
                TFT_cursor(0);
                return_menu = true;
              }
              break;
          }
        }
        break;

      case 1: {
          switch (menu_num_B) {
            case 0: {
                char *menu_str_c[2] = { "Quadro.", "nRF24"};
                if (key_get(2, 1) || key_get(3, 1)) {
                  mode_protocol = !mode_protocol;
                  tft.fillRect(0, 40, tft_width, 100, tft_colorB);
                }
                for (uint8_t c = 0; c < 2; c++) {
                  TFT_menu(c, menu_str_c[c]);
                }

                TFT_cursor(mode_protocol);
                return_menu = true;
              }
              break;
            case 1: {
#if !defined(RF_PORT)
                char *menu_str_c[5] = {"9600", "19200", "38400", "57600", "115200"};
#endif
                if (key_get(2, 1)) {
                  mwc_channal++;
                  tft.fillRect(0, 40, tft_width, 100, tft_colorB);
                }
                if (key_get(3, 1)) {
                  mwc_channal--;
                  tft.fillRect(0, 40, tft_width, 100, tft_colorB);
                }

#if !defined(RF_PORT)
                mwc_channal = constrain(mwc_channal, 0, 4);
                TFT_menu(0, menu_str_c[mwc_channal]);
#else
                mwc_channal = constrain(mwc_channal, 11, 26);
                TFT_menu(0, mwc_channal);
#endif
                TFT_cursor(0);
                return_menu = true;
              }
              break;

            case 2: {
                if (key_get(2, 1)) {
                  nrf_channal++;
                  tft.fillRect(0, 40, tft_width, 100, tft_colorB);
                }
                if (key_get(3, 1)) {
                  nrf_channal--;
                  tft.fillRect(0, 40, tft_width, 100, tft_colorB);
                }
                nrf_channal = constrain(nrf_channal, 0, 125);

                TFT_menu(0, nrf_channal);
                TFT_cursor(0);
                return_menu = true;
              }
              break;
          }
        }
        break;
      case 2: {
          switch (menu_num_B) {
            case 0: {
                tft_theme = !tft_theme;
                TFT_init(true, tft_rotation);
                tft_cache = 1;
                tft.setTextColor(tft_colorA);
                menu_sta --;
              }
              break;
            case 1: {
                tft_rotation = !tft_rotation;
                TFT_init(true, tft_rotation);
                tft_cache = 1;
                tft.setTextColor(tft_colorA);
                menu_sta --;
              }
              break;
          }

        }
        break;

#if defined(LARGE_FLASH)
      case 3: { //mpu
          mode_mpu = menu_num_B;
          tft_cache = 1;
          menu_sta = 0; //back main menu
          menu_num_B = 0; //zero
        }
        break;
#endif
    }
  }

  /*
    Serial.print(menu_sta);
    Serial.print(",");
    Serial.print(menu_num_A);
    Serial.print(",");
    Serial.println(menu_num_B);
  */
  //----------------------------
  if (menu_sta == 1) {
#if defined(LARGE_FLASH)
    int8_t meun_b_max[5] = {1, 2, 1, 1, 0};
#else
    int8_t meun_b_max[4] = {1, 2, 1, 0};
#endif
    if (!meun_b_max[menu_num_A]) {
      return false;
    }
    else {
      if (key_get(2, 1)) {
        tft.fillRect(0, 40, 5, 100, tft_colorB);
        menu_num_B--;
      }
      if (key_get(3, 1)) {
        tft.fillRect(0, 40, 5, 100, tft_colorB);
        menu_num_B++;
      }
      menu_num_B = constrain(menu_num_B, 0, meun_b_max[menu_num_A]);

      TFT_cursor(menu_num_B);

      if (tft_cache) {
        for (uint8_t b = 0; b < (meun_b_max[menu_num_A] + 1); b++) {
          TFT_menu(b, menu_str_b[menu_num_A][b]);
        }
      }
    }
  }

  //main menu
  if (menu_sta == 0) {
    //custer
    if (key_get(2, 1)) {
      tft.fillRect(0, 40, 5, 100, tft_colorB);
      menu_num_A--;
    }
    if (key_get(3, 1)) {
      tft.fillRect(0, 40, 5, 100, tft_colorB);
      menu_num_A++;
    }
#if defined(LARGE_FLASH)
    menu_num_A = constrain(menu_num_A, 0, 4);
#else
    menu_num_A = constrain(menu_num_A, 0, 3);
#endif

    TFT_cursor(menu_num_A);

    if (tft_cache) {
#if defined(LARGE_FLASH)
      for (uint8_t a = 0; a < 5; a++) {
#else
      for (uint8_t a = 0; a < 4; a++) {
#endif
        TFT_menu(a, menu_str_a[a]);
      }
    }
  }

  if (tft_cache) {
    //BACK
    tft.fillCircle(12, 149, 8, tft_colorA);
    tft.drawLine(11, 145, 7, 149, tft_colorB);
    tft.drawLine(7, 149, 11, 153, tft_colorB);
    tft.drawLine(7, 149, 17, 149, tft_colorB);
    //ENTER
    tft.fillCircle(12 + 20, 149, 8, tft_colorA);
    tft.drawLine(10 + 20, 146, 7 + 20, 149, tft_colorB);
    tft.drawLine(7 + 20, 149, 10 + 20, 152, tft_colorB);
    tft.drawLine(7 + 20, 149, 15 + 20, 149, tft_colorB);
    tft.drawLine(15 + 20, 146, 15 + 20, 149, tft_colorB);
    //PREV
    tft.fillCircle(127 - 12, 149, 8, tft_colorA);
    tft.drawLine(127 - 12, 153, 127 - 8, 149, tft_colorB);
    tft.drawLine(127 - 12, 153, 127 - 16, 149, tft_colorB);
    tft.drawLine(127 - 12, 153, 127 - 12, 145, tft_colorB);
    //NEXT
    tft.fillCircle(127 - 32, 149, 8, tft_colorA);
    tft.drawLine(127 - 32, 145, 127 - 28, 149, tft_colorB);
    tft.drawLine(127 - 32, 145, 127 - 36, 149, tft_colorB);
    tft.drawLine(127 - 32, 145, 127 - 32, 153, tft_colorB);
  }
  tft_cache --;
  if (tft_cache < 0)  tft_cache = 0;

  return true;
}

//------------------
#define _C_x_S  (_Q_x + 1)
#define _C_x_M  (_Q_x + ((_W_x + 1) / 2))
#define _C_x_E  (_Q_x + _W_x - 1)

char *NAME[8] = {
  "ROLL", "PITCH", "YAW", "THROT", "AUX1", "AUX2", "AUX3", "AUX4"
};

void TFT_ready(boolean _Mode) {
  if (_Mode) {
    tft.setTextColor(tft_colorA);
    setFont_M;
    tft.setCursor(buf_type ? 40 : 32, 5);
    tft.print(buf_type ? "LOCK" : "UNLOCK");
    delay(500);
  }
  tft.fillRect(0, 0, 128, 26, tft_colorA);

  tft.drawRect(tft_width - tft_bat_x - tft_bat_x_s - 2, 2, tft_bat_x, tft_bat_y, tft_colorB);
  tft.drawRect(tft_width - tft_bat_x_s - 2, 2 + (tft_bat_y - tft_bat_y_s) / 2, tft_bat_x_s, tft_bat_y_s, tft_colorB);

  tft.drawRect(tft_width - tft_bat_x - tft_bat_x_s - 2 - 4 - tft_bat_x, 2, tft_bat_x, tft_bat_y, tft_colorB);
  tft.drawRect(tft_width - tft_bat_x_s - 2 - 4 - tft_bat_x, 2 + (tft_bat_y - tft_bat_y_s) / 2, tft_bat_x_s, tft_bat_y_s, tft_colorB);

  tft.setTextColor(tft_colorB);
  setFont_S;

  tft.setCursor(_Q_font_x, 3);
  tft.print(mode_protocol ? "nRF24" : "Quadr");
  tft.print(" CHAN.");
  tft.print(mode_protocol ? nrf_channal : mwc_channal);
  tft.setCursor(_Q_font_x, 16);
  tft.print("Time:");

  tft.setTextColor(tft_colorA);
  for (uint8_t a = 0; a < 8; a++) {
    tft.setCursor(_Q_font_x, _Q_font_y + a * 15);
    tft.print(NAME[a]);
    //------------------------------------------
    tft.drawRect(_Q_x, _Q_y + a * 15, _W_x, _W_y, tft_colorA);
  }
}

boolean _a = false;
unsigned long _b = 0;
void TFT_run() {
  if (!nodeClock()) {
    RSSI = -127;
  }

  if (outBuf[3] > (Joy_MID - Joy_maximum)) {
    if (_a) {
      Joy_time[0] = millis() - Joy_time[1];
      _a = false;
    }
    Joy_time[1] = millis() - Joy_time[0];
  }
  else {
    _a = true;
  }

  if (millis() < _b) _b = millis();
  if (millis() - _b > 500) {
    tft.fillRect(_Q_font_x + 30, 16, 45, 7, tft_colorA);
    tft.setTextColor(tft_colorB);
    tft.setCursor(_Q_font_x + 30, 16);
    tft.print((Joy_time[1] / 1000) / 60);
    tft.print("m");
    tft.print((Joy_time[1] / 1000) % 60);
    tft.print("s");

    tft.fillRect(tft_width - 44, 16, 44, 7, tft_colorA);
    tft.setTextColor(tft_colorB);
    tft.setCursor(tft_width - 44, 16);
    tft.print(RSSI);
    tft.print("dBm");

    //battery------------------
    tft.fillRect(tft_width - tft_bat_x - 3, 3, map(_V_bat, _V_min, _V_max, 0, tft_bat_x - 2) , tft_bat_y - 2, tft_colorB);
    tft.fillRect(tft_width - tft_bat_x - 3 + map(_V_bat, _V_min, _V_max, 0, tft_bat_x - 2), 3, map(_V_bat, _V_min, _V_max, tft_bat_x - 2, 0) , tft_bat_y - 2, tft_colorA);
    tft.fillRect(tft_width - tft_bat_x - 3 - 4 - tft_bat_x, 3, map(_V_bat_RC, _V_min, _V_max, 0, tft_bat_x - 2) , tft_bat_y - 2, tft_colorB);
    tft.fillRect(tft_width - tft_bat_x - 3 - 4 - tft_bat_x + map(_V_bat_RC, _V_min, _V_max, 0, tft_bat_x - 2), 3, map(_V_bat_RC, _V_min, _V_max, tft_bat_x - 2, 0) , tft_bat_y - 2, tft_colorA);

    _b = millis();
  }

  for (uint8_t a = 0; a < 8; a++) {
    int8_t _C_x_A0, _C_x_B0, _C_x_A, _C_x_B, _C_x_A1, _C_x_B1;
    int8_t _C_x;

    if (outBuf[a] < Joy_MID) {
      outBuf[a] = constrain(outBuf[a], Joy_MID - Joy_maximum, Joy_MID + Joy_maximum);
      _C_x = map(outBuf[a], Joy_MID - Joy_maximum, Joy_MID, _C_x_S, _C_x_M);

      _C_x_A0 = _C_x_S;
      _C_x_B0 = _C_x - _C_x_S;

      _C_x_A = _C_x;
      _C_x_B = _C_x_M - _C_x;

      _C_x_A1 = _C_x_M;
      _C_x_B1 = _C_x_E - _C_x_M;
    } else if (outBuf[a] > Joy_MID) {
      _C_x = map(outBuf[a], Joy_MID, Joy_MID + Joy_maximum, _C_x_M, _C_x_E);

      _C_x_A0 = _C_x_S;
      _C_x_B0 = _C_x_M - _C_x_S;

      _C_x_A = _C_x_M;
      _C_x_B = _C_x - _C_x_M;

      _C_x_A1 = _C_x;
      _C_x_B1 = _C_x_E - _C_x;
    } else {
      _C_x_A0 = _C_x_S;
      _C_x_B0 = _C_x_M - _C_x_S;

      _C_x_A = _C_x_M;
      _C_x_B = 0;

      _C_x_A1 = _C_x_M;
      _C_x_B1 = _C_x_E - _C_x_M;
    }
    tft.fillRect(_C_x_A0,  _Q_y + a * 15 + 1, _C_x_B0, _W_y - 2, tft_colorB);
    tft.fillRect(_C_x_A,  _Q_y + a * 15 + 1, _C_x_B, _W_y - 2, tft_colorC);
    tft.fillRect(_C_x_A1,  _Q_y + a * 15 + 1, _C_x_B1, _W_y - 2, tft_colorB);

    tft.fillRect(_C_x_M,  _Q_y + a * 15 - 1, 1, _W_y + 2, tft_colorD);
  }
}
