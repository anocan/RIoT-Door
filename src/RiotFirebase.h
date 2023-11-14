#ifndef RIOTFIREBASE_H_
#define RIOTFIREBASE_H_

#include <Firebase_ESP_Client.h>

#define FIREBASE_PIN D1

extern FirebaseData fbdo;
extern FirebaseAuth auth;
extern FirebaseConfig config;

extern const u_int8_t FIREBASE_PIN;

enum UpdateModes {
    increment_by_one,
    decrease_by_one,
    update_inOrOutToLabData,
};


const char* firestoreGetCardData(String documentPath, String elementName, String elementType);

int initFirebase();
FirebaseJson firestoreGetJson(String documentPath);
void firestoreUpdateData(FirebaseJson jsonObject, String documentPath, String updateWhere, String updateValue);
String firestoreCompare(String documentPath, String compareField, String compareValue, String iteration, bool count);
String getDataFromJsonObject(FirebaseJson jsonObject, String fieldPath);

#endif