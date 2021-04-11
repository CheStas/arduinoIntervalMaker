#ifndef LIGHT_COMPONENT_H
#define LIGHT_COMPONENT_H

#include <Arduino.h>
#include <common/mediator.cpp>

class LightComponent : public BaseComponent {
    int brightness = 0;
    int targetBrightness = 0;
    unsigned long savedMillis = 0;
    uint8_t pin;

public:
    void begin(uint8_t lightPin);

    void loop();

    void setBrightness(int num);

    int getBrightness();
};

#endif
