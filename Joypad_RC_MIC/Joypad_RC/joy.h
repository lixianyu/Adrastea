#include "Arduino.h"
#include <Joypad.h>

#define Mpu_maximum 70
#define Joy_s_maximum 200 //MAX 300
#define Joy_maximum 450 //MAX 500
#define Joy_MID 1500  //1500

uint8_t Joy_deadzone_val = 10;
int16_t joy_correct_max[4],
        joy_correct_min[4];

int16_t Joy_x,
        Joy_y,
        Joy1_x,
        Joy1_y;

boolean Joy_sw_l, Joy_sw_r;
boolean AUX[4] = {0, 0, 0, 0};

uint8_t key_pin[5] = {CH_SWITCH_1, CH_SWITCH_2, CH_SWITCH_3, CH_SWITCH_4, CH_JOYSTICK1_SW}; //1 2 3 4 5
boolean key_status[5],
        key_cache[5];

void key_init() {
  for (uint8_t a = 0; a < 5; a++) {
    key_status[a] = LOW;
    key_cache[a] = HIGH;
  }
}

boolean key_get(uint8_t _key_num, boolean _key_type) {
  key_cache[_key_num] = key_status[_key_num];
  key_status[_key_num] = !Joypad.readButton(key_pin[_key_num]);
  switch (_key_type) {
    case 0:
      if (!key_status[_key_num] && key_cache[_key_num])
        return true;
      else
        return false;
      break;
    case 1:
      if (key_status[_key_num] && !key_cache[_key_num])
        return true;
      else
        return false;
      break;
  }
}

int16_t Joy_dead_zone(int16_t _Joy_vol) {
  if (abs(_Joy_vol) > Joy_deadzone_val)
    return ((_Joy_vol > 0) ? (_Joy_vol - Joy_deadzone_val) : (_Joy_vol + Joy_deadzone_val));
  else
    return 0;
}

int16_t Joy_i(int16_t _Joy_i, boolean _Joy_b, int16_t _Joy_MIN, int16_t _Joy_MAX) {
  int16_t _Joy_a;
  switch (_Joy_i) {
    case 0:
      _Joy_a = Joy_dead_zone(Joypad.readJoystickX());
      break;
    case 1:
      _Joy_a = Joypad.readJoystickY();    //throt
      break;
    case 2:
      _Joy_a = Joy_dead_zone(Joypad.readJoystick1X());
      break;
    case 3:
      _Joy_a = Joy_dead_zone(Joypad.readJoystick1Y());
      break;
  }

  if (_Joy_b) {
    if (_Joy_a < 0)
      _Joy_a = map(_Joy_a, joy_correct_min[_Joy_i], 0, _Joy_MAX, Joy_MID);
    else
      _Joy_a = map(_Joy_a, 0, joy_correct_max[_Joy_i], Joy_MID, _Joy_MIN);

    _Joy_a = constrain(_Joy_a, _Joy_MIN, _Joy_MAX);
  }
  return _Joy_a;
}

boolean joypadUpdata() {
  boolean _k = key_get(4, 0);
  for (uint8_t a = 0; a < 4; a++)
    if (key_get(a, 0))  AUX[a] = !AUX[a];
  Joy_sw_l = Joypad.readButton(CH_SWITCH_L);
  Joy_sw_r = Joypad.readButton(CH_SWITCH_R);

#if defined(LARGE_FLASH)
  int16_t y[3];
  if (mode_mpu) {
    for (uint8_t a = 0; a < 3; a++) {
      y[a] = ypr[a] * 180 / M_PI;
      y[a] = constrain(y[a], -Mpu_maximum, Mpu_maximum);
    }
  }
#endif

  if (Joy_sw_r) {
    Joy_x = Joy_i(0, true, Joy_MID - Joy_maximum, Joy_MID + Joy_maximum);
    Joy_y = Joy_i(1, true, Joy_MID - Joy_maximum, Joy_MID + Joy_maximum);

#if defined(LARGE_FLASH)
    if (mode_mpu) {
      Joy1_x = map(y[2], -Mpu_maximum, Mpu_maximum, Joy_MID - Joy_maximum, Joy_MID + Joy_maximum);
      Joy1_y = map(y[1], -Mpu_maximum, Mpu_maximum, Joy_MID - Joy_maximum, Joy_MID + Joy_maximum);
    }
    else
#endif
    {
      Joy1_x = Joy_i(2, true, Joy_MID - Joy_maximum, Joy_MID + Joy_maximum);
      Joy1_y = Joy_i(3, true, Joy_MID - Joy_maximum, Joy_MID + Joy_maximum);
    }
  }
  else {
    Joy_x = Joy_i(0, true, Joy_MID - Joy_s_maximum, Joy_MID + Joy_s_maximum);
    Joy_y = Joy_i(1, true, mode_protocol ? Joy_MID - Joy_s_maximum : Joy_MID - Joy_maximum, mode_protocol ? Joy_MID + Joy_s_maximum : Joy_MID + Joy_maximum); //  Robot,QuadCopter

#if defined(LARGE_FLASH)
    if (mode_mpu) {
      Joy1_x = map(y[2], -Mpu_maximum, Mpu_maximum, Joy_MID - Joy_s_maximum, Joy_MID + Joy_s_maximum);
      Joy1_y = map(y[1], -Mpu_maximum, Mpu_maximum, Joy_MID - Joy_s_maximum, Joy_MID + Joy_s_maximum);
    }
    else
#endif
    {
      Joy1_x = Joy_i(2, true, Joy_MID - Joy_s_maximum, Joy_MID + Joy_s_maximum);
      Joy1_y = Joy_i(3, true, Joy_MID - Joy_s_maximum, Joy_MID + Joy_s_maximum);
    }
  }

  return  _k;
}
