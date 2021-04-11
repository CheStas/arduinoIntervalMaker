#include <light/light.component.h>
#include <common/event_names.h>

void LightComponent::begin(uint8_t lightPin) {
    pin = lightPin;
    pinMode(lightPin, OUTPUT);
}

void LightComponent::loop() {
    if (brightness != targetBrightness && millis() > savedMillis) {
        if (brightness < targetBrightness) brightness += 1;
        if (brightness > targetBrightness) brightness -= 1;
        analogWrite(pin, brightness);
        savedMillis = millis() + 10;

        if (brightness == targetBrightness) {
            this->mediator_->Notify(this, eventNames.lightStatusEventName);
        }
    }
}

void LightComponent::setBrightness(int num) {
    if (num >= 0 && num <= 100) {
        targetBrightness = map(num, 0, 100, 0, 1024);
    }
}

int LightComponent::getBrightness() {
    return map(brightness, 0, 1024, 0, 100);
}
