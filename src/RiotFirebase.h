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
FirebaseJson firestoreGetJson(const char* documentPath);
void firestoreUpdateData(FirebaseJson jsonObject, const char* documentPath, const char* updateWhere, const char* updateValue);
const char* firestoreCompare(FirebaseJson jsonObject, const char* compareField, const char* compareValue, const char* iteration, bool count);
String getDataFromJsonObject(FirebaseJson jsonObject, const char* fieldPath);
void changeRiotCardStatus();
void resetInOrOutStatus();
void uploadAllFirestoreTasks(FirebaseJson jsonObjectRiotCard, const char* riotCardID);
void testPtr();

#endif