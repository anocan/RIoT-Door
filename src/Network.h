#ifndef NETWORK_H_
#define NETWORK_H_

#include <Arduino.h>

#define NETWORK_PIN D0

extern const char* ssid;
extern const char* password;

extern const u_int8_t NETWORK_PIN;

int initWiFi();

#endif