#include <shutter/shutter.component.h>

void ShutterComponent::begin(uint8_t shutterPin) {
    pin = shutterPin;
    pinMode(pin, OUTPUT);
}

void ShutterComponent::loop() {
    if (isPressed && millis() > savedMillis) {
        digitalWrite(pin, LOW);
        isPressed = false;
    }
}

void ShutterComponent::shut() {
    if (!isPressed) {
        isPressed = true;
        savedMillis = millis() + 1000;
        digitalWrite(pin, HIGH);
    }
}
