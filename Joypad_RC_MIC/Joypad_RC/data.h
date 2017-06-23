#include "Arduino.h"
#include "joy.h"

byte inBuf[16];

int16_t outBuf[8] = {
  Joy_MID, Joy_MID, Joy_MID, Joy_MID, Joy_MID, Joy_MID, Joy_MID, Joy_MID
};

boolean data_begin(int16_t * _channal_0, int16_t * _channal_1, int16_t * _channal_2, int16_t * _channal_3, int16_t * _channal_4, int16_t * _channal_5, int16_t * _channal_6, int16_t * _channal_7) {
  boolean _sta = true;

  if (joypadUpdata()) {
    if (!mode_protocol) {
      if (Joy_y < Joy_MID - Joy_maximum + 50) {
        buf_type = !buf_type;

        Joy1_x = buf_type ? Joy_MID - Joy_maximum : Joy_MID;
        Joy1_y = !buf_type ? Joy_MID - Joy_maximum : Joy_MID;

        _sta = false;
      }
    }
  }

  if (mode_protocol) {  //Robot
    if (!Joy_sw_l) {
      Joy_x = Joy_MID;
      Joy_y = Joy_MID;
      Joy1_x = Joy_MID;
      Joy1_y = Joy_MID;
    }
  }
  else {       //QuadCopter
    if (!Joy_sw_l) Joy_y = Joy_MID - Joy_maximum;
  }

  int16_t * _i = _channal_0;
  _i[0] = Joy1_x;
  _i = _channal_1;
  _i[0] = Joy1_y;
  _i = _channal_2;
  _i[0] = Joy_x;
  _i = _channal_3;
  _i[0] = Joy_y;
  _i = _channal_4;
  _i[0] = AUX[0] ? Joy_MID + Joy_maximum : Joy_MID - Joy_maximum;
  _i = _channal_5;
  _i[0] = AUX[1] ? Joy_MID + Joy_maximum : Joy_MID - Joy_maximum;
  _i = _channal_6;
  _i[0] = AUX[2] ? Joy_MID + Joy_maximum : Joy_MID - Joy_maximum;
  _i = _channal_7;
  _i[0] = AUX[3] ? Joy_MID + Joy_maximum : Joy_MID - Joy_maximum;

  return _sta;
}
