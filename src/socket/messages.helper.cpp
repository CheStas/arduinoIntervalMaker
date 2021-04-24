#include <socket/messages.helper.h>

String MessageHelper::getMessageToSend(const char* fieldName, String value) {
    int size = sizeof(nameField) + sizeof(valueField) + sizeof(fieldName) + sizeof(value);
    DynamicJsonDocument jsonDoc(size * 4);
    jsonDoc[nameField] = fieldName;
    jsonDoc[valueField] = value;
    String data;
    serializeJson(jsonDoc, data);
    return data;
}

String MessageHelper::getMessageToSend(const char* fieldName, int value) {
    // TODO check !! count size
    int size = sizeof(nameField) + sizeof(valueField) + sizeof(fieldName) + sizeof(value);
    DynamicJsonDocument jsonDoc(size * 4);
    jsonDoc[nameField] = fieldName;
    jsonDoc[valueField] = value;
    String data;
    serializeJson(jsonDoc, data);
    return data;
}

parsedData MessageHelper::parseAndGetNameValue(char *msg) {
    // TODO COUNT DOC size
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, msg);

    struct parsedData data;

    strcpy(data.name, doc[nameField]);
    data.value = doc[valueField];

    return data;
}
