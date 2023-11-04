#include "RiotSystem.h"
#include "Network.h"
#include "RiotFirebase.h"
#include "RFID.h"

void setup() {
    attachInterrupt(digitalPinToInterrupt(0), backUpRead, RISING);
    Serial.begin(9600);
    initWiFi();
    initFirebase();
    initRFID();
}

void loop() {
   readRFID();
}
