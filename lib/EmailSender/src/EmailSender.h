#pragma once
#include "secrets.h"
#include <ESP_Mail_Client.h>
#include <vector>

class EmailSender
{
public:
    static EmailSender &getInstance();
    void addRecipient(const String &email);
    void clearRecipients();
    bool sendMail(const String &subject, const String &body);

private:
    EmailSender();
    SMTPSession smtp;
    Session_Config session_config;
    std::vector<String> recipients;
};