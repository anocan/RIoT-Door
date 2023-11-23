#include "RiotSystem.h"
#include "Network.h"
#include "RiotFirebase.h"
#include "RFID.h"

SYSTEM_STATUS SYSTEM = SYS_NORMAL;

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3*3600; // +3 UTC in seconds
const int   daylightOffset_sec = 0;

const int maintenanceLowerHour = 19;
const int maintenanceLowerMinute = 20;

const int maintenanceUpperHour = 19;
const int maintenanceUpperMinute = 20;

bool taskExecuted = false;

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

void releaseDoor() {
  Serial.println("Door has been unlocked!");
}

void systemMaintenance(){
  time_t now;
  time(&now);
  struct tm timeinfo;
  //char formattedTime[30]; // Buffer to hold the formatted time

  localtime_r(&now, &timeinfo);

  // Get current time
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

    // Check if it's between maintenanceLower Time and maintenanceUpper Time
  if (timeinfo.tm_hour == maintenanceLowerHour && timeinfo.tm_min >= maintenanceLowerMinute && timeinfo.tm_hour == maintenanceUpperHour && timeinfo.tm_min <= maintenanceUpperMinute) {
    if (!taskExecuted) {
      Serial.println("SYSTEM MAINTENANCE!");
      changeRiotCardStatus();
      resetInOrOutStatus();
      taskExecuted = true;
    }
  } else {
    taskExecuted = false; // Reset the flag if not within the specified time
  }

  delay(1000); // Delay to avoid excessive checking
  // Format time using strftime
  // strftime(formattedTime, sizeof(formattedTime), "%A, %B %d %Y %H:%M:%S", &timeinfo);
    //Serial.println(formattedTime);

}