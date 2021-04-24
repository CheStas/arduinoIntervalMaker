#include <light/light.component.h>

void LightComponent::begin(uint8_t lightPin) {
    pin = lightPin;
    pinMode(lightPin, OUTPUT);
}

void LightComponent::loopIsTimeToOn(long savedEpochTime) {
    if (lightOnAtSecondsTooday && lightOffAtSecondsTooday) {
        int secondsFromDayStart = savedEpochTime % 86400;
        // debug
        Serial.print("Seconds from midnight: ");
        Serial.println(secondsFromDayStart);

        bool isNowInRange = isInRange(lightOnAtSecondsTooday, lightOffAtSecondsTooday, secondsFromDayStart);
        if (isNowInRange && getBrightness() == 0) {
            setBrightness(100);
        } else if (!isNowInRange && getBrightness() > 0) {
            setBrightness(0);
        }
    }
}

void LightComponent::loop() {

    if (brightness != targetBrightness && millis() > savedMillis) {
        if (brightness < targetBrightness) brightness += 1;
        else if (brightness > targetBrightness) brightness -= 1;
        analogWrite(pin, brightness);
        savedMillis = millis() + 10;

        if (brightness == targetBrightness) {
            this->mediator_->Notify(this, eventNames.lightStatusEventName);
        }
    }
}

boolean LightComponent::setWorkingHours(int from, int to) {
    if (from == 0 && to == 0) {
        lightOnAtSecondsTooday = 0;
        lightOffAtSecondsTooday = 0;
        return true;
    }

    if (to > 0 && to < 86400 && from > 0 && from < 86400) {
        lightOnAtSecondsTooday = from;
        lightOffAtSecondsTooday = to;
        return true;
    }

    return false;
}

boolean LightComponent::isInRange(int from, int to, int target) {
    if (from > to && target > from && target > to) {
        return true;
    } else if (from > to && target < from && target < to) {
        return true;
    } else if (from < to && target > from && target < to) {
        return true;
    }

    return false;
}

void LightComponent::setBrightness(int num) {
    if (num >= 0 && num <= 100) {
        targetBrightness = map(num, 0, 100, 0, 1024);
    }
}

int LightComponent::getBrightness() {
    return map(brightness, 0, 1024, 0, 100);
}
