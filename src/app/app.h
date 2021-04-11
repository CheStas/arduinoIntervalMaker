#ifndef APP_H
#define APP_H

#include <common/mediator.cpp>
#include <common/event_names.h>
#include <Arduino.h>

#include <box/box.component.h>
#include <light/light.component.h>
#include <shutter/shutter.component.h>
#include <time/time.component.h>
#include <intervalometer/intervalometer.component.h>

#include <database/database.service.h>
#include <socket/socket.service.h>

class App : public Mediator {
private:
    BoxComponent *box;
    LightComponent *light;
    ShutterComponent *shutter;
    TimeComponent *time;
    IntervalometerComponent *intervalometer;
    DatabaseService *database = DatabaseService::getInstance();
    unsigned long savedMillis = 0;
    void startWork();
    void stopWork();

public:
    SocketService *socket;

    App(
            BoxComponent *box,
            LightComponent *light,
            ShutterComponent *shutter,
            TimeComponent *time,
            IntervalometerComponent *intervalometer,
            SocketService *socket
    );

    void Notify(BaseComponent *sender, eventNameType event);
    void Notify(BaseComponent *sender, eventNameType event, char* name, int value);
    void Notify(BaseComponent *sender, eventNameType event, uint8_t num);
    void loop();
};

#endif
