#include <time/time.component.h>

void TimeComponent::begin() {
    timeClient.begin();
}

void TimeComponent::loop() {
    if (millis() - savedMillisForTime > 1000) {
        timeClient.update();
        savedMillisForTime = millis();
        updateTimeString(timeClient.getFormattedTime());
        updateEpochTime(timeClient.getEpochTime());
        // debug
        // Serial.println(savedEpochTime);
    }
}

void TimeComponent::updateTimeString(String time) {
    savedTimeString = time;
    this->mediator_->Notify(this, eventNames.timeEventName);
}

void TimeComponent::updateEpochTime(long time) {
    savedEpochTime = time;
}

TimeComponent::TimeComponent() : timeClient(ntpUDP, "europe.pool.ntp.org", utcOffsetInSeconds) {};
