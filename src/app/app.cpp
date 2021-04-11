#include <app/app.h>

App::App(
        BoxComponent *b1,
        LightComponent *l1,
        ShutterComponent *h1,
        TimeComponent *t1,
        IntervalometerComponent *i1,
        SocketService *s1
) :
        box(b1),
        light(l1),
        shutter(h1),
        time(t1),
        intervalometer(i1),
        socket(s1) {
    this->box->set_mediator(this);
    this->light->set_mediator(this);
    this->shutter->set_mediator(this);
    this->time->set_mediator(this);
    this->intervalometer->set_mediator(this);
    this->socket->set_mediator(this);

    this->database->setDataFromDb();

    this->box->setGoalPosition(this->database->box.goalGatePosition);

    this->light->setBrightness(this->database->light.lightStatus);

    this->intervalometer->setInterval(this->database->intervalometer.intervalCounter);
    this->intervalometer->setWorkingTime(this->database->intervalometer.workingTimeSec);
    this->intervalometer->setIdleTime(this->database->intervalometer.idleTimeSec);
    this->intervalometer->setLastIntervalTime(this->database->intervalometer.lastIntervalTime);
    this->intervalometer->setWorkStatus(this->database->intervalometer.workStatus);
};

void App::loop() {
    database->loop();

    if (savedMillis && millis() > savedMillis) {
        shutter->shut();
        savedMillis = 0;
    }
}

void App::startWork() {
    box->openGate();
    light->setBrightness(100);

    savedMillis = millis() + 2000;
}

void App::stopWork() {
    box->closeGate();
    light->setBrightness(0);
}

void App::Notify(BaseComponent *sender, eventNameType event) {

    if (strcmp(event, eventNames.timeEventName) == 0) {

        socket->broadcastTXT(eventNames.timeEventName, time->savedTimeString);
 
    } else if (strcmp(event,eventNames.gateEventName) == 0) {

        socket->broadcastTXT(eventNames.gateEventName, box->gateStatus);

    } else if (strcmp(event, eventNames.intervalsEventName) == 0) {

        database->intervalometer.intervalCounter = intervalometer->intervalCounter;
        database->setDataInBuffer();
        socket->broadcastTXT(eventNames.intervalsEventName, intervalometer->intervalCounter);

    } else if (strcmp(event, eventNames.idleTimeEventName) == 0) {
 
        database->intervalometer.idleTimeSec = intervalometer->idleTimeSec;
        database->setDataInBuffer();
        socket->broadcastTXT(eventNames.idleTimeEventName, intervalometer->idleTimeSec);

    } else if (strcmp(event, eventNames.workingTimeEventName) == 0) {

        database->intervalometer.workingTimeSec = intervalometer->workingTimeSec;
        database->setDataInBuffer();
        socket->broadcastTXT(eventNames.workingTimeEventName, intervalometer->workingTimeSec);

    } else if (strcmp(event, eventNames.lastIntervalTimeEventName) == 0) {

        database->intervalometer.lastIntervalTime = intervalometer->lastIntervalTime;
        database->setDataInBuffer();
        socket->broadcastTXT(eventNames.lastIntervalTimeEventName, intervalometer->lastIntervalTime);

    } else if (strcmp(event, eventNames.workStatusEventName) == 0) {

        if (intervalometer->workStatus) {
            startWork();
        } else {
            stopWork();
        }

        database->intervalometer.workStatus = intervalometer->workStatus;
        database->setDataInBuffer();
        socket->broadcastTXT(eventNames.workStatusEventName, intervalometer->workStatus);

    } else if (strcmp(event, eventNames.estimatedFinishTimeEventName) == 0) {

        socket->broadcastTXT(eventNames.estimatedFinishTimeEventName, intervalometer->estimatedFinishTime);

    } else if (strcmp(event, eventNames.lightStatusEventName) == 0) {

        socket->broadcastTXT(eventNames.lightStatusEventName, light->getBrightness());
        database->light.lightStatus = light->getBrightness();
        database->setDataInBuffer();

    } else {
        Serial.print("unnkown event name: ");
        Serial.println(event);
    }
}

void App::Notify(BaseComponent *sender, eventNameType event, char* name, int value) {

    if (strcmp(event, eventNames.clientNewEventName) == 0) {
        if (strcmp(name, eventNames.gateEventName) == 0) {
            box->toggleGate();
        } else if (strcmp(name, eventNames.lightStatusEventName) == 0) {
            light->setBrightness(value);
        } else if (strcmp(name, eventNames.shutterEventName) == 0) {
            shutter->shut();
        } else if (strcmp(name, eventNames.intervalsEventName) == 0) {
            intervalometer->setInterval(value);
        } else if (strcmp(name, eventNames.workingTimeEventName) == 0) {
            intervalometer->setWorkingTime(value);
        } else if (strcmp(name, eventNames.idleTimeEventName) == 0) {
            intervalometer->setIdleTime(value);
        } else {
            Serial.print("unnkown client event name: ");
            Serial.print(name);
            Serial.print(", value: ");
            Serial.println(value);
        };

    }

}

void App::Notify(BaseComponent *sender, eventNameType event, uint8_t num) {

    if (strcmp(event, eventNames.newUsersEventName) == 0) {
        socket->sendTo(num, eventNames.timeEventName, time->savedTimeString);

        socket->sendTo(num, eventNames.gateEventName, box->gateStatus);

        socket->sendTo(num, eventNames.lightStatusEventName, light->getBrightness());

        socket->sendTo(num, eventNames.intervalsEventName, intervalometer->intervalCounter);
        socket->sendTo(num, eventNames.workingTimeEventName, intervalometer->workingTimeSec);
        socket->sendTo(num, eventNames.idleTimeEventName, intervalometer->idleTimeSec);
        socket->sendTo(num, eventNames.lastIntervalTimeEventName, intervalometer->lastIntervalTime);
    }

}