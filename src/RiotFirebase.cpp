#include "RiotFirebase.h"
#include "credentials.h"
#include "RiotSystem.h"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

    /**
     * Initialize the Firebase
     *
     * @return none
     *
     */
int initFirebase() {
    pinMode(FIREBASE_PIN, OUTPUT);
    digitalWrite(FIREBASE_PIN, HIGH);
    config.api_key = API_KEY;
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    Firebase.begin(&config, &auth);
    while (!Firebase.ready()) {
        if (SYSTEM == SYS_NORMAL) {
            Serial.print("-");
            delay(100);
        } else if (SYSTEM == SYS_BACKUP) {
            return -1;
        }
    }
    if (SYSTEM == SYS_NORMAL) {
        digitalWrite(FIREBASE_PIN, LOW);
    }

    return 1;
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
        FirebaseJsonData jsonCardStatus;
        int i =0;
        while (payload.get(jsonData,
        "fields/" + elementName + "/" + elementType + "/values/" + "[" + i + "]" + "/mapValue/fields/riotCardID/stringValue", 
        true) &&
        payload.get(jsonCardStatus,
        "fields/" + elementName + "/" + elementType + "/values/" + "[" + i + "]" + "/mapValue/fields/riotCardStatus/stringValue", 
        true) 
        ) {
            //Serial.println(jsonData.stringValue);
            //Serial.println(jsonCardStatus.stringValue);

            if (jsonCardStatus.stringValue == "active") {
                //Serial.println(jsonData.stringValue);
                activeTagUIDS += jsonData.stringValue + bookmark;
            } else if (jsonCardStatus.stringValue == "disabled") {
                //Serial.println("DISABLED CARD!");
            } else if (jsonCardStatus.stringValue == "inactive") {
                //Serial.println("INACTIVE CARD!");
            } else {
                //Serial.println("UNKNOWN STATUS!");
            }
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

    /**
     * Get FirebaseJson object from FirebaseFirestore.
     *
     * @param documentPath The document path to the Firestore, e.g. "riotCards/riot-cards"
     * @return FirebaseJson data
     *
     */
FirebaseJson firestoreGetJson(String documentPath) {
        FirebaseJson jsonObject;
        if (WiFi.status() == WL_CONNECTED && Firebase.ready()) {
       if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), "")) {
        jsonObject.setJsonData(fbdo.payload().c_str());

        //jsonObject.toString(Serial, true);
        //Serial.println(jsonObject.toString(Serial, true));

      return jsonObject;
    }
    }
    
    return jsonObject;
}

    /**
     * Update FirebaseFirestore using FirebaseJson.
     *
     * @param jsonObject Raw json data before any change.
     * @param documentPath The document path to the Firestore, e.g. "riotCards/riot-cards"
     * @param updateWhere Path to the update field, e.g. "fields/riotCardList/arrayValue/values/[0]/mapValue/fields/inOrOut/stringValue
     * @param updateValue Update value 
     * @return nothing
     *
     */
void firestoreUpdateData(FirebaseJson jsonObject, String documentPath, String updateWhere, String updateValue) {
         if (WiFi.status() == WL_CONNECTED && Firebase.ready()) {
            jsonObject.set(updateWhere, updateValue);
            //jsonObject.toString(Serial, true);
               int firstSlash = updateWhere.indexOf('/');
               if (firstSlash != -1) {
                // Find the position of the second last '/'
                int secondSlash = updateWhere.indexOf('/', firstSlash + 1);
                if (secondSlash != -1) {
                // Extract the substring between the second last '/' and the last '/' to get the update field
                String updateField = updateWhere.substring(firstSlash + 1, secondSlash);
                //Serial.print("Extracted substring: ");
                //Serial.println(extracted);

    if(Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), jsonObject.raw(), updateField)){
            //Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
                }
                }
            
            }           
      return;
    }else{
      Serial.println(fbdo.errorReason());
    }
}

    /**
     * Comperate the compareValue in the compareField, set iteration path if compareField is an array
     *
     * @param documentPath The document path to the Firestore, e.g. "riotCards/riot-cards"
     * @param compareField Field to compared to e.g. "fields/riotCardList/arrayValue/values"
     * @param compareValue Value to be compared e.g. "riotCardID"
     * @param iteration If iteration is needed remain path to compareField should be satisfied in this e.g. "mapValue/fields/riotCardID/stringValue"
     * @param count Count is default set to false, if number of correct matches is needed set this to true
     * @return Function returns the index of the array if iteration is set with count is set to false, 
     * number of correct matches if iteration and count is set, or bool value of the comparison if iteration is unset
     *
     */
String firestoreCompare(String documentPath, String compareField, String compareValue, String iteration = "none", bool count = false) {
    if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), "")) {

        FirebaseJson jsonObject = firestoreGetJson(documentPath);
        FirebaseJsonData jsonData;
        
        if (iteration != "none") {
            if (count != false) {
                int i = 0;
                int count = 0;
                while (jsonObject.get(jsonData,
                compareField + "/[" + i + "]/" + iteration, 
                true)) {
                if (compareValue == jsonData.stringValue) {
                    count++;
                }
                i++;
                }
                return String(count);
            } else {
                int i = 0;
                while (jsonObject.get(jsonData,
                compareField + "/[" + i + "]/" + iteration, 
                true)) {
                    if (compareValue == jsonData.stringValue) {
                        return String(i);
                    }
                    i++;
                }
            }     
        } else {
            jsonObject.get(jsonData,
            compareField,
            true
            );
            if (compareValue == jsonData.stringValue) {
                return "true";
            }
        }
    return "false";
    }
    return "null";
}

    /**
     * Comperate the compareValue in the compareField, set iteration path if compareField is an array
     *
     * @param jsonObject The raw json data to be processed
     * @param fieldPath Path to the Firestore element to be returned
     * @return Value corresponding to the field
     *
     */
String getDataFromJsonObject(FirebaseJson jsonObject, String fieldPath) {
    FirebaseJsonData jsonData;

    jsonObject.get(jsonData, fieldPath, true);
    
    return jsonData.stringValue;
}