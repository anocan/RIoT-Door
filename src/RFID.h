#ifndef RFID_H_
#define RFID_H_

#include <Arduino.h>

#define RST_PIN D2
#define SS_PIN D4
#define READY_PIN D8

extern const uint8_t RST_PIN;
extern const uint8_t SS_PIN;

extern const u_int8_t READY_PIN;

void initRFID();
String readRFID();

#endif