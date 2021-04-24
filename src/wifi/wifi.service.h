#ifndef WIFI_H
#define WIFI_H

#include <Arduino.h>
#include "LittleFS.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <config.h>

class WifiService {
private:
    char ssid[7];
    char password[9];
    MDNSResponder mdns;
    ESP8266WiFiMulti WiFiMulti;
    ESP8266WebServer server;

    String getContentType(String filename);

    bool handleFileRead(String path);

public:
    WifiService();

    void begin();
    void loop();
};

#endif
