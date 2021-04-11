#ifndef SHUTTER_COMPONENT_H
#define SHUTTER_COMPONENT_H

#include <Arduino.h>
#include <common/mediator.cpp>

class ShutterComponent : public BaseComponent {
    uint8_t pin;
    bool isPressed;
    unsigned long savedMillis = 0;;

public:
    void begin(uint8_t pin);

    void loop();

    void shut();
};

#endif
