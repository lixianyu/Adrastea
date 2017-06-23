#include "Arduino.h"

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

RF24 radio(9, 10);   //ce,cs
RF24Network network(radio);

#define this_node  0
#define other_node 1

struct send_a	{
  uint32_t ms;
  uint16_t rf_CH0;
  uint16_t rf_CH1;
  uint16_t rf_CH2;
  uint16_t rf_CH3;
  uint16_t rf_CH4;
  uint16_t rf_CH5;
  uint16_t rf_CH6;
  uint16_t rf_CH7;
};

struct receive_a {
  uint32_t node_ms;
};

boolean nrf_send(uint8_t _code, int16_t _channal[8]) {
#ifdef Serial_DEBUG
  Serial.print("Sending...");
#endif
  send_a sen = {
    millis(), _channal[0], _channal[1], _channal[2], _channal[3], _channal[4], _channal[5], _channal[6], _channal[7]
  };
  RF24NetworkHeader header(other_node);
  if (network.write(header, &sen, sizeof(sen))) {
#ifdef Serial_DEBUG
    Serial.println("Is ok.");
#endif
    return true;
  }
#ifdef Serial_DEBUG
  else
    Serial.println("Is failed.");
#endif
  return false;
}

uint32_t nrf_rece() {
  network.update();
  // If it's time to send a message, send it!
  while ( network.available() ) {
    // If so, grab it and print it out
    RF24NetworkHeader header;
    receive_a rec;
    network.read(header, &rec, sizeof(rec));

    _V_bat_RC = _V_max; //NULL
    RSSI = -40;  //NULL
    return rec.node_ms;
  }
  return 0;
}
