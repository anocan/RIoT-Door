#include "RFID.h"
#include "RiotFirebase.h"
#include "RiotSystem.h"
#include <SPI.h>
#include <MFRC522.h>


MFRC522 mfrc522(SS_PIN, RST_PIN);

const char* knownTagUIDs[] = {
    "ec2ff537",
    "e3f76c19",
    "4c60a25f"
};

const int numKnownTags = sizeof(knownTagUIDs) / sizeof(knownTagUIDs[0]);
const char* correspondingIDs[] = {
    "MASTER KEY",
    "ID1",
    "ZGwWrS4bjrZPXa8V2ddsES2Api33"
};

String compareTagUID(String tagUID,const char* data) {
    const char bookmark = '|';
    String temp = "";
    for(size_t i=0; i < strlen(data); i++) {
        //Serial.println(data[i]);
        if (data[i] != bookmark) {
            temp += data[i];
        } else {
            if (tagUID == temp) {
                //Serial.println("CORRECT:");
                //Serial.println(temp);
                return tagUID;
            } else {
                //Serial.println("WRONG:");
                //Serial.println(temp);
                temp = "";
                continue;
            }
        }
    }
    return "false";
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

        if (SYSTEM == SYS_NORMAL) {
            char riotCardPath[64];
            strcpy(riotCardPath, "riotCards/");
            strcat(riotCardPath, tagUID.c_str());
             FirebaseJson jsonObjectRiotCard = firestoreGetJson(riotCardPath);
             FirebaseJson jsonObjectDoor = firestoreGetJson("labData/lab-data");
             

             String dataDoor = getDataFromJsonObject(jsonObjectDoor,
             "fields/labDoor/stringValue"
             );

             Serial.println(dataDoor);
            if (dataDoor == "locked") {


                String jsonDataRiotCardID = getDataFromJsonObject(jsonObjectRiotCard, 
                "fields/riotCardStatus/stringValue"
                );
                if (jsonDataRiotCardID == "active") {
                    releaseDoor();
                    Serial.println("aaa");
                }

                //const char* serializedCardData = getActiveRiotCardIDs(jsonObjectRiotCard);
                //String riotCardID = compareTagUID(tagUID, serializedCardData).c_str();
                //if (riotCardID != "false") {
                //releaseDoor();
                //uploadAllFirestoreTasks(jsonObjectRiotCard, riotCardID.c_str()); // Takes significant time
                //changeRiotCardStatus();
     
                //Serial.println("FIREBASE CARD CORRECT!");
                //} else {
                //    Serial.println("FIREBASE CARD WRONG!");
                //}

            } else if (dataDoor == "unlocked") {
                releaseDoor();
                Serial.println("RIoT door is already unlocked.");
            } else if (dataDoor == "secured") {
                String userType = getDataFromJsonObject(jsonObjectRiotCard, "fields/userType/stringValue");
                if (userType == "admin" || userType == "superadmin") {
                    releaseDoor();
                    Serial.println("vvvv");
                }
            }


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

