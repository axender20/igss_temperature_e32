#ifndef TEMP_MONITOR_CONFIG_H
#define TEMP_MONITOR_CONFIG_H

#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

class TempMonitorConfig {
private:
    static const char* JSON_CONFIG_FILE;
    static const size_t MAX_FIELD_LENGTH_W_NULL = 51;
    static const size_t MAX_FIELD_LENGTH = MAX_FIELD_LENGTH_W_NULL - 1;
    WiFiManager wm;
    bool shouldSaveConfig;
    
    // Campos de configuración
    int fieldFrecMuestreo;
    float fieldUmbMax;
    float fieldUmbMin;
    char fieldEmailSender[MAX_FIELD_LENGTH_W_NULL];
    char fieldPassEmailSender[MAX_FIELD_LENGTH_W_NULL];
    char fieldEmailRec1[MAX_FIELD_LENGTH_W_NULL];
    char fieldEmailRec2[MAX_FIELD_LENGTH_W_NULL];
    char fieldEmailRec3[MAX_FIELD_LENGTH_W_NULL];
    char fieldEmailRec4[MAX_FIELD_LENGTH_W_NULL];
    char fieldEmailRec5[MAX_FIELD_LENGTH_W_NULL];
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
    bool getAlertsActive() const { return fieldAlertsActive; }
    float getUmbMax() const { return fieldUmbMax; }
    float getUmbMin() const { return fieldUmbMin; }
    const char* getEmailSender() const { return fieldEmailSender; }
    const char* getPassEmailSender() const { return fieldPassEmailSender; }
    const char* getEmailRec1() const { return fieldEmailRec1; }
    const char* getEmailRec2() const { return fieldEmailRec2; }
    const char* getEmailRec3() const { return fieldEmailRec3; }
    const char* getEmailRec4() const { return fieldEmailRec4; }
    const char* getEmailRec5() const { return fieldEmailRec5; }
};

#endif