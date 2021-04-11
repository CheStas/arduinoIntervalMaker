#include <Arduino.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <common/event_names.h>
#include <database/database.service.h>

void DatabaseService::setDataInBuffer() {
    isDataInBuffer = true;
}

void DatabaseService::updateAllDataInDb() {
    File db = SPIFFS.open("/data.json", "w+");
    // if (db.available()) {
    // TODO count doc size!!!
    DynamicJsonDocument jsonDoc(1024);
    deserializeJson(jsonDoc, db);

    jsonDoc["box"]["goalGatePosition"] = box.goalGatePosition;
    jsonDoc["light"]["lightStatus"] = light.lightStatus;
    jsonDoc["intervalometer"]["intervalCounter"] = intervalometer.intervalCounter;
    jsonDoc["intervalometer"]["workingTimeSec"] = intervalometer.workingTimeSec;
    jsonDoc["intervalometer"]["idleTimeSec"] = intervalometer.idleTimeSec;
    jsonDoc["intervalometer"]["lastIntervalTime"] = intervalometer.lastIntervalTime;
    jsonDoc["intervalometer"]["workStatus"] = intervalometer.workStatus;

    serializeJson(jsonDoc, db);

    // }
    db.close();
}

void DatabaseService::loop() {
    if (isDataInBuffer) {
        updateAllDataInDb();
        isDataInBuffer = false;
    }
}

void DatabaseService::setDataFromDb() {
    File dataFile = SPIFFS.open("/data.json", "r");
    if (dataFile.available()) {
            // TODO count doc size!!!
        DynamicJsonDocument jsonDoc(dataFile.size()*2);
        deserializeJson(jsonDoc, dataFile);
        box.goalGatePosition = jsonDoc["box"]["goalGatePosition"];
        light.lightStatus = jsonDoc["light"]["lightStatus"];
        intervalometer.intervalCounter = jsonDoc["intervalometer"]["intervalCounter"];
        intervalometer.workingTimeSec = jsonDoc["intervalometer"]["workingTimeSec"];
        intervalometer.idleTimeSec = jsonDoc["intervalometer"]["idleTimeSec"];
        intervalometer.lastIntervalTime = jsonDoc["intervalometer"]["lastIntervalTime"];
        intervalometer.workStatus = jsonDoc["intervalometer"]["workStatus"];
        // debug
        Serial.print("init data from file: ");
        serializeJson(jsonDoc, Serial);
        Serial.println();

    }

    dataFile.close();
}

DatabaseService *DatabaseService::database = nullptr;

DatabaseService *DatabaseService::getInstance() {
    if (database == nullptr) {
        database = new DatabaseService();
    }

    return database;
}
