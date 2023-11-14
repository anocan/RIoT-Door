#include "RiotSystem.h"
#include "Network.h"
#include "RiotFirebase.h"
#include "RFID.h"

SYSTEM_STATUS SYSTEM = SYS_NORMAL;

boolean startTimer = true;
long now = micros();
long lastTrigger = 0;
int resetCounter = 0;
int resetThreshold = 5;

void IRAM_ATTR backUpRead() {
  startTimer = true;
  resetCounter++;
  if (resetCounter >= resetThreshold) {
    // System resets after sufficient amount of trigger
    ESP.restart();
  }

    if(startTimer && (now - lastTrigger > (INTERRUPT_SECONDS*1000000))) {
    pinMode(NETWORK_PIN, OUTPUT);
    pinMode(FIREBASE_PIN, OUTPUT); 
    pinMode(READY_PIN, OUTPUT);   
    Serial.println("BACKUP RFID READ ACTIVATED");
    lastTrigger = micros();
    startTimer = false;
    SYSTEM = SYS_BACKUP;
    digitalWrite(NETWORK_PIN,HIGH);
    digitalWrite(FIREBASE_PIN,HIGH);
    digitalWrite(READY_PIN,HIGH);
  }
}