//> Nivel de debug
#define CORE_DEBUG_LEVEL ARDUHAL_LOG_LEVEL_VERBOSE
#include "EmailSender.h"
#include "esp_log.h"

static const char *TAG = "email";

EmailSender &EmailSender::getInstance()
{
    static EmailSender instance;
    return instance;
}

EmailSender::EmailSender()
{
    MailClient.networkReconnect(true);
    // smtp.debug(1); // debug only
    smtp.debug(0);
    session_config.server.host_name = "smtp.gmail.com";
    session_config.server.port = esp_mail_smtp_port_465;
    session_config.login.email = AUTHOR_EMAIL;
    session_config.login.password = AUTHOR_PASSWORD;
    session_config.login.user_domain = F("127.0.0.1");

    smtp.callback([](SMTP_Status status)
                  { ESP_LOGI(TAG, "%s", status.info()); });
    smtp.setTCPTimeout(10);
}

void EmailSender::addRecipient(const String &email)
{
    if (recipients.size() < 5 && email.length() > 0)
    {
        recipients.push_back(email);
    }
}

void EmailSender::clearRecipients()
{
    recipients.clear();
}

bool EmailSender::sendMail(const String &subject, const String &body)
{

    if (!smtp.connect(&session_config))
    {
        ESP_LOGE(TAG, "No se pudo conectar al servidor SMTP");
        return false;
    }
    if (!smtp.isLoggedIn())
    {
        ESP_LOGW(TAG, "Aun no ha iniciado sesion");
    }
    else
    {
        if (smtp.isAuthenticated())
        {
            ESP_LOGI(TAG, "Inicio de sesion exitoso");
        }
        else
        {
            ESP_LOGW(TAG, "Conectado sin autenticacion");
        }
    }

    SMTP_Message message;
    message.sender.name = "Bot sensor de temperatura";
    message.sender.email = session_config.login.email;
    for (const auto &email : recipients)
    {
        message.addRecipient("Destinatario", email);
    }
    message.timestamp.tag = "#esp_mail_current_time";
    message.timestamp.format = "%d-%m-%Y %H:%M:%S";
    message.subject = subject;
    message.text.content = body.c_str();

    bool result = MailClient.sendMail(&smtp, &message);
    if (!result)
    {
        ESP_LOGE(TAG, "Error, Status Code: %d, Error Code: %d, Reason: %s",
                 smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
    }
    smtp.sendingResult.clear();
    return result;
}