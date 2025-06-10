#include "TempMonitorConfig.h"

const char *TempMonitorConfig::JSON_CONFIG_FILE = "/mConf.json";

TempMonitorConfig::TempMonitorConfig() : fieldAlertsActive(false),
                                         fieldFrecMuestreo(16),
                                         fieldUmbMax(0),
                                         fieldUmbMin(0)
{
  strcpy(fieldEmailSender, "user@example.com");
  strcpy(fieldPassEmailSender, "***");
  strcpy(fieldEmailRec1, "user@example.com");
  shouldSaveConfig = false;
}

void TempMonitorConfig::saveConfigFile()
{
  Serial.print(F("Saving configuration..."));

  JsonDocument json;
  json["frecMuestreo"] = fieldFrecMuestreo;
  json["umbMax"] = fieldUmbMax;
  json["umbMin"] = fieldUmbMin;
  json["emailSender"] = fieldEmailSender;
  json["passEmailSender"] = fieldPassEmailSender;
  json["emailRec1"] = fieldEmailRec1;
  json["alertsActive"] = fieldAlertsActive;

  File configFile = SPIFFS.open(JSON_CONFIG_FILE, "w");
  if (!configFile)
  {
    Serial.println("Failed to open config file for writing");
    return;
  }

  serializeJsonPretty(json, Serial);
  if (serializeJson(json, configFile) == 0)
  {
    Serial.println(F("Failed to write to file"));
  }
  configFile.close();
}

bool TempMonitorConfig::loadConfigFile()
{
  // uncomment if we need to format filesystem
  // SPIFFS.format();
  if (!SPIFFS.begin(false) && !SPIFFS.begin(true))
  {
    Serial.println("Failed to mount FS");
    return false;
  }

  if (!SPIFFS.exists(JSON_CONFIG_FILE))
  {
    Serial.println("Config file not found");
    return false;
  }

  File configFile = SPIFFS.open(JSON_CONFIG_FILE, "r");
  if (!configFile)
  {
    Serial.println("Failed to open config file");
    return false;
  }

  JsonDocument json;
  DeserializationError error = deserializeJson(json, configFile);
  configFile.close();

  if (error)
  {
    Serial.println("Failed to parse JSON");
    return false;
  }

  strcpy(fieldEmailSender, json["emailSender"] | "user@example.com");
  strcpy(fieldPassEmailSender, json["passEmailSender"] | "***");
  strcpy(fieldEmailRec1, json["emailRec1"] | "user@example.com");
  fieldFrecMuestreo = json["frecMuestreo"].as<int>() | 15;
  fieldUmbMax = json["umbMax"].as<float>();
  fieldUmbMin = json["umbMin"].as<float>();
  fieldAlertsActive = json["alertsActive"].as<bool>();

  return true;
}

void TempMonitorConfig::configModeCallback(WiFiManager *myWiFiManager)
{
  Serial.println("Entered configuration mode");
  Serial.print("Config SSID: ");
  Serial.println(myWiFiManager->getConfigPortalSSID());
  Serial.print("Config IP Address: ");
  Serial.println(WiFi.softAPIP());
}

bool TempMonitorConfig::begin(bool forceConfig)
{
  WiFi.mode(WIFI_STA);
  forceConfig = true; // debug only

  if (!loadConfigFile())
  {
    Serial.println(F("Forcing config mode as there is no saved config"));
    forceConfig = true;
  }

  // wm.resetSettings(); // debug only

  wm.setSaveConfigCallback([this]()
                           {
        Serial.println("Should save config");
        this->shouldSaveConfig=true; });

  wm.setAPCallback(configModeCallback);

  // frecuencia de muestreo
  char frecConvertedValue[6];
  sprintf(frecConvertedValue, "%d", fieldFrecMuestreo);
  WiFiManagerParameter frec_muestreo_text_box(
      "id_frec_muestreo",
      "Frecuencia de muestreo (seg)",
      frecConvertedValue, 7, "type='number' min=15 max=14400 required");

  frec_muestreo_text_box.setValidation([](const char *value)
                                       {
                                          int freq = String(value).toInt();
                                          Serial.print("validando freq: ");
                                          Serial.println(freq);
                                          if (isnan(freq) || freq < 1 || freq > 14400) {
                                            return String("Debe estar entre 1 y 14400 seg.");
                                          }
                                          return String(""); });

  // Check Box
  const char *chkAlertActiveHtml;
  if (fieldAlertsActive)
  {
    chkAlertActiveHtml = "type=\"checkbox\" checked";
  }
  else
  {
    chkAlertActiveHtml = "type=\"checkbox\"";
  }
  WiFiManagerParameter alert_active_checkbox("id_alert_active", "Alertas activadas", "C", 2, chkAlertActiveHtml);

  WiFiManagerParameter email_sender_text_box(
      "id_email_sender", "Correo electronico de origen",
      fieldEmailSender, 50, "type='email' required");

  email_sender_text_box.setValidation([](const char *value)
                                      {
                                        String email = String(value);
                                        Serial.print("validando email: ");
                                        Serial.println(email);
                                        if (email.indexOf('@') == -1 || email.indexOf('.') == -1)
                                        {
                                          return String("Formato de email inválido");
                                        }
                                        return String(""); // validación exitosa
                                      });

  char umbMaxConvertedValue[7];
  sprintf(umbMaxConvertedValue, "%.2f", fieldUmbMax);
  WiFiManagerParameter umb_max_text_box(
      "id_umb_max",
      "Alerta umbral max. (°C)",
      umbMaxConvertedValue, 8, "type='number' step=0.01 min=-40 max=125 required");

  umb_max_text_box.setValidation([](const char *value)
                                 {
                                            int umb = String(value).toFloat();
                                            Serial.print("validando umbral maximo: ");
                                            Serial.println(umb);
                                            if (isnan(umb) || umb < -40 || umb > 125) {
                                                return String("Debe estar entre -40 y 125 °C");
                                            }
                                            return String(""); });

  char umbMinConvertedValue[7];
  sprintf(umbMinConvertedValue, "%.2f", fieldUmbMin);
  Serial.print("umbminconvert");
  Serial.println(umbMinConvertedValue);
  WiFiManagerParameter umb_min_text_box(
      "id_umb_min",
      "Alerta umbral min. (°C)",
      umbMinConvertedValue, 8, "type='number' step=0.01 min=-40 max=125 required");

  umb_min_text_box.setValidation([](const char *value)
                                 {
                                            int umb = String(value).toFloat();
                                            Serial.print("validando umbral minimo: ");
                                            Serial.println(umb);
                                            if (isnan(umb) || umb < -40 || umb > 125) {
                                                return String("Debe estar entre -40 y 125 °C");
                                            }
                                            return String(""); });

  wm.addParameter(&frec_muestreo_text_box);
  wm.addParameter(&alert_active_checkbox);
  wm.addParameter(&umb_max_text_box);
  wm.addParameter(&umb_min_text_box);
  wm.addParameter(&email_sender_text_box);
  wm.setTitle("Configuración Monitor de Temperatura");

  bool connected;
  if (forceConfig)
  {
    connected = wm.startConfigPortal("SG-TEMP-4528");
  }
  else
  {
    connected = wm.autoConnect("SG-TEMP-4528");
  }

  if (!connected)
  {
    Serial.println("Failed to connect");
    return false;
  }

  // Procesar los valores ingresados
  fieldAlertsActive = (strncmp(alert_active_checkbox.getValue(), "C", 1) == 0);
  strncpy(fieldEmailSender, email_sender_text_box.getValue(), sizeof(fieldEmailSender));
  fieldFrecMuestreo = atoi(frec_muestreo_text_box.getValue());
  fieldUmbMax = atoff(umb_max_text_box.getValue());
  fieldUmbMin = atoff(umb_min_text_box.getValue());

  if (shouldSaveConfig)
  {
    saveConfigFile();
  }

  return true;
}