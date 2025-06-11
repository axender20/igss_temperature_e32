#include "TempMonitorConfig.h"

const char *TempMonitorConfig::JSON_CONFIG_FILE = "/mConf.json";

String TempMonitorConfig::validateEmail(const String &email)
{
  if (email.length() == 0)
  {
    return String("");
  }

  if (email.indexOf('@') == -1)
  {
    return String("Falta el símbolo @");
  }

  if (email.indexOf('.') == -1)
  {
    return String("Falta el punto en el dominio");
  }

  if (email.indexOf('@') > email.lastIndexOf('.'))
  {
    return String("Formato de dominio inválido");
  }

  if (email.indexOf('@') == 0)
  {
    return String("El email debe tener un nombre de usuario");
  }

  return String("");
}

TempMonitorConfig::TempMonitorConfig() : fieldAlertsActive(false),
                                         fieldFrecMuestreo(15),
                                         fieldUmbMax(0),
                                         fieldUmbMin(0)
{
  strcpy(fieldEmailSender, "user@example.com");
  strcpy(fieldPassEmailSender, "*****");
  strcpy(fieldEmailRec1, "user@example.com");
  strcpy(fieldEmailRec2, "");
  strcpy(fieldEmailRec3, "");
  strcpy(fieldEmailRec4, "");
  strcpy(fieldEmailRec5, "");
  shouldSaveConfig = false;
}

void TempMonitorConfig::saveConfigFile()
{
  Serial.print(F("Saving configuration..."));

  JsonDocument json;
  json["frecMuestreo"] = fieldFrecMuestreo;
  json["alertsActive"] = fieldAlertsActive;
  if (fieldAlertsActive)
  {
    json["umbMax"] = fieldUmbMax;
    json["umbMin"] = fieldUmbMin;
    json["emailSender"] = fieldEmailSender;
    json["passEmailSender"] = fieldPassEmailSender;
    json["emailRec1"] = fieldEmailRec1;
    json["emailRec2"] = fieldEmailRec2;
    json["emailRec3"] = fieldEmailRec3;
    json["emailRec4"] = fieldEmailRec4;
    json["emailRec5"] = fieldEmailRec5;
  }

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

  fieldFrecMuestreo = json["frecMuestreo"].as<int>();
  if (isnan(fieldFrecMuestreo))
    fieldFrecMuestreo = 15;
  fieldAlertsActive = json["alertsActive"].as<bool>() | false;
  if (fieldAlertsActive)
  {
    fieldUmbMax = json["umbMax"].as<float>();
    if (isnan(fieldUmbMax))
      fieldUmbMax = 0.0f;
    fieldUmbMin = json["umbMin"].as<float>();
    if (isnan(fieldUmbMin))
      fieldUmbMin = 0.0f;
    strcpy(fieldEmailSender, json["emailSender"] | "user@example.com");
    strcpy(fieldPassEmailSender, json["passEmailSender"] | "*****");
    strcpy(fieldEmailRec1, json["emailRec1"] | "user@example.com");
    strcpy(fieldEmailRec2, json["emailRec2"] | "");
    strcpy(fieldEmailRec3, json["emailRec3"] | "");
    strcpy(fieldEmailRec4, json["emailRec4"] | "");
    strcpy(fieldEmailRec5, json["emailRec5"] | "");
  }
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
                           { this->shouldSaveConfig = true; });

  wm.setAPCallback(configModeCallback);

  WiFiManagerParameter sep_br("<br>");
  WiFiManagerParameter sep_hr("<hr>");

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
                                          if (isnan(freq) || freq < 1 || freq > 14400) {
                                            return String("Debe estar entre 1 y 14400 seg.");
                                          }
                                          return String(""); });

  // check box alert
  const char *chkAlertActiveHtml;
  if (fieldAlertsActive)
  {
    chkAlertActiveHtml = "type=\"checkbox\" checked";
  }
  else
  {
    chkAlertActiveHtml = "type=\"checkbox\"";
  }
  WiFiManagerParameter alert_active_checkbox("id_alert_active", "Alertas activadas", "C", 2, chkAlertActiveHtml, 2);
  // WiFiManagerParameter a("id1", "label", "defVal", 50, "customhtml", labelPlacement);

  char umbMaxConvertedValue[7];
  sprintf(umbMaxConvertedValue, "%.2f", fieldUmbMax);
  WiFiManagerParameter umb_max_text_box(
      "id_umb_max",
      "Alerta umbral max. (°C)",
      umbMaxConvertedValue, 8, "type='number' step=0.01 min=-40 max=125 required");

  umb_max_text_box.setValidation([](const char *value)
                                 {
                                            float umb = String(value).toFloat();
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
                                            float umb = String(value).toFloat();
                                            if (isnan(umb) || umb < -40 || umb > 125) {
                                                return String("Debe estar entre -40 y 125 °C");
                                            }
                                            return String(""); });

  WiFiManagerParameter email_sender_text_box(
      "id_email_sender", "Email de origen",
      fieldEmailSender, 50, "type='email'");

  email_sender_text_box.setValidation([](const char *value)
                                      { return validateEmail(String(value)); });

  WiFiManagerParameter pass_email_sender_text_box(
      "id_pass_email_sender", "Contraseña email de origen",
      fieldPassEmailSender, 50);

  WiFiManagerParameter email_rec1_text_box(
      "id_email_rec1", "Email de destino 1",
      fieldEmailRec1, 50, "type='email'");

  email_rec1_text_box.setValidation([](const char *value)
                                    { return validateEmail(String(value)); });

  WiFiManagerParameter email_rec2_text_box(
      "id_email_rec2", "Email de destino 2",
      fieldEmailRec2, 50, "type='email'");

  email_rec2_text_box.setValidation([](const char *value)
                                    { return validateEmail(String(value)); });

  WiFiManagerParameter email_rec3_text_box(
      "id_email_rec3", "Email de destino 3",
      fieldEmailRec3, 50, "type='email'");

  email_rec3_text_box.setValidation([](const char *value)
                                    { return validateEmail(String(value)); });

  WiFiManagerParameter email_rec4_text_box(
      "id_email_rec4", "Email de destino 4",
      fieldEmailRec4, 50, "type='email'");

  email_rec4_text_box.setValidation([](const char *value)
                                    { return validateEmail(String(value)); });

  WiFiManagerParameter email_rec5_text_box(
      "id_email_rec5", "Email de destino 5",
      fieldEmailRec5, 50, "type='email'");

  email_rec5_text_box.setValidation([](const char *value)
                                    { return validateEmail(String(value)); });

  wm.addParameter(&frec_muestreo_text_box);
  wm.addParameter(&sep_hr);
  wm.addParameter(&alert_active_checkbox);
  wm.addParameter(&sep_br);
  wm.addParameter(&sep_br);
  wm.addParameter(&umb_max_text_box);
  wm.addParameter(&umb_min_text_box);
  wm.addParameter(&email_sender_text_box);
  wm.addParameter(&pass_email_sender_text_box);
  wm.addParameter(&sep_br);
  wm.addParameter(&email_rec1_text_box);
  wm.addParameter(&email_rec2_text_box);
  wm.addParameter(&email_rec3_text_box);
  wm.addParameter(&email_rec4_text_box);
  wm.addParameter(&email_rec5_text_box);
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
  fieldFrecMuestreo = atoi(frec_muestreo_text_box.getValue());
  fieldAlertsActive = (strncmp(alert_active_checkbox.getValue(), "C", 1) == 0);
  if (fieldAlertsActive)
  {
    fieldUmbMax = atoff(umb_max_text_box.getValue());
    fieldUmbMin = atoff(umb_min_text_box.getValue());
    strncpy(fieldEmailSender, email_sender_text_box.getValue(), sizeof(fieldEmailSender));
    strncpy(fieldPassEmailSender, pass_email_sender_text_box.getValue(), sizeof(fieldPassEmailSender));
    strncpy(fieldEmailRec1, email_rec1_text_box.getValue(), sizeof(fieldEmailRec1));
    strncpy(fieldEmailRec2, email_rec2_text_box.getValue(), sizeof(fieldEmailRec2));
    strncpy(fieldEmailRec3, email_rec3_text_box.getValue(), sizeof(fieldEmailRec3));
    strncpy(fieldEmailRec4, email_rec4_text_box.getValue(), sizeof(fieldEmailRec4));
    strncpy(fieldEmailRec5, email_rec5_text_box.getValue(), sizeof(fieldEmailRec5));
    if (String(fieldEmailSender).length() == 0 ||
        String(fieldPassEmailSender).length() == 0 ||
        !(
            String(fieldEmailRec1).length() != 0 ||
            String(fieldEmailRec2).length() != 0 ||
            String(fieldEmailRec3).length() != 0 ||
            String(fieldEmailRec4).length() != 0 ||
            String(fieldEmailRec5).length() != 0))
    {
      fieldAlertsActive = false;
    }
  }

  if (shouldSaveConfig)
  {
    saveConfigFile();
  }

  return true;
}