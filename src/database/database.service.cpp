#include <database/database.service.h>

void DatabaseService::setDataInBuffer() {
    isDataInBuffer = true;
}

void DatabaseService::updateAllDataInDb() {
    File db = LittleFS.open("/data.json", "w+");
    // if (db.available()) {
    // TODO count doc size!!!
    DynamicJsonDocument jsonDoc(1024);
    deserializeJson(jsonDoc, db);

    jsonDoc["app"]["isOpenBoxAtWork"] = app.isOpenBoxAtWork;
    jsonDoc["app"]["isLightOnAtWork"] = app.isLightOnAtWork;
    jsonDoc["app"]["isMakePhotoAtWork"] = app.isMakePhotoAtWork;
    jsonDoc["box"]["goalGatePosition"] = box.goalGatePosition;
    jsonDoc["light"]["lightStatus"] = light.lightStatus;
    jsonDoc["light"]["lightOnAtSecondsTooday"] = light.lightOnAtSecondsTooday;
    jsonDoc["light"]["lightOffAtSecondsTooday"] = light.lightOffAtSecondsTooday;
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
    File dataFile = LittleFS.open("/data.json", "r");
    if (dataFile.available()) {
            // TODO count doc size!!!
        DynamicJsonDocument jsonDoc(dataFile.size()*2);
        deserializeJson(jsonDoc, dataFile);
        box.goalGatePosition = jsonDoc["box"]["goalGatePosition"];
        app.isOpenBoxAtWork = jsonDoc["app"]["isOpenBoxAtWork"];
        app.isLightOnAtWork = jsonDoc["app"]["isLightOnAtWork"];
        app.isMakePhotoAtWork = jsonDoc["app"]["isMakePhotoAtWork"];
        light.lightStatus = jsonDoc["light"]["lightStatus"];
        light.lightOnAtSecondsTooday = jsonDoc["light"]["lightOnAtSecondsTooday"];
        light.lightOffAtSecondsTooday = jsonDoc["light"]["lightOffAtSecondsTooday"];
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
