// TODO

// 2. + show lastIntervalTime
// 4. + show current process (is at work?)
// 3. + show estimated finished time
// 7. + button light on/off, show light status
// 5. + connect and test button to make a photo
// 11. + set brighness
// 8. + smooth on/off light
// 12. + ARDUINO POWER 12v !!!
// 6. + light on at, light off at
// 1. +++ contorl work prosess:
//   - should box open?
//   - should light on?
//   - should press button?
// 11. wifi save to EEPROM
// 9. connect photoresistor (test diff resisot to choose sensetivity), show current ligh status
// 10. adjust light use photoresistor


// ;  to upload data folder:
// ;  pio run -t uploadfs

#include <wifi/wifi.service.h>
#include <socket/socket.service.h>
#include <time/time.component.h>
#include <box/box.component.h>
#include <light/light.component.h>
#include <shutter/shutter.component.h>
#include <intervalometer/intervalometer.component.h>

#include <app/app.h>

WifiService * wifi;
SocketService * socket;
TimeComponent * timeComponent;
BoxComponent * box;
LightComponent * light;
ShutterComponent * shutter;
IntervalometerComponent * intervalometer;
App * app;

void setup() {
    Serial.begin(9600);

    wifi = new WifiService();
    wifi->begin();

    socket = new SocketService();
    socket->begin();

    timeComponent = new TimeComponent();
    timeComponent->begin();

    box = new BoxComponent();
    box->begin(D10);

    light = new LightComponent();
    light->begin(D13);

    shutter = new ShutterComponent();
    shutter->begin(D8);

    intervalometer = new IntervalometerComponent();
 
    app = new App(box, light, shutter, timeComponent, intervalometer, socket);
    // MAKE LIGHT NOT BLICK ON CAMERA
    analogWriteFreq(5000);

    Serial.println("setup done");
}

void loop() {
    wifi->loop();
    socket->loop();
    timeComponent->loop();
 
    intervalometer->loop(timeComponent->savedEpochTime);
    light->loopIsTimeToOn(timeComponent->savedEpochTime, intervalometer->workStatus);
    light->loop();
    box->loop();
    shutter->loop();

    app->loop();
}
