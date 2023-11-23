#include "RiotSystem.h"
#include "Network.h"
#include "RiotFirebase.h"
#include "RFID.h"

void setup() {
    attachInterrupt(digitalPinToInterrupt(0), backUpRead, RISING);
    Serial.begin(9600);
    initWiFi();
    //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    initFirebase();
    initRFID();
}

void loop() {
    //systemMaintenance();
    readRFID();
}
