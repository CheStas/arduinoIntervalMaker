#ifndef BOX_COMPONENT_H
#define BOX_COMPONENT_H

#include <Arduino.h>
#include <Servo.h>
#include <common/mediator.cpp>
#include <common/event_names.h>

class BoxComponent : public BaseComponent {
    Servo myservo;
    uint8_t pin;
    int gatePosition = 0;
    int goalGatePosition = 0;
    unsigned long savedMillisForServo = 0;

public:
    void begin(uint8_t pin);

    void closeGate();

    void openGate();

    void setGoalPosition(int p);

    void toggleGate();

    void setGateStatus();

    void loop();

    int gateStatus = 0;
};

#endif
