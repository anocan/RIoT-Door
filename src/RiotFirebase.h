#ifndef RIOTFIREBASE_H_
#define RIOTFIREBASE_H_

#include <Firebase_ESP_Client.h>

#define FIREBASE_PIN D1

extern FirebaseData fbdo;
extern FirebaseAuth auth;
extern FirebaseConfig config;

extern const u_int8_t FIREBASE_PIN;


int initFirebase();
const char* getActiveRiotCardIDs(FirebaseJson jsonObject);
FirebaseJson firestoreGetJson(String documentPath);
void firestoreUpdateData(FirebaseJson jsonObject, String documentPath, String updateWhere, String updateValue);
String firestoreCompare(FirebaseJson jsonObject, String compareField, String compareValue, String iteration, bool count);
String getDataFromJsonObject(FirebaseJson jsonObject, String fieldPath);
void changeRiotCardStatus();
void uploadAllFirestoreTasks(FirebaseJson jsonObjectRiotCard, String riotCardID);

#endif