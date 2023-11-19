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

    /**
     * Get active RIoT card ids.
     *
     * @param jsonObject Json object to get the active RIoT card id
     * @return nothing
     *
     */
const char* getActiveRiotCardIDs(FirebaseJson jsonObject) {
    String activeTagUIDS = "";
    const char bookmark = '|';

    FirebaseJsonData iterator;
        
    int i =0;
    while(jsonObject.get(iterator,
    "fields/riotCardList/arrayValue/values/[" + String(i) + "]/mapValue/fields/id/stringValue", 
    true)){
        String riotCardStatusBool = firestoreCompare(
        jsonObject,
        "fields/riotCardList/arrayValue/values/[" + String(i) + "]/mapValue/fields/riotCardStatus/stringValue",
        "active",
        "none",
        false
        );
        //Serial.println(riotCardStatusBool);
        if (riotCardStatusBool == "true") {
            String riotCardID = getDataFromJsonObject(
            jsonObject, 
            "fields/riotCardList/arrayValue/values/[" + String(i)  + "]/mapValue/fields/riotCardID/stringValue"
            );
            activeTagUIDS += riotCardID + bookmark;
        }
        i++;
    }

    char* activeTagUIDs = new char[activeTagUIDS.length() + 1];
    strcpy(activeTagUIDs, activeTagUIDS.c_str());
    //Serial.println(activeTagUIDs);
    return activeTagUIDs;
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
     * @param updateWhere Path to the update field, e.g. "fields/riotCardList/arrayValue/values/[0]/mapValue/fields/inOrOut/stringValue"
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
                //Serial.println(updateField);
               


    if(Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), jsonObject.raw(), updateField)){
            //Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
                } else {
                    Serial.println("Error Patching Document!");
                }
                }
            
        }           
    }else{
      Serial.println(fbdo.errorReason());
    }
}

    /**
     * Comperate the compareValue in the compareField, set iteration path if compareField is an array
     *
     * @param jsonObject Json data to compare
     * @param compareField Field to compared to e.g. "fields/riotCardList/arrayValue/values"
     * @param compareValue Value to be compared e.g. "riotCardID"
     * @param iteration If iteration is needed remain path to compareField should be satisfied in this e.g. "mapValue/fields/riotCardID/stringValue"
     * @param count Count is default set to false, if number of correct matches is needed set this to true
     * @return Function returns the index of the array if iteration is set with count is set to false, 
     * number of correct matches if iteration and count is set, or bool value of the comparison if iteration is unset
     *
     */
String firestoreCompare(FirebaseJson jsonObject, String compareField, String compareValue, String iteration = "none", bool count = false) {
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

    /**
     * Upload and complete all FirebaseFirestore related tasks
     *
     * @param jsonObjectRiotCard RIoT card's json object
     * @param riotCardID RIoT card id
     * @return none
     *
     */
void uploadAllFirestoreTasks(FirebaseJson jsonObjectRiotCard, String riotCardID) {
    FirebaseJson jsonObjectLabData = firestoreGetJson("labData/lab-data");
                String riotCardListIndex = firestoreCompare(
                jsonObjectRiotCard,
                "fields/riotCardList/arrayValue/values",
                riotCardID,
                "mapValue/fields/riotCardID/stringValue",
                false
                );

                firestoreUpdateData(
                jsonObjectRiotCard, 
                "riotCards/riot-cards", 
                "fields/riotCardList/arrayValue/values/[" + riotCardListIndex + "]/mapValue/fields/inOrOut/stringValue",
                "in"
                );

                String userID = getDataFromJsonObject(
                jsonObjectRiotCard,
                "fields/riotCardList/arrayValue/values/[" + riotCardListIndex + "]/mapValue/fields/id/stringValue"
                );
                FirebaseJson jsonObjectUser = firestoreGetJson("users/" + userID); 

                firestoreUpdateData(
                jsonObjectUser,
                "users/" + userID,  
                "fields/riotCard/mapValue/fields/inOrOut/stringValue",
                "in"
                );

                FirebaseJson jsonObjectRiotCardUpdated = firestoreGetJson("riotCards/riot-cards");
                String noPeopleInTheLab = firestoreCompare(
                jsonObjectRiotCardUpdated,
                "fields/riotCardList/arrayValue/values",
                "in",
                "mapValue/fields/inOrOut/stringValue",
                true
                );

                firestoreUpdateData(
                jsonObjectLabData, 
                "labData/lab-data", 
                "fields/labPeople/stringValue",
                noPeopleInTheLab
                );
}

    /**
     * Updates card status when user account is deleted
     *
     * @return none
     *
     */
void changeRiotCardStatus() {
    // Allocate memory for FirebaseJson objects on the heap
    FirebaseJson* jsonObjectUsersPtr = new FirebaseJson();
    FirebaseJson* jsonObjectRiotCardsPtr = new FirebaseJson();

    if (jsonObjectUsersPtr == nullptr || jsonObjectRiotCardsPtr == nullptr) {
        Serial.println("Memory allocation failed");
        // Handle the failure case accordingly
        return;
    }

    String path = "users/";

    // Populate jsonObjectUsers using Firebase
    Firebase.Firestore.listDocuments(&fbdo, FIREBASE_PROJECT_ID, "", path.c_str(), 100, "", "", "", false);
    jsonObjectUsersPtr->setJsonData(fbdo.payload().c_str());

    int i = 0;
    FirebaseJsonData jsonData;

    while (jsonObjectUsersPtr->get(jsonData, "documents/[" + String(i) + "]/fields/userType/stringValue")) {
        if (jsonData.stringValue == "deleted") {
            String uID = getDataFromJsonObject(*jsonObjectUsersPtr, "documents/[" + String(i) + "]/fields/id/stringValue");
            *jsonObjectRiotCardsPtr = firestoreGetJson("riotCards/riot-cards");
            String index = firestoreCompare(*jsonObjectRiotCardsPtr,                
                                             "fields/riotCardList/arrayValue/values",
                                             uID,
                                             "mapValue/fields/id/stringValue",
                                             false);

            jsonObjectRiotCardsPtr->set("fields/riotCardList/arrayValue/values/[" + String(index) + "]/mapValue/fields/riotCardStatus/stringValue","inactive");
            jsonObjectRiotCardsPtr->set("fields/riotCardList/arrayValue/values/[" + String(index) + "]/mapValue/fields/inOrOut/stringValue","out");
            
            //Serial.println(jsonObjectRiotCardsPtr->toString(Serial, true));
            
            Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", "riotCards/riot-cards", jsonObjectRiotCardsPtr->raw(), "riotCardList");

            //Serial.println("b");
        }
        i++;
    }

    // Clean up dynamically allocated memory
    delete jsonObjectUsersPtr;
    delete jsonObjectRiotCardsPtr;
}

    /**
     * Resets inOrOut status of all instances in the riotCards
     *
     * @return none
     *
     */
void resetInOrOutStatus() {
    String documentPath = "riotCards/riot-cards";
    FirebaseJson jsonRiotCards = firestoreGetJson(documentPath);
    FirebaseJson jsonObjectLabData = firestoreGetJson("labData/lab-data");
    FirebaseJsonData jsonData;

    int i = 0;
    while (jsonRiotCards.get(jsonData,
        "fields/riotCardList/arrayValue/values/[" + String(i) + "]/mapValue/fields/inOrOut/stringValue", 
        true)) {
        jsonRiotCards.set("fields/riotCardList/arrayValue/values/[" + String(i) + "]/mapValue/fields/inOrOut/stringValue", 
        "out"
        );
    i++;
    }
    if(Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), jsonRiotCards.raw(), "riotCardList")){
    //Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
    } else {
        Serial.println("Error Patching Document!");
    }

    String noPeopleInTheLab = firestoreCompare(
    jsonRiotCards,
    "fields/riotCardList/arrayValue/values",
    "in",
    "mapValue/fields/inOrOut/stringValue",
    true
    );

    firestoreUpdateData(
    jsonObjectLabData, 
    "labData/lab-data", 
    "fields/labPeople/stringValue",
    noPeopleInTheLab
    );

    int j = 0;
    while (jsonRiotCards.get(jsonData,
        "fields/riotCardList/arrayValue/values/[" + String(j) + "]/mapValue/fields/id/stringValue", 
        true)) {
        String userID = jsonData.stringValue;
        FirebaseJson jsonObjectUser = firestoreGetJson("users/" + userID); 

        firestoreUpdateData(
        jsonObjectUser,
        "users/" + userID,  
        "fields/riotCard/mapValue/fields/inOrOut/stringValue",
        "out"
        );
    j++;
    }

}