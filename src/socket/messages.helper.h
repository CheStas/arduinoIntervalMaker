#ifndef MSHELPER_H
#define MSHELPER_H

#include <Arduino.h>
#include <ArduinoJson.h>

struct parsedData {
    char name[10];
    int value;
};

class MessageHelper {
public:
    const char nameField[5] = "name";
    const char valueField[6] = "value";

    String getMessageToSend(const char* fieldName, String value);

    String getMessageToSend(const char* fieldName, int value);

    parsedData parseAndGetNameValue(char *msg);

};

#endif
