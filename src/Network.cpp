#include "Network.h"
#include "credentials.h"
#include <ESP8266WiFi.h>

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

void initWiFi() {
    pinMode(NETWORK_PIN, OUTPUT);
    digitalWrite(NETWORK_PIN,HIGH);
    WiFi.disconnect();
    WiFi.mode(WIFI_STA); // Optional
    WiFi.begin(ssid, password);
    Serial.println("\nConnecting");

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    
    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());
    digitalWrite(NETWORK_PIN,LOW);
}
