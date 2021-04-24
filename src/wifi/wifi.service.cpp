
#include <wifi/wifi.service.h>

WifiService::WifiService() : server(80) {};

void WifiService::begin() {
    WiFiMulti.addAP(ssid, password);
    while (WiFiMulti.run() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    if (mdns.begin("espWebSock", WiFi.localIP())) {
        Serial.println("MDNS responder started");
        mdns.addService("http", "tcp", 80);
        mdns.addService("ws", "tcp", 81);
    } else {
        Serial.println("MDNS.begin failed");
    }
    Serial.print("Connect to http://");
    Serial.println(WiFi.localIP());

    LittleFS.begin();

    server.onNotFound([this]() {                              // If the client requests any URI
        if (!handleFileRead(server.uri()))                  // send it if it exists
            server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
    });
    server.begin();
}

void WifiService::loop() {
    server.handleClient();
}

bool WifiService::handleFileRead(String path) {  // send the right file to the client (if it exists)
    Serial.println("handleFileRead: " + path);
    if (path.endsWith("/")) path += "index.html";           // If a folder is requested, send the index file
    String contentType = getContentType(path);             // Get the MIME type
    String pathWithGz = path + ".gz";
    if (LittleFS.exists(pathWithGz) ||
        LittleFS.exists(path)) {  // If the file exists, either as a compressed archive, or normal
        if (LittleFS.exists(pathWithGz))                          // If there's a compressed version available
            path += ".gz";                                         // Use the compressed version
        File file = LittleFS.open(path, "r");                    // Open the file
        server.streamFile(file, contentType);    // Send it to the client
        file.close();                                          // Close the file again
        Serial.println(String("\tSent file: ") + path);
        return true;
    }
    Serial.println(String("\tFile Not Found: ") + path);
    return false;                                          // If the file doesn't exist, return false
}

String WifiService::getContentType(String filename) {
    if (filename.endsWith(".htm")) return "text/html";
    if (filename.endsWith(".html")) return "text/html";
    if (filename.endsWith(".css")) return "text/css";
    if (filename.endsWith(".js")) return "application/javascript";
    if (filename.endsWith(".json")) return "application/json";
    if (filename.endsWith(".svg")) return "image/svg+xml";
    if (filename.endsWith(".png")) return "image/png";
    if (filename.endsWith(".gif")) return "image/gif";
    if (filename.endsWith(".jpg")) return "image/jpeg";
    if (filename.endsWith(".ico")) return "image/x-icon";
    if (filename.endsWith(".xml")) return "text/xml";
    if (filename.endsWith(".pdf")) return "application/x-pdf";
    if (filename.endsWith(".zip")) return "application/x-zip";
    if (filename.endsWith(".gz")) return "application/x-gzip";
    return "text/plain";
}