#include "RFID.h"
#include "RiotFirebase.h"
#include "RiotSystem.h"
#include <SPI.h>
#include <MFRC522.h>


MFRC522 mfrc522(SS_PIN, RST_PIN);

const char* knownTagUIDs[] = {
    "ec2ff537",
    "e3f76c19",
};

const int numKnownTags = sizeof(knownTagUIDs) / sizeof(knownTagUIDs[0]);
const char* correspondingIDs[] = {
    "MASTER KEY",
    "ID1"
};

bool compareTagUID(String tagUID,const char* data) {
    const char bookmark = 'X';
    String temp = "";
    for(size_t i=0; i < strlen(data); i++) {
        //Serial.println(data[i]);
        if (data[i] != bookmark) {
            temp += data[i];
        } else {
            if (tagUID == temp) {
                //Serial.println("CORRECT:");
                //Serial.println(temp);
                return true;
            } else {
                //Serial.println("WRONG:");
                //Serial.println(temp);
                temp = "";
                continue;
            }
        }
    }
    return false;
};

void initRFID() {
    if (SYSTEM == SYS_NORMAL) {
        pinMode(READY_PIN, OUTPUT);
    }
    SPI.begin();
    mfrc522.PCD_Init();
    if (SYSTEM == SYS_NORMAL) {
        digitalWrite(READY_PIN, HIGH);
    }

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
        if (SYSTEM == SYS_NORMAL) {
            const char* serializedCardData = firestoreGetCardData("riotCards/active-cards", "activeCards", "arrayValue");
            if (compareTagUID(tagUID, serializedCardData )) {
                firestoreDataUpdate("labData/lab-people", "labPeople", "stringValue",increment_by_one);
                Serial.println("FIREBASE CARD CORRECT!");
            } else {
                Serial.println("FIREBASE CARD WRONG!");
            }
            delete[] serializedCardData;

        } else if (SYSTEM == SYS_BACKUP) {
    
            for (int i = 0; i < numKnownTags; i++) {
                if (tagUID == knownTagUIDs[i]) {
                    // Match found

                    /// UNLOCK THE DOOR
                   Serial.println("CARD READ VIA BACKUP");
                    break; // Exit the loop when a match is found
                }
            }
    
        }

            mfrc522.PICC_HaltA();

        }
    }
}

