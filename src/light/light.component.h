#ifndef LIGHT_COMPONENT_H
#define LIGHT_COMPONENT_H

#include <Arduino.h>
#include <common/mediator.cpp>
#include <common/event_names.h>

class LightComponent : public BaseComponent {
    int brightness = 0;
    int targetBrightness = 0;
    unsigned long savedMillis = 0;
    uint8_t pin;
    boolean isInRange(int from, int to, int target);

public:
    int lightOnAtSecondsTooday = 0;
    int lightOffAtSecondsTooday = 0;

    void begin(uint8_t lightPin);

    void loop();
    void loopIsTimeToOn(long savedEpochTime, bool disableCheck);

    void setBrightness(int num);
    void setWorkingHoursFrom(int from);
    void setWorkingHoursTo(int to);
    int getBrightness();
};

#endif
