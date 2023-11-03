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
};

void initFirebase();
const char* firestoreGetCardData(String documentPath, String elementName, String elementType);
String firestoreGetData(String documentPath, String elementName, String elementType);
void firestoreDataUpdate(String documentPath, String elementName, String elementType, UpdateModes mode);

#endif