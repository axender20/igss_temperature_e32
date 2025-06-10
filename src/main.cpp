#include <Arduino.h>
#include "TempMonitorConfig.h"

TempMonitorConfig config;

void setup() {
    Serial.begin(115200);
    
    if (!config.begin()) {
        Serial.println("Failed to configure WiFi");
        delay(3000);
        ESP.restart();
    }

    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void loop() {
}