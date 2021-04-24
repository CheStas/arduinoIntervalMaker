#ifndef TIME_H
#define TIME_H

#include <Arduino.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <common/mediator.cpp>
#include <common/event_names.h>

class TimeComponent : public BaseComponent {

    const long utcOffsetInSeconds = 10800;
    char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    long savedMillisForTime = 0;
    WiFiUDP ntpUDP;
    NTPClient timeClient;

    void updateTimeString(String time);

    void updateEpochTime(long time);

public:
    String savedTimeString = "notConnected";
    long savedEpochTime = 0;

    TimeComponent();

    void begin();
    void loop();
};

#endif
