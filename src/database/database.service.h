#ifndef DATABASE_SERVICE_H
#define DATABASE_SERVICE_H

#include <Arduino.h>
#include "LittleFS.h"
#include <ArduinoJson.h>
#include <common/event_names.h>

struct boxStruct {
    int goalGatePosition;
};

struct lightStruct {
    int lightStatus;
};

struct intervalometerStruct {
    int intervalCounter;
    int workingTimeSec;
    int idleTimeSec;
    long lastIntervalTime;
    bool workStatus;
};

class DatabaseService {
protected:
    DatabaseService() {}

    static DatabaseService *database;
public:
    bool isDataInBuffer;
    boxStruct box;
    lightStruct light;
    intervalometerStruct intervalometer;

    void setDataInBuffer();

    DatabaseService(DatabaseService &other) = delete;

    void operator=(const DatabaseService &) = delete;

    static DatabaseService *getInstance();

    void loop();

    void setDataFromDb();

private:
    void updateAllDataInDb();
};

#endif
