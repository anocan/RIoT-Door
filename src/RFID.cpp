#include "RFID.h"
#include "RiotFirebase.h"
#include <SPI.h>
#include <MFRC522.h>

MFRC522 mfrc522(SS_PIN, RST_PIN);

const char* knownTagUIDs[] = {
    "e3f76c19", // Replace with the actual tag UID
};

const int numKnownTags = sizeof(knownTagUIDs) / sizeof(knownTagUIDs[0]);
const char* correspondingIDs[] = {
    "ID1",
};


void initRFID() {
    pinMode(READY_PIN, OUTPUT);
    SPI.begin();
    mfrc522.PCD_Init();
    digitalWrite(READY_PIN, HIGH);
}   

void readRFID() {
    if (mfrc522.PICC_IsNewCardPresent()) {
        if (mfrc522.PICC_ReadCardSerial()) {
            Serial.print("Tag UID: ");
            String tagUID = "";
            for (byte i = 0; i < mfrc522.uid.size; i++) {
                tagUID += (mfrc522.uid.uidByte[i] < 0x10 ? "0" : "") + String(mfrc522.uid.uidByte[i], HEX);
                Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
                Serial.print(mfrc522.uid.uidByte[i], HEX);
            }
            Serial.println();

        //Serial.println("TAGUID:: " + tagUID);
        for (int i = 0; i < numKnownTags; i++) {
            if (tagUID == knownTagUIDs[i]) {
                // Match found
                Serial.println("Tag matches ID: " + String(correspondingIDs[i]));
                firestoreDataUpdate("labData/lab-people", "labPeople", "stringValue",increment_by_one);
                break; // Exit the loop when a match is found
            }
        }
            mfrc522.PICC_HaltA();

        }
    }
}