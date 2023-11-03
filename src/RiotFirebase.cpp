#include "RiotFirebase.h"
#include "credentials.h"
#include <vector>

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void initFirebase() {
    pinMode(FIREBASE_PIN, OUTPUT);
    digitalWrite(FIREBASE_PIN, HIGH);
    config.api_key = API_KEY;
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    Firebase.begin(&config, &auth);
    digitalWrite(FIREBASE_PIN, LOW);
}

const char* firestoreGetCardData(String documentPath, String elementName, String elementType) {
    // Connect to Firebase
    if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), "")) {
        String activeTagUIDS = "";
        const char bookmark = 'X';

        FirebaseJson payload;
        payload.setJsonData(fbdo.payload().c_str());
        //Serial.println(fbdo.payload());

        FirebaseJsonData jsonData;
        int i =0;
        while (payload.get(jsonData,"fields/" + elementName + "/" + elementType + "/values/" + "[" + i + "]" + "/stringValue", true)) {
            //Serial.println(jsonData.stringValue);
            activeTagUIDS += jsonData.stringValue + bookmark;
            i++;
        }
        char* activeTagUIDs = new char[activeTagUIDS.length() + 1];
        strcpy(activeTagUIDs, activeTagUIDS.c_str());
        //Serial.println(activeTagUIDs);
        return activeTagUIDs;
    } else {
        Serial.println("Error reading Firestore document");
        Serial.println(fbdo.errorReason());
        return nullptr;
    }
}

String firestoreGetData(String documentPath, String elementName, String elementType) {
    // Connect to Firebase
    if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), "")) {
        FirebaseJson payload;
        payload.setJsonData(fbdo.payload().c_str());
        //Serial.println(fbdo.payload());

        FirebaseJsonData jsonData;
        payload.get(jsonData,"fields/" + elementName + "/" + elementType, true);
        //Serial.println(jsonData.intValue);
        
        return jsonData.stringValue;
    } else {
        Serial.println("Error reading Firestore document");
        Serial.println(fbdo.errorReason());
    }
    return "null";
}

void firestoreDataUpdate(String documentPath, String elementName, String elementType, UpdateModes mode) {
    if (WiFi.status() == WL_CONNECTED && Firebase.ready()) {
        FirebaseJson content;
  
        switch (mode) {
            case increment_by_one:
                int currentValue = firestoreGetData(documentPath, elementName, elementType).toInt();
                //Serial.println(currentValue);
                currentValue++;
                content.set("fields/" + elementName + "/" + elementType, String(currentValue));
                break;
            case decrease_by_one:
                int currentValue = firestoreGetData(documentPath, elementName, elementType).toInt();
                //Serial.println(currentValue);
                currentValue--;
                content.set("fields/" + elementName + "/" + elementType, String(currentValue));
                break;           
        }

            if(Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "labPeople")){
      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
      return;
    }else{
      Serial.println(fbdo.errorReason());
    }

        if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw())) {
            Serial.printf("OK\n%s\n\n", fbdo.payload().c_str());
            return;
        } else {
            Serial.println(fbdo.errorReason());
        }
    }
}
