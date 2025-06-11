#ifndef TEMP_MONITOR_CONFIG_H
#define TEMP_MONITOR_CONFIG_H

#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

class TempMonitorConfig {
private:
    static const char* JSON_CONFIG_FILE;
    WiFiManager wm;
    bool shouldSaveConfig;
    
    // Campos de configuración
    int fieldFrecMuestreo;
    float fieldUmbMax;
    float fieldUmbMin;
    char fieldEmailSender[50];
    char fieldPassEmailSender[50];
    char fieldEmailRec1[50];
    char fieldEmailRec2[50];
    char fieldEmailRec3[50];
    char fieldEmailRec4[50];
    char fieldEmailRec5[50];
    bool fieldAlertsActive;

    void saveConfigFile();
    bool loadConfigFile();
    static void configModeCallback(WiFiManager* myWiFiManager);

    static String validateEmail(const String& email);
    
public:
    TempMonitorConfig();
    bool begin(bool forceConfig = false);
    
    // Getters
    int getFrecMuestreo() const { return fieldFrecMuestreo; }
    float getUmbMax() const { return fieldUmbMax; }
    float getUmbMin() const { return fieldUmbMin; }
    const char* getEmailSender() const { return fieldEmailSender; }
    const char* getPassEmailSender() const { return fieldPassEmailSender; }
    const char* getEmailRec1() const { return fieldEmailRec1; }
};

#endif