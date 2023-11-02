#include "Network.h"
#include "RiotFirebase.h"
#include "RFID.h"

void setup() {
    Serial.begin(9600);
    delay(1000);
    initWiFi();
    initFirebase();
    initRFID();
}

void loop() {
   readRFID();
}
