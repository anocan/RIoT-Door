#ifndef RIOTFIREBASE_H_
#define RIOTFIREBASE_H_

#include <Firebase_ESP_Client.h>

#define FIREBASE_PIN D1

extern FirebaseData fbdo;
extern FirebaseAuth auth;
extern FirebaseConfig config;

extern const u_int8_t FIREBASE_PIN;

/// LEGACY
/// DO NOT USE IT UNLESS NECESSARY
const char* getActiveRiotCardIDs(FirebaseJson jsonObject);
const char* firestoreCompare(FirebaseJson jsonObject, const char* compareField, const char* compareValue, const char* iteration, bool count);
void resetInOrOutStatus();
/// @brief

int initFirebase();
FirebaseJson firestoreGetJson(const char* documentPath);
void firestoreUpdateField(FirebaseJson jsonObject, const char* documentPath, const char* updateWhere, const char* updateValue);
String getDataFromJsonObject(FirebaseJson jsonObject, const char* fieldPath);
void changeRiotCardStatus();
String firebaseJsonIterator(FirebaseJson jsonObject, String pathToArray, const char* updateField,const char* funcValue, void (*func) (const char*, const char*, int*));
void uploadAllFirestoreTasks(FirebaseJson jsonObjectRiotCard, const char* riotCardID);
void testPtr();

#endif