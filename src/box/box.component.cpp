#include <box/box.component.h>

void BoxComponent::begin(uint8_t pin) {
    myservo.attach(pin);
}

void BoxComponent::loop() {
    gatePosition = myservo.read();
    if (gatePosition != goalGatePosition && millis() > savedMillisForServo) {
        if (gatePosition < goalGatePosition) myservo.write(gatePosition + 1);
        if (gatePosition > goalGatePosition) myservo.write(gatePosition - 1);

        gatePosition = myservo.read();
        if (gatePosition == goalGatePosition) {
            setGateStatus();
        }
        savedMillisForServo = millis() + 30;
    }
}

void BoxComponent::closeGate() {
    setGoalPosition(180);
}

void BoxComponent::openGate() {
    setGoalPosition(0);
}

void BoxComponent::setGoalPosition(int p) {
    goalGatePosition = p;
}

void BoxComponent::toggleGate() {
    gateStatus ? closeGate() : openGate();
}

void BoxComponent::setGateStatus() {
    gateStatus = myservo.read() == 0 ? 1 : 0;
    this->mediator_->Notify(this, eventNames.gateEventName);
}
